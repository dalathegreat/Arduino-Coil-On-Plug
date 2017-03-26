# Arduino-Coil-On-Plug
Source code for Arduino Uno application, used for converting Nissan 'SR' engines to coil on plug.

Functional description:
This application samples in two distributor signals, 'Camshaft Reference' & 'Camshaft Position', hereby refered to as 'CR' & 'CP'. It then syncs the firing sequence to Cylinder 1, and ignores CP information after sync has been achieved. It then alternates 4 digital outputs depending on which cylinder should fire. 

Hardware prerequisites:
1. This application in its original form needs to be paired with an 1991->1994? SR20DE distributor. These can be identified by seeing how many electrical pins they have (4). They are also connected to a big external coil. If you want to use a different distributor, modify the number of teeth. This row 'if (pos > 6)' is used for SR20DE.
2. This application is written for an Arduino Uno type board. If needed it can be modified to work with an Nano/Micro/Mini style board, feel free to rewrite it :)

Electrical drawings:
Electrical drawings can be found as .png files in the repository.
