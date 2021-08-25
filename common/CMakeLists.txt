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

file(GLOB_RECURSE CCONTROL_SRCS ${CMAKE_CURRENT_LIST_DIR}/CControl/src/CControl/Sources/*.c)
target_sources(whistler-blackcomb PUBLIC ${CCONTORL_SRCS})

add_library(IMU_interpret STATIC
    src/IMU_interpret.c
)

add_library(state_machine STATIC src/state_machine.c)
add_library(prediction STATIC src/prediction.c)

add_library(buffer STATIC src/buffer.c)

target_link_libraries(buffer freertos)

add_subdirectory(include)
add_subdirectory(src)
#add_subdirectory(../envs/x86/freertos freertos)
#add_subdirectory(../source freertosconfig)

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET buffer PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET buffer PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(buffer "${CMAKE_THREAD_LIBS_INIT}")
endif()