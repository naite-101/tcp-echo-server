include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

set(SOURCES
    main.c
    server_init.c
    epoll_manager.c
    connection.c
    logger.c          # ← 添加这一行
)

add_executable(echo_server ${SOURCES})