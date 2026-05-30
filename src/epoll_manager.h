#ifndef EPOLL_MANAGER_H
#define EPOLL_MANAGER_H

#include <sys/epoll.h>

// 初始化epoll函数
int epoll_init(void);
// 向epoll添加监控对象的函数
int epoll_add_fd(int epfd, int fd, uint32_t events);
// 从epoll中删除一个监控对象
int epoll_del_fd(int epfd, int fd);
// 等待一个epoll事件的发生
int epoll_wait_events(int epfd, struct epoll_event *events, int max_events, int timeout);

#endif