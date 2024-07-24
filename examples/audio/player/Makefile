SDK_DEMO_PATH ?= .
BL_SDK_BASE ?= $(SDK_DEMO_PATH)/../../..
CONFIG_BOARD_CONFIG_8M ?= config_8M

export BL_SDK_BASE
export CONFIG_BOARD_CONFIG_8M

CHIP ?= bl616
BOARD ?= bl616dk
CROSS_COMPILE ?= riscv64-unknown-elf-

# add custom cmake definition
#cmake_definition+=-Dxxx=sss

include $(BL_SDK_BASE)/project.build
$(shell ./tools/genromfs/genromfs -d romfs/ -f ./build/build_out/romfs.bin)
