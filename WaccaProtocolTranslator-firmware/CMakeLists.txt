cmake_minimum_required(VERSION 3.13)

set(PICO_SDK_FETCH_FROM_GIT on)

include(pico_sdk_import.cmake)

project(WaccaProtocolTranslator-firmware)

pico_sdk_init()

add_definitions($ENV{CUSTOM_CFLAGS})

add_executable(wacca_protocol_translator
    main.c
    serial.c
    waccaserial.c
    queue.c
    usbDescriptors.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(wacca_protocol_translator
    pico_stdlib
    hardware_i2c
    pico_multicore
    tinyusb_device
)

target_include_directories(wacca_protocol_translator PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(wacca_protocol_translator)