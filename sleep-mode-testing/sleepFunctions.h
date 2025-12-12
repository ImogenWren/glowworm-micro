



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
        wokeFromInterrupt = false;
        digitalWrite(MOSFET_CTRL, true);
        button.buttonReset();
        return;  // SUCCESS: stay awake
      }
    }
    // if function hasnt hit return, then go back to sleep
    USBDevice.detach();
    LowPower.sleep();
  }
}