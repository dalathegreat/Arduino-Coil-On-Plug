/**********************************************************************
       __________  ______      ____  _   __   ____  __    __  ________
      / ____/ __ \/  _/ /     / __ \/ | / /  / __ \/ /   / / / / ____/
     / /   / / / // // /     / / / /  |/ /  / /_/ / /   / / / / / __  
    / /___/ /_/ // // /___  / /_/ / /|  /  / ____/ /___/ /_/ / /_/ /  
    \____/\____/___/_____/  \____/_/ |_/  /_/   /_____/\____/\____/   
    
COIL ON PLUG ARDUINO UNO Beta v1.4 (2.5.2017) Daniel Öster
This application samples in two distributor signals, (CR & CP), Camshaft Reference & Camshaft Position
It then syncs the firing sequence to Cylinder 1, and ignores CP information from there on.
It then outputs 4 digital outs depending on which cylinder should fire.
It also inputs ignition signal and disables the sequencing while a sparkplug fires and creates interference

Changelog v1.4 , Added ignition input to disable interrupts while spark fires
***********************************************************************/

const byte CamshaftPositionPin = 3;   //Interrupt pin for camshaft position (pin3) [datarange 0-255]
const byte CamshaftReferencePin = 2;  //Interrupt pin for camshaft reference (pin2) [datarange 0-255]
const byte IgnitionPin = 28;          //Interrupt pin for ignition (pin28) [datarange 0-255]
volatile boolean state = LOW;         //State variable is used by CR interrupt to perform different tasks depending on signal state [datarange 0-1]
volatile byte pos = 0;                //Global position pulse counter [datarange 0-255]
volatile boolean syncAchieved = 0;    //Set this to true if sync has been achieved [datarange 0-255] (Does this bit need to be volatile?)
volatile byte cylinderCounter = 0;    //Sequence which cylinder should fire (0-1-2-3) [datarange 0-255]
const byte fireOrder[] = {            //Table containing the fireorder (1-3-4-2) [datarange 0-255]
  B00000001,
  B00000100,
  B00001000,
  B00000010};

void setup() { //Define setup variables and pin confiurations
  pinMode(CamshaftPositionPin, INPUT_PULLUP);   //Interrupt pin for camshaft position (pin3) Use pullup to get rid of dangerous wire break situation.
  pinMode(CamshaftReferencePin, INPUT_PULLUP);  //Interrupt pin for camshaft reference (pin2) Use pullup to get rid of dangerous wire break situation.
  //Check if we stopped the engine on high ref pin. 4% chance of this happening :)
  boolean startupPin = 0; //Variable for storing startup behaviour of reference pin
  startupPin = digitalRead(CamshaftReferencePin); //Read reference pin
  if (startupPin == 1){state = HIGH;};            //If high,change state to high to correct sync
  DDRB = 0b00001111; //Set the first 4 pins on the B register to outputs. (Digital 11<->8 on arduino Uno) //Connect Cylinder1 to pin8, Cylinder 2 to pin9, Cylinder 3 to pin10 and Cylinder 4 to pin11.
  attachInterrupt(digitalPinToInterrupt(CamshaftReferencePin), ISR1, CHANGE); //Reference must trigger on CHANGE to sniff sync window. Will later on in code be written to RISING after sync has been achieved
  //This is executed last in the setup to avoid cancelling the setup with an interrupt
}

void loop() { //Program main loop
  while(syncAchieved == 1){//Start sequencing ignition outputs when sync has occured. Otherwise just wait until engine starts rotating.
    PORTB = fireOrder[cylinderCounter];}
}

void ISR0() //Keep constant track of RISING position pin
{
  pos++; //Increment one every pulse (max 8 in practice during large sync window, 360 disc total but variable never stores more than 8)
}
void ISR1() { //Keep constant track of CHANGE/(RISING after sync) state for reference pin
  
  if (syncAchieved == 1) { //If we have achieved sync, do only this part of the interrupt
    if(digitalRead(CamshaftReferencePin) == HIGH){ //This is an "unneeded if". It filters out spark EMI, incorrect pulses will be counted otherwise.
      cylinderCounter++; //we sequence the next cylinder that should fire
      if (cylinderCounter > 3){cylinderCounter = 0;} //This a reset for the cylinder firing sequencer. We have 4 cylinders, so we reset after we reached the final one.
    }
  }

  else //Perform the code below only before sync has been achieved. This saves CPU resources at high engine RPMs.
  {
    state = !state; //Flip the state bit to indicate rising/falling sitation on reference pin
    if (state == 1) //RISING SIGNAL on referene pin
      { 
        attachInterrupt(digitalPinToInterrupt(CamshaftPositionPin), ISR0, RISING); //Before sync is achieved, we enable keeping track of the position pin during rising state. This info is used to calculate if we are inside large window.
      }
    else //(state == 0) FALLING SIGNAL on reference pin
      {  
        detachInterrupt(digitalPinToInterrupt(CamshaftPositionPin)); //We disable keeping track of the position pin info during rising state
      }
    if (pos > 6) //We hit the jackpot, we have confirmed that cyl1 will fire next. The >6 makes it so that only big window can sync
      {
         detachInterrupt(digitalPinToInterrupt(CamshaftPositionPin)); //If sync is achieved, we immedeatly disable keeping track of the position pin permanently
         syncAchieved = 1; //Set variable used to start ignition and disable unnecessary measuring of position pin

           //Ignition sampling settings, start capturing ignition events
            PCICR |= 0b00000010;     //Turn on port C pin change interrupts, used for Ignition sampling
            PCMSK1 |= 0b00100000;    //Turn on pin PC5, which is PCINT11, physical pin 28

         attachInterrupt(digitalPinToInterrupt(CamshaftReferencePin), ISR1, RISING); //We no longer keep track of change state once synced. Now we locate rising change on the ref pin.
      } 
      pos = 0; //Reset after each count. This is an important reset before large window has been found (moved up)
  }   
}

ISR(PCINT1_vect){ // Port C, PCINT8 - PCINT14, Interrupt for Ignition High on A5 (pin 28)
  if(digitalRead(IgnitionPin) == HIGH) //Filtering, determine if pin is high. 
  {
    detachInterrupt(digitalPinToInterrupt(CamshaftReferencePin)); //If pin is high, disable sequencing
  }
else 
  {  
   attachInterrupt(digitalPinToInterrupt(CamshaftReferencePin), ISR1, RISING); //Otherwise enable sequencing. This can even be called unnecessarily without issues.
  }
  
}
