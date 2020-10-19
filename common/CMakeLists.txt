target_sources( 
    whistler-blackcomb
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/prediction.c
        ${CMAKE_CURRENT_LIST_DIR}/src/IMU_interpret.c
)