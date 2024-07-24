cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(.)

target_sources(app PRIVATE ble_tp_svc.c
                           utils_sha256.c
                           ${CMAKE_SOURCE_DIR}/port/${CHIP}/bflb_port_boot2.c
)

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/port/${CHIP})
include_directories(${CMAKE_SOURCE_DIR}/port/${CHIP})
else()
message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/port/${CHIP} is not exist")
endif()

sdk_set_main_file(main.c)

project(peripheral)
