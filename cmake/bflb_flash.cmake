if("${CMAKE_SYSTEM_NAME}" STREQUAL "Generic")
set(TOOL_SUFFIX ".exe")
set(CMAKE ${BL_SDK_BASE}/tools/cmake/bin/cmake.exe)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
set(TOOL_SUFFIX "-ubuntu")
set(CMAKE cmake)
endif()

set(BL_FW_POST_PROC ${BL_SDK_BASE}/tools/bflb_tools/bflb_fw_post_proc/bflb_fw_post_proc${TOOL_SUFFIX})

set(BL_FW_POST_PROC_CONFIG --chipname=${CHIP} --imgfile=${BIN_FILE})

if(BOARD_DIR)
list(APPEND BL_FW_POST_PROC_CONFIG --brdcfgdir=${BOARD_DIR}/${BOARD}/config)
elseif(CONFIG_BOARD_CONFIG_8M)
list(APPEND BL_FW_POST_PROC_CONFIG --brdcfgdir=${BL_SDK_BASE}/bsp/board/${BOARD}/${CONFIG_BOARD_CONFIG_8M})
else()
list(APPEND BL_FW_POST_PROC_CONFIG --brdcfgdir=${BL_SDK_BASE}/bsp/board/${BOARD}/config)
endif()

if(CONFIG_AES_KEY)
list(APPEND BL_FW_POST_PROC_CONFIG --key=${CONFIG_AES_KEY})
endif()

if(CONFIG_AES_IV)
list(APPEND BL_FW_POST_PROC_CONFIG --iv=${CONFIG_AES_IV})
endif()

if(CONFIG_PUBLIC_KEY)
list(APPEND BL_FW_POST_PROC_CONFIG --publickey=${CONFIG_PUBLIC_KEY})
endif()

if(CONFIG_PRIVATE_KEY)
list(APPEND BL_FW_POST_PROC_CONFIG --privatekey=${CONFIG_PRIVATE_KEY})
endif()

if(CONFIG_FW_POST_PROC_CUSTOM)
list(APPEND BL_FW_POST_PROC_CONFIG ${CONFIG_FW_POST_PROC_CUSTOM})
endif()

add_custom_target(combine
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${BL_FW_POST_PROC} ${BL_FW_POST_PROC_CONFIG})

if("${CONFIG_POST_BUILD}" STREQUAL "CONCAT_WITH_LP_FW")
add_custom_target(post_build
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND ${BL_SDK_BASE}/tools/lpfw/patch_lpfw${TOOL_SUFFIX} ${BIN_FILE} ${BL_SDK_BASE}/tools/lpfw/bin/${CHIP}_lp_fw.bin)
else()
add_custom_target(post_build)
endif()
