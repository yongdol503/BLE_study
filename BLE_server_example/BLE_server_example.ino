#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID        "65709da4-67fd-4002-a7fe-6c6667e49b5f"
#define CHARACTERISTIC_UUID "1719703f-847c-4624-8b0f-ac358ee695b3"

const int LED = 5;
const int LED2 = 19;
bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      digitalWrite(LED,HIGH);
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      digitalWrite(LED,LOW);
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);
        Serial.println();

        if (value.find("ON") != -1) {
          Serial.println("Turning ON!");
          digitalWrite(LED, LOW);
        }
        else if (value.find("OFF") != -1) {
          Serial.println("Turning OFF!");
          digitalWrite(LED, HIGH);
        }
        
        Serial.println("*********");
      }
      
    }
};

void setup() {
  Serial.begin(115200);

  pinMode(LED,OUTPUT);
  pinMode(LED2,OUTPUT);
  digitalWrite(LED,HIGH);
  //digitalWrite(LED2,HIGH);

  BLEDevice::init("MY ESP32 server yyg");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("INPUT ON or OFF");
  pService->start();

  //BLEAdvertising *pAdvertising = pServer->getAdvertising();
  //pAdvertising->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(deviceConnected){
    Serial.println("connected!");
  }
  delay(2000);
}
