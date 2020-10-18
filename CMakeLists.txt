cmake_minimum_required(VERSION 3.0.0)
project(whistler-blackcomb VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(whistler-blackcomb source/Whistler-Blackcomb.c)

# add_subdirectory(board)

target_include_directories(whistler-blackcomb PUBLIC
    common/include
    envs/x86
    envs/x86/freertos/include
    envs/x86/freertos/portable
    source
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
