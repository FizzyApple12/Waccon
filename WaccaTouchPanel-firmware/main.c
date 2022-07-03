#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "mpr121.h"

//#define DEBUG_MODE // uncomment for debug messages (note: this will slow things down!)

#if OPT_PANEL1 // used for automatic compilation
    #define MOTHERBOARD_I2C_HWID 0x10
#elif OPT_PANEL2
    #define MOTHERBOARD_I2C_HWID 0x11
#elif OPT_PANEL3
    #define MOTHERBOARD_I2C_HWID 0x12
#elif OPT_PANEL4
    #define MOTHERBOARD_I2C_HWID 0x13
#elif OPT_PANEL5
    #define MOTHERBOARD_I2C_HWID 0x14
#elif OPT_PANEL6
    #define MOTHERBOARD_I2C_HWID 0x15
#endif

#define MOTHERBOARD_I2C_PORT i2c1
#define MOTHERBOARD_I2C_IRQ I2C1_IRQ
#ifndef MOTHERBOARD_I2C_HWID
    #define MOTHERBOARD_I2C_HWID 0x10 // 0x1[0-5 depending on which position it's in bottom to top]
#endif
#define MOTHERBOARD_I2C_SDA 2
#define MOTHERBOARD_I2C_SCL 3
#define MOTHERBOARD_I2C_FREQ 100000

#define TOUCH_I2C_PORT i2c0
#define TOUCH_I2C_SDA 4
#define TOUCH_I2C_SCL 5
#define INNER_MPR121_ADDR 0x5A
#define OUTER_MPR121_ADDR 0x5B
#define TOUCH_I2C_FREQ 100000
#define MPR121_TOUCH_THRESHOLD 16
#define MPR121_RELEASE_THRESHOLD 10

// Buttons
// [--] [--] [--] [--] [--]
// [--] [--] [--] [--] [--]
// [--] [--] [  ] [  ] [  ]
// [  ] [  ] [  ] [  ] [  ]

struct mpr121_sensor innerSensor;
uint16_t touchedInner = 0;

// Buttons
// [  ] [  ] [  ] [  ] [  ]
// [  ] [  ] [  ] [  ] [  ]
// [  ] [  ] [--] [--] [--]
// [--] [--] [--] [--] [--]

struct mpr121_sensor outerSensor;
uint16_t touchedOuter = 0;

// Button bits
// [ 0] [ 1] [ 2] [ 3] [ 4]
// [ 5] [ 6] [ 7] [ 8] [ 9]
// [10] [11] [12] [13] [14]
// [15] [16] [17] [18] [19]

uint32_t previousTouchDataPacket = 0;
bool writingPacket = false;
uint32_t touchDataPacket = 0;

uint8_t byteSelector = 0;

#define txByte(i2c, byte) while (!i2c_get_write_available(i2c)) { ; } i2c->hw->data_cmd = byte

void i2cSlaveI2CIRQHandler() {
    uint32_t status = MOTHERBOARD_I2C_PORT->hw->intr_stat;

    if (status & I2C_IC_INTR_STAT_R_RD_REQ_BITS) {
        uint32_t dataToSend = previousTouchDataPacket;

        if (!writingPacket) dataToSend = touchDataPacket;

        #ifdef DEBUG_MODE
            printf("Master requested packet, sending [%u %u %u %u]...\n", (uint8_t) (dataToSend), (uint8_t) (dataToSend >> 8), (uint8_t) (dataToSend >> 16), (uint8_t) (dataToSend >> 24));
        #endif

        txByte(MOTHERBOARD_I2C_PORT, (uint8_t) (dataToSend));
        txByte(MOTHERBOARD_I2C_PORT, (uint8_t) (dataToSend >> 8));
        txByte(MOTHERBOARD_I2C_PORT, (uint8_t) (dataToSend >> 16));
        txByte(MOTHERBOARD_I2C_PORT, (uint8_t) (dataToSend >> 24));

        MOTHERBOARD_I2C_PORT->hw->clr_rd_req;
    }
}



