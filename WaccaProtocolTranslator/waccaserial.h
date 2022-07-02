#ifndef _WACCA_SERIAL_
#define _WACCA_SERIAL_

#include "pico.h"

// Massive thank you to the contributors for the WACVR project <3

//#define DEBUG_MODE // uncomment for debug messages (note: this will slow things down!)

//#define RIGHT //uncomment to enable righty flip on interface and sync board data swap

#if OPT_RIGHT // used for automatic compilation
    #ifndef RIGHT
        #define RIGHT
    #endif
#endif

#define CMD_GET_SYNC_BOARD_VER 0xa0
#define CMD_NEXT_READ 0x72
#define CMD_GET_UNIT_BOARD_VER 0xa8
#define CMD_MYSTERY1 0xa2
#define CMD_MYSTERY2 0x94
#define CMD_START_AUTO_SCAN 0xc9
#define CMD_BEGIN_WRITE 0x77
#define CMD_NEXT_WRITE 0x20

static const uint8_t SYNC_BOARD_VER[6] = "190523";
static const uint8_t UNIT_BOARD_VER[6] = "190514";
static const uint8_t read1[80] = "    0    0    1    2    3    4    5   15   15   15   15   15   15   11   11   11";
static const uint8_t read2[80] = "   11   11   11  128  103  103  115  138  127  103  105  111  126  113   95  100";
static const uint8_t read3[80] = "  101  115   98   86   76   67   68   48  117    0   82  154    0    6   35    4";

static const uint8_t SettingData_160[8] = { 160, 49, 57, 48, 53, 50, 51, 44 };
static const uint8_t SettingData_162[7] = { 162, 63, 29, 0, 0, 0, 0 };
static const uint8_t SettingData_148[7] = { 148, 0, 20, 0, 0, 0, 0 };
static const uint8_t SettingData_201[7] = { 201, 0, 73, 0, 0, 0, 0 };

extern uint8_t TouchPack[36];
extern bool StartUp;
extern uint8_t inByte;

void ws_start(); //public

void ws_touchThreadLoop();

void ws_sendTouchState();

void ws_readHead();

void ws_sendResp(char importantNextReadData);

void ws_getTouchPack();

void ws_sendTouch();

void ws_setTouch0Data(uint32_t touchSegment0); //public

void ws_setTouch1Data(uint32_t touchSegment1); //public

void ws_setTouch2Data(uint32_t touchSegment2); //public

void ws_setTouch3Data(uint32_t touchSegment3); //public

void ws_setTouch4Data(uint32_t touchSegment4); //public

void ws_setTouch5Data(uint32_t touchSegment5); //public

void ws_setTouchData(uint32_t touchSegment0, uint32_t touchSegment1, uint32_t touchSegment2, uint32_t touchSegment3, uint32_t touchSegment4, uint32_t touchSegment5); //public

inline uint8_t bh_calcChecksum(uint8_t * bytes, int length) {
    uint8_t checkSum = 0x00;

    for (int i = 0; i < length; i++) {
        checkSum ^= *(bytes + i);
    }

    return checkSum;
}

#endif