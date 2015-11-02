
/* Declare: 
 *  int wpPin = x - Any PWM pin in Arduino board
 *  This pin will be used to generate 0-5Vdc output
 * In the setup function:
 * void setup(){
 * ...
 * pinMode(wpPin,OUTPUT)
 * ...
 * }
*/


void WaterPump(float value, int wpPin) {
  if(value>5)
  value = 5;
  if(value<0)
  value = 0;
  int x = mapping(value);
  analogWrite(wpPin,x);
//  Serial.print("value: ");
//  Serial.println(x);
}

/* Declare: 
 *  int readPin = x - Any analog input pin in Arduino board
 *  This pin will be used to read 0-5Vdc that corresponds to the output power of the water pump
*/

float WaterPumpPower(int readPin){
  int val=analogRead(readPin);
  float volt=2*(val*5/1024);  //It is times two because we are dividing by 2 the output voltage of the WP (0-10V -> 0-5V)
  return volt;
  }

/* This functions does a mapping between input and output */

int mapping (float val){
  int res = val*255/5;  
  return res;
}
