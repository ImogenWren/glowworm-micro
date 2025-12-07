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
  if (button.shortPress || button.longPress) {
    buttonPressed++;

    button.buttonReset();
  }
  // Power Control State Machines
  // sbc_state_machine();
  // exp_state_machine();

  // Voltage/Current Data Collection & Reporting
  // if (millis() - lastSample_mS >= 200) {
  //   lastSample_mS = millis();
  ////  sample_power();
  //   samples_taken++;
  // }


  // if (samples_taken >= samples_required) {
  //    //samplingComplete = true;  // this does nothing lol
  ////    calculate_power();
  //   print_serial_stats();
  //   print_trigger_status();
  //   Serial.println();
  update_oled();
  //  samples_taken = 0;
  // }
  if (ISR_triggered) {
    currentLED.ch_A_hue = encoderVal;
    currentLED.ch_B_hue = encoderVal+150;
    for (int i = 0; i < currentLED.num_leds/2; ++i) {
      leds[i] = CHSV(currentLED.ch_A_hue, currentLED.ch_A_sat, currentLED.ch_A_bright);
    }
      for (int i = currentLED.num_leds/2; i < currentLED.num_leds; ++i) {
      leds[i] = CHSV(currentLED.ch_B_hue, currentLED.ch_B_sat, currentLED.ch_B_bright);
    }

    FastLED.show();
    // FastLED.delay(1000 / UPDATES_PER_SECOND);
    ISR_triggered = false;
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
