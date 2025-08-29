[![Github](https://img.shields.io/badge/Version_1.0-blue)](https://github.com/labdipsco/open-trigger-2input)
[![Github](https://img.shields.io/badge/GNU_General_Public_License-orange)](https://github.com/labdipsco/open-trigger-2input)

#Open Trigger 2 Serial Inputs 

[go to github repository](https://github.com/labdipsco/open-trigger-2input/)

Trigger Interface for Experiment and Stimulus.  It can receive two triggers from two usb ports, combine and sends to parallel port. 

Features:
 - Two Usb connection to send trigger simultaneously from two pc.
 - Auto trigger function, send a trigger and set the parallel port to 0 after a delay
 - Sync mode on/off, in on mode, wait that both triggers are received before send to the parallel port. In off mode every trigger are sent to the parallel immediately and independently.
 - Trigger Elaboration < 1ms

## How it works

## System Overview: Dual Serial Text-Based Trigger Input with Parallel Output Encoding

The Arduino microcontroller listens on two USB serial interfaces: `Serial` (the arduino usb connection) and `Serial2` (via  module FT232RL,  USB-C to TTL ).  
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

Upload parallel2UsbInputDecMega.ino sketch to Arduino Mega.

Hardware:
- Arduino Mega 2560 R3
- USB to TTL serial adapter (FT232-AZ) connected to arduino pin 16 (tx) and (17) rx
- Parallel port cable connected to arduino pins D22 to D29

<img src="https://github.com/user-attachments/assets/7cb8b0bf-258f-4f8e-a3a4-b13eee1292fd" width="300">

<img width="600"  alt="ard1" src="https://github.com/user-attachments/assets/359c86e6-f563-4f6f-88e3-5805c24531b5" />

Arduino Mega pins:
 - pins used D22 to D29; Usb Serial port  set pin D22 to D25, Serial1 set pin D26 to D29
 - example:
   - send 08 via Usb Serial Port set 1000 on pins: 1(D25)0(D24)0(D23)0(D22)
   - send 06 via Serial1 Port set 0110 on pins: 0(D29)1(D28)1(D27)0(D26)
   

Alvailable Commands:
- send trigger code 00-15 to serial or/and serial2, the code must be in two digits (for 2 send 02). It's not necessary to send carriage return or other control character.
- show current configuration, send command: p@
- change configuration: p<auto[on|off]wait[on|off]time[ms]>@  example: pautoon@ , pautooff@, ptime1000@, pautoonwaitofftime120@,...

How to send command for testing on linux:
- Connect to the arduino serial2 with screen command: screen /dev/ttyUSB0 115200 (with dmesg command check the device name)
- Connect to the arduino serial via the arduino IDE Serial Monitor, Tools --> Serial Monitor


Arduino One Wifi R4 version:
- There is a sketch parallel2UsbInputDecR4Wifi.ino for arduino One Wifi R4 hardware. It's works as the Mega version but pinout are different. Note also that in R4 version the pin connected to the parallel port are setted sequentially, it takes 10 tp 15 microseconds to set all the pins.

## 📢 Disclaimer

This project is provided “as is” without warranty of any kind. By using or building this system, you acknowledge and agree that you do so at your own risk. The creators of this project are not responsible for any injury, damage, or legal issues that may arise from improper use, installation, or handling of high-voltage equipment. Always follow local safety regulations and consult a professional if unsure.









