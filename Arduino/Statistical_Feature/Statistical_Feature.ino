#include <Arduino_LSM9DS1.h>
#include "RunningMedian.h"

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

//Buffer
int i=0;
float ax_buf[200],ay_buf[200],az_buf[200];
float gx_buf[200],gy_buf[200],gz_buf[200];
float pitch_buf[200],roll_buf[200];
float ax_max,ax_min,ax_med,ax_mean,ax_std = 0;
float ay_max,ay_min,ay_med,ay_mean,ay_std = 0;
float az_max,az_min,az_med,az_mean,az_std = 0;
float gx_max,gx_min,gx_med,gx_mean,gx_std = 0;
float gy_max,gy_min,gy_med,gy_mean,gy_std = 0;
float gz_max,gz_min,gz_med,gz_mean,gz_std = 0;
float pit_max,pit_min,pit_med,pit_mean,pit_std = 0;
float roll_max,roll_min,roll_med,roll_mean,roll_std = 0;
RunningMedian samples_ax = RunningMedian(200);
RunningMedian samples_ay = RunningMedian(200);
RunningMedian samples_az = RunningMedian(200);
RunningMedian samples_gx = RunningMedian(200);
RunningMedian samples_gy = RunningMedian(200);
RunningMedian samples_gz = RunningMedian(200);
RunningMedian samples_pit = RunningMedian(200);
RunningMedian samples_roll = RunningMedian(200);

void setup() {
  Serial.begin(9600);
  Serial.println("In?");
  int imu = IMU.begin();
  pinMode(buttonPin, INPUT);
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
      ax_min = samples_ax.getLowest();
      ax_max = samples_ax.getHighest();
      ax_med = samples_ax.getMedian();
      ax_mean = samples_ax.getAverage();
      ax_std = stds(ax_buf,ax_mean,i);
      ay_min = samples_ay.getLowest();
      ay_max = samples_ay.getHighest();
      ay_med = samples_ay.getMedian();
      ay_mean = samples_ay.getAverage();
      ay_std = stds(ay_buf,ay_mean,i);
      az_min = samples_az.getLowest();
      az_max = samples_az.getHighest();
      az_med = samples_az.getMedian();
      az_mean = samples_az.getAverage();
      az_std = stds(az_buf,az_mean,i);  
      gx_min = samples_gx.getLowest();
      gx_max = samples_gx.getHighest();
      gx_med = samples_gx.getMedian();
      gx_mean = samples_gx.getAverage();
      gx_std = stds(gx_buf,gx_mean,i);     
      gy_min = samples_gy.getLowest();
      gy_max = samples_gy.getHighest();
      gy_med = samples_gy.getMedian();
      gy_mean = samples_gy.getAverage();
      gy_std = stds(gy_buf,gy_mean,i);         
      gz_min = samples_gz.getLowest();
      gz_max = samples_gz.getHighest();
      gz_med = samples_gz.getMedian();
      gz_mean = samples_gz.getAverage();
      gz_std = stds(gz_buf,gz_mean,i);    
      pit_min = samples_pit.getLowest();
      pit_max = samples_pit.getHighest();
      pit_med = samples_pit.getMedian();
      pit_mean = samples_pit.getAverage();
      pit_std = stds(pitch_buf,pit_mean,i);
      roll_min = samples_roll.getLowest();
      roll_max = samples_roll.getHighest();
      roll_med = samples_roll.getMedian();
      roll_mean = samples_roll.getAverage();
      roll_std = stds(roll_buf,roll_mean,i); 
      Serial.print("Count:");
      Serial.println(samples_ax.getCount());                  
      Serial.print("Ax_Min, Max, Median, Mean, Std:");
      Serial.print(ax_min,5); Serial.print('\t');
      Serial.print(ax_max,5); Serial.print('\t');
      Serial.print(ax_med,5); Serial.print('\t');
      Serial.print(ax_mean,5); Serial.print('\t');
      Serial.println(ax_std,5);      
      Serial.print("Ay_Min, Max, Median, Mean, Std:");
      Serial.print(ay_min,5); Serial.print('\t');
      Serial.print(ay_max,5); Serial.print('\t');
      Serial.print(ay_med,5); Serial.print('\t');
      Serial.print(ay_mean,5); Serial.print('\t');
      Serial.println(ay_std,5);   
      Serial.print("Az_Min, Max, Median, Mean, Std:");
      Serial.print(az_min,5); Serial.print('\t');
      Serial.print(az_max,5); Serial.print('\t');
      Serial.print(az_med,5); Serial.print('\t');
      Serial.print(az_mean,5); Serial.print('\t');
      Serial.println(az_std,5);
      Serial.print("Gx_Min, Max, Median, Mean, Std:");
      Serial.print(gx_min,5); Serial.print('\t');
      Serial.print(gx_max,5); Serial.print('\t');
      Serial.print(gx_med,5); Serial.print('\t');
      Serial.print(gx_mean,5); Serial.print('\t');
      Serial.println(gx_std,5);          
      Serial.print("Gy_Min, Max, Median, Mean, Std:");
      Serial.print(gy_min,5); Serial.print('\t');
      Serial.print(gy_max,5); Serial.print('\t');
      Serial.print(gy_med,5); Serial.print('\t');
      Serial.print(gy_mean,5); Serial.print('\t');
      Serial.println(gy_std,5);   
      Serial.print("Gz_Min, Max, Median, Mean, Std:");
      Serial.print(gz_min,5); Serial.print('\t');
      Serial.print(gz_max,5); Serial.print('\t');
      Serial.print(gz_med,5); Serial.print('\t');
      Serial.print(gz_mean,5); Serial.print('\t');
      Serial.println(gz_std,5);     
      Serial.print("Pitch_Min, Max, Median, Mean, Std:");
      Serial.print(pit_min,5); Serial.print('\t');
      Serial.print(pit_max,5); Serial.print('\t');
      Serial.print(pit_med,5); Serial.print('\t');
      Serial.print(pit_mean,5); Serial.print('\t');
      Serial.println(pit_std,5);
      Serial.print("Roll_Min, Max, Median, Mean, Std:");
      Serial.print(roll_min,5); Serial.print('\t');
      Serial.print(roll_max,5); Serial.print('\t');
      Serial.print(roll_med,5); Serial.print('\t');
      Serial.print(roll_mean,5); Serial.print('\t');
      Serial.println(roll_std,5);                             
      samples_ax.clear();       
      samples_ay.clear();
      samples_az.clear();         
      samples_gx.clear();       
      samples_gy.clear();
      samples_gz.clear();
      samples_pit.clear();
      samples_roll.clear();      
      i = 0; 
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

      //Add new data
        samples_ax.add(axAverage);        
        samples_ay.add(ayAverage);
        samples_az.add(azAverage);
        samples_gx.add(gx);
        samples_gy.add(gy);
        samples_gz.add(gz);
        samples_pit.add(pitch);
        samples_roll.add(roll);       
        ax_buf[i] = axAverage;
        ay_buf[i] = ayAverage;
        az_buf[i] = azAverage;
        gx_buf[i] = gx;
        gy_buf[i] = gy;
        gz_buf[i] = gz;
        pitch_buf[i] = pitch;
        roll_buf[i] = roll;
        i=i+1;
    }
  }
  buttonOld=buttonNew;
  delay(50);
}

float stds(float val[], float avg, int cnt){
  float total = 0;
  for (int i = 0; i < cnt; i++) {
    total = total + (val[i] - avg) * (val[i] - avg);
  }
  float variance = total/(float)cnt;
  float stdDev = sqrt(variance);
  return stdDev;
}
