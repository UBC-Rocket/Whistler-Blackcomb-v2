
add_definitions( -DXBEE_PLATFORM_HEADER=\"whistler/platform_config.h\" )
add_definitions( -DCOMPILE_x86 )
add_definitions(-DPOSIX)
add_definitions(-DZCL_ENABLE_TIME_SERVER)
add_definitions(-DXBEE_CELLULAR_ENABLED)
add_definitions(-DXBEE_DEVICE_ENABLE_ATMODE)
add_definitions(-DXBEE_XMODEM_TESTING)


add_executable(basic_test_app
    basicTest.c
)

target_link_libraries(basic_test_app
    Unity
)

add_test(basic_test basic_test_app)

add_executable(IMU_test_app
    test_IMU_interp.c
)

target_link_libraries(IMU_test_app
    IMU_interpret
    Unity
)

add_test(IMU_test IMU_test_app)

add_executable(state_machine_test_app
     test_state_machine.c
)

target_link_libraries(state_machine_test_app
     Unity
     state_machine
 )

 add_test(state_machine_test state_machine_test_app)


 add_executable(prediction_test_app
     test_prediction.c
)

target_link_libraries(prediction_test_app
     Unity
     prediction
     m
 )

 add_test(prediction_test prediction_test_app)

 add_executable(buffer_test_app 
    test_buffer.c)

target_link_libraries(buffer_test_app
    Unity
    buffer)

add_test(buffer_test buffer_test_app)

add_definitions( -DXBEE_PLATFORM_HEADER=\"whistler/platform_config.h\" )
add_definitions( -DCOMPILE_x86 )

add_executable(radio_protocol_test_app 
test_radio_protocol.c)

target_link_libraries(radio_protocol_test_app
Unity
radio_protocol)

add_test(radio_protocol_test radio_protocol_test_app)

add_definitions( -DXBEE_PLATFORM_HEADER=\"whistler/platform_config.h\" )
add_definitions( -DCOMPILE_x86 )
