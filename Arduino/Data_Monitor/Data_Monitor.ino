#include <Arduino_LSM9DS1.h>
#include <SPI.h>
#include <SD.h>

#define SIZE 4 //Define window size for average filter


//Debugging system variable definition
int OutState=0;
int LEDPin=8;
int buttonPin=7;
int buttonNew;
int buttonOld=1;
int flag=0;

//Feature definition
float gx, gy, gz, ax, ay, az;
float pitch, roll, yaw;

//Moving average filter variable definition
int INDEX = 0;
float axSum,aySum,azSum = 0;
float axReadings[SIZE],ayReadings[SIZE],azReadings[SIZE];
float axAverage, ayAverage, azAverage=0;


void setup() {
  Serial.begin(9600);
  int imu = IMU.begin();
  pinMode(buttonPin, INPUT);
}

void loop() {
buttonNew = digitalRead(buttonPin);
Serial.println(buttonNew);
  if(buttonOld==0 && buttonNew==1){
    if(OutState==0){
      digitalWrite(LEDPin,HIGH);
      flag=1;
      OutState=1;
    }
    else{
      digitalWrite(LEDPin,LOW);
      flag=0;
      OutState=0;
    }
  }
  
  if(flag==1){
    if(IMU.accelerationAvailable() && IMU.gyroscopeAvailable()){
      IMU.readAcceleration(ax, ay, az);
      IMU.readGyroscope(gx, gy, gz);
      
      //Compute Moving average filter
      axSum = axSum - axReadings[INDEX]; // Remove the oldest entry from the sum
      axReadings[INDEX] = ax; // store new values into window
      axSum = axSum + ax; //Add the newest reading to the sum
      aySum = aySum - ayReadings[INDEX];
      ayReadings[INDEX] = ay;
      aySum = aySum + ay;
      azSum = azSum - azReadings[INDEX];
      azReadings[INDEX] = az;
      azSum = azSum + az;

      //Increment the index, and wrap to 0 if it exceeds the window size
      INDEX = (INDEX+1) % SIZE;
     
      //Compute filtered values
      axAverage = axSum/SIZE;
      ayAverage = aySum/SIZE;
      azAverage = azSum/SIZE;

      //Compute Euler angle based on accelerometer
      pitch = 180 * atan (axAverage/sqrt(ayAverage*ayAverage + azAverage*azAverage))/3.141592654; 
      roll = 180 * atan (ayAverage/sqrt(axAverage*axAverage + azAverage*azAverage))/3.141592654;
      //Serial.print("Orientation: ");
      //Serial.print(pitch);
      //Serial.print(" ");
      //Serial.print(roll);
      //Serial.print(" ");
       Serial.print(axAverage);
       Serial.print('\t');
       Serial.print(ayAverage);
       Serial.print('\t');
       Serial.print(az);
       Serial.print('\t');
       Serial.println(azAverage);
       Serial.print('\t');
       Serial.print(gx);
       Serial.print('\t');
       Serial.println(gy);
       Serial.print('\t');
       Serial.println(gz);
    }
  }
  buttonOld=buttonNew;
  delay(50);
}
