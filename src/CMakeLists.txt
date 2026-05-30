# 添加头文件搜索路径（让编译器能找到 common.h 和 server_init.h）
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# 源文件列表
set(SOURCES
    main.c
    server_init.c
    epoll_manager.c
)

# 生成可执行文件
add_executable(echo_server ${SOURCES})