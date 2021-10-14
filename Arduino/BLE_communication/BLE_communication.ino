#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

const char* Peripheralname = "IMU";

const char* ImuUUID = "00001234-0000-1000-8000-00805f9b34fb";
const char* axCharuuid = "00001111-0000-1000-8000-00805f9b34fb";
const char* ayCharuuid = "00002222-0000-1000-8000-00805f9b34fb";
const char* azCharuuid = "00003333-0000-1000-8000-00805f9b34fb";
const char* axDirCharuuid = "00004444-0000-1000-8000-00805f9b34fb";
const char* ayDirCharuuid = "00005555-0000-1000-8000-00805f9b34fb";
const char* azDirCharuuid = "00006666-0000-1000-8000-00805f9b34fb";

BLEService imuService (ImuUUID);
BLEByteCharacteristic axChar(axCharuuid , BLERead | BLENotify | BLEBroadcast);
BLEByteCharacteristic ayChar(ayCharuuid , BLERead | BLENotify | BLEBroadcast);
BLEByteCharacteristic azChar(azCharuuid , BLERead | BLENotify | BLEBroadcast);
BLEByteCharacteristic axDirChar(axDirCharuuid , BLERead | BLENotify | BLEBroadcast);
BLEByteCharacteristic ayDirChar(ayDirCharuuid , BLERead | BLENotify | BLEBroadcast);
BLEByteCharacteristic azDirChar(azDirCharuuid , BLERead | BLENotify | BLEBroadcast);

const char* gxCharuuid = "00006667-0000-1000-8000-00805f9b34fb";
const char* gyCharuuid = "00006668-0000-1000-8000-00805f9b34fb";
const char* gzCharuuid = "00006669-0000-1000-8000-00805f9b34fb";
const char* gxDirCharuuid = "00004445-0000-1000-8000-00805f9b34fb";
const char* gyDirCharuuid = "00005556-0000-1000-8000-00805f9b34fb";
const char* gzDirCharuuid = "00006667-0000-1000-8000-00805f9b34fb";

BLEByteCharacteristic gxChar(gxCharuuid , BLERead | BLENotify | BLEBroadcast);
BLEByteCharacteristic gyChar(gyCharuuid , BLERead | BLENotify | BLEBroadcast);
BLEByteCharacteristic gzChar(gzCharuuid , BLERead | BLENotify | BLEBroadcast);
BLEByteCharacteristic gxDirChar(gxDirCharuuid , BLERead | BLENotify | BLEBroadcast);
BLEByteCharacteristic gyDirChar(gyDirCharuuid , BLERead | BLENotify | BLEBroadcast);
BLEByteCharacteristic gzDirChar(gzDirCharuuid , BLERead | BLENotify | BLEBroadcast);

long previous = 0;  // last time the battery level was checked, in ms

void setup() {
  // Monitor Setup and Check whether connect to IMU
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // BLE Peripheral Setup
  if (!BLE.begin()) { 
    Serial.println("starting BLE failed!");
  }
  BLE.setLocalName(Peripheralname);
  
  BLE.setAdvertisedService(imuService);
  imuService.addCharacteristic(axChar);
  imuService.addCharacteristic(ayChar);
  imuService.addCharacteristic(azChar);
  imuService.addCharacteristic(axDirChar);
  imuService.addCharacteristic(ayDirChar);
  imuService.addCharacteristic(azDirChar);
  imuService.addCharacteristic(gxChar);
  imuService.addCharacteristic(gyChar);
  imuService.addCharacteristic(gzChar);
  imuService.addCharacteristic(gxDirChar);
  imuService.addCharacteristic(gyDirChar);
  imuService.addCharacteristic(gzDirChar);
  BLE.addService(imuService);
  
  // start advertising
  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
 // wait for a BLE central
  BLEDevice central = BLE.central();
 // if a central is connected to the peripheral:
 if (central) {
    Serial.print("Connected to central: ");
    // print the central's BT address:
    Serial.println(central.address());
    while(central.connected()){
    long present = millis();
    if((present - previous) >= 200){
        updateAcc();
        updateGyro();
     previous = present;
    }    
    }
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
 }
 
}


void updateAcc(){
  float ax, ay, az;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ax, ay, az);
    Serial.print("ax:");
    Serial.print(ax);
    Serial.print('\t');
    Serial.print("ay:");
    Serial.print(ay);
    Serial.print('\t');
    Serial.print("az:");
    Serial.print(az);
    Serial.print('\t');
    if(ax<0){
    ax = (-ax)*100;
    axDirChar.writeValue(255);
    }else{
    ax = ax*100;
    axDirChar.writeValue(0);
    }
    
    if(ay<0){
    ay = (-ay)*100;
    ayDirChar.writeValue(255);  
    }else{
    ay = ay*100;
    ayDirChar.writeValue(0); 
    }

    if(az<0){
    az = (-az)*100;
    azDirChar.writeValue(255);  
    }else{
    az = az*100;
    azDirChar.writeValue(0); 
    }
   
    axChar.writeValue(ax);
    ayChar.writeValue(ay);
    azChar.writeValue(az);
  }  
}

void updateGyro(){
  float gx,gy,gz;
  
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(gx, gy, gz);
    Serial.print("gx:");
    Serial.print(gx);
    Serial.print('\t');
    Serial.print("gy:");
    Serial.print(gy);
    Serial.print('\t');
    Serial.print("gz:");
    Serial.println(gz);
    if(gx<0){
    gx = -gx;
    gxDirChar.writeValue(255);
    }else{
    gxDirChar.writeValue(0);
    }
    if(gy<0){
    gy = -gy;
    gyDirChar.writeValue(255);
    }else{
    gyDirChar.writeValue(0);
    }
    if(gz<0){
    gz = -gz;
    gzDirChar.writeValue(255);
    }else{
    gzDirChar.writeValue(0);
    }
    gxChar.writeValue(gx);
    gyChar.writeValue(gy);
    gzChar.writeValue(gz);
  }
}
