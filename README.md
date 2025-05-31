[![Github](https://img.shields.io/badge/Version_1.0-blue)](https://github.com/labdipsco/open-trigger-2input)
[![Github](https://img.shields.io/badge/GNU_General_Public_License-orange)](https://github.com/labdipsco/open-trigger-2input)

#Open Trigger 

Trigger Interface for Psychological Experiment and Stimulus. It works with major Stimulus Presentation Software. It can receive two triggers from two usb ports and sends to parallel port. 

Features:
 - Two Usb connection to send trigger simultaneously fromt two pc.
 - Auto trigger function, send a trigger and set the parallel port to  0 after a delay
 - Sync mode on/off, in on mode, wait that both the trigger are received before send to the parallel. In off mode every trigger are sent to the parallel immediately
 - Trigger Elaboration < 1ms

## How it works

Arduino listens on both USB serial ports for the arrival of a trigger.
The trigger can have a value from 00 to 15.
The triggers received from Serial1 and Serial2 are each converted into 4-bit binary numbers and then combined into a single 8-bit binary number, which is written to the parallel port.
For example, if Serial1 receives 5 (0101) and Serial2 receives 12 (1100), the parallel port will be set to: 11000101.

Installation:

Upload parallel2Inputs.ino sketch to Arduino Mega

Hardware:
- Arduino Mega https://www.arduino.cc/
- USB to TTL serial adapter (FT232-AZ)
- Parallel port cable

<img src="https://github.com/user-attachments/assets/7cb8b0bf-258f-4f8e-a3a4-b13eee1292fd" width="300">

Arduino to Parallel port Connections:
 - arduino pin 22 --> pin D0 parallel port
 - arduino pin 23 --> pin D1 parallel port
   ...
 - arduino pin 27 --> pin D5 parallel port
 - arduino pin 28 --> pin D6 parallel port
 - arduino pin 29 --> pin D7 parallel port

Alvailable Commands:
- send trigger code 0-255:  S<trigger code>E    es: 12E
- auto trigger: <trigger codeT<time milliseconds>E>  es: 12T2000E   send code 12 for 2 seconds and set all pin to 0

Python example:

Eprime example:

Test:
- tested with eprime3, opensesame, psychopy

Try it on SimulIDE:
- https://simulide.com/p/  send command via SimulIDE console


![image](https://github.com/user-attachments/assets/f9195bcd-48f6-46df-9165-b114d715a06b)

  







