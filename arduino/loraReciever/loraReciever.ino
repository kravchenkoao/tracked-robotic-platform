/*
  RadioLib SX127x Receive with Interrupts Example

  This example listens for LoRa transmissions and tries to
  receive them. Once a packet is received, an interrupt is
  triggered. To successfully receive data, the following
  settings have to be the same on both transmitter
  and receiver:
  - carrier frequency
  - bandwidth
  - spreading factor
  - coding rate
  - sync word

  Other modules from SX127x/RFM9x family can also be used.

  For default module settings, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/Default-configuration#sx127xrfm9x---lora-modem

  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>

// RFM95 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
RFM95 radio = new Module(10, 2, 9, 3);

// or detect the pinout automatically using RadioBoards
// https://github.com/radiolib-org/RadioBoards

int joy1PosX, joy1PosY, joy2PosX, joy2PosY = 0;

int motorAA = 12;
int motorAB = 4;
int motorBA = 8;
int motorBB = 7;
int enA = 5;
int enB = 6;
int speed = 0;

int count = 0;

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!

void setFlag(void) {
  // we got a packet, set the flag
  receivedFlag = true;
}

void printStatus(int state, String str);

void setup() {
  // debug
  Serial.begin(9600);

  // setting up hardware
  pinMode(motorAA, OUTPUT);
  pinMode(motorAB, OUTPUT);
  pinMode(motorBA, OUTPUT);
  pinMode(motorBB, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  digitalWrite(enA, HIGH);
  digitalWrite(enB, HIGH);

  // initialize RFM95 with default settings
  Serial.print(F("[RFM95] Initializing ... "));
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }

  // set the function that will be called
  // when new packet is received
  radio.setPacketReceivedAction(setFlag);

  // start listening for LoRa packets
  Serial.print(F("[RFM95] Starting to listen ... "));
  state = radio.startReceive();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }
}

void loop() {
  // check if the flag is set
  if(receivedFlag) {
    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    String str;
    int state = radio.readData(str);
    String word = "";
    int spaceCount = 0;
    for (int i = 0; i < str.length(); i++) {
      if (str.charAt(i) == ' ' or i == str.length() - 1) {
        spaceCount++;
        switch(spaceCount) {
          case 1:
            joy1PosX = word.toInt();
            break;
          case 2:
            joy1PosY = word.toInt();
            break;
          case 3:
            joy2PosX = word.toInt();
            break;
          case 4:
            joy2PosY = word.toInt();
            break;
        }
        word = "";
      } else {
        word.concat(str.charAt(i));
      }
    }
    // Serial.println(joy1PosX);
    // Serial.println(joy1PosY);
    if (joy1PosY >= joy1PosX && joy1PosY >= -joy1PosX) {
      Serial.println("- Forward -");
      digitalWrite(motorAA, HIGH);
      digitalWrite(motorAB, LOW);
      digitalWrite(motorBA, HIGH);
      digitalWrite(motorBB, LOW);
    } else if (joy1PosY < joy1PosX && joy1PosY > -joy1PosX) {
      Serial.println("- Right -");
      digitalWrite(motorAA, LOW);
      digitalWrite(motorAB, HIGH);
      digitalWrite(motorBA, HIGH);
      digitalWrite(motorBB, LOW);
    } else if (joy1PosY <= joy1PosX && joy1PosY <= -joy1PosX) {
      Serial.println("- Backward -");
      digitalWrite(motorAA, LOW);
      digitalWrite(motorAB, HIGH);
      digitalWrite(motorBA, LOW);
      digitalWrite(motorBB, HIGH);
    } else if (joy1PosY > joy1PosX && joy1PosY < -joy1PosX) {
      Serial.println("- Left -");
      digitalWrite(motorAA, HIGH);
      digitalWrite(motorAB, LOW);
      digitalWrite(motorBA, LOW);
      digitalWrite(motorBB, HIGH);
    }

    speed = (int) (sqrt(( pow(joy1PosX,2)+pow(joy1PosY,2) )/200) * 255);

    if (speed > 255) speed = 255;
    analogWrite(enA, speed);
    analogWrite(enB, speed);

    printStatus(state, str);
    count = 0;
  }
  else {
    count++;
    if (count >= 10) {
      count = 0;
      digitalWrite(motorAA, LOW);
      digitalWrite(motorAB, LOW);
      digitalWrite(motorBA, LOW);
      digitalWrite(motorBB, LOW);
    }
  }
  delay(100);
}

void printStatus(int state, String str) {
  if (state == RADIOLIB_ERR_NONE) {
      // packet was successfully received
    Serial.println(F("[SX1278] Received packet!"));

    // print data of the packet
    Serial.print(F("[SX1278] Data:\t\t"));
    Serial.println(str);

    // print RSSI (Received Signal Strength)
    Serial.print(F("[SX1278] RSSI:\t\t"));
    Serial.print(radio.getRSSI());
    Serial.println(F("dBm"));

    // print SNR (Signal-to-Noise Ratio)
    // Serial.print(F("[SX1278] SNR:\t\t"));
    // Serial.print(radio.getSNR());
    // Serial.println(F("dB"));

    // print frequency error
    // Serial.print(F("[SX1278] Frequency err:\t"));
    // Serial.print(radio.getFrequencyError());
    // Serial.println(F("Hz"));

  } else if (state == RADIOLIB_ERR_CRC_MISMATCH) {
    // packet was received, but is malformed
    Serial.println(F("[SX1278] CRC error!"));

  } else {
    // some other error occurred
    Serial.print(F("[SX1278] Failed, code "));
    Serial.println(state);

  }
}