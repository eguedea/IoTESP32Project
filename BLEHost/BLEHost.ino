/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleWrite.cpp
    Ported to Arduino ESP32 by Evandro Copercini
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <WiFi.h>
#include <HTTPClient.h>
 
// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "3f7b3140-002e-11ea-8d71-362b9e155667"
#define CHARACTERISTIC_UUID "00000215-0000-1000-8000-00805f9b34fb"
//const char* ssid = "IoT";
const char* ssid = "iPhone de Eric";
//const char* password =  "1t3s0IoT18";
const char* password =  "eguedea223";
const int ledwifi = 5;
const int ledble = 18; 
void sendata(String data)
{
 // String fullname = "pepe";
  //String email = "aaaaaaa@hotmail.com";
  String id = "'5da8cf331c9d440000ef1319'";
  int httpResponseCode;
  Serial.println(data+"\n");
  //tuppersens.us-west-2.elasticbeanstalk.com
 // http://tuppersens.us-west-2.elasticbeanstalk.com/api/servicesiot/statustopper
 //"http://172.20.10.3/IoT/api/servicesiot/statustopper
  String url ="http://172.20.10.3/IoT/api/servicesiot/statustopper";
  String urlget;

  String urlfinal;
  HTTPClient http;
  //fullname"+fullname+"&email"+email+"
  urlfinal = url;//"?data"+data;
  http.begin(urlfinal);
  http.addHeader("Content-Type", "application/json");            
 //{fullname:'pepe',email:'aaaaaaa@hotmail.com'}
  httpResponseCode = http.POST("{id:"+id+",ph:"+data+"}");
  //("{id:"+id+",ph:"+data+"}");
 // int httpCode = http.GET();
    if(httpResponseCode>0){
 
    String response = http.getString();   
 
   Serial.println(httpResponseCode);
   Serial.println(response);          
 
   }else{
 
    Serial.print("Error on sending PUT Request: ");
    Serial.println(httpResponseCode);
 
   }

 
  http.end(); 
 
  delay(30000);
 
}

class MyCallbacks: public BLECharacteristicCallbacks {
    
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      Serial.println("CallbackOnWrite");
      Serial.println(pCharacteristic->getValue().c_str());
      sendata( pCharacteristic->getValue().c_str());
 
    }
     void onRead(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      Serial.println("CallbackOnRead");
      Serial.println(pCharacteristic->getValue().c_str());
      sendata(pCharacteristic->getValue().c_str());
    }

    
};

void setup() {
  Serial.begin(115200);

//Connecting to Wifi
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
 //   Serial.println("Connecting to WiFi..");
  }
  
  Serial.println("Connected to the WiFi network");

  //END WIFI CONNECTION
  //STARTING BT CONNECTION

  BLEDevice::init("TH");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
                                    

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Waiting for value");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Ready to be connected to.");
  //END BT CONNECTION

}
     
void loop() {
  // put your main code here, to run repeatedly:

  delay(1000);
}
