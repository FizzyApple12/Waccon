#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "waccaserial.h"

#define TOUCH_I2C_PORT i2c0
#define TOUCH_I2C_SDA 6
#define TOUCH_I2C_SCL 7
#define TOUCH_PANEL_0_ADDR 0x11
#define TOUCH_PANEL_1_ADDR 0x11 //0x12
#define TOUCH_PANEL_2_ADDR 0x11 //0x13
#define TOUCH_PANEL_3_ADDR 0x11 //0x14
#define TOUCH_PANEL_4_ADDR 0x11 //0x15
#define TOUCH_PANEL_5_ADDR 0x11 //0x16
#define TOUCH_I2C_FREQ 100000

uint32_t touchPanel0Data = 0;
uint32_t touchPanel1Data = 0;
uint32_t touchPanel2Data = 0;
uint32_t touchPanel3Data = 0;
uint32_t touchPanel4Data = 0;
uint32_t touchPanel5Data = 0;


uint32_t getTouchPacket(i2c_inst_t *port, uint8_t addr) {
    int bytesRead = 0;
    uint8_t data[4];
    uint8_t dummyRegister = 0x69;

    i2c_write_blocking(port, addr, &dummyRegister, 1, true); // it's okay to use blocking methods, the main protocol translation is running on a seperate cpu core
    bytesRead = i2c_read_blocking(port, addr, data, 4, false);

    if (bytesRead != 4) return 0;

    return (uint32_t) (data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24));
}

#define PingWaccaPanel(ADDR, final) do { final = getTouchPacket(TOUCH_I2C_PORT, ADDR); } while (final == 0)

void setupTouchPanels() {
    i2c_init(TOUCH_I2C_PORT, TOUCH_I2C_FREQ);

    gpio_set_function(TOUCH_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(TOUCH_I2C_SCL, GPIO_FUNC_I2C);

    gpio_pull_up(TOUCH_I2C_SDA);
    gpio_pull_up(TOUCH_I2C_SCL);

    PingWaccaPanel(TOUCH_PANEL_0_ADDR, touchPanel0Data);
    PingWaccaPanel(TOUCH_PANEL_1_ADDR, touchPanel1Data);
    PingWaccaPanel(TOUCH_PANEL_2_ADDR, touchPanel2Data);
    PingWaccaPanel(TOUCH_PANEL_3_ADDR, touchPanel3Data);
    PingWaccaPanel(TOUCH_PANEL_4_ADDR, touchPanel4Data);
    PingWaccaPanel(TOUCH_PANEL_5_ADDR, touchPanel5Data);

    ws_setTouchData(touchPanel0Data, touchPanel1Data, touchPanel2Data, touchPanel3Data, touchPanel4Data, touchPanel5Data);
}

int main() {
    setupTouchPanels();

    ws_start();

    while(1) { // note: getting a touch packet might take a bit, use this rolling update to speed up input throughput to the game
        touchPanel0Data = getTouchPacket(TOUCH_I2C_PORT, TOUCH_PANEL_0_ADDR);
        ws_setTouch0Data(touchPanel0Data);

        touchPanel1Data = getTouchPacket(TOUCH_I2C_PORT, TOUCH_PANEL_1_ADDR);
        ws_setTouch1Data(touchPanel1Data);

        touchPanel2Data = getTouchPacket(TOUCH_I2C_PORT, TOUCH_PANEL_2_ADDR);
        ws_setTouch2Data(touchPanel2Data);

        touchPanel3Data = getTouchPacket(TOUCH_I2C_PORT, TOUCH_PANEL_3_ADDR);
        ws_setTouch3Data(touchPanel3Data);

        touchPanel4Data = getTouchPacket(TOUCH_I2C_PORT, TOUCH_PANEL_4_ADDR);
        ws_setTouch4Data(touchPanel4Data);

        touchPanel5Data = getTouchPacket(TOUCH_I2C_PORT, TOUCH_PANEL_5_ADDR);
        ws_setTouch5Data(touchPanel5Data);
    }

    return 0;
}