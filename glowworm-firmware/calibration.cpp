
#include "calibration.h"

calibration::calibration() {
}


void calibration::print_cal_table(const cal_data *_cal_map, int16_t size) {
  for (int i = 0; i < size; i++) {
    // calibration::cal_data entry;
    // memcpy_P(&datapoint, &_cal_map[i], sizeof(datapoint));
    Serial.print(F("key: "));
    Serial.print(_cal_map[i].key, 3);
    Serial.print(F(" val: "));
    Serial.println(_cal_map[i].value, 3);
  }
}


void calibration::sort_cal_table(cal_data *_cal_map) {
  if (_cal_map) {
    for (int i = 1; i < MAX_ENTRIES; ++i) {
      cal_data current = _cal_map[i];
      int j = i - 1;
      // Move entries greater than current.key one position ahead
      while (j >= 0 && _cal_map[j].key > current.key) {
        _cal_map[j + 1] = _cal_map[j];
        --j;
      }
      _cal_map[j + 1] = current;
    }
  }
}






/*
 Float comparisons can be imprecise, so consider tolerances (e.g., fabsf(a - b) < EPSILON) if needed.
 - This returns the absolute value of the difference (i,e, always positive) to allow comparason to a tolerance figure EPSILON
 (fabsf(cal_map[i].key - inputVal) < EPSILON )
*/

