/*
  EXAMPLE - MUX Sensing with I2CLCD and EEPROM (BLE MIDI)

  With this sample code, you can make the octapad with MUX(74HC4051).
  And you can set the value of sensitivity etc.. with LCD I2C.
  https://open-e-drums.tumblr.com/
*/

/* NOTICE

  You have to install the LiquidCrystal_I2C library.
  LiquidCrystal_I2C : https://github.com/johnrickman/LiquidCrystal_I2C

*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <hellodrum.h>
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

/*
Mux Breakout ----------- ESP32
S0 ------------------- 4
S1 ------------------- 16
S2 ------------------- 17
Z -------------------- 33
 */
//Define MUX Pins
HelloDrumMUX mux(4, 16, 2, 36);

//Please name your piezo.
HelloDrum pad1(0); //mux pin
HelloDrum pad2(1); //mux pin
HelloDrum pad3(2); //mux pin
HelloDrum pad4(3); //mux pin
HelloDrum pad5(4); //mux pin
HelloDrum pad6(5); //mux pin
HelloDrum pad7(6); //mux pin
HelloDrum pad8(7); //mux pin
//HelloDrum snare(4, 2);

//Define control buttons
HelloDrumButton button(12, 14, 27, 26, 25); //(EDIT,UP,DOWN,NEXT,BACK)

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
  lcd.init(); // initialize the lcd
  lcd.backlight();

  //if you use ESP32, you have to uncomment the next line.
  EEPROM_ESP.begin(512);

  pad1.settingName("PAD 1");
  pad2.settingName("PAD 2");
  pad3.settingName("PAD 3");
  pad4.settingName("PAD 4");
  pad5.settingName("PAD 5");
  pad6.settingName("PAD 6");
  pad7.settingName("PAD 7");
  pad8.settingName("PAD 8");
  //snare.settingName("SNARE");

  pad1.loadMemory();
  pad2.loadMemory();
  pad3.loadMemory();
  pad4.loadMemory();
  pad5.loadMemory();
  pad6.loadMemory();
  pad7.loadMemory();
  pad8.loadMemory();
  //snare.loadMemory();

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
  bool buttonPush = button.GetPushState();
  bool editStart = button.GetEditState();
  bool editDone = button.GetEditdoneState();
  char *padName = button.GetPadName();
  char *item = button.GetSettingItem();
  int settingValue = button.GetSettingValue();

  button.readButtonState();

  pad1.settingEnable();
  pad2.settingEnable();
  pad3.settingEnable();
  pad4.settingEnable();
  pad5.settingEnable();
  pad6.settingEnable();
  pad7.settingEnable();
  pad8.settingEnable();
  //snare.settingEnable();

  if (buttonPush == true)
  {
    lcd.clear();
    lcd.print(padName);
    lcd.setCursor(0, 1);
    lcd.print(item);
    lcd.setCursor(13, 1);
    lcd.print(settingValue);
  }

  if (editStart == true)
  {
    lcd.clear();
    lcd.print("EDIT START");
    delay(500);
    lcd.clear();
    lcd.print(padName);
    lcd.setCursor(0, 1);
    lcd.print(item);
    lcd.setCursor(13, 1);
    lcd.print(settingValue);
  }

  if (editDone == true)
  {
    lcd.clear();
    lcd.print("EDIT DONE");
    delay(500);
    lcd.clear();
    lcd.print(padName);
    lcd.setCursor(0, 1);
    lcd.print(item);
    lcd.setCursor(13, 1);
    lcd.print(settingValue);
  }

  //Sensing and Sending MIDI
  if (deviceConnected)
  {
    //scanning 8 piezos.
    mux.scan();

    //Piezo sensing is done in this line. And it is returned as a velocity of 127 stages.
    //For each piezo, one line is required.
    pad1.singlePiezoMUX();
    pad2.singlePiezoMUX();
    pad3.singlePiezoMUX();
    pad4.singlePiezoMUX();
    pad5.singlePiezoMUX();
    pad6.singlePiezoMUX();
    pad7.singlePiezoMUX();
    pad8.singlePiezoMUX();
    //snare.dualPiezoMUX();

    if (pad1.hit == true)
    {
      // noteOn
      midiPacket[2] = 0x90;                     // note down, channel 0
      midiPacket[3] = pad1.note;                //snare note is 38
      midiPacket[4] = pad1.velocity;            // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80;                     // note up, channel 0
      midiPacket[3] = pad1.note;                //snare note is 38
      midiPacket[4] = 0;                        // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();

      int velocity = button.GetVelocity();
      char *hitPad = button.GetHitPad();
      lcd.clear();
      lcd.print(hitPad);
      lcd.setCursor(0, 1);
      lcd.print(velocity);
    }

    if (pad2.hit == true)
    {
      // noteOn
      midiPacket[2] = 0x90;                     // note down, channel 0
      midiPacket[3] = pad2.note;                //snare note is 38
      midiPacket[4] = pad2.velocity;            // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80;                     // note up, channel 0
      midiPacket[3] = pad2.note;                //snare note is 38
      midiPacket[4] = 0;                        // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();

      int velocity = button.GetVelocity();
      char *hitPad = button.GetHitPad();
      lcd.clear();
      lcd.print(hitPad);
      lcd.setCursor(0, 1);
      lcd.print(velocity);
    }

    if (pad3.hit == true)
    {
      // noteOn
      midiPacket[2] = 0x90;                     // note down, channel 0
      midiPacket[3] = pad3.note;                //snare note is 38
      midiPacket[4] = pad3.velocity;            // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80;                     // note up, channel 0
      midiPacket[3] = pad3.note;                //snare note is 38
      midiPacket[4] = 0;                        // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();

      int velocity = button.GetVelocity();
      char *hitPad = button.GetHitPad();
      lcd.clear();
      lcd.print(hitPad);
      lcd.setCursor(0, 1);
      lcd.print(velocity);
    }

    if (pad4.hit == true)
    {
      // noteOn
      midiPacket[2] = 0x90;                     // note down, channel 0
      midiPacket[3] = pad4.note;                //snare note is 38
      midiPacket[4] = pad4.velocity;            // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80;                     // note up, channel 0
      midiPacket[3] = pad4.note;                //snare note is 38
      midiPacket[4] = 0;                        // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();

      int velocity = button.GetVelocity();
      char *hitPad = button.GetHitPad();
      lcd.clear();
      lcd.print(hitPad);
      lcd.setCursor(0, 1);
      lcd.print(velocity);
    }

    if (pad5.hit == true)
    {
      // noteOn
      midiPacket[2] = 0x90;                     // note down, channel 0
      midiPacket[3] = pad5.note;                //snare note is 38
      midiPacket[4] = pad5.velocity;            // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80;                     // note up, channel 0
      midiPacket[3] = pad5.note;                //snare note is 38
      midiPacket[4] = 0;                        // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();

      int velocity = button.GetVelocity();
      char *hitPad = button.GetHitPad();
      lcd.clear();
      lcd.print(hitPad);
      lcd.setCursor(0, 1);
      lcd.print(velocity);
    }

    if (pad6.hit == true)
    {
      // noteOn
      midiPacket[2] = 0x90;                     // note down, channel 0
      midiPacket[3] = pad6.note;                //snare note is 38
      midiPacket[4] = pad6.velocity;            // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80;                     // note up, channel 0
      midiPacket[3] = pad6.note;                //snare note is 38
      midiPacket[4] = 0;                        // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();

      int velocity = button.GetVelocity();
      char *hitPad = button.GetHitPad();
      lcd.clear();
      lcd.print(hitPad);
      lcd.setCursor(0, 1);
      lcd.print(velocity);
    }

    if (pad7.hit == true)
    {
      // noteOn
      midiPacket[2] = 0x90;                     // note down, channel 0
      midiPacket[3] = pad7.note;                //snare note is 38
      midiPacket[4] = pad7.velocity;            // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80;                     // note up, channel 0
      midiPacket[3] = pad7.note;                //snare note is 38
      midiPacket[4] = 0;                        // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();

      int velocity = button.GetVelocity();
      char *hitPad = button.GetHitPad();
      lcd.clear();
      lcd.print(hitPad);
      lcd.setCursor(0, 1);
      lcd.print(velocity);
    }

    if (pad8.hit == true)
    {
      // noteOn
      midiPacket[2] = 0x90;                     // note down, channel 0
      midiPacket[3] = pad8.note;                //snare note is 38
      midiPacket[4] = pad8.velocity;            // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80;                     // note up, channel 0
      midiPacket[3] = pad8.note;                //snare note is 38
      midiPacket[4] = 0;                        // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();

      int velocity = button.GetVelocity();
      char *hitPad = button.GetHitPad();
      lcd.clear();
      lcd.print(hitPad);
      lcd.setCursor(0, 1);
      lcd.print(velocity);
    }

    /* 
    if (snare.hit == true)
    {
      // noteOn
      midiPacket[2] = 0x90;                     // note down, channel 0
      midiPacket[3] = snare.note;               //snare note is 38
      midiPacket[4] = snare.velocity;           // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80;                     // note up, channel 0
      midiPacket[3] = snare.note;               //snare note is 38
      midiPacket[4] = 0;                        // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();
    }
    else if (snare.hitRim == true)
    {
      // noteOn
      midiPacket[2] = 0x90;                     // note down, channel 0
      midiPacket[3] = snare.noteRim;            //snare note is 38
      midiPacket[4] = snare.velocity;           // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes
      pCharacteristic->notify();
      // noteOff
      midiPacket[2] = 0x80;                     // note up, channel 0
      midiPacket[3] = snare.noteRim;            //snare note is 38
      midiPacket[4] = 0;                        // velocity
      pCharacteristic->setValue(midiPacket, 5); // packet, length in bytes)
      pCharacteristic->notify();
    }
    */
  }
}