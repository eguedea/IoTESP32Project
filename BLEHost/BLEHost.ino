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
#define CHARACTERISTIC_UUID "3c14101f-3139-4887-9c6f-dd60f1562be5"
const char* ssid = "IoT";
const char* password =  "1t3s0IoT18";
 
void sendata(String data)
{
 // String fullname = "pepe";
  //String email = "aaaaaaa@hotmail.com";
  int httpResponseCode;
  //tuppersens.us-west-2.elasticbeanstalk.com
  String url = "http://148.201.214.14/iot/api/servicesiot/dummydata";
  String urlget;
  String urlfinal;
  HTTPClient http;
  //fullname"+fullname+"&email"+email+"
  urlfinal = url;//"?data"+data;
  http.begin(urlfinal);
  http.addHeader("Content-Type", "application/json");            
 //{fullname:'pepe',email:'aaaaaaa@hotmail.com'}
  httpResponseCode = http.POST("{data:'"+data+"'}"); 
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
      sendata(pCharacteristic->getValue().c_str());

     /* if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);

        Serial.println();
        Serial.println("*********");
      }*/
    }
    
};

void setup() {
  Serial.begin(115200);

     Serial.println("IIIIIIIIIIN");
//Connecting to Wifi
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
 //   Serial.println("Connecting to WiFi..");
  }
  
  Serial.println("Connected to the WiFi network");

  //END WIFI CONNECTION
  //STARTING BT CONNECTION

  BLEDevice::init("TupperHost");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello World");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
  //END BT CONNECTION

}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
}
