


void ISR_encoder() {
  if ((millis() - last_time) < 1) {  // debounce time is 50ms
    return;
  }
  int16_t clock = digitalRead(ENCODER_CLK);
  int16_t data = digitalRead(ENCODER_DAT);
  // encoderVal += (clock != data) ? +1 : -1;
  if (clock != data) {
    encoderVal++;
  } else {
    encoderVal--;
  }

  ISR_triggered = true;

  last_time = millis();
}



/*
// if ((millis() - last_time) < 50)  // debounce time is 50ms
 //   return;

  if (digitalRead(ENCODER_DAT) == HIGH) {
    // The encoder is rotating in counter-clockwise direction => decrease the counter
    encoderVal--;
    direction = DIRECTION_CCW;
  } else {
    // The encoder is rotating in clockwise direction => increase the counter
    encoderVal++;
    direction = DIRECTION_CW;
  }

 // last_time = millis();
 */