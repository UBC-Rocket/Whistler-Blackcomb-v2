target_include_directories(IMU_interpret PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_include_directories(state_machine PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_include_directories(prediction PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_include_directories(buffer PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_include_directories(radio PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_include_directories(radio_protocol PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_include_directories(variables PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)