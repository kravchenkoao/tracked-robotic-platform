#include <AlfredoCRSF.h>
#include <SoftwareSerial.h>

#define PIN_RX 2
#define PIN_TX 3
#define CRSF_BAUDRATE 115200

// Set up a new Serial object
SoftwareSerial crsfSerial(PIN_RX, PIN_TX);
AlfredoCRSF crsf;

int joy1PosX, joy1PosY, joy2PosX, joy2PosY = 0;

int motorAA = 4;
int motorAB = 12;
int motorBA = 7;
int motorBB = 8;
int enA = 5;
int enB = 6;
int speed = 0;

int count = 0;

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("COM Serial initialized");
  crsfSerial.begin(CRSF_BAUDRATE);
  if (!crsfSerial) while (1) Serial.println("Invalid crsfSerial configuration");

  crsf.begin(crsfSerial);

  pinMode(motorAA, OUTPUT);
  pinMode(motorAB, OUTPUT);
  pinMode(motorBA, OUTPUT);
  pinMode(motorBB, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  digitalWrite(enA, HIGH);
  digitalWrite(enB, HIGH);

  joy1PosX = 0;
  joy1PosY = 0;
  receivedFlag = true;
}

void loop() {
  // put your main code here, to run repeatedly:
  crsf.update();
  Serial.println(crsf.isLinkUp());
  if(crsf.isLinkUp()) {
    // reset flag
    // receivedFlag = false;

    Serial.print(crsf.getChannel(3));
    Serial.print(", ");
    Serial.print(crsf.getChannel(4));
    Serial.print(", ");

    joy1PosY = map(crsf.getChannel(3), 980, 2020, -100, 100);
    joy1PosX = map(crsf.getChannel(4), 980, 2020, -100, 100);

    joy1PosY = constrain(joy1PosY, -100, 100);
    joy1PosX = constrain(joy1PosX, -100, 100);

    Serial.print(joy1PosY);
    Serial.print(", ");
    Serial.print(joy1PosX);
    Serial.println();

    if (joy1PosY >= joy1PosX && joy1PosY >= -joy1PosX) {
      Serial.println("- Forward -");
      digitalWrite(motorAA, HIGH);
      digitalWrite(motorAB, LOW);
      digitalWrite(motorBA, HIGH);
      digitalWrite(motorBB, LOW);
    } else if (joy1PosY < joy1PosX && joy1PosY > -joy1PosX) {
      Serial.println("- Right -");
      digitalWrite(motorAA, HIGH);
      digitalWrite(motorAB, LOW);
      digitalWrite(motorBA, LOW);
      digitalWrite(motorBB, HIGH);
    } else if (joy1PosY <= joy1PosX && joy1PosY <= -joy1PosX) {
      Serial.println("- Backward -");
      digitalWrite(motorAA, LOW);
      digitalWrite(motorAB, HIGH);
      digitalWrite(motorBA, LOW);
      digitalWrite(motorBB, HIGH);
    } else if (joy1PosY > joy1PosX && joy1PosY < -joy1PosX) {
      Serial.println("- Left -");
      digitalWrite(motorAA, LOW);
      digitalWrite(motorAB, HIGH);
      digitalWrite(motorBA, HIGH);
      digitalWrite(motorBB, LOW);
    }

    // speed = (int) (sqrt( ( pow(joy1PosX,2)+pow(joy1PosY,2) )/20000 ) * 255);
    // Serial.println(speed);
    // speed = constrain(speed, 0, 255);

    speed = 255;

    analogWrite(enA, speed);
    analogWrite(enB, speed);

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
