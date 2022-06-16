#include <ArduinoBLE.h>
#include <PPMReader.h>
#include "PPMEncoder.h"
#include "BBTimer.h"
#include "opentxbt.h"

#define PPM_INPUT_PIN 9
#define PPM_OUTPUT_PIN 10

#define RECEIVER_CHANNEL_NUM 3
#define HEAD_TRACKER_CHANNEL_NUM 3

typedef union
{
  uint8_t bytes[BT_CHANNEL_NUM * 2];
} ppm_data_ut;

ppm_data_ut ppmData;
PPMReader ppmReader(PPM_INPUT_PIN, PPM_DEFAULT_CHANNELS);
BBTimer ppmInputTimer(BB_TIMER3);

const char* deviceServiceUuid = "FFF0";
const char* deviceServiceCharacteristicUuid = "FFF6";


void setup() {
  Serial.begin(9600);

  // Setup PPM decoder
  ppmInputTimer.setupTimer(PPM_FRAME_LENGTH_uS, ppmInputTimerCallback);
  ppmInputTimer.timerStart();


  // Setup PPM encoder
  ppmEncoder.begin(PPM_OUTPUT_PIN);
  ppmEncoder.setChannel(1, 2000);
  
  
  if (!BLE.begin()) {
    Serial.println("* Starting Bluetooth® Low Energy module failed!");
    while (1);
  }
  
  BLE.setLocalName("Nano 33 BLE (Central)"); 
  BLE.advertise();

  Serial.println("Arduino Nano 33 BLE Sense (Central Device)");
  Serial.println(" ");
}

void loop() {
  connectToPeripheral();
}

void connectToPeripheral(){
  BLEDevice peripheral;
  
  Serial.println("- Discovering peripheral device...");

  do
  {
    BLE.scanForUuid(deviceServiceUuid);
    peripheral = BLE.available();
  } while (!peripheral);
  
  if (peripheral) {
    Serial.println("* Peripheral device found!");
    Serial.print("* Device MAC address: ");
    Serial.println(peripheral.address());
    Serial.print("* Device name: ");
    Serial.println(peripheral.localName());
    Serial.print("* Advertised service UUID: ");
    Serial.println(peripheral.advertisedServiceUuid());
    Serial.println(" ");
    BLE.stopScan();
    controlPeripheral(peripheral);
  }
}

void controlPeripheral(BLEDevice peripheral) {
  Serial.println("- Connecting to peripheral device...");

  if (peripheral.connect()) {
    Serial.println("* Connected to peripheral device!");
    Serial.println(" ");
  } else {
    Serial.println("* Connection to peripheral device failed!");
    Serial.println(" ");
    return;
  }

  Serial.println("- Discovering peripheral device attributes...");
  if (peripheral.discoverAttributes()) {
    Serial.println("* Peripheral device attributes discovered!");
    Serial.println(" ");
  } else {
    Serial.println("* Peripheral device attributes discovery failed!");
    Serial.println(" ");
    peripheral.disconnect();
    return;
  }
  
  BLECharacteristic frSkyCharacteristic = peripheral.characteristic(deviceServiceCharacteristicUuid);
  if (!frSkyCharacteristic) {
    Serial.println("* Peripheral device does not have frSky_type characteristic!");
    peripheral.disconnect();
    return;
  } else if (!frSkyCharacteristic.canWrite()) {
    Serial.println("* Peripheral does not have a writable frSky_type characteristic!");
    peripheral.disconnect();
    return;
  }
  
  while (peripheral.connected()) {
    if (frSkyCharacteristic.valueUpdated()) {
      frSkyCharacteristic.readValue(ppmData.bytes, sizeof ppmData.bytes);
 
      byte length = frSkyCharacteristic.valueLength();
 
      if (!ppmData.bytes) {
        continue;
      }

      // Simulate sending byte by byte like opentx uses, stores in global
      for(int i = 0; i < length; i++) {
          processTrainerByte(((uint8_t *)ppmData.bytes)[i]);
      }
      
      for (byte channel = RECEIVER_CHANNEL_NUM; channel < RECEIVER_CHANNEL_NUM + HEAD_TRACKER_CHANNEL_NUM; ++channel) {
        ppmEncoder.setChannel(channel, BtChannelsIn[channel - RECEIVER_CHANNEL_NUM]);
      }
    }
  }

  Serial.println("- Peripheral device disconnected!");
}

void ppmInputTimerCallback() {
  // Print latest valid values from all channels
  for (byte channel = 0; channel < RECEIVER_CHANNEL_NUM; ++channel) {
    unsigned value = ppmReader.latestValidChannelValue(channel + 1, 0);
    ppmEncoder.setChannel(channel, value);
  } 
}
