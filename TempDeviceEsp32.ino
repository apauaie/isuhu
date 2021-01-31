
         // Modificado por Juan A. Villalpando
  // http://kio4.com/arduino/160i_Wemos_ESP32_BLE.htm


  #include <BLEDevice.h>
  #include <BLEServer.h>
  #include <BLEUtils.h>
  #include <BLE2902.h>
  #define IRTrigger 12 //  pin12 PushButton
  #include <Adafruit_MLX90614.h>
  #include <IRremote.h>
  #include <Adafruit_NeoPixel.h>

    int freq = 38000;
    int channel = 0;
    int resolution = 8;
    
 //   #define PIN_IR 3   for testing
   #define PIN_DETECT 34
   #define PIN_STATUS 2
   #define PIN        25
   #define NUMPIXELS 1
   #define Pushbutton    18

 Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

 Adafruit_MLX90614 mlx = Adafruit_MLX90614();

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
       pixels.clear();
      pixels.setPixelColor(0, pixels.Color(0, 200, 0));
      pixels.show();
     delay(1000);

  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
      Serial.print("Bluetooth Disconnected");
       pixels.clear();
       pixels.setPixelColor(0, pixels.Color(200, 0, 0));
       pixels.show();
      delay(1000);
  }
 };

  void setup() {

  Serial.begin(115200);
  Serial.println("JomHadir iSuhu001 1.0.1");
  pinMode(PIN_DETECT, INPUT);
  pinMode(PIN_STATUS, OUTPUT);
  pinMode(IRTrigger, OUTPUT);
  pinMode(Pushbutton, INPUT);
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(IRTrigger, channel);
  ledcWriteTone(channel, 38000);
  
    mlx.begin(); 
   // Create the BLE Device
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
    
    pixels.begin();
    pixels.setPixelColor(0, pixels.Color(200, 200, 200));
      pixels.show();
    
}

  

  void loop() {
  // notify changed value
     Serial.println(digitalRead(PIN_DETECT));
     Serial.print("Pushbutton ");
     Serial.println(digitalRead(Pushbutton));
    String readString;
  while (Serial.available())
  {
    if (Serial.available() >0)
    {
      char c = Serial.read();
      readString += c;
    }
        //BTSerial.println(readString);

  }
  int x=readString.lastIndexOf("T body = ");


     
  if (deviceConnected) {
    pixels.clear();
    pixels.setPixelColor(0, pixels.Color(0, 0, 200));
    pixels.show();  
  String val =readString.substring(x+9,x+14);
    if (x!=-1)
    {
      if(val.toFloat()<39.0 && val.toFloat()>35.0){
         output=val;
      }
        pCharacteristic->setValue(output.c_str()); // Output
        pCharacteristic->notify();   
        pixels.clear();
        pixels.setPixelColor(0, pixels.Color(200, 0, 0));
        pixels.show();   
        delay(1000);   
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
  }
