

#include <ledObject.h>
#include <buttonObject.h>


#define ENCODER_BUTTON_EXTINT8 3

ledObject indicator(LED_BUILTIN);
buttonObject button(ENCODER_BUTTON_EXTINT8, BUTTON_PULL_HIGH);

#include <ArduinoLowPower.h>
#define WAKE_UP_TIME 4000
volatile bool wokeFromInterrupt = false;
volatile bool sleep_active = false;

bool sleep_block = false;
unsigned long wake_start;

void onWakeup() {
  // Interrupt handler: just marks that we woke up
  //wake_start = millis();
  if (sleep_active) {
    wokeFromInterrupt = true;
  }
}

void check_wake_button() {
  // ---- Resume here after ANY wake event ----
  if (wokeFromInterrupt) {
    USBDevice.attach();
    delay(50);
    Serial.println("Woke from pin interrupt, checking long-press...");

    wake_start = millis();

    // Check if pin stays HIGH long enough
    // change this to NON BLOCKING
    while (digitalRead(ENCODER_BUTTON_EXTINT8) == HIGH) {
      if (millis() - wake_start >= WAKE_UP_TIME) {
        Serial.println("Long press detected. Staying awake.");
        // Turn on any MOSFETS powering external devices
        indicator.turnOn();
        indicator.callBlink(10, 40, 130);
        sleep_block = true;
        sleep_active = false;
        return;  // SUCCESS: stay awake
      }
    }
    // if function hasnt hit return, then go back to sleep
    // USBDevice.detach();
    // LowPower.sleep();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("sleep & wakeup test");
  button.begin();

  // Wake on rising edge (HIGH)
  LowPower.attachInterruptWakeup(ENCODER_BUTTON_EXTINT8, onWakeup, RISING);

  indicator.begin(150);
  indicator.callBlink(10, 40, 130);
}

uint8_t i;

void loop() {
  button.buttonLoop(4000);

  if (button.longPress) {
    if (!sleep_block) {
      Serial.println(F("going to sleep"));
      // turn off any MOSFETS powering external devices
      wokeFromInterrupt = false;
      USBDevice.detach();
      sleep_active = true;
      LowPower.sleep();  // MCU sleeps her
    }
    button.buttonReset();
  }

  check_wake_button();

  // reset the sleep blocker after a few seconds
  if (sleep_block) {
    if (millis() - wake_start >= 10000) {
      Serial.println("ending sleep block");
      sleep_block = false;
    }
  }

  Serial.println(i++);
  indicator.performBlink();
  delay(50);
}