void setupTouch() {
    i2c_init(TOUCH_I2C_PORT, TOUCH_I2C_FREQ);

    gpio_set_function(TOUCH_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(TOUCH_I2C_SCL, GPIO_FUNC_I2C);

    gpio_pull_up(TOUCH_I2C_SDA);
    gpio_pull_up(TOUCH_I2C_SCL);

    mpr121_init(TOUCH_I2C_PORT, INNER_MPR121_ADDR, &innerSensor);
    mpr121_set_thresholds(MPR121_TOUCH_THRESHOLD, MPR121_RELEASE_THRESHOLD, &innerSensor);

    mpr121_init(TOUCH_I2C_PORT, OUTER_MPR121_ADDR, &outerSensor);
    mpr121_set_thresholds(MPR121_TOUCH_THRESHOLD, MPR121_RELEASE_THRESHOLD, &outerSensor);
}

void setupMotherboard() {
    i2c_init(MOTHERBOARD_I2C_PORT, MOTHERBOARD_I2C_FREQ);
    i2c_set_slave_mode(MOTHERBOARD_I2C_PORT, true, MOTHERBOARD_I2C_HWID);

    MOTHERBOARD_I2C_PORT->hw->enable = 0;
    hw_set_bits(&MOTHERBOARD_I2C_PORT->hw->con, I2C_IC_CON_RX_FIFO_FULL_HLD_CTRL_BITS);
    MOTHERBOARD_I2C_PORT->hw->enable = 1;

    gpio_set_function(MOTHERBOARD_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(MOTHERBOARD_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(MOTHERBOARD_I2C_SDA);
    gpio_pull_up(MOTHERBOARD_I2C_SCL);

    MOTHERBOARD_I2C_PORT->hw->intr_mask = I2C_IC_INTR_MASK_M_RD_REQ_BITS;//(I2C_IC_INTR_MASK_M_RD_REQ_BITS | I2C_IC_INTR_MASK_M_RX_FULL_BITS);

    irq_set_exclusive_handler(MOTHERBOARD_I2C_IRQ, i2cSlaveI2CIRQHandler);

    irq_set_enabled(MOTHERBOARD_I2C_IRQ, true);
}

#ifdef DEBUG_MODE
    #define formatButton(data, number) (((data) & (1 << (number))) ? "#" : " ")
    #define printMultiButtonRow(data1, data2, n1, n2, n3, n4, n5) printf("[%s] [%s] [%s] [%s] [%s]\n", formatButton(data1, n1), formatButton(data1, n2), formatButton(data2, n3), formatButton(data2, n4), formatButton(data2, n5))
    #define printButtonRow(data, n1, n2, n3, n4, n5) printf("[%s] [%s] [%s] [%s] [%s]\n", formatButton(data, n1), formatButton(data, n2), formatButton(data, n3), formatButton(data, n4), formatButton(data, n5))
#endif

int main() {
    stdio_init_all();
    printf("Wacca Touch Panel v0.1\n");

    printf("Setting up Touch...");
    setupTouch();
    printf("OK\n");

    printf("Setting up Communications...");
    setupMotherboard();
    printf("OK\n");

    while(true) {
        mpr121_touched(&touchedInner, &innerSensor);
        mpr121_touched(&touchedOuter, &outerSensor);

        touchedInner = touchedInner & 0b0000111111111111;
        touchedOuter = touchedOuter & 0b0000000011111111;

        writingPacket = true;

        touchDataPacket = (((uint32_t) touchedOuter << 12) | touchedInner) | 0x69000000;

        #ifdef DEBUG_MODE
            printf("-------------------\n");
            printButtonRow(touchedInner, 0, 1, 2, 3, 4);
            printButtonRow(touchedInner, 5, 6, 7, 8, 9);
            printMultiButtonRow(touchedInner, touchedOuter, 10, 11, 0, 1, 2);
            printButtonRow(touchedOuter, 3, 4, 5, 6, 7);
            printf("-------------------\n");
        #endif

        writingPacket = false;

        sleep_ms(10);
    }

    return 0;
}