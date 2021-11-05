cmake_minimum_required(VERSION 3.13.0)
project(whistler-blackcomb VERSION 0.1.0)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with "
                        "CMakeLists.txt file). Please make a build subdirectory. Feel free to "
                        "remove CMakeCache.txt and CMakeFiles.")
endif()

include(CTest)
enable_testing()

add_executable(whistler-blackcomb source/Whistler-Blackcomb.c)

add_library(freertos)

add_library(hal)

add_subdirectory(envs/x86)
add_subdirectory(common)
add_subdirectory(external)
add_subdirectory(test)

target_include_directories(whistler-blackcomb PUBLIC
    common/include
    common/xbee/include/
    common/xbee/ports/
    common/CControl/src/CControl/Headers/
    board
    device
    CMSIS
    envs/include
    envs/x86/include
    envs/x86/freertos/include
    envs/x86/freertos/portable/utils
    envs/x86/freertos/portable
    source
)

target_include_directories(freertos PUBLIC
    
    envs/include
    envs/x86/include
    envs/x86/freertos/include
    envs/x86/freertos/portable/utils
    envs/x86/freertos/portable
    source
)

target_link_libraries(hal freertos)
target_include_directories(hal PUBLIC
  device
  CMSIS
  board
  envs/include
  envs/
)


target_link_libraries(whistler-blackcomb m)
target_link_libraries(whistler-blackcomb radio_protocol)
target_link_libraries(whistler-blackcomb prediction)

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET whistler-blackcomb PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET whistler-blackcomb PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(whistler-blackcomb "${CMAKE_THREAD_LIBS_INIT}")
endif()

# Get git version for radio packet string
execute_process(COMMAND git rev-parse HEAD
                OUTPUT_VARIABLE GIT_REV
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

string(REPLACE "\n" "" GIT_REV ${GIT_REV})
string(REPLACE "\"" "" GIT_REV ${GIT_REV})

# Wno-int-to-pointer-cast is to disable a warning from core_cm4.h on compilation
# Might be bad to disable it this way, not entirely sure
# Wno-unused-parameter is to enable hal taking parameters and not using them
target_compile_options(whistler-blackcomb PRIVATE 
    -Wno-int-to-pointer-cast
    -Wno-unused-parameter 
    #-Werror 
    -Wall 
    -Wextra
    -DPOSIX
    -DZCL_ENABLE_TIME_SERVER
    -DXBEE_CELLULAR_ENABLED
    -DXBEE_DEVICE_ENABLE_ATMODE
    -DXBEE_XMODEM_T)

message(STATUS "Git version: ${GIT_REV}")


add_definitions(-DXBEE_PLATFORM_HEADER=\"whistler/platform_config.h\")
add_definitions(-DGIT_REV="${GIT_REV}")
add_definitions(-DCOMPILE_x86)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
