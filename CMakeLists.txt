# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(blinky)


target_sources(app PRIVATE 
    src/main.c
    src/bluetooth_service/bluetooth_service.c
    src/driver/dht/dht.c
    src/driver/spi/spi.c
    src/driver/gpio/gpio.c
)

zephyr_library_include_directories(src/driver)
zephyr_library_include_directories(src/bluetooth_service)

