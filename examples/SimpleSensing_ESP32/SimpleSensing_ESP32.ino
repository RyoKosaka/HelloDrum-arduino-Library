/*
  EXAMPLE - Simple Piezo Sensing With ESP32(BLE MIDI)

  With this sample code, you will make snare and tom using two piezo.
  And send MIDI signals with bluetooth. 
  https://open-e-drums.tumblr.com/
*/

///////////////////////////////   SETTING VALUE   ///////////////////////////////////

//Determine the setting value.
//By changing the number in this array you can set sensitivity, threshold and so on.

int SNARE[5] = {
    100, //sensitivity (1-100)
    10,  //threshold (1-100)
    50,  //scan time (1-)
    10,  //mask time (1-)
    38   //note (0-127)
};

int TOM[5] = {
    100, //sensitivity (1-100)
    10,  //threshold (1-100)
    50,  //scan time (1-)
    10,  //mask time (1-)
    50   //note (0-127)
};

/////////////////////////////////////////////////////////////////////////////////////

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <hellodrum.h>

//Please name your piezo.
//The piezo named snare is connected to the IO33 pin and the piezo named tom is connected to the IO1 pin.

HelloDrum snare(33);
HelloDrum tom(36);

///////////////////////////////   BLE MIDI SETTING   ///////////////////////////////////

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

#define MIDI_SERVICE_UUID "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define MIDI_CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"

uint8_t midiPacket[] = {
    0x80, // header
    0x80, // timestamp, not implemented
    0x00, // status
    0x3c, // 0x3c == 60 == middle c == note
    0x00  // velocity
};

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
  }
};

/////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  ///////////////////////////////   BLE MIDI INITIALIZE   ///////////////////////////////////

  BLEDevice::init("DRUM"); //Device Name

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEDevice::setEncryptionLevel((esp_ble_sec_act_t)ESP_LE_AUTH_REQ_SC_BOND);

  // Create the BLE Service
  BLEService *pService = pServer->createService(BLEUUID(MIDI_SERVICE_UUID));

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
      BLEUUID(MIDI_CHARACTERISTIC_UUID),
      BLECharacteristic::PROPERTY_READ |
          BLECharacteristic::PROPERTY_NOTIFY |
          BLECharacteristic::PROPERTY_WRITE_NR);
  pCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
  pSecurity->setCapability(ESP_IO_CAP_NONE);
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

  pServer->getAdvertising()->addServiceUUID(MIDI_SERVICE_UUID);
  pServer->getAdvertising()->start();

  /////////////////////////////////////////////////////////////////////////////////////
}

void loop()
{

  if (deviceConnected)
  {

    //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
    //For each piezo, one line is required.
    snare.singlePiezo(SNARE[0], SNARE[1], SNARE[2], SNARE[3]);
    tom.singlePiezo(TOM[0], TOM[1], TOM[2], TOM[3]);

    if (snare.hit == true)
    {
      // noteOn
      midiPacket[2] = 0x90;                     // note down, channel 0
      midiPacket[3] = SNARE[4];                 //snare note
      midiPacket[4] = snare.velocity;           // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80;                     // note up, channel 0
      midiPacket[3] = SNARE[4];                 //snare note
      midiPacket[4] = 0;                        // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();
    }

    if (tom.hit == true)
    {
      // noteOn
      midiPacket[2] = 0x90;                     // note down, channel 0
      midiPacket[3] = TOM[4];                   // tom note
      midiPacket[4] = tom.velocity;             // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80;                     // note up, channel 0
      midiPacket[3] = TOM[4];                   // tom note
      midiPacket[4] = 0;                        // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();
    }
  }
}