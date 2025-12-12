/* GlowWorm Micro -> Mini lighting controller for creative Macro Photography



Example functions for printing bitmap images to SSD1306 OLED screen
// Converted using https://mischianti.org/images-to-byte-array-online-converter-cpp-arduino/
*/

/* Changelog

*/



#include "globals.h"





void setup() {
  // Setup Serial
  Serial.begin(115200);
  digitalWrite(MOSFET_CTRL, true);
  delay(3000);
  Serial.println(F("\nGlowWorm Micro"));
  Serial.println(F("lighting controller"));

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DAT, INPUT);
  indicator.begin(150);
  indicator.callBlink(10, 40, 130);

  // Setup IO Pins
  delay(1000);
  show_splash();
  // Set up button Object
  button.begin();


  // use interrupt for CLK pin is enough
  // call ISR_encoder() when CLK pin changes from LOW to HIGH
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), ISR_encoder, RISING);
  // Wake on rising edge (HIGH)
  LowPower.attachInterruptWakeup(ENCODER_BUTTON_EXTINT8, onWakeupISR, RISING);



  // set up LED string
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(START_BRIGHTNESS);


  // Set up i2c slave to handle requests from Raspberry Pi
  // Wire.begin(I2C_ADDRESS);
  // Wire.onRequest(requestEvent);
}



void loop() {

  // Sample OLED Button
  button.buttonLoop(WAKE_UP_TIME);
  // SBC power cycle trigger
  if (button.shortPress) {
    select_mode = !select_mode;
    button.buttonReset();
  }

  if (button.longPress) {
    // do shutdown
    indicator.callBlink(10, 40, 130);
    onShutdown();
    button.buttonReset();
  }
  check_wake_button();
  unblock_sleep();


  update_oled();


  if (ISR_triggered) {
    if (select_mode) {
      active_stat = encoder_rollover(active_stat, direction, 0, 7);
    } else {
      // adjust the live value

      if (active_stat == 0) {
        currentLED.num_leds = encoder_rollover(currentLED.num_leds, direction, 0, MAX_LED_LENGTH);
        num_leds_updated = true;
      } else if (active_stat == 1) {
        currentLED.blend = encoder_rollover(currentLED.blend, direction, 0, WHEEL_LONG);
      } else if (active_stat == 2) {
        currentLED.ch_A_hue = encoder_rollover(currentLED.ch_A_hue, direction, 0, 255);
      } else if (active_stat == 3) {
        currentLED.ch_A_sat = encoder_rollover(currentLED.ch_A_sat, direction, 0, 255);
      } else if (active_stat == 4) {
        currentLED.ch_A_bright = encoder_rollover(currentLED.ch_A_bright, direction, 0, 255);
      } else if (active_stat == 5) {
        currentLED.ch_B_hue = encoder_rollover(currentLED.ch_B_hue, direction, 0, 255);
      } else if (active_stat == 6) {
        currentLED.ch_B_sat = encoder_rollover(currentLED.ch_B_sat, direction, 0, 255);
      } else if (active_stat == 7) {
        currentLED.ch_B_bright = encoder_rollover(currentLED.ch_B_bright, direction, 0, 255);
      } else if (active_stat > 7) {
        Serial.println(F("ERROR, illegal active stat"));
      }

      updateLEDs = true;
    }
    ISR_triggered = false;
  }

  if (num_leds_updated) {
    for (int i = currentLED.num_leds; i < MAX_LED_LENGTH; i++) {
      leds[i] = CHSV(0, 0, 0);  // turn all the remaining LEDs back, rather than just have them not update
    }
    num_leds_updated = false;
  }

  if (updateLEDs) {
    // then update the LED outputs depending on blend mode
    // if blend mode is NONE
    if (currentLED.blend == NO_BLEND) {
      for (int i = 0; i < currentLED.num_leds / 2; ++i) {
        leds[i] = CHSV(currentLED.ch_A_hue, currentLED.ch_A_sat, currentLED.ch_A_bright);
      }
      for (int i = currentLED.num_leds / 2; i < currentLED.num_leds; ++i) {
        leds[i] = CHSV(currentLED.ch_B_hue, currentLED.ch_B_sat, currentLED.ch_B_bright);
      }
    } else if (currentLED.blend == DIRECT_BLEND) {
      // use pallets?
      //  for (int i = 0; i < currentLED.num_leds / 2; i++) {
      //   outputPalette[0] = CHSV(currentLED.ch_A_hue, currentLED.ch_A_sat, currentLED.ch_A_bright);  // fill entire pallette with both colours
      // }
      // for (int i = currentLED.num_leds / 2; i < currentLED.num_leds; i++) {
      //  outputPalette[1] = CHSV(currentLED.ch_B_hue, currentLED.ch_B_sat, currentLED.ch_B_bright);
      //  }
      colourA = CHSV(currentLED.ch_A_hue, currentLED.ch_A_sat, currentLED.ch_A_bright);
      colourB = CHSV(currentLED.ch_B_hue, currentLED.ch_B_sat, currentLED.ch_B_bright);
      buildPalette();

      // trying to get a pallette using just the two colours
      for (int i = 0; i < currentLED.num_leds; i++) {
        uint8_t index = map(i, 0, currentLED.num_leds - 1, 0, 255);
        leds[i] = ColorFromPalette(outputPalette, index, currentLED.ch_A_bright, LINEARBLEND_NOWRAP);
      }
    } else if (currentLED.blend == WHEEL_SHORT) {
      colourA = CHSV(currentLED.ch_A_hue, currentLED.ch_A_sat, currentLED.ch_A_bright);
      colourB = CHSV(currentLED.ch_B_hue, currentLED.ch_B_sat, currentLED.ch_B_bright);
      fill_gradient(leds, currentLED.num_leds - 1, colourA, colourB, SHORTEST_HUES);
      //leds[currentLED.num_leds - 1] = CHSV(currentLED.ch_B_hue, currentLED.ch_B_sat, currentLED.ch_B_bright);  // this line makes sure our maths works, the penultamate LED should blend into the last one that is set to the channel B colour
    } else if (currentLED.blend == WHEEL_LONG) {
      colourA = CHSV(currentLED.ch_A_hue, currentLED.ch_A_sat, currentLED.ch_A_bright);
      colourB = CHSV(currentLED.ch_B_hue, currentLED.ch_B_sat, currentLED.ch_B_bright);
      fill_gradient(leds, currentLED.num_leds - 1, colourA, colourB, LONGEST_HUES);
      // leds[currentLED.num_leds - 1] = CHSV(currentLED.ch_B_hue, currentLED.ch_B_sat, currentLED.ch_B_bright);  // this line makes sure our maths works, the penultamate LED should blend into the last one that is set to the channel B colour
    }

    FastLED.show();
    //  // FastLED.delay(1000 / UPDATES_PER_SECOND);
    //  multi_update++;
    //  if (multi_update > 2){
    //     multi_update = 0;
    updateLEDs = false;  // this is here to try and prevent LEDs getting out of sync because of a bad interrupt
    //  }
    // updateLEDs = false;
  }
  indicator.performBlink();
}



void requestEvent() {  // note try this command on raspberry pi to get data in hex `i2cget -y 1 0x08 0x00 i` then decode hex as UTF-16 big endian
  //  Wire.write(adj_V_buf);
  ///  Wire.write(", ");
  // Wire.write(adj_I_buf);
  // Wire.write(", ");
  // Wire.write(sbc_V_buf);
  // Wire.write(", ");
  // Wire.write(sbc_I_buf);
  //  Wire.write(".\n ");
}
