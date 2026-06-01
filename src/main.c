#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "common.h"
#include "server_init.h"
#include "epoll_manager.h"
#include "connection.h"
#include "logger.h"      // ← 添加

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    
    if (argc > 1) {
        port = atoi(argv[1]);
    }
    
    // 设置日志级别
    log_set_level(LOG_LEVEL_INFO);
    // 调试模式可以改为: log_set_level(LOG_LEVEL_DEBUG);
    
    // 忽略 SIGPIPE 信号
    signal(SIGPIPE, SIG_IGN);
    
    int listen_fd = init_server(port);
    if (listen_fd < 0) {
        log_error("服务器初始化失败");  // ← 改用日志
        return 1;
    }
    
    int epfd = epoll_init();
    if (epfd < 0) {
        close(listen_fd);
        return 1;
    }
    
    if (epoll_add_fd(epfd, listen_fd, EPOLLIN | EPOLLET) < 0) {
        close(listen_fd);
        close(epfd);
        return 1;
    }
    
    log_info("Echo Server 启动成功");  // ← 改用日志
    printf("监听端口: %d\n", port);
    printf("按 Ctrl+C 退出\n\n");
    
    struct epoll_event events[MAX_EVENTS];
    
    while (1) {
        int nfds = epoll_wait_events(epfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            break;
        }
        
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == listen_fd) {
                handle_accept(listen_fd, epfd);
            } else if (events[i].events & EPOLLIN) {
                handle_client_data(events[i].data.fd, epfd);
            } else if (events[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
                log_disconnect(events[i].data.fd);
                epoll_del_fd(epfd, events[i].data.fd);
                close(events[i].data.fd);
            }
        }
    }
    
    close(listen_fd);
    close(epfd);
    log_info("服务器已关闭");
    return 0;
}