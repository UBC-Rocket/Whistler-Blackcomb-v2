target_sources( 
    whistler-blackcomb
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/src/prediction.c
        ${CMAKE_CURRENT_LIST_DIR}/src/IMU_interpret.c
)

add_library(IMU_interpret STATIC
    src/IMU_interpret.c
)

target_include_directories(IMU_interpret PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include
    ${CMAKE_CURRENT_LIST_DIR}/src
)