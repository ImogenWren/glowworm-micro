



int16_t encoder_rollover(int currentVal, int direction = 1, int minVal = 0, int maxVal = 8) {
  int newVal = currentVal + direction;
  if (newVal > maxVal) {
    newVal = minVal;
  } else if (newVal < minVal) {
    newVal = maxVal;
  }
  return newVal;
}



void ISR_encoder() {
  if ((millis() - last_time) < 1) {  // debounce time is 50ms
    return;
  }
  int16_t clock = digitalRead(ENCODER_CLK);
  int16_t data = digitalRead(ENCODER_DAT);
  // encoderVal += (clock != data) ? +1 : -1;
  if (clock != data) {
    encoderVal++;
    direction = DIRECTION_CW;
  } else {
    encoderVal--;
    direction = DIRECTION_CCW;
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