float calibration::do_table_calibration(float inputVal, const cal_data *_cal_map) {
#if DEBUG_LEVEL >= INFO
 // Serial.print(F("inputVal: "));
 // Serial.println(inputVal, 3);
#endif
  float lowerKey = 0;
  float upperKey = 0;
  float lowerVal = 0;
  float upperVal = 0;
  float lastLowerKey = 0;
  float lastLowerVal = 0;
  float returnVal = 0;
  uint8_t matchState = 0b00000000;  // Used to track what has been matched,
                                    // can use bitmasks to set and check individual bits,
                                    //00 = no match, starting sort  01 = lower value found, 10 = upper value found 11 = both values found 100 = exact match (concept, actual implemention might differ)
                                    //1000 = sort started, -> its important to differentiate because the floats are undefined at the start so useless to compare against themselves
                                    // This method tracks 4 independent bool states (4 bytes) with 1 8bit variable (1 byte)

  // Search Algorithm
  for (int i = 0; i < MAX_ENTRIES; ++i) {  //try to find "exact" match - doing some float magic
    if (fabsf(_cal_map[i].key - inputVal) < EPSILON) {
      if (returnValOption == RETURN_CORRECTED_INPUT) {
        returnVal = inputVal + _cal_map[i].value;
      } else if (returnValOption == RETURN_TABLE_VALUE) {
        returnVal = _cal_map[i].value;
      } else {
#if DEBUG_LEVEL >= ERROR
        Serial.println(F("No return Val option specified"));
#endif
        returnVal = inputVal + _cal_map[i].value;
      }
#if DEBUG_LEVEL >= INFO
 //     Serial.print(F("key-found: "));
 //     Serial.print(_cal_map[i].key, 3);
 //     Serial.print(F(" value: "));
 //     Serial.print(_cal_map[i].value, 3);
 //     Serial.print(F(" return-val: "));
 //     Serial.println(returnVal, 3);
#endif
      return returnVal;
    } else {                                                        // "exact match" was not found so finding upper and lower values to interpolate
      if ((matchState & LOWER_KEY_ENTERED) != LOWER_KEY_ENTERED) {  //if any lower key has not been entered
        if (_cal_map[i].key < inputVal) {                           // get the first key that is lower than input value
          lowerKey = _cal_map[i].key;
          lowerVal = _cal_map[i].value;
          lastLowerKey = lowerKey;
          lastLowerVal = lowerVal;
#if DEBUG_LEVEL >= INFO
   ///       Serial.print(F("initial low key found: "));
   //       Serial.print(lowerKey, 3);
  //        Serial.print(F(" lowerVal: "));
  //        Serial.println(lowerVal, 3);
#endif
          matchState |= LOWER_KEY_ENTERED;  // flag that some data has been captured (though might not be correct yet!)
        }
      } else {  // lower key has been entered, so now need to only update if it is HIGHER than existing key, but less than inputVal
        if (_cal_map[i].key > lowerKey && _cal_map[i].key < inputVal) {
          lastLowerKey = lowerKey;
          lowerKey = _cal_map[i].key;
          lastLowerVal = lowerVal;  // used later if input is offscale
          lowerVal = _cal_map[i].value;
#if DEBUG_LEVEL >= INFO
  //        Serial.print(F("low key found: "));
   //       Serial.print(lowerKey, 3);
   //       Serial.print(F(" lowerVal: "));
   //       Serial.println(lowerVal, 3);
#endif
        }
      }
      if ((matchState & UPPER_KEY_ENTERED) != UPPER_KEY_ENTERED) {  //if any upper key has not been entered
        if (_cal_map[i].key > inputVal) {
          upperKey = _cal_map[i].key;  // get the first key that is higher than input value
          upperVal = _cal_map[i].value;
#if DEBUG_LEVEL >= INFO
    //      Serial.print(F("initial upper key found: "));
   //       Serial.print(upperKey, 3);
    //      Serial.print(F(" upperVal: "));
   //       Serial.println(upperVal, 3);
#endif
          matchState |= UPPER_KEY_ENTERED;
        }
      } else {  // some upper key has been found, so only update if new key is LESS than existing upper key, but higher than inputVal
        if (_cal_map[i].key < upperKey && _cal_map[i].key > inputVal) {
          upperKey = _cal_map[i].key;
          upperVal = _cal_map[i].value;
#if DEBUG_LEVEL >= INFO
    //      Serial.print(F("upper key found: "));
  //        Serial.print(upperKey, 3);
   //       Serial.print(F(" upperVal: "));
  //        Serial.println(upperVal, 3);
#endif
        }
      }
    }
  }  // end of for loop, if function has not returned already, then we must now have an upper and lower key and upper and lower value to do some interpolation with, unless floats are still empty
     // first check flags again, as an inputVal that is out of bounds of the list should not have been picked up
  if ((matchState & BOTH_KEYS_ENTERED) != BOTH_KEYS_ENTERED) {
#if DEBUG_LEVEL >= WARNING
  //  Serial.println(F("inputVal out of range of cal table"));
 //   Serial.print(F("matchState: "));
 //   Serial.println(matchState, BIN);
#endif
    if ((matchState & LOWER_KEY_ENTERED) == LOWER_KEY_ENTERED) {  // Only lower value has been found, therefore inputVal was OVERSCALE, interpolate using highest two values to estimate slope (ALMOST WORKS, need to get the difference)
#if DEBUG_LEVEL >= WARNING
  //    Serial.println(F("Input OVERSCALE, interpolating using uppermost values to estimate slope"));  // and use that to estimate a new high point, the current high point becomes the low point last low point becomes low point
  //    Serial.print(F(" lowerKey: "));
  //    Serial.print(lowerKey);
  //    Serial.print(F(" lastLowerKey: "));
 //     Serial.print(lastLowerKey);
 //     Serial.print(F(" lowerVal: "));
  //    Serial.print(lowerVal);
 //     Serial.print(F(" lastLowerVal: "));
  //    Serial.println(lastLowerVal);
#endif
      upperKey = lowerKey;
      lowerKey = lastLowerKey;
      upperVal = lowerVal;
      lowerVal = lastLowerVal;
      matchState |= UPPER_KEY_ENTERED;
    } else if ((matchState & UPPER_KEY_ENTERED) == UPPER_KEY_ENTERED) {  // only upper key was found, therefore inputVal was UNDERSCALE, interpolate using lowest two values to estimate slope // THIS ASSUMES SORTED LIST!
#if DEBUG_LEVEL >= WARNING
 //     Serial.println(F("Input UNDERSCALE, interpolating using lowermost values to estimate slope"));
#endif
      upperKey = _cal_map[1].key;
      lowerKey = _cal_map[0].key;
      upperVal = _cal_map[1].value;
      lowerVal = _cal_map[0].value;
      matchState |= LOWER_KEY_ENTERED;
#if DEBUG_LEVEL >= ERROR
 //     Serial.println(F("No keys were matched, unknown error, this shouldnt even be possible"));
#endif
    }
  }

  // this IF has been changed from an else, now checks again if both keys have been found as the section above to estimate the slope may (should) have found them and set the flags
  if ((matchState & BOTH_KEYS_ENTERED) == BOTH_KEYS_ENTERED) {
    returnVal = lowerVal + (upperVal - lowerVal) * float(inputVal - lowerKey) / fabsf(upperKey - lowerKey);
#if DEBUG_LEVEL >= INFO
  //  Serial.println(F("Both Keys found, Interpolating..."));
 //   Serial.print(F("interpolated offset calculated: "));
 //   Serial.println(returnVal, 3);
 //   Serial.print(F("corrected inputVal: "));
 //   Serial.println(returnVal, 3);
#endif
    if (returnValOption == RETURN_CORRECTED_INPUT) {
      returnVal += inputVal;
    } else if (returnValOption == RETURN_TABLE_VALUE) {
      // return val is already correct
    } else {
#if DEBUG_LEVEL >= ERROR
  //    Serial.println(F("No return Val option specified"));
#endif
      returnVal += inputVal;
    }
    return returnVal;
  }
#if DEBUG_LEVEL >= ERROR
 // Serial.println(F("Something Went Wrong, returning inputVal"));
#endif
  return inputVal;  // not found, just return input value
}
