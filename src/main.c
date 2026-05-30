#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"
#include "server_init.h"
#include "epoll_manager.h"

int main(int argc, char *argv[])
{
    int port = DEFAULT_PORT;

    // 命令行参数：指定端口
    if (argc > 1)
    {
        port = atoi(argv[1]);
    }

    // ========== 功能1：初始化服务器 ==========
    int listen_fd = init_server(port);
    if (listen_fd < 0)
    {
        printf("❌ 服务器初始化失败\n");
        return 1;
    }
    printf("监听套接字 fd = %d\n", listen_fd);

    // ========== 功能2：创建 epoll 实例 ==========
    int epfd = epoll_init();
    if (epfd < 0)
    {
        close(listen_fd);
        return 1;
    }

    // ========== 将监听套接字加入 epoll ==========
    if (epoll_add_fd(epfd, listen_fd, EPOLLIN | EPOLLET) < 0)
    {
        close(listen_fd);
        close(epfd);
        return 1;
    }
    printf("✅ 监听套接字已加入 epoll\n");

    printf("\n🚀 Echo Server 启动成功，监听端口: %d\n", port);
    printf("按 Ctrl+C 退出\n\n");

    // ========== 暂时阻塞在这里，后续功能3+4+5会替换这个循环 ==========
    printf("等待客户端连接...\n");
    printf("（按回车键退出）\n");
    getchar();

    // 清理
    close(listen_fd);
    close(epfd);
    printf("服务器已关闭\n");

    return 0;
}