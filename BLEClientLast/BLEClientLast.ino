/**
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */
//6.40 A 6.6
//ABAJO DE 6.1 ESTA MUERTO

#include "BLEDevice.h"
//#include "BLEScan.h"

//Variables PH
const int phpin = 34;
const int ledpinconnected = 5;
const int ledphred = 18; 
const int ledphgreen = 19;
int sensorValue = 0; 
unsigned long int avgValue; 
float b;
int buf[10],temp;

///////////////////////////

// The remote service we wish to connect to.
static BLEUUID serviceUUID("3f7b3140-002e-11ea-8d71-362b9e155667");
// The characteristic of the remote service we are interested in.
static BLEUUID charUUID((uint16_t)0x0215);

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    // Read the value of the characteristic.
    if(pRemoteCharacteristic->canRead()) {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
    }

    if(pRemoteCharacteristic->canNotify())
      pRemoteCharacteristic->registerForNotify(notifyCallback);
    Serial.println("DONE CONNECTING");
    connected = true;
    return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.println("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {

      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;

    } // Found our server
  } // onResult
}; // MyAdvertisedDeviceCallbacks

byte* serialFloatPrint(float f) {
  byte * b = (byte *) &f;
  Serial.write(b[0]);
  Serial.write(b[1]);
  Serial.write(b[2]);
  Serial.write(b[3]);
  return b;
}

byte* phget() {
  char* phbuffer;
  int buf[20];
  int toAvg = 0;
  for(int i = 0; i<20;i++)
  {
    buf[i] = analogRead(phpin);
    delay(50);
  }
  for(int i = 0; i<20;i++)
  {
    toAvg += buf[i];
  }
  
  /*
  for(int i=0;i<10;i++) 
 { 
  buf[i]=analogRead(phpin);
  delay(10);
 }
 for(int i=0;i<9;i++)
 {
  for(int j=i+1;j<10;j++)
  {
   if(buf[i]>buf[j])
   {
    temp=buf[i];
    buf[i]=buf[j];
    buf[j]=temp;
   }
  }
 }
 avgValue=0;
 for(int i=2;i<8;i++)
 avgValue+=buf[i];*/
 avgValue = toAvg/20;
 Serial.println(avgValue);
 float pHVol=(float)(avgValue)*5.0/1024/6;
 
 float phValue = -5.70 * pHVol + 21.34;
 Serial.println("BEFORE CRAASH");
 //dtostrf(phValue,4,3,phbuffer);
/*                    |         |             |               \_ buffer donde almacenaremos la cadena
                      |         |             \_ Precisión (nº decimales)
                      |         \_Tamaño del número en carácteres
                      \_ Número a convertir
                  */
  byte * b = (byte *) &phValue;
  Serial.write("sensor = ");
  Serial.println(phValue);
  //Serial.write(phValue);
  Serial.write("\r\n");
  delay(5000);
  
  return b;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  pinMode(34, INPUT);
  pinMode(5, OUTPUT);
  pinMode(18, OUTPUT);
  pinMode (19, OUTPUT);
  

  
  BLEDevice::init("TPC");

  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
 // pBLEScan->setInterval(1349);
 // pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
} // End of setup.

void checkph(byte* ph)
{
  if((*((float*)&ph))<6.1)
  {
    digitalWrite(ledphred, HIGH);
  }
  else if(((*((float*)&ph))>6.1))
  {
    digitalWrite(ledphgreen, HIGH);
  }
}
// This is the Arduino main loop function.
void loop() {
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    phget();
    Serial.println(String((char*)phget()));
    //(String((char*)phget()))
    String newValue = "Prueba";
    pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
    digitalWrite(ledpinconnected, HIGH);
/*
    //String(millis()/1000
    Serial.println("Setting new characteristic value to \"" + newValue + "\"");
    
    // Set the characteristic's value to be the array of bytes that is actually a string.
    */
  }
  else if(doScan){
    BLEDevice::getScan()->start(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
  }
  
  delay(2000); // Delay a second between loops.
} // End of loop
