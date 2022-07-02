#include "waccaserial.h"

#include "pico.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include "hardware/i2c.h"
#include <stdbool.h>
#include <string.h>
#include "queue.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include "serial.h"

// Massive thank you to the contributors for the WACVR project <3

uint8_t TouchPack[36] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
bool StartUp = false;
uint8_t inByte = 0;

node_t *touchQueueHead = NULL;

uint32_t elapsedTime = 0;
uint32_t previousTime = 0;

bool runningMulticore = false;

#ifdef DEBUG_MODE
    #define led(state) gpio_put(PICO_DEFAULT_LED_PIN, state)
    #define blink() led(1); sleep_ms(25); led(0); sleep_ms(75)
    #define blinkL() led(1); sleep_ms(1000); led(0); sleep_ms(1000)
    #define blink0() led(1); sleep_ms(1); led(0); sleep_ms(75)
#endif

void ws_start() {
    setup();

    #ifdef DEBUG_MODE
        gpio_init(PICO_DEFAULT_LED_PIN);
        gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

        blinkL();
    #endif

    previousTime = to_ms_since_boot(get_absolute_time());
    
    multicore_launch_core1(ws_touchThreadLoop);
    runningMulticore = true;
}

void ws_touchThreadLoop() {
    while (true) {
        #ifdef DEBUG_MODE
            blink0();
        #endif

        update(); // update tinyusb

        uint32_t currentTime = to_ms_since_boot(get_absolute_time());
        elapsedTime += currentTime - previousTime;
        previousTime = currentTime;

        if (elapsedTime >= 10) {
            elapsedTime -= 10;
            enqueue(&touchQueueHead, 1);
        } 

        if (!isEmpty(&touchQueueHead)) {
            dequeue(&touchQueueHead);
            ws_sendTouchState();
        }
        ws_readHead(); 
    }
}

void ws_sendTouchState() {
    if (StartUp) ws_sendTouch();
}

void ws_readHead() {
    inByte = read();

    if (inByte != 0xff && inByte != 0) {
        uint8_t data = 0;
        uint8_t importantData = 0;
        int i = 0;
        do {
            data = read();
            if (i == 2) importantData = data;
            i++;
        } while (data != 0xff && data != 0);

        #ifdef DEBUG_MODE
            blink();
        #endif

        ws_sendResp(importantData);
    }
}

void ws_sendResp(char importantNextReadData) {
    switch (inByte) {
        case CMD_GET_SYNC_BOARD_VER:
            #ifdef DEBUG_MODE
                blink();
            #endif

            StartUp = false;
            
            write(inByte);

            for (int i = 0; i < 6; i++) {
                write(SYNC_BOARD_VER[i]);
            }

            write(44);
            break;
        case CMD_NEXT_READ:
            #ifdef DEBUG_MODE
                blink();
                blink();
            #endif

            StartUp = false;
            switch (importantNextReadData) {
                case 0x30:
                    for (int i = 0; i < 80; i++) {
                        write(read1[i]);
                    }
                    
                    write(bh_calcChecksum((uint8_t *) read1, 80));
                    break;
                case 0x31:
                    for (int i = 0; i < 80; i++) {
                        write(read2[i]);
                    }
                    
                    write(bh_calcChecksum((uint8_t *) read2, 80));
                    break;
                case 0x33:
                    for (int i = 0; i < 80; i++) {
                        write(read3[i]);
                    }
                    
                    write(bh_calcChecksum((uint8_t *) read3, 80));
                    break;
                default:
                    break;
            }
            break;
        case CMD_GET_UNIT_BOARD_VER:
            #ifdef DEBUG_MODE
                blink();
                blink();
                blink();
            #endif

            write(inByte);
            
            for (int i = 0; i < 6; i++) {
                write(SYNC_BOARD_VER[i]);
            }

            #ifdef RIGHT
                write('R');
            #endif
            #ifndef RIGHT
                write('L');
            #endif

            for (int i = 0; i < 6; i++) {
                for (int i = 0; i < 6; i++) {
                    write(UNIT_BOARD_VER[i]);
                }
            }

            #ifdef RIGHT
                write(118);
            #endif
            #ifndef RIGHT
                write(104);
            #endif

            break;
        case CMD_MYSTERY1:
            #ifdef DEBUG_MODE
                blink();
                blink();
                blink();
                blink();
            #endif

            StartUp = false;

            for (int i = 0; i < 7; i++) {
                write(SettingData_162[i]);
            }
            break;
        case CMD_MYSTERY2:
            #ifdef DEBUG_MODE
                blink();
            #endif

            StartUp = false;

            for (int i = 0; i < 7; i++) {
                write(SettingData_148[i]);
            }
            break;
        case CMD_START_AUTO_SCAN:
            #ifdef DEBUG_MODE
                blink();
                blink();
                blink();
                blink();
                blink();
            #endif

            for (int i = 0; i < 7; i++) {
                write(SettingData_201[i]);
            }

            StartUp = true;
            break;
        case CMD_BEGIN_WRITE:
            break;
        case CMD_NEXT_WRITE:
            break;
        case 154:
            StartUp = false;
            break;
    }
    sendRemaining();
}

