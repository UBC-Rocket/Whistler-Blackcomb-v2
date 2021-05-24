target_sources( 
    whistler-blackcomb
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/src/hal.c
        ${CMAKE_CURRENT_LIST_DIR}/src/hal_io.c
        ${CMAKE_CURRENT_LIST_DIR}/src/hal_uart.c
        ${CMAKE_CURRENT_LIST_DIR}/src/hal_time.c
        ${CMAKE_CURRENT_LIST_DIR}/src/hal_sd.c
        ${CMAKE_CURRENT_LIST_DIR}/src/stdio_controller.c
        ${CMAKE_CURRENT_LIST_DIR}/src/console.c

        ${CMAKE_CURRENT_LIST_DIR}/freertos/croutine.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/event_groups.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/list.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/queue.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/stream_buffer.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/tasks.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/timers.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/portable/port.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/portable/utils/wait_for_event.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/portable/MemMang/heap_3.c
)

target_sources( 
    freertos
    PRIVATE

        ${CMAKE_CURRENT_LIST_DIR}/freertos/croutine.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/event_groups.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/list.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/queue.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/stream_buffer.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/tasks.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/timers.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/portable/port.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/portable/utils/wait_for_event.c
        ${CMAKE_CURRENT_LIST_DIR}/freertos/portable/MemMang/heap_3.c
)

