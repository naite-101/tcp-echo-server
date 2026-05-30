#include <stdio.h>
#include <unistd.h>
#include "epoll_manager.h"

int main() {
    // 1. 创建 epoll 实例
    int epfd = epoll_init();
    if (epfd < 0) {
        return 1;
    }
    
    // 2. 添加标准输入（fd=0）到 epoll
    if (epoll_add_fd(epfd, 0, EPOLLIN) < 0) {
        close(epfd);
        return 1;
    }
    
    printf("正在等待键盘输入（5秒超时）...\n");
    
    // 3. 等待事件
    struct epoll_event events[1];
    int nfds = epoll_wait_events(epfd, events, 1, 5000);
    
    if (nfds > 0) {
        printf("✅ 检测到键盘输入！\n");
    } else if (nfds == 0) {
        printf("⏰ 5秒超时，没有输入\n");
    }
    
    // 4. 删除并关闭
    epoll_del_fd(epfd, 0);
    close(epfd);
    
    return 0;
}