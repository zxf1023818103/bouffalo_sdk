SDK_DEMO_PATH ?= .
BL_SDK_BASE ?= $(SDK_DEMO_PATH)/../../..

export BL_SDK_BASE

CHIP ?= bl616
BOARD ?= bl616dk
CROSS_COMPILE ?= riscv64-unknown-elf-

cmake_definition+=-DOT_FTD=1
cmake_definition+=-DOT_CLI=1
cmake_definition+=-DOT_SERIAL=SYS_CLI

cmake_definition+=-DCONFIG_SHELL_CMD_SIZE=256
cmake_definition+=-DCONFIG_SHELL=1

cmake_definition+=-DOPENTHREAD_PROJECT_CORE_CONFIG_FILE=\"openthread-core-proj-config.h\"
cmake_definition+=-DOPENTHREAD_CORE_CONFIG_PLATFORM_CHECK_FILE=\"openthread-core-platform-config-check.h\"

include $(BL_SDK_BASE)/project.build
