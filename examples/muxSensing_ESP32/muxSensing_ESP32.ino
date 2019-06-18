/*
  EXAMPLE - Simple Piezo Sensing With ESP32(BLE MIDI)

  With this sample code, you can make the octapad with ESP32 and 74HC4051.
  And send MIDI signals with bluetooth. 
  https://open-e-drums.tumblr.com/
*/

///////////////////////////////   INITIAL VALUE   ///////////////////////////////////

//Determine the initial value.
//By changing the number in this array you can set sensitivity, threshold and so on.

int PAD1[5] = {
  3000, //sensitivity
  500, //threshold
  10,   //scan time
  20,  //mask time
  38   //note
};

int PAD2[5] = {
  3000, //sensitivity
  500, //threshold
  10,   //scan time
  20,  //mask time
  36   //note
};

int PAD3[5] = {
  3000, //sensitivity
  500, //threshold
  10,   //scan time
  20,  //mask time
  41   //note
};

int PAD4[5] = {
  3000, //sensitivity
  500, //threshold
  10,   //scan time
  20,  //mask time
  46   //note
};

int PAD5[5] = {
  3000, //sensitivity
  500, //threshold
  10,   //scan time
  20,  //mask time
  47   //note
};

int PAD6[5] = {
  3000, //sensitivity
  500, //threshold
  10,   //scan time
  20,  //mask time
  48   //note
};

int PAD7[5] = {
  3000, //sensitivity
  500, //threshold
  10,   //scan time
  20,  //mask time
  55   //note
};

int PAD8[5] = {
  3000, //sensitivity
  500, //threshold
  10,   //scan time
  20,  //mask time
  84   //note
};

/////////////////////////////////////////////////////////////////////////////////////

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <hellodrum.h>

/*
Mux Breakout ----------- ESP32
S0 ------------------- 4
S1 ------------------- 16
S2 ------------------- 17
Z -------------------- 33
 */
//Define MUX Pins
HelloDrumMUX mux(4,16,17,33);

//Please name your piezo.
//The piezo named snare is connected to the A0 pin and the piezo named tom is connected to the A1 pin.
HelloDrum pad1(0); //mux pin
HelloDrum pad2(1); //mux pin
HelloDrum pad3(2); //mux pin
HelloDrum pad4(3); //mux pin
HelloDrum pad5(4); //mux pin
HelloDrum pad6(5); //mux pin
HelloDrum pad7(6); //mux pin
HelloDrum pad8(7); //mux pin

///////////////////////////////   BLE MIDI SETTING   ///////////////////////////////////

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

#define MIDI_SERVICE_UUID        "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define MIDI_CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"

uint8_t midiPacket[] = {
  0x80,  // header
  0x80,  // timestamp, not implemented
  0x00,  // status
  0x3c,  // 0x3c == 60 == middle c == note
  0x00   // velocity
};

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

/////////////////////////////////////////////////////////////////////////////////////


void setup() {
  //Serial.begin(115200);

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
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_WRITE_NR
                    );
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

void loop() {

  if (deviceConnected) {
    //scanning 8 piezos.
    mux.scan();

    //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
    //For each piezo, one line is required.
    pad1.singlePiezoMUX(PAD1[0], PAD1[1], PAD1[2], PAD1[3]);
    pad2.singlePiezoMUX(PAD2[0], PAD2[1], PAD2[2], PAD2[3]);
    pad3.singlePiezoMUX(PAD3[0], PAD3[1], PAD3[2], PAD3[3]);
    pad4.singlePiezoMUX(PAD4[0], PAD4[1], PAD4[2], PAD4[3]);
    pad5.singlePiezoMUX(PAD5[0], PAD5[1], PAD5[2], PAD5[3]);
    pad6.singlePiezoMUX(PAD6[0], PAD6[1], PAD6[2], PAD6[3]);
    pad7.singlePiezoMUX(PAD7[0], PAD7[1], PAD7[2], PAD7[3]);
    pad8.singlePiezoMUX(PAD8[0], PAD8[1], PAD8[2], PAD8[3]);

    if (pad1.hit == true) {
      // noteOn
      midiPacket[2] = 0x90; // note down, channel 0
      midiPacket[3] = PAD1[4]; //snare note is 38
      midiPacket[4] = pad1.velocity;  // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80; // note up, channel 0
      midiPacket[3] = PAD1[4]; //snare note is 38
      midiPacket[4] = 0;    // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();
    }

    if (pad2.hit == true) {
      // noteOn
      midiPacket[2] = 0x90; // note down, channel 0
      midiPacket[3] = PAD2[4]; //snare note is 38
      midiPacket[4] = pad2.velocity;  // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80; // note up, channel 0
      midiPacket[3] = PAD2[4]; //snare note is 38
      midiPacket[4] = 0;    // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();
    }

    if (pad3.hit == true) {
      // noteOn
      midiPacket[2] = 0x90; // note down, channel 0
      midiPacket[3] = PAD3[4]; //snare note is 38
      midiPacket[4] = pad3.velocity;  // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80; // note up, channel 0
      midiPacket[3] = PAD3[4]; //snare note is 38
      midiPacket[4] = 0;    // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();
    }

    if (pad4.hit == true) {
      // noteOn
      midiPacket[2] = 0x90; // note down, channel 0
      midiPacket[3] = PAD4[4]; //snare note is 38
      midiPacket[4] = pad4.velocity;  // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80; // note up, channel 0
      midiPacket[3] = PAD4[4]; //snare note is 38
      midiPacket[4] = 0;    // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();
    }

    if (pad5.hit == true) {
      // noteOn
      midiPacket[2] = 0x90; // note down, channel 0
      midiPacket[3] = PAD5[4]; //snare note is 38
      midiPacket[4] = pad5.velocity;  // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80; // note up, channel 0
      midiPacket[3] = PAD5[4]; //snare note is 38
      midiPacket[4] = 0;    // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();
    }

    if (pad6.hit == true) {
      // noteOn
      midiPacket[2] = 0x90; // note down, channel 0
      midiPacket[3] = PAD6[4]; //snare note is 38
      midiPacket[4] = pad6.velocity;  // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80; // note up, channel 0
      midiPacket[3] = PAD6[4]; //snare note is 38
      midiPacket[4] = 0;    // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();
    }

    if (pad7.hit == true) {
      // noteOn
      midiPacket[2] = 0x90; // note down, channel 0
      midiPacket[3] = PAD7[4]; //snare note is 38
      midiPacket[4] = pad7.velocity;  // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80; // note up, channel 0
      midiPacket[3] = PAD7[4]; //snare note is 38
      midiPacket[4] = 0;    // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();
    }

    if (pad8.hit == true) {
      // noteOn
      midiPacket[2] = 0x90; // note down, channel 0
      midiPacket[3] = PAD8[4]; //snare note is 38
      midiPacket[4] = pad8.velocity;  // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80; // note up, channel 0
      midiPacket[3] = PAD8[4]; //snare note is 38
      midiPacket[4] = 0;    // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();
    }
  }
}