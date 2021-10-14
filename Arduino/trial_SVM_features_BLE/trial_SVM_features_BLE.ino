#include <Arduino_LSM9DS1.h>
#include <SPI.h>
#include "model3.h" // C code model of SVM converted from jupyter notebook
#include "RunningMedian.h"
#include <ArduinoBLE.h>

#define NUM_SAMPLES 250 // buffer size increased to accomodate low speed
#define NUM_AXES 40 // total number of features

#define SIZE 4


//Debugging system variable definition
int OutState=0;
int LEDPin=8;
int buttonPin=7;
int buttonNew;
int buttonOld=1;
int flag=0;
int control;
bool awake =false;

//Feature definition
float gx, gy, gz, ax, ay, az;
float pitch, roll, yaw;

//Feature vectors and porting the SVM model
float features[NUM_AXES];
Eloquent::ML::Port::SVM clf;

//Moving average filter variable definition
int INDEX = 0;
float axSum,aySum,azSum = 0;
float axReadings[SIZE],ayReadings[SIZE],azReadings[SIZE];
float axAverage, ayAverage, azAverage=0;

//Buffer
int i=0;
float ax_buf[NUM_SAMPLES],ay_buf[NUM_SAMPLES],az_buf[NUM_SAMPLES];
float gx_buf[NUM_SAMPLES],gy_buf[NUM_SAMPLES],gz_buf[NUM_SAMPLES];
float pitch_buf[NUM_SAMPLES],roll_buf[NUM_SAMPLES];

RunningMedian samples_ax = RunningMedian(NUM_SAMPLES);
RunningMedian samples_ay = RunningMedian(NUM_SAMPLES);
RunningMedian samples_az = RunningMedian(NUM_SAMPLES);
RunningMedian samples_gx = RunningMedian(NUM_SAMPLES);
RunningMedian samples_gy = RunningMedian(NUM_SAMPLES);
RunningMedian samples_gz = RunningMedian(NUM_SAMPLES);
RunningMedian samples_pit = RunningMedian(NUM_SAMPLES);
RunningMedian samples_roll = RunningMedian(NUM_SAMPLES);

const char* Peripheralname = "Arm";
const char* ImuUUID = "00001234-0000-1000-8000-00805f9b34fb";
const char* resultCharuuid = "00001111-0000-1000-8000-00805f9b34fb";
const char* uuidOfRxChar = "00002A3D-0000-1000-8000-00805f9b34fb";
const char* startCharuuid = "00002222-0000-1000-8000-00805f9b34fb";
const char* updateCharuuid = "00003333-0000-1000-8000-00805f9b34fb";

// Setup the incoming data characteristic (RX).
const int RX_BUFFER_SIZE = 256;
bool RX_BUFFER_FIXED_LENGTH = false;
 int label_prev = 16;
 
BLEService imuService (ImuUUID);
BLEByteCharacteristic resultChar(resultCharuuid , BLERead | BLENotify | BLEBroadcast);
BLEByteCharacteristic startChar(startCharuuid , BLERead | BLENotify | BLEBroadcast);
BLEByteCharacteristic updateChar(updateCharuuid , BLERead | BLENotify | BLEBroadcast);
BLECharacteristic rxChar(uuidOfRxChar, BLEWriteWithoutResponse | BLEWrite, RX_BUFFER_SIZE, RX_BUFFER_FIXED_LENGTH);


void setup() {
    Serial.begin(9600);
 
    if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
    } else {
      Serial.println("Ready to begin classifying arm exercises");
    }
    // BLE Peripheral Setup
    if (!BLE.begin()) { 
    Serial.println("starting BLE failed!");
    }

    pinMode(buttonPin, INPUT);
      
    BLE.setLocalName(Peripheralname);
    BLE.setAdvertisedService(imuService);
    imuService.addCharacteristic(resultChar);
    imuService.addCharacteristic(startChar);
    imuService.addCharacteristic(updateChar);
    imuService.addCharacteristic(rxChar);
    BLE.addService(imuService);
    // Bluetooth LE connection handlers.
    BLE.setEventHandler(BLEConnected, onBLEConnected);
    BLE.setEventHandler(BLEDisconnected, onBLEDisconnected);
    // Event driven reads.
    rxChar.setEventHandler(BLEWritten, onRxCharValueUpdate); 
    // start advertising
    BLE.advertise();
    // Print out full UUID and MAC address.
    Serial.println("Peripheral advertising info: ");
    Serial.print("Name: ");
    Serial.println(Peripheralname);
    Serial.print("MAC: ");
    Serial.println(BLE.address());
    Serial.print("Service UUID: ");
    Serial.println(imuService.uuid());
    Serial.print("rxCharacteristic UUID: ");
    Serial.println(rxChar);
    Serial.print("txCharacteristics UUID: ");
    Serial.println(resultChar);
    Serial.println("Bluetooth device active, waiting for connections...");
    resultChar.writeValue(16);
    startChar.writeValue(0);
}

