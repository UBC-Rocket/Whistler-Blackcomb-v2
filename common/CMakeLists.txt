target_sources( 
    whistler-blackcomb
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src/prediction.c
        ${CMAKE_CURRENT_LIST_DIR}/src/IMU_interpret.c
        ${CMAKE_CURRENT_LIST_DIR}/src/radio.c
)

file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_LIST_DIR}/xbee/src/*.c)
file(GLOB_RECURSE PORT ${CMAKE_CURRENT_LIST_DIR}/xbee/ports/whistler/*.c)

target_sources(whistler-blackcomb PUBLIC ${SRCS})
target_sources(whistler-blackcomb PUBLIC ${PORT})

add_library(IMU_interpret STATIC
    src/IMU_interpret.c
)

add_subdirectory(include)
add_subdirectory(src)
