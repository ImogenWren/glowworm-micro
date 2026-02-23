/*  Testing a battery switch

Such that ->

- When device is off, switch powers MCU via closing circuit to ground via MOSFET. 
- Switch (D23) is polled on startup for at least 2 seconds
- If switch remains high -> (D35) powers and keeps MOSFET closed

- When device is powered, holding switch closed for at least 2 seconds resets MOSFET
- so when switch is released, MCU powers off (unless plugged in via USB)

-> This is all working now. Schematic V2 will reflect changes required to make this work

*/

#include <ledObject.h>
#include <buttonObject.h>

const int BUTTON_PIN = 23;
const int MOSFET_PIN = 32;

const int GREEN_LED_PIN = 5;
const int RED_LED_PIN = 19;

ledObject ledGRN(GREEN_LED_PIN);
ledObject ledRED(RED_LED_PIN);
buttonObject button(BUTTON_PIN, BUTTON_PULL_HIGH);


void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("GLOWWORM -> Battery Switch Test");
  // set up IO
  pinMode(BUTTON_PIN, INPUT);
  pinMode(MOSFET_PIN, OUTPUT);
  ledGRN.begin(false);
  ledRED.begin(false);
  // Check whether button is being held
  uint32_t power_on_time_ms = millis();
  while (digitalRead(BUTTON_PIN)) {
    Serial.println("button held");
    if (millis() - power_on_time_ms >= 2000) {
      Serial.println("Turning On");
      digitalWrite(MOSFET_PIN, HIGH);
      ledGRN.turnOn();
    }
  }
  button.begin();
  ledRED.startBlink(1000, 1000);
}


void loop() {
  if (button.longPress) {
    Serial.println("turning off...");
    digitalWrite(MOSFET_PIN, LOW);
    ledGRN.turnOff();
    button.buttonReset();  // this probably doesnt matter in this topology
  }
  ledRED.performBlink();
  button.buttonLoop(2000);
}