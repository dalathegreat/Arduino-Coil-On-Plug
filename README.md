# Arduino-Coil-On-Plug
Source code for Arduino Uno app, used for converting distributor based Nissan engines to more modern coil on plug ignition. SR20-engine is used in master branch, but many other distributor based Nissan engines can be converted.

## Functional description:
This application samples in two distributor signals, 'Camshaft Reference' & 'Camshaft Position', hereby refered to as 'CR' & 'CP'. It then syncs the firing sequence to Cylinder 1, and ignores CP information after sync has been achieved. It then alternates 4 digital outputs depending on which cylinder should fire. The digital outputs enter a separate logic AND component with the ECU Ignition signal, and the resulting signal is finally led to the correct coil on plug.

## Why coil on plug?
Coil on plug (COP) is a more modern way of starting the combustion process. This guide will not go into a full explanation of what COP is, so lets just list a few advantages of using COP versus classic distributor systems
1. Stronger spark
2. Longer intervals between needing replacement parts
3. Slightly lower emissions when in wasted spark mode
4. Less cluttered enginebay

## Difficulty level 
Depending on your skill level, the hardest part of converting to COP will be the mechanical side. The wiring is not for beginners, but it's easy to follow the premade schematics.

## Components needed
If you go with smart coils you will need:

1. Smart coils (3-4pin) of your choice 
2. Suitable connectors and wires

If you go with dumb coils you will need:

1. Dumb coils (2pin) of your choice 
2. Coil driver
3. Suitable connectors and wires

Arduino prerequisites:
1. This application is written for an Arduino Uno type board. If needed it can be modified to work with an Nano/Micro/Mini style board, feel free to rewrite it :)
2. This application in its original form needs to be paired with an 1991->1994? SR20DE distributor. These can be identified by seeing how many electrical pins they have (4). They are also connected to a big external coil which is easy to spot. See the engine compability section for how to modify the code for other Nissan engine types.
3. To output a proper ignition signal, one additional component is needed; TC4468EPD. The schematic can be found in the docs folder.
4. To cope with quick starting, the arduino must be flashed with a programmer. This reduces the bootup time from 5seconds down to a few milliseconds. You can use another arduino to flash it, or a dedicated avrisp programmer. The code can be tested/debugged/rewritten freely without this step, but for the final permanent installation this is a very important step.

## Engine compability
### Make it sync with your engine
If you want to convert another type of Nissan engine you first need to modify the number of pulses used for sync. Here is a great read which explains some of the different trigger discs found in various Nissans: http://datsun1200.com/modules/mediawiki/index.php?title=Nissan_Optical_CAS This application can easily be modified to work with the following Nissan engines:

1. SR needs 6 pulses https://www.youtube.com/watch?v=OISRDuSyMRY 
  -SR20DE Distributor: 6 pulses
  -SR20DE Distributor(w coil): x pulses
  -SR20VE Distributor: x pulses
  -SR20 CAS: x pulses
2. KA needs ? pulses
3. CA needs ? pulses
4. CG needs ? pulses
5. FJ needs ? pulses

To use the code for your engine type, verify how many small CP pulses occur during the large CR window. The code row that needs to be modified is the one inside the ISR1: "if (pos > 6)" Change this value to the correct amount of pulses for your engine.

### Firing order for your engine
The firing order is defined at the beginning of the code, inside a global variable. The default firing order is (1-3-4-2). If needed this can be modified to suit the engine you are using.

**Schematics:**

Electrical drawings for SR-style engine can be found as .png files in the repository.