void ws_getTouchPack() {
    TouchPack[0] = 129;
    TouchPack[34] = TouchPack[34] + 1;
    TouchPack[35] = 128;
    TouchPack[35] = bh_calcChecksum(TouchPack, 36);
    if (TouchPack[34] > 127) TouchPack[34] = 0;
}

void ws_sendTouch() {
    if (StartUp) {
        ws_getTouchPack();

        for (int i = 0; i < 36; i++) {
            write(TouchPack[i]);
        }

        sendRemaining();
    }
}

// since i want the wacca panels to be as simple as possible, i need to reverse the input order on the right side to fit with how the game reads data
#ifdef RIGHT
uint8_t reverseAndShift(uint8_t b) {
    b = ((b & 0xF0) >> 4) | ((b & 0x0F) << 4);
    b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2);
    b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1);
    return b >> 3;
}
#endif

// if im not on on the right then no flip needs to happen so we can just return the byte as normal
#ifndef RIGHT
uint8_t reverseAndShift(uint8_t b) {
    return b;
}
#endif

#define getRow1(segment) reverseAndShift((uint8_t) ((segment & 0b00000000000000000000000000011111)))
#define getRow2(segment) reverseAndShift((uint8_t) ((segment & 0b00000000000000000000001111100000) >> 5))
#define getRow3(segment) reverseAndShift((uint8_t) ((segment & 0b00000000000000000111110000000000) >> 10))
#define getRow4(segment) reverseAndShift((uint8_t) ((segment & 0b00000000000011111000000000000000) >> 15))

void ws_setTouch0Data(uint32_t touchSegment0) {
    TouchPack[6] = getRow1(touchSegment0);
    TouchPack[12] = getRow2(touchSegment0);
    TouchPack[18] = getRow3(touchSegment0);
    TouchPack[24] = getRow4(touchSegment0);
}

void ws_setTouch1Data(uint32_t touchSegment1) {
    TouchPack[5] = getRow1(touchSegment1);
    TouchPack[11] = getRow2(touchSegment1);
    TouchPack[17] = getRow3(touchSegment1);
    TouchPack[23] = getRow4(touchSegment1);
}

void ws_setTouch2Data(uint32_t touchSegment2) {
    TouchPack[4] = getRow1(touchSegment2);
    TouchPack[10] = getRow2(touchSegment2);
    TouchPack[16] = getRow3(touchSegment2);
    TouchPack[22] = getRow4(touchSegment2);
}

void ws_setTouch3Data(uint32_t touchSegment3) {
    TouchPack[3] = getRow1(touchSegment3);
    TouchPack[9] = getRow2(touchSegment3);
    TouchPack[15] = getRow3(touchSegment3);
    TouchPack[21] = getRow4(touchSegment3);
}

void ws_setTouch4Data(uint32_t touchSegment4) {
    TouchPack[2] = getRow1(touchSegment4);
    TouchPack[8] = getRow2(touchSegment4);
    TouchPack[14] = getRow3(touchSegment4);
    TouchPack[20] = getRow4(touchSegment4);
}

void ws_setTouch5Data(uint32_t touchSegment5) {
    TouchPack[1] = getRow1(touchSegment5);
    TouchPack[7] = getRow2(touchSegment5);
    TouchPack[13] = getRow3(touchSegment5);
    TouchPack[19] = getRow4(touchSegment5);
}

void ws_setTouchData(uint32_t touchSegment0, uint32_t touchSegment1, uint32_t touchSegment2, uint32_t touchSegment3, uint32_t touchSegment4, uint32_t touchSegment5) {
    ws_setTouch0Data(touchSegment0);
    ws_setTouch1Data(touchSegment1);
    ws_setTouch2Data(touchSegment2);
    ws_setTouch3Data(touchSegment3);
    ws_setTouch4Data(touchSegment4);
    ws_setTouch5Data(touchSegment5);
}