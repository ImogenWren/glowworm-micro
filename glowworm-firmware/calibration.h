/*  Calibration.h

      Multi point generic calibration using key->value pair offset tables

Author: Imogen Wren
Date:   08/08/2025

*/

#pragma once

#ifndef calibration_h
#define calibration_h

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <wProgram.h>
#endif


#include <debugging.h>


#define DEBUG_LEVEL DEBUG_NONE

// This calibration table should be filled in with the voltage offset measured at the sensing pin of the microcontroller.
// This list is arbritary length, limited only by microcontroller memory.
// To avoid dynamic memory allocation, array size is defined in this header, but can be expanded to allow for longer arrays,
// Or limit removed entirely to allow dynamic memory allocation> -> maybe at your own risk but you still need to pass the array size

/*
calibration::cal_data cal_map[MAX_ENTRIES] = {
  { 0.00, 0.00 },
  { 0.50, -0.022 },
  { 1.00, -0.035 },
  { 1.50, -2.063 },
  { 2.00, -1.100 },
  { 2.50, -4.124 },
  { 3.00, -3.143 },
  { 3.50, -1.180 },
  { 4.00, -2.213 },
  { 4.50, -3.250 },
  { 5.00, -3.270 }
};

*/


// Previous implmentation was done with std::map, howver as this is not available across all arduino implementions, this version will attempt to do the same with simple arrays
// I think the best option is to use an 1-dimensional array of 2 dimensional arrays or data structures.

#define MAX_ENTRIES 8  // Max entries in calibration table (limited only by memory)
#define EPSILON 0.1     // Define the tolerance of float comparasons for "exact match"

//#define DEBUG_LEVEL WARNING

/*  Return Value option
_Should the do_table_calibration function return the corrected input value, or the interpolated value directly from the table, each mode has
value for different use cases_
*/

#define RETURN_CORRECTED_INPUT 0
#define RETURN_TABLE_VALUE 1

#define RETURN_VALUE_OPTION RETURN_TABLE_VALUE


/*
The last problem to solve: how to deal with out of bounds inputs

- Default behaviour is to extrapolate out using the first two datapoints and the last two datapoints
- Alternative option is to return error code with UNDERSCALE/OVERSCALE  -> program already contains this if errors are enabled
- Or just use the first/final offset value in the table

- Honestly this is going to add so much code and I want to keep this as light as possible so default behaviour is the only option. Sorry

*/



class calibration {

public:
  // Constructor
  calibration();

  struct cal_data {  // Structure to hold key-value pairs
    float key;
    float value;
  };





  void print_cal_table(const cal_data *_cal_map, int16_t size = MAX_ENTRIES);

  void sort_cal_table(cal_data *_cal_map);

  float do_table_calibration(float inputVal, const cal_data *_cal_map);  // table must be sorted!

private:

  uint8_t returnValOption = RETURN_VALUE_OPTION;


// bitmasks used internally for tracking state
#define LOWER_KEY_ENTERED 0b00000001
#define UPPER_KEY_ENTERED 0b00000010
#define BOTH_KEYS_ENTERED 0b00000011
};







#endif