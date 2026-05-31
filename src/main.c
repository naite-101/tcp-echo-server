#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"
#include "server_init.h"
#include "epoll_manager.h"
#include "connection.h" // ← 添加这一行

int main(int argc, char *argv[])
{
    int port = DEFAULT_PORT;

    if (argc > 1)
    {
        port = atoi(argv[1]);
    }

    // 功能1：初始化服务器
    int listen_fd = init_server(port);
    if (listen_fd < 0)
    {
        printf("❌ 服务器初始化失败\n");
        return 1;
    }
    printf("监听套接字 fd = %d\n", listen_fd);

    // 功能2：创建 epoll 实例
    int epfd = epoll_init();
    if (epfd < 0)
    {
        close(listen_fd);
        return 1;
    }

    // 将监听套接字加入 epoll
    if (epoll_add_fd(epfd, listen_fd, EPOLLIN | EPOLLET) < 0)
    {
        close(listen_fd);
        close(epfd);
        return 1;
    }
    printf("✅ 监听套接字已加入 epoll\n");

    printf("\n🚀 Echo Server 启动成功，监听端口: %d\n", port);
    printf("按 Ctrl+C 退出\n\n");

    // ========== 事件循环（功能3+4+5） ==========
    struct epoll_event events[MAX_EVENTS];

    while (1)
    {
        int nfds = epoll_wait_events(epfd, events, MAX_EVENTS, -1);
        if (nfds == -1)
        {
            break;
        }

        for (int i = 0; i < nfds; i++)
        {
            if (events[i].data.fd == listen_fd)
            {
                // 功能3：接受新连接
                handle_accept(listen_fd, epfd);
            }
            else if (events[i].events & EPOLLIN)
            {
                // 功能4+5：处理客户端数据（目前是占位）
                handle_client_data(events[i].data.fd, epfd);
            }
            else if (events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
            {
                // 错误或断开（暂时简单处理）
                printf("❌ 客户端 fd=%d 异常或断开\n", events[i].data.fd);
                epoll_del_fd(epfd, events[i].data.fd);
                close(events[i].data.fd);
            }
        }
    }

    // 清理
    close(listen_fd);
    close(epfd);
    printf("服务器已关闭\n");

    return 0;
}