void loop() {
  // wait for a BLE central
  BLEDevice central = BLE.central();
  if (central) {
      Serial.print("Connected to central: ");
     // print the central's BT address:
      while(central.connected()){
          buttonNew = digitalRead(buttonPin);
          if((buttonOld==0 && buttonNew==1)){
          if(OutState==0){
          startChar.writeValue(1);
          digitalWrite(LEDPin,HIGH);
          flag=1;
          OutState=1;
          }
          else if (OutState ==1){
          digitalWrite(LEDPin,LOW);
          startChar.writeValue(0);
          flag=0;
          OutState=0;
          //print features vector as input for the model
          //printFeatures();
          //perform classification by the model
          update_features();
          int label;
          label=classify();
          resultChar.writeValue(label);
           if (label_prev == label){
             updateChar.writeValue(1); 
           }
           else{
             updateChar.writeValue(0);
           }
           label_prev = label;
          clearfeaturesBuffer();
          
          Serial.println();   
          }
          }
        if (control == 1 && awake){
           digitalWrite(LEDPin,HIGH);
           awake = false;
           flag=1;
         }
        else if(control == 0 && awake){
          digitalWrite(LEDPin,LOW);
          awake = false;
          flag=0;
          update_features();
          int label;
          label=classify();
          resultChar.writeValue(label);
           if (label_prev == label){
           updateChar.writeValue(1); 
           }
           else{
           updateChar.writeValue(0);
           }
           label_prev = label;
          clearfeaturesBuffer();
         }
        if(flag==1){
         //gather IMU data of exercise movement after pressing the PB
         //perform features calulcation from the recorded IMU data stored in buffer
          recordIMU();
         }
        buttonOld=buttonNew;
        delay(50);
      }
       resultChar.writeValue(16);
    }
  }

//procedure for recording IMU data when pushing PB and storing in buffer
void recordIMU() {
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

/**
 * Dump the feature vector to Serial monitor
 */
void printFeatures() {
    const uint16_t numFeatures = sizeof(features);

    for (int i = 0; i < numFeatures; i++) {
        Serial.print(features[i]);
        Serial.print(i == numFeatures - 1 ? '\n' : ',');
    }
}

// procedure to feed the model and perform classification by the model
int classify() {
    int idx;
    idx = clf.predictIndex(features);
    Serial.print("Detected exercise: ");
    Serial.println(clf.predictLabel(features));
    return idx;
}

void update_features(){
    features[0] = samples_ax.getLowest();
    features[1] = samples_ax.getHighest();
    features[4] = samples_ax.getMedian();
    features[2] = samples_ax.getAverage();
    features[3] = stds(ax_buf,features[2],i);
    features[5] = samples_ay.getLowest();
    features[6] = samples_ay.getHighest();
    features[9] = samples_ay.getMedian();
    features[7] = samples_ay.getAverage();
    features[8] = stds(ay_buf,features[7],i);
    features[10] = samples_az.getLowest();
    features[11] = samples_az.getHighest();
    features[14] = samples_az.getMedian();
    features[12] = samples_az.getAverage();
    features[13] = stds(az_buf,features[12],i);  
    features[15] = samples_gx.getLowest();
    features[16] = samples_gx.getHighest();
    features[19] = samples_gx.getMedian();
    features[17] = samples_gx.getAverage();
    features[18] = stds(gx_buf,features[17],i);     
    features[20] = samples_gy.getLowest();
    features[21] = samples_gy.getHighest();
    features[24] = samples_gy.getMedian();
    features[22] = samples_gy.getAverage();
    features[23] = stds(gy_buf,features[22],i);         
    features[25] = samples_gz.getLowest();
    features[26] = samples_gz.getHighest();
    features[30] = samples_gz.getMedian();
    features[27]= samples_gz.getAverage();
    features[28]= stds(gz_buf,features[27],i);    
    features[29] = samples_pit.getLowest();
    features[31] = samples_pit.getHighest();
    features[34] = samples_pit.getMedian();
    features[32] = samples_pit.getAverage();
    features[33] = stds(pitch_buf,features[32],i);
    features[35] = samples_roll.getLowest();
    features[36] = samples_roll.getHighest();
    features[39] = samples_roll.getMedian();
    features[37] = samples_roll.getAverage();
    features[38] = stds(roll_buf,features[37],i);
}

void clearfeaturesBuffer(){
      //clear out the buffer after 1 exercise movement is recorded and the result is displayed on serial monitor
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
 
// procedure for finding std value
float stds(float val[], float avg, int cnt){
  float total = 0;
  for (int i = 0; i < cnt; i++) {
    total = total + (val[i] - avg) * (val[i] - avg);
  }
  float variance = total/(float)cnt;
  float stdDev = sqrt(variance);
  return stdDev;
}

void onBLEConnected(BLEDevice central) {
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void onBLEDisconnected(BLEDevice central) {
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}

void onRxCharValueUpdate(BLEDevice central, BLECharacteristic characteristic) {
  // central wrote new value to characteristic, update LED
  Serial.print("Characteristic event, read: ");
  byte tmp[256];
  awake = true;
  int dataLength = rxChar.readValue(tmp, 256);
  if (tmp[0] == 1){
  control = 1;
  Serial.print("Control true ");
  }
  else if(tmp[0] == 0){
  control = 0;
  Serial.print("Control false ");
  }
  Serial.println();
  Serial.print("Received = ");
  Serial.println(tmp[0]);
  Serial.print("Value length = ");
  Serial.println(rxChar.valueLength());
}
