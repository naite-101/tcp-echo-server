#include "epoll_manager.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

// 创建并且初始化一个epoll
int epoll_init(void)
{
    // 创建一个epllo实例
    int epfd = epoll_create1(0);
    // 判断epoll_create1是否调用失败
    if (epfd == -1)
    {
        perror("epoll_create1");
        return -1;
    }
    printf("epoll 实例创建成功,epfd = %d\n", epfd);
    return epfd;
}

// 添加一个文件描述符到epoll中，用于监听
int epoll_add_fd(int epfd, int fd, uint32_t events)
{
    // 定义一个名为epoll_event这个结构体，用于配置要监听的事件
    struct epoll_event ev;
    // 把函数里的参数赋值到结构体中
    ev.events = events;
    // 把文件描述符fd赋值到结构体中
    ev.data.fd = fd;
    // 在epoll中添加一个文件描述符是否成功
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        perror("epoll_ctl ADD");
        return -1;
    }
    return 0;
}
// 等待epoll实例上注册的文件描述符发生
int epoll_wait_events(int epfd, struct epoll_event *events, int max_events, int timeout)
{
    // 用来储存就绪的文件描述符的数量
    int nfds;
    do
    {
        // 就绪文件描述符的个数赋值到nfds
        nfds = epoll_wait(epfd, events, max_events, timeout);
        // 如果被信号中断，就重新调用epoll_wait
    } while (nfds == -1 && errno == EINTR);
    // 调用函数出错
    if (nfds == -1)
    {
        perror("epoll_wait");
        return -1;
    }
    return nfds;
}