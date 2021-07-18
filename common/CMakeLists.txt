add_definitions( -DXBEE_PLATFORM_HEADER=\"whistler/platform_config.h\" )
add_definitions( -DCOMPILE_x86 )
add_definitions(-DPOSIX)
add_definitions(-DZCL_ENABLE_TIME_SERVER)
add_definitions(-DXBEE_CELLULAR_ENABLED)
add_definitions(-DXBEE_DEVICE_ENABLE_ATMODE)
add_definitions(-DXBEE_XMODEM_TESTING)

target_sources( 
    whistler-blackcomb
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src/prediction.c
        ${CMAKE_CURRENT_LIST_DIR}/src/IMU_interpret.c
        ${CMAKE_CURRENT_LIST_DIR}/src/radio.c
        ${CMAKE_CURRENT_LIST_DIR}/src/state_machine.c
)

file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_LIST_DIR}/xbee/src/*.c)
file(GLOB_RECURSE PORT ${CMAKE_CURRENT_LIST_DIR}/xbee/ports/whistler/*.c)

target_sources(whistler-blackcomb PUBLIC ${SRCS})
target_sources(whistler-blackcomb PUBLIC ${PORT})



add_library(IMU_interpret STATIC
    src/IMU_interpret.c
)

add_library(variables STATIC src/variables.c)
target_link_libraries(variables freertos)

add_library(state_machine STATIC src/state_machine.c)
add_library(prediction STATIC src/prediction.c)

add_library(buffer STATIC src/buffer.c)



target_link_libraries(buffer freertos)

target_link_libraries(state_machine freertos)



add_library(radio STATIC src/radio.c)

target_link_libraries(radio hal)



add_library(radio_protocol STATIC src/radio_protocol.c)



target_link_libraries(radio_protocol buffer)
target_link_libraries(radio_protocol radio)
target_link_libraries(radio_protocol variables)
target_link_libraries(radio_protocol state_machine)
target_sources(radio_protocol PUBLIC ${SRCS})
target_sources(radio_protocol PUBLIC ${PORT})

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(xbee)




#add_subdirectory(../envs/x86/freertos freertos)
#add_subdirectory(../source freertosconfig)

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET buffer PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET buffer PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")

  set_property(TARGET radio_protocol PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET radio_protocol PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(buffer "${CMAKE_THREAD_LIBS_INIT}")
endif()