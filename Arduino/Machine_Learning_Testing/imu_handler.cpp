
#include <Arduino_LSM9DS1.h>
#include "imu_handler.h"
#include "RunningMedian.h"

#define SIZE 4 //Define window size for average filter
#define INPUT_SIZE 33


//Feature definition
float gx, gy, gz, ax, ay, az;
float pitch, roll, yaw;

//Moving average filter variable definition
int INDEX = 0;
float axSum,aySum,azSum = 0;
float axReadings[SIZE],ayReadings[SIZE],azReadings[SIZE];
float axAverage, ayAverage, azAverage=0;

//Buffer
float ax_buf[500],ay_buf[500],az_buf[500];
float gx_buf[500],gy_buf[500],gz_buf[500];
float pitch_buf[500],roll_buf[500];
float save_buff[INPUT_SIZE] = {};
float ax_max,ax_min,ax_med,ax_mean,ax_std = 0;
float ay_max,ay_min,ay_med,ay_mean,ay_std = 0;
float az_max,az_min,az_med,az_mean,az_std = 0;
float gx_max,gx_min,gx_med,gx_mean,gx_std = 0;
float gy_max,gy_min,gy_med,gy_mean,gy_std = 0;
float gz_max,gz_min,gz_med,gz_mean,gz_std = 0;
float pit_max,pit_min,pit_med,pit_mean,pit_std = 0;
float roll_max,roll_min,roll_med,roll_mean,roll_std = 0;
int i = 0;
int begin_idx = 0;

RunningMedian samples_ax = RunningMedian(500);
RunningMedian samples_ay = RunningMedian(500);
RunningMedian samples_az = RunningMedian(500);
RunningMedian samples_gx = RunningMedian(500);
RunningMedian samples_gy = RunningMedian(500);
RunningMedian samples_gz = RunningMedian(500);
RunningMedian samples_pit = RunningMedian(500);
RunningMedian samples_roll = RunningMedian(500);

TfLiteStatus SetupIMU(tflite::ErrorReporter* error_reporter) {
  // Switch on the IMU
  if (!IMU.begin()) {
    TF_LITE_REPORT_ERROR(error_reporter, "Failed to initialize IMU");
    return kTfLiteError;
  }
  TF_LITE_REPORT_ERROR(error_reporter, "Exercise starts!");
  return kTfLiteOk;
}

bool ReadIMU(tflite::ErrorReporter* error_reporter){
  bool new_data = false;
    if (!IMU.accelerationAvailable() && !IMU.gyroscopeAvailable()) {
      TF_LITE_REPORT_ERROR(error_reporter, "Failed to read data");
    }
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
        begin_idx = i;
        new_data = true;
    }
  if (!new_data) {
    return false;
  }
  return new_data;
}

void Result(float* input, int len){
      save_buff[0] = samples_ax.getLowest();
      save_buff[1] = samples_ax.getHighest();
      save_buff[2] = samples_ax.getAverage();
      save_buff[3] = stds(ax_buf,save_buff[2],i);
      save_buff[4] = samples_ax.getMedian();
      save_buff[5]  = samples_ay.getLowest();
      save_buff[6]  = samples_ay.getHighest();
      save_buff[7] = samples_ay.getAverage();
      save_buff[8]= stds(ay_buf,save_buff[7],i);
      ay_med = samples_ay.getMedian();
      save_buff[9] = samples_az.getLowest();
      save_buff[10] = samples_az.getHighest();
      save_buff[11] = samples_az.getAverage();
      save_buff[12] = stds(az_buf,save_buff[11],i); 
      az_med = samples_az.getMedian();
      save_buff[13] = samples_gx.getLowest();
      save_buff[14] = samples_gx.getHighest();
      save_buff[15] = samples_gx.getAverage();
      save_buff[16] = stds(gx_buf,save_buff[15],i);
      save_buff[17] = samples_gx.getMedian();
      save_buff[18] = samples_gy.getLowest();
      save_buff[19] = samples_gy.getHighest();
      save_buff[20] = samples_gy.getAverage();
      save_buff[21] = stds(gy_buf,save_buff[20],i);
      save_buff[22] = samples_gy.getMedian();       
      save_buff[23] = samples_gz.getLowest();
      save_buff[24] = samples_gz.getHighest();
      save_buff[25] = samples_gz.getAverage();
      save_buff[26] = stds(gz_buf,save_buff[25],i); 
      save_buff[27] = samples_gz.getMedian();   
      save_buff[28] = samples_pit.getLowest();
      save_buff[29] = samples_pit.getHighest();
      pit_mean = samples_pit.getAverage();
      pit_std = stds(pitch_buf,pit_mean,i);
      pit_med = samples_pit.getMedian();
      save_buff[30] = samples_roll.getLowest();
      save_buff[31] = samples_roll.getHighest();
      roll_mean = samples_roll.getAverage();
      save_buff[32] = stds(roll_buf,roll_mean,i);
      roll_med = samples_roll.getMedian();   
      for(int j = 0; j< len; j++){
        input[j] = save_buff[j];
      }   
  //print_Result();
  clear_buffer();
}
  
