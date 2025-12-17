calibration::cal_data cal_map[8] = {
  { 1.68, 2.965 },
  { 1.87, 3.161 },
  { 2.08, 3.358 },
  { 2.39, 3.672 },
  { 2.50, 3.797 },
  { 2.70, 3.978 },
  { 2.89, 4.177 },
  { 3.04, 4.324 }
};

// Battery level goes from 0 to above 3
// now with averaging to stop the animation jumping up and down all the time

void batteryMonitor() {
  uint8_t temp_batt_level = 0;


  battery_adc = analogRead(BATTERY_SENSE_PIN);
  float measured_voltage = battery_adc * 3.15E-3;  //current value found via measurement  //22265625
  Serial.print("adc: ");
  Serial.print(battery_adc);
  Serial.print("  measuredV: ");
  Serial.print(measured_voltage);
  batteryVoltage = cal.do_table_calibration(measured_voltage, cal_map);
  if (batteryVoltage <= 3.35) temp_batt_level = 0;
  else if (batteryVoltage > 3.37 && batteryVoltage <= 3.75) temp_batt_level = 1;
  else if (batteryVoltage > 3.75 && batteryVoltage <= 4.0) temp_batt_level = 2;
  else if (batteryVoltage > 4.0 && batteryVoltage <= 4.1) temp_batt_level = 3;
  else if (batteryVoltage > 4.1) temp_batt_level = 4;


  if (battery_samples_taken < BATTERY_SAMPLES_REQ) {
    batt_indicator_unsmoothed[battery_samples_taken] = temp_batt_level;
    battery_samples_taken++;
  } else if (battery_samples_taken >= BATTERY_SAMPLES_REQ) {
    battery_indicator_level = 0;
    for (int i = 0; i < battery_samples_taken; i++) {
      battery_indicator_level += batt_indicator_unsmoothed[i];
    }
    battery_indicator_level = battery_indicator_level / battery_samples_taken;
    battery_samples_taken = 0;
  }

  Serial.print(" Vbat: ");
  Serial.print(batteryVoltage);
  Serial.print(" Indicator: ");
  Serial.println(battery_indicator_level);
}
