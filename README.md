# Arduino-Coil-On-Plug
Source code for Arduino Uno app, used for converting distributor based Nissan engines to more modern coil on plug ignition. SR-engine is used in master branch.

Functional description:

This application samples in two distributor signals, 'Camshaft Reference' & 'Camshaft Position', hereby refered to as 'CR' & 'CP'. It then syncs the firing sequence to Cylinder 1, and ignores CP information after sync has been achieved. It then alternates 4 digital outputs depending on which cylinder should fire.

Hardware prerequisites:
1. This application is written for an Arduino Uno type board. If needed it can be modified to work with an Nano/Micro/Mini style board, feel free to rewrite it :)
2. This application in its original form needs to be paired with an 1991->1994? SR20DE distributor. These can be identified by seeing how many electrical pins they have (4). They are also connected to a big external coil which is easy to spot.
3. To output a proper ignition signal, one additional component is needed; TC4468EPD. The schematic can be found in the docs folder.

Engine compability:

If you want to convert another type of Nissan engine, modify the number of pulses used for sync. Here is a great read which explains some of the different trigger discs found in various Nissans: http://datsun1200.com/modules/mediawiki/index.php?title=Nissan_Optical_CAS This application can easily be modified to work with the following Nissan engines:

1. SR20 TESTED! https://www.youtube.com/watch?v=OISRDuSyMRY 
2. KA24
3. CA18
4. CG10/13
5. FJ20

Schematics:

Electrical drawings for SR-style engine can be found as .png files in the repository.
