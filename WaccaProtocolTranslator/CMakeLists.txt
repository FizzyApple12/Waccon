cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_FETCH_FROM_GIT on)

include(pico_sdk_import.cmake)

project(WaccaTouchPanel)

pico_sdk_init()

add_executable(wacca_protocol_translator
    main.c
    waccaserial.h
    waccaserial.c
    queue.h
    queue.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(wacca_protocol_translator
    pico_stdlib
    hardware_i2c
    pico_multicore
    pico_stdio
    pico_stdio_usb
)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(wacca_protocol_translator)

pico_enable_stdio_usb(wacca_protocol_translator 1)
pico_enable_stdio_uart(wacca_protocol_translator 0)