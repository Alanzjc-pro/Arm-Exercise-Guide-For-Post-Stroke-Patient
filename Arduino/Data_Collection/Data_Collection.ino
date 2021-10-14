#include <Arduino_LSM9DS1.h>
#include <SPI.h>
#include <SD.h>

#define SIZE 4 //Define window size for average filter

File testfile;
String fileName = "SL.csv"; // change filename according to different exercises

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
  int imu = IMU.begin();
  // wait for SD module to start
  if (!SD.begin(4)) {
    Serial.println("No SD Module Detected");
    while (1);
  }
  pinMode(buttonPin, INPUT);
  testfile = SD.open(fileName, FILE_WRITE);
  if (testfile) {
    // save headers to file
    testfile.println("aX,aY,aZ,gX,gY,gZ,Roll,Pitch");
    testfile.close();
  } 
}

void loop() {
buttonNew = digitalRead(buttonPin);
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
      testfile = SD.open(fileName, FILE_WRITE);
      if (testfile) {
        testfile.println();
        testfile.close();
      } 
    }
  }
  
  if(flag==1){
    if(IMU.accelerationAvailable() && IMU.gyroscopeAvailable()){
      IMU.readAcceleration(ax, ay, az);
      IMU.readGyroscope(gx, gy, gz);
      Serial.println("IN");
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

      testfile = SD.open(fileName, FILE_WRITE);
      if (testfile) {
        // save IMU data to SD card each loop when activated by PB       
        testfile.print(ax, 3);
        testfile.print(',');
        testfile.print(ay, 3);
        testfile.print(',');
        testfile.print(az, 3);
        testfile.print(',');
        testfile.print(gx, 3);
        testfile.print(',');
        testfile.print(gy, 3);
        testfile.print(',');
        testfile.print(gz, 3);
        testfile.print(',');
        testfile.print(roll, 3);
        testfile.print(',');
        testfile.print(pitch, 3);
        testfile.println();
        testfile.close();
      } 
    }
  }
  buttonOld=buttonNew;
  delay(50);
}
