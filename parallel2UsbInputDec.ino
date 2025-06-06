//command 0 to 15on Serial and Serial1
//config command: pauto<on/off>wait<on/off>time<0 to 99999999>@
//arduino - parallel port pins: 37-D0, 36-D1, ...,30-D7
//serial - arduino pins: 
//Serial set pin 34,35,36,37 ; send 8 via Serial set 1000 on pin 34,..,37
//Serial1 set pin 30,31,32,33 ; send 6 via Serial set 0110 on pin 30,..,33 

#include <EEPROM.h>

struct Settings {
    uint16_t signature;     // validation field
    unsigned long duration;
    bool waitMode = false;
    bool autoMode = false;
};

const uint16_t CONFIG_SIGNATURE = 0xBEEF;
Settings config;


void setup() {
    Serial.begin(115200);
    Serial.setTimeout(7200000); // Timeout di 2h
    Serial1.begin(115200);
    Serial1.setTimeout(7200000); // Timeout di 2h

    EEPROM.get(0, config);
    if (config.signature != CONFIG_SIGNATURE) {
        config.signature = CONFIG_SIGNATURE;
        config.duration = 1000;
        config.waitMode = false;
        config.autoMode = false;
        EEPROM.put(0, config);
    }

    // Configurazione per Arduino Mega 2560
    #if defined(__AVR_ATmega2560__)
        DDRA = 0b11111111; 
        PORTA = 0b00000000;  
    #else
        Serial.println("Scheda non riconosciuta");
    #endif
}

void SetParallelPort(String pvalue) {
    uint8_t porta_out = strtol(pvalue.c_str(), nullptr, 2);  // Base 2 per il binario
    
    #if defined(__AVR_ATmega2560__)
        PORTA = porta_out;
    #endif
}


bool isHexChar1(char c) {
    return (c >= '0' && c <= '1') ;
}

bool isHexChar2(char c) {
    return (c >= '0' && c <= '5') ;
}

String readHexCharFromSerial(HardwareSerial &serialx) {
    if (serialx.available() >= 2) {
        char firstChar = serialx.peek();
        if (firstChar=='p') {return "-1";}
        char c1 = serialx.read();
        if (!(isHexChar1(c1))) {serialx.read(); return "-1"; }
        char c2 = serialx.read();
        if (!(isHexChar2(c2))) {serialx.read(); return "-1"; }
        return String(String(c1) + String(c2));
        
    }
    return "-1";
}

String convertToBinary4Bit(String decvalue) {
  int number = decvalue.toInt();               // Convert the string to an integer
  String binary = String(number, BIN);     // Convert the number to a binary string

  // Pad with leading zeros if needed
  while (binary.length() < 4) {
    binary = "0" + binary;
  }

  return binary;  // Return the 4-bit binary string
}

String NibbleToBinaryString(String hexChar) {
    int value = (int) strtol(hexChar.c_str(), NULL, 16);
    String binStr = "";
    for (int i = 3; i >= 0; i--) {
        binStr += bitRead(value, i);
    }
    return binStr;
}

unsigned long uno;
unsigned long due;

String trig1 = "-1";
String trig2 = "-1";
String bin1 = "0000";
String bin2 = "0000";

unsigned long startTime1 = 0;
unsigned long startTime2 = 0;
bool timer1 = false;
bool timer2 = false;
bool received1 = false;
bool received2 = false;

void liveConfiguration(HardwareSerial &serialx) {
    char firstChar = serialx.peek();
    firstChar = tolower(firstChar);

    if (firstChar == 'p') {
        String cmd = serialx.readStringUntil('@');
        cmd.toLowerCase();
        serialx.println(cmd);

        if (cmd.indexOf("autoon") >= 0) {
            config.autoMode = true;
        } else if (cmd.indexOf("autooff") >= 0) {
            config.autoMode = false;
        }

        if (cmd.indexOf("waiton") >= 0) {
            config.waitMode = true;
        } else if (cmd.indexOf("waitoff") >= 0) {
            config.waitMode = false;
        }

        int tIndex = cmd.indexOf("time");
        Serial.println(tIndex);
        if (tIndex >= 0) {
            String tPart = cmd.substring(tIndex + 4);
            config.duration = tPart.toInt();
        }

        serialx.println("---- Comando ricevuto ----");
        serialx.print("autoMode: ");
        serialx.println(config.autoMode ? "ON" : "OFF");
        serialx.print("waitMode: ");
        serialx.println(config.waitMode ? "ON" : "OFF");
        serialx.print("duration: ");
        serialx.println(config.duration);
        serialx.println("--------------------------");
    }
}

void serialWaitModeAutoOnOff() {
     
    trig1 = readHexCharFromSerial(Serial);
    if ((trig1 != "-1") && (received1 == false)) {
        bin1 = convertToBinary4Bit(trig1);
        received1 = true;
    }

    trig2 = readHexCharFromSerial(Serial1);
    if ((trig2 != "-1") && (received2 == false)) {
        bin2 = convertToBinary4Bit(trig2);
        received2 = true;
    }

    if ((received1 == true) && (received2 == true)) {
        String concatenatedBinary = bin1 + bin2;
        SetParallelPort(concatenatedBinary);
        received1 = false;
        received2 = false;

        if (config.autoMode == true) {
            unsigned long duration = config.duration;
            duration = duration * 1000;
            unsigned long startTime = micros();

            if (duration <= 16383) {
                delayMicroseconds(duration);
            } else {
                while (micros() - startTime < duration);
            }

            SetParallelPort("00000000");
        }
    }
}

void serialNoWaitModeAutoModeOff() {
    trig1 = readHexCharFromSerial(Serial);
    trig2 = readHexCharFromSerial(Serial1);

    if (trig1 != "-1") bin1 = convertToBinary4Bit(trig1);
    if (trig2 != "-1") bin2 = convertToBinary4Bit(trig2);

    SetParallelPort(bin2 + bin1);
}

void serialNoWaitModeAutoModeOn() {
    trig1 = readHexCharFromSerial(Serial);
    unsigned long duration = config.duration;
    duration = duration * 1000;

    if ((timer1 == true) && (trig1 == "-1") && (micros() - startTime1 >= duration)) {
        trig1 = "0";
        timer1 = false;
    }

    trig2 = readHexCharFromSerial(Serial1);
    if ((timer2 == true) && (trig2 == "-1") && (micros() - startTime2 >= duration)) {
        trig2 = "0";
        timer2 = false;
    }

    if (trig1 != "-1") {
        bin1 = convertToBinary4Bit(trig1);
        Serial.println(trig1);
        if (trig1 != "0") {
            timer1 = true;
            startTime1 = micros();
        }
        SetParallelPort(bin1 + bin2);
    }

    if (trig2 != "-1") {
        bin2 = convertToBinary4Bit(trig2);
        if (trig2 != "0") {
            timer2 = true;
            startTime2 = micros();
        }
        SetParallelPort(bin1 + bin2);
    }
}

void loop() {
   //pin 29 28 .... 23 22
  //  PORTA=0b00110011;
//return;
    if (Serial.available()>0)  {
        liveConfiguration(Serial);
    }
    if (Serial1.available()>0)  {
        liveConfiguration(Serial1);
    }
        
    if (config.waitMode == true) {
            serialWaitModeAutoOnOff();
    }

    if ((config.waitMode == false) && (config.autoMode == false)) {
            serialNoWaitModeAutoModeOff();
    }
    

    if ((config.waitMode == false) && (config.autoMode == true)) {
        serialNoWaitModeAutoModeOn();
    }
}
