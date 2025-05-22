
//command 0 to F on Serial and Serial1
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

// Maschera per i pin D30–D37 (PORTC)
const uint8_t maskC = 0b11111111;  // D30–D37 (PC0–PC7)

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
        DDRC = 0b11111111; 
        PORTC = 0b00000000;  
    #else
        Serial.println("Scheda non riconosciuta");
    #endif
}

void SetParallelPort(String pvalue) {
    uint8_t portc_out = strtol(pvalue.c_str(), nullptr, 2);  // Base 2 per il binario
    #if defined(__AVR_ATmega2560__)
        PORTC = portc_out;
    #endif
}

bool isHexChar(char c) {
    return (c >= '0' && c <= '9') || 
           (c >= 'A' && c <= 'F') || 
           (c >= 'a' && c <= 'f');
}

String readHexCharFromSerial(HardwareSerial &serialx) {
    if (serialx.available() > 0) {
        char firstChar = serialx.peek();
        if (isHexChar(firstChar) && serialx.available() == 1) {
            char hexChar = serialx.read();
            return String(hexChar);
        }
    }
    return "-1";
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

void liveConfiguration() {
    char firstChar = Serial.peek();
    firstChar = tolower(firstChar);

    if (firstChar == 'p') {
        String cmd = Serial.readStringUntil('@');
        cmd.toLowerCase();
        Serial.println(cmd);

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

        Serial.println("---- Comando ricevuto ----");
        Serial.print("autoMode: ");
        Serial.println(config.autoMode ? "ON" : "OFF");
        Serial.print("waitMode: ");
        Serial.println(config.waitMode ? "ON" : "OFF");
        Serial.print("duration: ");
        Serial.println(config.duration);
        Serial.println("--------------------------");
    }
}

void serialWaitModeAutoOnOff() {
     
    trig1 = readHexCharFromSerial(Serial);
    if ((trig1 != "-1") && (received1 == false)) {
        bin1 = NibbleToBinaryString(trig1);
        received1 = true;
    }

    trig2 = readHexCharFromSerial(Serial1);
    if ((trig2 != "-1") && (received2 == false)) {
        bin2 = NibbleToBinaryString(trig2);
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

    if (trig1 != "-1") bin1 = NibbleToBinaryString(trig1);
    if (trig2 != "-1") bin2 = NibbleToBinaryString(trig2);

    SetParallelPort(bin1 + bin2);
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
        bin1 = NibbleToBinaryString(trig1);
        Serial.println(trig1);
        if (trig1 != "0") {
            timer1 = true;
            startTime1 = micros();
        }
        SetParallelPort(bin1 + bin2);
    }

    if (trig2 != "-1") {
        bin2 = NibbleToBinaryString(trig2);
        if (trig2 != "0") {
            timer2 = true;
            startTime2 = micros();
        }
        SetParallelPort(bin1 + bin2);
    }
}

void loop() {
    if (Serial.available()>0)  {
        liveConfiguration();
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
