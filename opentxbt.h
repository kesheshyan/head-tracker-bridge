#include <ArduinoBLE.h>
#pragma once

#define BT_CHANNEL_NUM                   8
#define BLUETOOTH_LINE_LENGTH           32
#define BLUETOOTH_PACKET_SIZE           14

void processTrainerByte(uint8_t data);
extern uint16_t BtChannelsIn[BT_CHANNEL_NUM];

enum {
  STATE_DATA_IDLE,
  STATE_DATA_START,
  STATE_DATA_XOR,
  STATE_DATA_IN_FRAME
};
