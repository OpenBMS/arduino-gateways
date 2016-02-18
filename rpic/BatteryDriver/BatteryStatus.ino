/*// In the main function you need to include BatteryStatus.h:
#include <BatteryStatus.h>

Needs to define arduino analog input pin in the main function
*/


float BatteryStatus(int analogPin) {
  int val=analogRead(analogPin);
  float volt = backMapping(val);    // Scaled output voltage
  float BatVolt = volt*(2.819)*1.0219;       // 2 is the ratio between voltage divider resistors r1/r2
  return BatVolt;
}

float backMapping(int value){
  float volt = value*5/1024;
  return volt;
  }
