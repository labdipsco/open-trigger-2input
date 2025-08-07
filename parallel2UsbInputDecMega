//command 00 to 15 on Serial and Serial1
//config command: pauto<on/off>wait<on/off>time<0 to 99999999>@
//arduino mega pins: D22 to D29 
//Usb Serial port  set pin D22 to D25; 08 set 1000 1(D25)0(D24)0(D23)0(D22)  
//Serial1 set pin D26 to D29; 06 set 0110 0(D29)1(D28)1(D27)0(D26)
//screen /dev/ttyUSB0 115200


#include <EEPROM.h>


//for shutdown-resistant configuration parameters
struct Settings {
    uint16_t signature;     // validation field
    unsigned long duration;
    bool waitMode = false;
    bool autoMode = false;
};
const uint16_t CONFIG_SIGNATURE = 0xBEEF;
Settings config;

unsigned long StartTimeLed13=0;

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
        DDRA = 0b11111111;   //arduino mega pins: D22 to D29
        PORTA = 0b00000000;  
    #else
        Serial.println("Scheda non riconosciuta");
    #endif

   
    pinMode(13, OUTPUT); //integrated led pin
    digitalWrite(13,LOW);

 
}

void SetParallelPort(String bin2, String bin1) {
  // Unisce i due nibble per formare un byte binario
  String fullBinary = bin2 + bin1;
  
  
  uint8_t porta_out = strtol(fullBinary.c_str(), nullptr, 2);  // Base 2 per il binario
    
  #if defined(__AVR_ATmega2560__)
       PORTA = porta_out;
  #endif

  //arduino LED (optional)
  digitalWrite(13, HIGH);
  StartTimeLed13 = millis();
  
}

bool isFirstChar01(char c) {
    return (c >= '0' && c <= '1') ;
}
bool isSecondChar05(char c) {
    return (c >= '0' && c <= '5') ;

}

String readHexCharFromSerial(HardwareSerial &serialx) {
    if (serialx.available() >= 2) {
        char firstChar = serialx.peek();
        if (firstChar=='p') {return "-1";}
        char c1 = serialx.read();
        if (!(isFirstChar01(c1))) {serialx.read(); return "-1"; }
        char c2 = serialx.read();
        if (c1=='0') {
            if (!isdigit(c2)) {serialx.read(); return "-1"; }
            return String(String(c1) + String(c2));
        }
        if (c1=='1') {
            if (!(isSecondChar05(c2))) {serialx.read(); return "-1"; }
            return String(String(c1) + String(c2));
        }
       
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
        //Serial.println(tIndex);
        if (tIndex >= 0) {
            String tPart = cmd.substring(tIndex + 4);
            config.duration = tPart.toInt();
        }
        EEPROM.put(0, config);


        String autoStatus = config.autoMode ? "ON" : "OFF";
        String waitStatus = config.waitMode ? "ON" : "OFF";
        String message = "Auto mode " + autoStatus + "  Wait mode " + waitStatus + " Time " + config.duration + "ms";
        
        //reset pin after configuration
        trig1="-1";
        trig2="-1";
        bin1 = "0000";
        bin2 = "0000";
        SetParallelPort(bin2,bin1);

        
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




void serialWaitOnModeAutoOnOff() {
     
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
        SetParallelPort(bin2,bin1);
        received1 = false;
        received2 = false;

        if (config.autoMode == true) {
            unsigned long duration = config.duration;
            duration = duration * 1000;
            unsigned long startTime = micros();
            
            if ((bin2!="0000") && (bin1!="0000")) {
                if (duration <= 16383) {
                    delayMicroseconds(duration);
                } else {
                    while (micros() - startTime < duration);
                }
            }

            SetParallelPort("0000","0000");
        }
    }
}

void serialWaitModeOFFAutoModeOff() {
    trig1 = readHexCharFromSerial(Serial);
    trig2 = readHexCharFromSerial(Serial1);

    if (trig1 != "-1") {
        bin1 = convertToBinary4Bit(trig1);
        SetParallelPort(bin2,bin1);
    }
    if (trig2 != "-1") {
        bin2 = convertToBinary4Bit(trig2);
        SetParallelPort(bin2,bin1);
    }
    

    //SetParallelPort(bin2,bin1);
}

void serialWaitModeOFFAutoModeOn() {
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
        SetParallelPort(bin2,bin1);
    }

    if (trig2 != "-1") {
        bin2 = convertToBinary4Bit(trig2);
        if (trig2 != "0") {
            timer2 = true;
            startTime2 = micros();
        }
        SetParallelPort(bin2,bin1);
    }
}

//turn OFF the integrated LED after 200ms if it is ON, so it flashes when data is transmitted
void Led13off() {
     if (StartTimeLed13!=0) {
        if ((millis()-StartTimeLed13)>200)  {  digitalWrite(13, LOW); StartTimeLed13=0; /*Serial.println("13 LOW");*/ }
    }
}


void loop() {
    //turn OFF the integrated LED after 100ms if it is ON, so it flashes when data is transmitted
    Led13off();
    
    if (Serial.available()>0)  {
        liveConfiguration(Serial);
    }
    if (Serial1.available()>0)  {
        liveConfiguration(Serial1);
    }
        
    if (config.waitMode == true) {
            serialWaitOnModeAutoOnOff();
    }

    if ((config.waitMode == false) && (config.autoMode == false)) {
            serialWaitModeOFFAutoModeOff();
    }
    

    if ((config.waitMode == false) && (config.autoMode == true)) {
        serialWaitModeOFFAutoModeOn();
    }
}
