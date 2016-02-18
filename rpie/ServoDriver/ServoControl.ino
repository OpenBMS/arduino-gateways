
/*// In the main function you need to include Servo.h and
Servo servo1:

#include <Servo.h>
Servo servo1;   // Creating servo1 object
int level = 175;  // This value is fan's lowest speed

"The speeds are: 175 -> Slow, 90 -> Medium, 7 -> Fast"
#define servoPin x // Any digital pin that supports pwm, i.e. 9

// And in the setup:

  servo1.attach(x);   
  servo1.write(level);
*/


void ServoControl(int level) {
     if(level < 7){
    level = 7;    // minimum servo value
  }else if(level > 175){
    level = 175;  // max servo value
  }
  //write the value
  servo1.write(level);    // Make sure the servo object is servo1 on the main routine
  
}