void get_Result(float* input, int len){
                      
  }

    
void print_Result(){                 
      Serial.print("Ax_Min, Max, Median, Mean, Std:");
      Serial.print(save_buff[0],5); Serial.print('\t');
      Serial.print(save_buff[1],5); Serial.print('\t');
      Serial.print(save_buff[4],5); Serial.print('\t');
      Serial.print(save_buff[2],5); Serial.print('\t');
      Serial.println(save_buff[3],5);      
      Serial.print("Ay_Min, Max, Median, Mean, Std:");
      Serial.print(save_buff[5],5); Serial.print('\t');
      Serial.print(save_buff[6],5); Serial.print('\t');
      Serial.print(ay_med,5); Serial.print('\t');
      Serial.print(save_buff[7],5); Serial.print('\t');
      Serial.println(save_buff[8],5);   
      Serial.print("Az_Min, Max, Median, Mean, Std:");
      Serial.print(save_buff[9],5); Serial.print('\t');
      Serial.print(save_buff[10],5); Serial.print('\t');
      Serial.print(az_med,5); Serial.print('\t');
      Serial.print(save_buff[11],5); Serial.print('\t');
      Serial.println(save_buff[12],5);
      Serial.print("Gx_Min, Max, Median, Mean, Std:");
      Serial.print(save_buff[13],5); Serial.print('\t');
      Serial.print(save_buff[14],5); Serial.print('\t');
      Serial.print(save_buff[17],5); Serial.print('\t');
      Serial.print(save_buff[15],5); Serial.print('\t');
      Serial.println(save_buff[16],5);          
      Serial.print("Gy_Min, Max, Median, Mean, Std:");
      Serial.print(save_buff[18],5); Serial.print('\t');
      Serial.print(save_buff[19],5); Serial.print('\t');
      Serial.print(save_buff[22],5); Serial.print('\t');
      Serial.print(save_buff[20],5); Serial.print('\t');
      Serial.println(save_buff[21],5);   
      Serial.print("Gz_Min, Max, Median, Mean, Std:");
      Serial.print(save_buff[23],5); Serial.print('\t');
      Serial.print(save_buff[24],5); Serial.print('\t');
      Serial.print(save_buff[27],5); Serial.print('\t');
      Serial.print(save_buff[25],5); Serial.print('\t');
      Serial.println(save_buff[26],5);     
      Serial.print("Pitch_Min, Max, Median, Mean, Std:");
      Serial.print(save_buff[28],5); Serial.print('\t');
      Serial.print(save_buff[29],5); Serial.print('\t');
      Serial.print(pit_med,5); Serial.print('\t');
      Serial.print(pit_mean,5); Serial.print('\t');
      Serial.println(pit_std,5);
      Serial.print("Roll_Min, Max, Median, Mean, Std:");
      Serial.print(save_buff[30],5); Serial.print('\t');
      Serial.print(save_buff[31],5); Serial.print('\t');
      Serial.print(roll_med,5); Serial.print('\t');
      Serial.print(roll_mean,5); Serial.print('\t');
      Serial.println(save_buff[32],5);  
  }
  
  void clear_buffer(){
      samples_ax.clear();       
      samples_ay.clear();
      samples_az.clear();         
      samples_gx.clear();       
      samples_gy.clear();
      samples_gz.clear();
      samples_pit.clear();
      samples_roll.clear(); 
      i=0;
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
