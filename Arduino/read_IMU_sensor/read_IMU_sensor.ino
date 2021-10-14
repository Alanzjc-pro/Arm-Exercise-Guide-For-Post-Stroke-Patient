#include <Arduino_LSM9DS1.h>

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
float x, y, z, delta = 0.05;


  if (IMU.accelerationAvailable())
  {
    IMU.readAcceleration(x, y, z);
    if(y <= delta && y >= -delta)
          Serial.println("flat");
    else if(y > delta && y < 1 - delta)
          Serial.println("tilted to the left");
    else if(y >= 1 - delta)
          Serial.println("left");
    else if(y < -delta && y > delta - 1)
          Serial.println("tilted to the right");
    else
          Serial.println("right");
}

}
