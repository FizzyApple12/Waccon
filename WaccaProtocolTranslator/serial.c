#include "serial.h"

#include "pico.h"
#include "tusb.h"

void setup() {
    tusb_init();
}

void update() {
    tud_task();
}

void write(uint8_t data) {
    tud_cdc_n_write_char(0, data);
}

void sendRemaining() {
    tud_cdc_n_write_flush(0);
}

uint8_t read() {
    if (!tud_cdc_n_available(0)) return 0xff;

    uint8_t buffer[1];
    uint32_t count = tud_cdc_n_read(0, buffer, 1);
    
    if (count == 0) return 0xff;

    return buffer[0];
}