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
  delay(3000);
  Serial.println(F("\nGlowWorm Micro"));
  Serial.println(F("lighting controller"));

  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DAT, INPUT);


  // Setup IO Pins
  delay(1000);
  show_splash();
  // Set up button Object
  button.begin();


  // use interrupt for CLK pin is enough
  // call ISR_encoder() when CLK pin changes from LOW to HIGH
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), ISR_encoder, RISING);


  // set up LED string
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(START_BRIGHTNESS);


  // Set up i2c slave to handle requests from Raspberry Pi
  // Wire.begin(I2C_ADDRESS);
  // Wire.onRequest(requestEvent);
}



void loop() {

  // Sample OLED Button
  button.buttonLoop(4000);
  // SBC power cycle trigger
  if (button.shortPress) {
    select_mode = !select_mode;
    button.buttonReset();
  }

  if (button.longPress) {
    // do shutdown
    button.buttonReset();
  }

  update_oled();


  if (ISR_triggered) {
    if (select_mode) {
      active_stat = encoder_rollover(active_stat, direction, 0, 7);
    } else {
      // adjust the live value

      if (active_stat == 0) {
        currentLED.num_leds = encoder_rollover(currentLED.num_leds, direction, 0, 255);
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
    }

    FastLED.show();

    // FastLED.delay(1000 / UPDATES_PER_SECOND);
    updateLEDs = false;
  }
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
