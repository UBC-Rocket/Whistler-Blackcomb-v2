  
add_executable(basic_test_app
    basicTest.c
)

target_link_libraries(basic_test_app
    Unity
)

add_test(basic_test basic_test_app)