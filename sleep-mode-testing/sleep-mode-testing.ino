

#include <ledObject.h>
#include <buttonObject.h>
#include "sleepFunctions.h"


#define ENCODER_BUTTON_EXTINT8 3
#define MOSFET_CTRL 7

//ledObject indicator(LED_BUILTIN);
buttonObject button(ENCODER_BUTTON_EXTINT8, BUTTON_PULL_HIGH);

#include <ArduinoLowPower.h>
#define WAKE_UP_TIME 3000
volatile bool wokeFromInterrupt = false;
volatile bool sleep_active = false;

bool sleep_block = false;
unsigned long wake_start;



void setup() {
  Serial.begin(115200);
  Serial.println("sleep & wakeup test");
  button.begin();
  pinMode(MOSFET_CTRL, OUTPUT);

  // Wake on rising edge (HIGH)
  LowPower.attachInterruptWakeup(ENCODER_BUTTON_EXTINT8, onWakeupISR, RISING);

  indicator.begin(150);
  indicator.callBlink(10, 40, 130);
  digitalWrite(MOSFET_CTRL, true);
}

uint8_t i;

void loop() {
  button.buttonLoop(WAKE_UP_TIME);

  if (button.longPress) {
    onshutdown();
  }

  check_wake_button();



  Serial.println(i++);
  indicator.performBlink();
  // delay(50);
}
