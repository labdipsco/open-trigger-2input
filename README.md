[![Github](https://img.shields.io/badge/Version_1.0-blue)](https://github.com/labdipsco/open-trigger-2input)
[![Github](https://img.shields.io/badge/GNU_General_Public_License-orange)](https://github.com/labdipsco/open-trigger-2input)

#Open Trigger 

Trigger Interface for Experiment and Stimulus. It works with major Stimulus Presentation Software. It can receive two triggers from two usb ports and sends to parallel port. 

Features:
 - Two Usb connection to send trigger simultaneously from two pc.
 - Auto trigger function, send a trigger and set the parallel port to  0 after a delay
 - Sync mode on/off, in on mode, wait that both triggers are received before send to the parallel port. In off mode every trigger are sent to the parallel immediately and independently.
 - Trigger Elaboration < 1ms

## How it works

## System Overview: Dual Serial Text-Based Trigger Input with Parallel Output Encoding

The Arduino microcontroller listens on two USB serial interfaces: `Serial` (the arduino usb connection) and `Serial1` (via  module FT232RL,  USB-C to TTL ).  
Each interface receives a **2-character ASCII-encoded decimal string** representing a trigger value in the range `"00"` to `"15"`, then converted to a 4-bit binary value. (0000b to 1111b).  

The final 8-bit output is composed as follows:

- **Upper nibble** (`bits 7–4`) = value from `Serial1`  
- **Lower nibble** (`bits 3–0`) = value from `Serial`  

The composed 8-bit value is written to an 8-bit digital parallel output port.

---

## 🔁 Synchronization Modes

### 🔹 Sync OFF
- A value received on **either** serial port is immediately written to the output.
- The other nibble retains its **last known** value.
- Enables low-latency partial updates.

### 🔸 Sync ON
- The system buffers inputs until **both** `Serial1` and `Serial2` have received new values.
- The combined byte is written only when both halves are ready.
- Ensures full synchronization between inputs.

---

## ⏱️ Auto Mode (autoOn / autoOff)

The system supports an **auto-reset** mechanism based on a timeout (in milliseconds), configurable through an `AutoMode` flag.

### 🟢 Auto ON
- After the output value is written, a countdown begins.
- Once the timeout expires:
  - **Sync ON mode**: the **entire** parallel port is cleared to `0x00`.
  - **Sync OFF mode**: **only the nibble** corresponding to the triggering serial port is reset to `0b0000`.
    - The other nibble remains unchanged.

### 🔴 Auto OFF
- No automatic reset is performed.
- The output value remains until new data is received.

This feature allows generation of **timed output pulses** on the parallel port for external device control or triggering logic.

---



Installation:

Upload parallel2UsbInputDecMega.ino sketch to Arduino Mega or parallel2UsbInputR4wifi.ini to Arduino Uno R4 wifi.

Hardware:
- Arduino Mega or Arduino Uno Wifi R4
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
- send trigger code 0-15, the code must be in two digits, for 2 send 02.     es: 12 01 02 ....
- auto trigger: <trigger codeT<time milliseconds>E>  es: 12T2000E   send code 12 for 2 seconds and set all pin to 0


Try it on SimulIDE:
- https://simulide.com/p/  send command via SimulIDE console









