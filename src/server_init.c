#include "server_init.h"
#include "../include/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <logger.h>

// 设置文件描述符为非阻塞模式
int set_nonblocking(int fd)
{
    // 获取当前的文件描述符的状态标志
    int flags = fcntl(fd, F_GETFL, 0);
    // 检查fcntl是否获取失败
    if (flags == -1)
    {
        // 如果失败了就打印错误的信息
        log_error("fcntl F_GETFL failed");
        return -1;
    }

    // 将非阻塞的标志位加到flags中
    flags |= O_NONBLOCK;

    // 把flags的状态设置到fd中失败
    if (fcntl(fd, F_SETFL, flags) == -1)
    {
        log_error("fcntl F_SETFL failed");
        return -1;
    }
    return 0;
}

// 初始化TCP
int init_server(int port)
{
    // 定义一个可以存放文件描述符的变量
    int listen_fd;
    // 存储服务器的IP地址和端口
    struct sockaddr_in addr;
    // 设置 socket的值是可以复用
    int reuse = 1;
    // 创建一个IPv4的TCP
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    // 创建失败
    if (listen_fd == -1)
    {
        log_error("socket failed");
        return -1;
    }

    // setsockopt 设置套接字选项 SOL_SOCKET 在socket这一层设置 SO_REUSEADDR 允许重用本地地址和端口
    // setsockopt是否设置成功
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
    {
        log_error("setsockopt failed");
        close(listen_fd);
        return -1;
    }

    // 清零addr中的字节
    memset(&addr, 0, sizeof(addr));
    // 设置地址族为IPv4
    addr.sin_family = AF_INET;
    // 将端口转变为字节的形式
    addr.sin_port = htons(port);
    // 监听服务器的所以接口
    addr.sin_addr.s_addr = INADDR_ANY;

    // 绑定地址到linsten——fd中
    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        log_error("bind failed");
        close(listen_fd);
        return -1;
    }

    // 开始监听TCP的连接请求
    if (listen(listen_fd, BACKLOG) == -1)
    {
        log_error("listen failed");
        close(listen_fd);
        return -1;
    }

    // 把监听的模式换成非阻塞模式
    if (set_nonblocking(listen_fd) == -1)
    {
        close(listen_fd);
        return -1;
    }
    log_error("服务器初始化成功，监听端口：%d\n", port);
    return listen_fd;
}