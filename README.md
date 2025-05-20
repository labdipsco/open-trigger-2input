[![Github](https://img.shields.io/badge/Version_1.0-blue)](https://github.com/labdipsco/TriggerBox)
[![Github](https://img.shields.io/badge/GNU_General_Public_License-orange)](https://github.com/labdipsco/TriggerBox)

#Open Trigger 

Trigger Interface for Psychological Experiment and Stimulus. It works with major Stimulus Presentation Software. It receives triggers from usb and sends to parallel port. 

Features:
 - Usb connection to Windows and Linux PC.
 - Auto trigger function, set a trigger timeout.
 - Trigger Elaboration < 1ms

Installation:

Upload parallel.ino sketch to Arduino uno R3 or R4 minima.

Hardware:
- Arduino Uno R4 minima or Uno R3 https://www.arduino.cc/
- Parallel port cable

<img src="https://github.com/user-attachments/assets/7cb8b0bf-258f-4f8e-a3a4-b13eee1292fd" width="300">

Arduino to Parallel port Connections:
- arduino pin D8   --> pin D0 parallel port
- arduino pin D9   --> pin D1 parallel port
- ...
- arduino pin D13  --> pin D5 parallel port
- arduino pin AO   --> pin D6 parallel port
- arduino pin A1   --> pin D7 parallel port

Alvailable Commands:
- send trigger code 0-255:  S<trigger code>E    es: 12E
- auto trigger: <trigger codeT<time milliseconds>E>  es: 12T2000E   send code 12 for 2 seconds and set all pin to 0

Python example:

Eprime example:

Test:
- tested with eprime3, opensesame, psychopy

Try it on tinkercad:
- https://www.tinkercad.com/  send command via tinkercard console


![image](https://github.com/user-attachments/assets/f9195bcd-48f6-46df-9165-b114d715a06b)

  







