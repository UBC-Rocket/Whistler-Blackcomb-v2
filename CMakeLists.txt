cmake_minimum_required(VERSION 3.0.0)
project(whistler-blackcomb VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(whistler-blackcomb source/Whistler-Blackcomb.c)

# add_compile_definitions(_POSIX_C_SOURCE=200809L)

add_subdirectory(envs/x86)
add_subdirectory(common)
add_subdirectory(external)
add_subdirectory(test)

target_include_directories(whistler-blackcomb PUBLIC
    common/include
    board
    device
    CMSIS
    envs/x86/include
    envs/x86/freertos/include
    envs/x86/freertos/portable/utils
    envs/x86/freertos/portable
    source
)

target_link_libraries(whistler-blackcomb m)

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET whistler-blackcomb PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET whistler-blackcomb PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(whistler-blackcomb "${CMAKE_THREAD_LIBS_INIT}")
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
