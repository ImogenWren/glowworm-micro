

#ifndef GLOBALS_H
#define GLOBALS_H




// External Libraries
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>





#include <Fonts/FreeMonoBold12pt7b.h >
#include <Fonts/FreeSansBold12pt7b.h >
#include <Fonts/FreeSansBold9pt7b.h >

#include <avr/dtostrf.h> //needed for SAMD21 only

#include <buttonObject.h>


// Configuration
#define SHOW_SPASH_SCREEN true

// I2C
#define SDA_PIN A4
#define SCL_PIN A5

// Encoder
#define ENCODER_CLK 2
#define ENCODER_DAT 3

#define DIRECTION_CW 0   // clockwise direction
#define DIRECTION_CCW 1  // counter-clockwise direction

#define ENCODER_PPR 600
#define ENCODER_TYPE TWO_WIRE
#define ENCODER_NAME "encoder"

// Button
#define ENCODER_BUTTON_EXTINT7 4

// LED
#define LED_DATA_PIN 6

#define NUM_LEDS 6
#define BRIGHTNESS 80
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100
CRGB leds[NUM_LEDS];

// Battery Monitor

#define BATTERY_SENSE_PIN A0




// ADC Consts
#define CURRENT_ADC_OFFSET 0  //-323
#define CURRENT_I_OFFSET -5

// Timing Consts



// Global Objects
buttonObject button(ENCODER_BUTTON_EXTINT7, BUTTON_PULL_HIGH);


volatile uint8_t encoderVal = 0;
uint8_t buttonPressed = 0;
volatile int direction = DIRECTION_CW;
volatile uint32_t last_time;
volatile bool ISR_triggered = false;

// Consts
//const char SYSTEM_NAME = {"Practable Power Hat V1.0"}
const uint8_t I2C_ADDRESS = 0x08;  // Address of i2c slave (self)




// Internal Headers
#include "encoder.h"
#include "oled.h"
#include "leds.h"
#include "state_machines.h"
#include "power_sense.h"


#endif