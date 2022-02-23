#include <Arduino.h>
#include <RF24.h>
#include <printf.h>
#include "MP1.h"
#include "Servo.h"

RF24 radio(RF24_CE, RF24_CSN);

byte name[] = "TNode";
bool tx, fail, rx = false;
Servo left;
Servo right;

#define min 800u
#define max 2200u

void nrfInterrupt() { radio.whatHappened(tx, fail, rx); }

void setup() {
  Serial.begin(115200);
  uint8_t radioSetup = radio.begin();
  left.attach(5);
  right.attach(3);
  // radio.setAutoAck(false);
  // Serial.print("Radio setup: ");
  // Serial.println(radioSetup);
  // radio.openWritingPipe(RF24_TX_ID);
  radio.setPALevel(RF24_PA_MIN);
  radio.openReadingPipe(1, name);
  // radio.maskIRQ(0, 0, 1);
  // attachInterrupt(digitalPinToInterrupt(2), nrfInterrupt, LOW);
  radio.startListening();
  // printf_begin();
  // radio.printDetails();
}

uint32_t prevTime = millis();
uint8_t pott = 0;
uint16_t gas = 0;
void loop() {

  if (radio.available()) {
    struct {
      int16_t j1;
      int16_t pot;
      uint32_t time;
    } data = {};
    // Variable for the received timestamp
    while (radio.available()) {        // While there is data ready
      radio.read(&data, sizeof(data)); // Get the payload
    }

    // radio.stopListening();                               // First, stop
    // listening so we can talk myData.value += 0.01; // Increment the float
    // value radio.write( &myData, sizeof(myData) );              // Send the
    // final one back. radio.startListening();                              //
    // Now, resume listening so we catch the next packets.
    Serial.print(F("Receive: "));
    Serial.print(data.j1);
    Serial.print(F(" : "));
    Serial.println(data.pot);
    Serial.println(data.time - prevTime);
    prevTime = data.time;
    rx = false;
    
    if(data.pot != pott) {
      gas = map(data.pot, 0, 255, min, max);
      left.writeMicroseconds(gas);
      right.writeMicroseconds(gas);
      pott = data.pot;
    }
    
  }
}