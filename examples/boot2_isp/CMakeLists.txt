cmake_minimum_required(VERSION 3.15)

include(proj.conf)

find_package(bouffalo_sdk REQUIRED HINTS $ENV{BL_SDK_BASE})

sdk_add_include_directories(.)

target_sources(app PRIVATE
                bflb_eflash_loader_interface.c
                bflb_eflash_loader_cmds.c
                bflb_eflash_loader_uart.c
                blsp_boot_decompress.c
                blsp_boot_parser.c
                blsp_common.c
                blsp_media_boot.c
                blsp_port.c
                blsp_ram_image_boot.c
                partition.c
                ${CMAKE_SOURCE_DIR}/port/${CHIP}/bflb_port_boot2.c
                )


if("${CHIP}" STREQUAL "bl616")
target_sources(app PRIVATE rv32i_xtheade_lz4.S)
endif()

if("${CHIP}" STREQUAL "bl808")
target_sources(app PRIVATE rv32i_xtheade_lz4.S)
endif()

if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/port/${CHIP})
include_directories(${CMAKE_SOURCE_DIR}/port/${CHIP})
if(EXISTS "${CMAKE_SOURCE_DIR}/port/${CHIP}/CMakeLists.txt")
    add_subdirectory(${CMAKE_SOURCE_DIR}/port/${CHIP})
endif()
else()
message(FATAL_ERROR "${CMAKE_SOURCE_DIR}/port/${CHIP} is not exist")
endif()
include_directories(${CMAKE_SOURCE_DIR})
sdk_set_main_file(main.c)
sdk_set_linker_script(port/${CHIP}/blsp_boot2_iap_flash.ld)

if("${CONFIG_DEBUG}" STREQUAL "y")
sdk_add_compile_definitions(-DCONFIG_DEBUG)
endif()
sdk_add_compile_definitions(-DBFLB_BOOT2_VER="${CONFIG_BOOT2_VER}")
sdk_add_compile_definitions(-DBFLB_BOOT2)
sdk_add_compile_definitions(-DCONFIG_TRAP_DUMP_DISABLE)
message(${CONFIG_BOOT2_VER})

if(CONFIG_BOOT2)
    sdk_add_compile_definitions(-DCONFIG_BOOT2)
endif()
sdk_add_compile_definitions(-DBOOT2_ANTI_ROLLBACK_VER=${CONFIG_BOOT2_ANTI_ROLLBACK_VER})

if((${CHIP} STREQUAL "bl602"))
    sdk_add_compile_definitions(-DCHIP_BL602)
endif()

if((${CHIP} STREQUAL "bl702"))
    sdk_add_compile_definitions(-DCHIP_BL702)
endif()

if((${CHIP} STREQUAL "bl702l"))
    sdk_add_compile_definitions(-DCHIP_BL702L)
endif()

if((${CHIP} STREQUAL "bl808"))
    sdk_add_compile_definitions(-DCHIP_BL808)
    sdk_add_compile_definitions(-DCONFIG_SUPPORT_UART_CLOCK_GET_ONLY)
    sdk_add_compile_definitions(-D CONIFG_DISABLE_ALL_PMP_EXCEPT_PMPADDR0)
    sdk_add_compile_definitions(-D CONIFG_DISABLE_ALL_CLOCK_GET_EXCEPT_SYSTEM_CLOCK)
    sdk_add_compile_definitions(-D CONIFG_DISABLE_ALL_UHS_EXCEPT_40M)
    sdk_add_compile_definitions(-D CONIFG_DISABLE_ALL_PSRAM_FRQ_EXCEPT_2000)
    sdk_add_compile_definitions(-D CONIFG_DISABLE_INTERRUPT_BEHADN_UART0)
    sdk_add_compile_definitions(-D CONIFG_DISABLE_OTHER_CORE_EXCEPT_M0)
    sdk_add_compile_definitions(-D CONFIG_NOT_SUPPORT_0X13_0X14_0X15_0X16_FLASH)
endif()

if((${CHIP} STREQUAL "bl606p"))
    sdk_add_compile_definitions(-DCHIP_BL606P)
    sdk_add_compile_definitions(-DCONFIG_SUPPORT_UART_CLOCK_GET_ONLY)
    sdk_add_compile_definitions(-D CONIFG_DISABLE_ALL_CLOCK_GET_EXCEPT_SYSTEM_CLOCK)
    sdk_add_compile_definitions(-D CONFIG_NOT_SUPPORT_0X13_0X14_0X15_0X16_FLASH)
endif()

if((${CHIP} STREQUAL "bl616"))
    sdk_add_compile_definitions(-DCHIP_BL616)
endif()
sdk_add_compile_definitions(-D CONIFG_DISABLE_NOCACHE_RAM_LOAD)
sdk_add_compile_definitions(-D CONFIG_DISABLE_SBUS2_ENABLE_SUPPORT)
execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions(-DGIT_COMMIT="${GIT_COMMIT}")

project(boot2_isp)
