  
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

# add_executable(SD_test
#     SD_test.c
# )

# target_link_libraries(SD_test
#     Unity
# )

# add_test(SD_test SD_test)