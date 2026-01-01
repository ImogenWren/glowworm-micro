

#ifndef globals_h
#define globals_h




// External Libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>
#include <ledObject.h>
#include <buttonObject.h>
//#include <multiMeter.h>
#include "calibration.h"



//#include <Fonts/FreeMonoBold12pt7b.h >
//#include <Fonts/FreeSansBold12pt7b.h >
//#include <Fonts/FreeSansBold9pt7b.h >

#include <avr/dtostrf.h>  //needed for SAMD21 only


// Configuration
#define SHOW_SPASH_SCREEN false

// IO PINS
// I2C
#define SDA_PIN A4
#define SCL_PIN A5
// Encoder
#define ENCODER_CLK 2
#define ENCODER_DAT 4
// smart LEDS
#define LED_DATA_PIN 6
// sleep function vars
#define ENCODER_BUTTON_EXTINT8 3
#define MOSFET_CTRL 7

// Battery Monitor
#define BATTERY_SENSE_PIN A0
//#define INPUT_V_MAX 3.3      // Max Voltage for the ADC Input Pin
//#define ADC_RESOLUTION 10  // Typically 10bit on Arduino Platforms
//#define DO_TABLE_CAL false

#define DIRECTION_CW 1    // clockwise direction
#define DIRECTION_CCW -1  // counter-clockwise direction

#define ENCODER_PPR 600
#define ENCODER_TYPE TWO_WIRE
#define ENCODER_NAME "encoder"

// Button


// LED


#define NUM_LEDS 6
#define START_BRIGHTNESS 200
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100
#define MAX_LED_LENGTH 255

//CRGB leds[NUM_LEDS];
CRGB leds[MAX_LED_LENGTH];  // set this for the maximum suitable string, and will only be limited in runtime via user setting ( default 8)





//ledObject indicator(LED_BUILTIN);


#include <ArduinoLowPower.h>
#define WAKE_UP_TIME 2500

volatile bool wokeFromInterrupt = false;
volatile bool sleep_active = false;

bool sleep_block = false;
unsigned long wake_start;




// ADC Consts
#define CURRENT_ADC_OFFSET 0  //-323
#define CURRENT_I_OFFSET -5

// Timing Consts



// Global Objects
buttonObject button(ENCODER_BUTTON_EXTINT8, BUTTON_PULL_HIGH);
ledObject indicator(LED_BUILTIN);
//multiMeter voltMeter(BATTERY_SENSE_PIN, METER_TYPE, INPUT_V_MAX, ADC_RESOLUTION, DO_TABLE_CAL);
calibration cal;

#include "sleepFunctions.h"

volatile uint8_t encoderVal = 0;
uint8_t buttonPressed = 0;
volatile int direction = DIRECTION_CW;
volatile uint32_t last_time;
volatile bool ISR_triggered = false;

// Consts
//const char SYSTEM_NAME = {"Practable Power Hat V1.0"}
const uint8_t I2C_ADDRESS = 0x08;  // Address of i2c slave (self)

uint8_t active_stat = 0;  // The active high bit highlights the selected sat in the UI
bool select_mode = true;  // when select mode is active, encoder used to select active stat. When false,
// encoder changes value of active stat
bool updateLEDs = true;
uint8_t multi_update = 0;
bool num_leds_updated = true;

// LED data struc -> set up like this to make saving and recalling data from memory easy

// blend modes
typedef enum {
  NO_BLEND,
  DIRECT_BLEND,
  WHEEL_SHORT,
  WHEEL_LONG
} blend_mode;

//blend_mode currentBlend = NO_BLEND;

static char blendNames[][8] = {
  "None",
  "Direct",
  "Short",
  "Long"
};



struct LED_data {  // Channel A
  uint8_t ch_A_hue;
  uint8_t ch_A_sat;
  uint8_t ch_A_bright;
  uint8_t ch_B_hue;
  uint8_t ch_B_sat;
  uint8_t ch_B_bright;
  int8_t blend;
  uint8_t num_leds;
};

LED_data defaultData = {
  100, 255, START_BRIGHTNESS,
  200, 255, START_BRIGHTNESS,
  DIRECT_BLEND, NUM_LEDS
};

LED_data currentLED = defaultData;


// palette blending options
CRGBPalette16 outputPalette;
TBlendType currentBlending = LINEARBLEND_NOWRAP;

CHSV colourA = CHSV(40, 255, 255);
CHSV colourB = CHSV(128, 255, 255);
// Channel A
/*
uint8_t ch_A_hue = 20;
uint8_t ch_A_sat = 255;
uint8_t ch_A_bright = START_BRIGHTNESS;

// Channel B
uint8_t ch_B_hue = 230;
uint8_t ch_B_sat = 255;
uint8_t ch_B_bright = START_BRIGHTNESS;
*/

// All Channels
//uint8_t blend = 0;
//uint8_t num_leds = 6;

#define BATTERY_SAMPLES_REQ 20
uint8_t battery_indicator_level = 0;  // while charging this can cylce from 1 to 3
uint8_t batt_indicator_unsmoothed[BATTERY_SAMPLES_REQ + 2];
uint8_t battery_samples_taken = 0;
int16_t battery_adc = 0;
float batteryVoltage;

// Internal Headers
#include "encoder.h"
#include "batteryMonitor.h"
#include "oled.h"
#include "leds.h"
#include "state_machines.h"
#include "power_sense.h"


#endif