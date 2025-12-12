
void onWakeupISR() {
  // Interrupt handler: just marks that we woke up
  //wake_start = millis();
  if (sleep_active) {
    wokeFromInterrupt = true;
  }
}



void onWakeup() {  // everything that needs to happen when board wakes up completely
                   // Power Up MOSFET
                   // OLED Wake Up Screen
                   // Reestablish LED Output
  Serial.println("Long press detected. Staying awake.");
  // Turn on any MOSFETS powering external devices
  indicator.turnOn();
  indicator.callBlink(10, 40, 130);
  sleep_block = true;
  sleep_active = false;
  wokeFromInterrupt = false;
  digitalWrite(MOSFET_CTRL, true);
  button.buttonReset();
}

void onShutdown() {  // everything that needs to happen when the system powers down
                      // Save Settings
                      // OLED Shutdown Screen
                      // Power down MOSFET
                      // Sleep
  if (!sleep_block) {
    digitalWrite(MOSFET_CTRL, false);
    Serial.println(F("going to sleep"));
    // turn off any MOSFETS powering external devices
    wokeFromInterrupt = false;
    USBDevice.detach();
    sleep_active = true;
   

    LowPower.sleep();  // MCU sleeps her
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
        onWakeup();
        return;  // SUCCESS: stay awake
      }
    }
    // if function hasnt hit return, then go back to sleep
    USBDevice.detach();
    LowPower.sleep();
  }
}

void unblock_sleep(){
    // reset the sleep blocker after a few seconds
  if (sleep_block) {
    if (millis() - wake_start >= 7000) {
      Serial.println("ending sleep block");
      sleep_block = false;
    }
  }
}
