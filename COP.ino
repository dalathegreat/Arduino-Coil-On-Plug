//COIL ON PLUG ARDUINO UNO Beta v1.0 (25.3.2017) Daniel Öster
//This application samples in two distributor signals, (CR & CP), Camshaft Reference & Camshaft Position
//It then syncs the firing sequence to Cylinder 1, and ignores CP information from there on.
//It then outputs 4 digital outs depending on which cylinder should fire. It can also do wasted spark.
//NOTE: Lean version without wasted spark. Wasted spark creates extra EMI that is currently breaking the sequencing.

const byte CamshaftPositionPin = 3;   //Interrupt pin for camshaft position (pin3)
const byte CamshaftReferencePin = 2;  //Interrupt pin for camshaft reference (pin2)
volatile boolean state = LOW;         //State variable is used by interrupt to perform different tasks depending on signal state
volatile byte pos = 0;                //Global position pulse counter
volatile byte ref = 0;                //Global reference pulse counter
volatile boolean syncAchieved = 0;    //Set this to true if sync has been achieved
volatile byte cylinderCounter = 0;    //Sequence which cylinder should fire (0-1-2-3)

void setup() {
  //Define setup variables
  boolean startupPin = 0; //Variable for storing startup behaviour of reference pin
  pinMode(CamshaftPositionPin, INPUT_PULLUP);   //Interrupt pin for camshaft position (pin3) Use pullup to get rid of dangerous wire break situation.
  pinMode(CamshaftReferencePin, INPUT_PULLUP);  //Interrupt pin for camshaft reference (pin2) Use pullup to get rid of dangerous wire break situation.
  attachInterrupt(digitalPinToInterrupt(CamshaftReferencePin), ISR1, CHANGE); //Reference must trigger on CHANGE to sniff sync window. Will later on in code be written to RISING after sync has been achieved
  //Check if we stopped the engine on high ref pin.
  startupPin = digitalRead(CamshaftReferencePin); //Read reference pin
  if (startupPin == 1){state = HIGH;};            //If high,change state to high to correct sync
  DDRB = 0b00001111; //Set the first 4 pins on the B register to outputs. (Digital 11<->8 on arduino Uno)
  PORTB = B00000000; //set all outputs to 0, is this line needed?
//Connect Cylinder1 to pin8, Cylinder 2 to pin9, Cylinder 3 to pin10 and Cylinder 4 to pin11.
//Sequential firing order for SR20 engine type is 1-3-4-2 
//1&4 - 2&3 when wasted
}
void loop() { //Program main loop
if (syncAchieved == 1) //Start sequencing ignition outputs when sync has occured. Otherwise just wait until engine starts rotating.
  {
    if (cylinderCounter == 0) //Fire Cylinder 1
    {PORTB = B00000001;}
    if (cylinderCounter == 1) //Fire Cylinder 3
    {PORTB = B00000100;}
    if (cylinderCounter == 2) //Fire Cylinder 4
    {PORTB = B00001000;}
    if (cylinderCounter == 3) //Fire Cylinder 2
    {PORTB = B00000010;} 
  }
}
void ISR0() //Keep constant track of RISING position pin
{ 
  pos++; //Increment one every pulse (max 8 in practice during large sync window, 360 disc total but variable never stores more than 8)
}
void ISR1() { //Keep constant track of CHANGE/(RISING after sync) state for reference pin
  
  if(digitalRead(CamshaftReferencePin) == HIGH) //This is an "unneeded if". It filters out spark EMI, incorrect pulses will be counted otherwise.
  {
    if (syncAchieved == 1){cylinderCounter++;} //If we have achieved sync, we sequence the next cylinder that should fire
    if (cylinderCounter > 3){cylinderCounter = 0;} //This is a reset for the cylinder firing sequencer. We have 4 cylinders, so we reset after we reached the final one.
  }
  state = !state; //Flip the state bit to indicate rising/falling sitation on reference pin
  if (syncAchieved == 0) //Perform the code below only before sync has been achieved. This saves CPU resources at high engine RPMs
  {
    if (state == 1) //RISING SIGNAL on referene pin
      { 
          attachInterrupt(digitalPinToInterrupt(CamshaftPositionPin), ISR0, RISING); //Before sync is achieved, we enable keeping track of the position pin during rising state. This info is used to calculate if we are inside large window.
      }
    if (state == 0) //FALLING SIGNAL on reference pin
      {  
        detachInterrupt(digitalPinToInterrupt(CamshaftPositionPin)); //We disable keeping track of the position pin info during rising state
      }
      if (pos > 6) //We hit the jackpot boys, we have confirmed that cyl1 will fire next. The >6 makes it so that only big window can sync
        {
         pos = 0; //Reset position counter
         syncAchieved = 1; //Variable used to start ignition and disable unnecessary measuring of position pin
         attachInterrupt(digitalPinToInterrupt(CamshaftReferencePin), ISR1, RISING); //We no longer keep track of change state once synced. Now we locate rising change on the ref pin.
         detachInterrupt(digitalPinToInterrupt(CamshaftPositionPin));} //If sync is achieved, we disable keeping track of the position pin permanently
  } 
      pos = 0; //Reset after each count. This is an important reset before large window has been found
}
