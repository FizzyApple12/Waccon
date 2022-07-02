cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_FETCH_FROM_GIT on)

include(pico_sdk_import.cmake)

project(WaccaTouchPanel)

pico_sdk_init()

add_definitions($ENV{CUSTOM_CFLAGS})

add_executable(wacca_touch_panel
    main.c
    mpr121.h
    mpr121.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(wacca_touch_panel 
    pico_stdlib
    hardware_i2c
)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(wacca_touch_panel)

pico_enable_stdio_usb(wacca_touch_panel 1)
pico_enable_stdio_uart(wacca_touch_panel 0)