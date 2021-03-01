
         // Modificado por Juan A. Villalpando
  // http://kio4.com/arduino/160i_Wemos_ESP32_BLE.htm


  #include <BLEDevice.h>
  #include <BLEServer.h>
  #include <BLEUtils.h>
  #include <BLE2902.h>
  



  String output ="LOW"; // Output to App. Notify.

  BLEServer* pServer = NULL;
  BLECharacteristic* pCharacteristic = NULL;
  bool deviceConnected = false;
  bool oldDeviceConnected = false;
  uint32_t value = 0;

  #define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
  #define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

  class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
     Serial.print("Bluetooth Connected");

     delay(1000);

  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
      Serial.print("Bluetooth Disconnected");

      delay(1000);
  }
 };

  void setup() {

  Serial.begin(115200);
  Serial.println("JomHadir iSuhu001 1.0.2");

   BLEDevice::init("iSuhu108");
   
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pCharacteristic = pService->createCharacteristic(
                    CHARACTERISTIC_UUID,
                    BLECharacteristic::PROPERTY_READ   |
                    BLECharacteristic::PROPERTY_WRITE  |
                    BLECharacteristic::PROPERTY_NOTIFY |
                    BLECharacteristic::PROPERTY_INDICATE
                  );

    // Create a BLE Descriptor
    pCharacteristic->addDescriptor(new BLE2902());

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    Serial.println("Waiting a client connection to notify...");
    

    
}

  

    
  String val;

  
  void loop() {
  String readString;
  while (Serial.available())
  {
    if (Serial.available() >0)
    {
      char c = Serial.read();

      readString += c;
      if (c==0x6f)  val="35.6";
  else if (c==0x70)  val="35.7";
  else if (c==0x71)  val="35.8";
  else if (c==0x72)  val="35.9";
  else if (c==0x73)  val="36.0";
  else if (c==0x74)  val="36.1";
  else if (c==0x75)  val="36.2";
  else if (c==0x76)  val="36.3";
  else if (c==0x77)  val="36.4";
  else if (c==0x78)  val="36.5";
  else if (c==0x79)  val="36.6";
  else if (c==0x7A)  val="36.7";
  else if (c==0x7B)  val="36.8";
  else if (c==0x7C)  val="36.9";
  else if (c==0x7D)  val="37.0";
  else if (c==0x7E)  val="37.1";
  else if (c==0x7F)  val="37.2";
  else if (c==0x80)  val="37.3";
  else if (c==0x81)  val="37.4";
  else if (c==0x82)  val="37.5";
  else  val="0.00";
  Serial.println(val);
    }
  }
  
     
  if (deviceConnected) {
  

    if (val!="0.00")
    {
        output=val;
        pCharacteristic->setValue(output.c_str()); // Output
        pCharacteristic->notify();
        val="0.00";       
        }
     }
   
  delay(100); // bluetooth stack will go into congestion, if too many packets are sent.       
  
  
  if (!deviceConnected && oldDeviceConnected) {
      delay(500); // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
      Serial.println("start advertising");
      oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
      // do stuff here on connecting
      oldDeviceConnected = deviceConnected;
  }

  delay(1);
  }
