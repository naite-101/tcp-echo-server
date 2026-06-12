#include "../include/common.h"
#include "epoll_manager.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "logger.h"

// 接受等待中的客户端连接
void handle_accept(int listen_fd, int epfd)
{
    // 接收accept返回的客户端的地址信息
    // struck sockaddr_in IPV4的地址结构体
    struct sockaddr_in client_addr;
    // 把客户端地址的大小暂时存储到这个变量中
    socklen_t addr_len = sizeof(client_addr);

    // 无限循环
    while (1)
    {
        // 从连接好的队列中取出一个连接从而创建新的socket
        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_len);
        // 检查是否创建成功
        if (client_fd == -1)
        {
            // 判断没有连接成功的的原因
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            log_error("accept failed");
            break;
        }
        // 获取client_fd当前的状态
        int flags = fcntl(client_fd, F_GETFL, 0);
        // 判断是否获取成功
        if (flags == -1)
        {
            log_error("fcntl F_GETFL failed");
            close(client_fd);
            continue;
        }
        // 判断是否将客户端socket设置成非阻塞状态
        // F_SETFL - 设置标志命令，O_NONBLOCK-非阻塞标志，
        if (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK) == -1)
        {
            log_error("fcntl,F_SETFL failed");
            close(client_fd);
            continue;
        }
        // 判断是否将其添加到epoll中
        // EPOLLIN | EPOLLET - 要监控的事件类型
        if (epoll_add_fd(epfd, client_fd, EPOLLIN | EPOLLET) == -1)
        {
            close(client_fd);
            continue;
        }
        // IPV4地址存放
        char ip[INET_ADDRSTRLEN];
        // 把二进制转换成字符串
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        log_connect(client_fd, &client_addr);
    }
}

// 读取客户端的数据
void handle_client_data(int client_fd, int epfd)
{
    // 设置缓冲区
    char buf[4096];

    // 无限循环
    while (1)
    {
        // 从客户端接受数据
        // ssize_t - 返回值
        ssize_t n = recv(client_fd, buf, sizeof(buf), 0);
        // 如果接受成功
        if (n > 0)
        {
            log_recv(client_fd, buf, n);

            // 记录返回了多少数据
            ssize_t sent = 0;
            // 确保所以的数据都发送完毕
            while (sent < n)
            {
                // 把数据发送到客户端
                ssize_t ret = send(client_fd, buf + sent, n - sent, 0);
                // 发送失败
                if (ret == -1)
                {
                    // 判断是不是缓冲区空了
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        break;
                    }
                    log_error("send sailed");
                    goto error;
                }
                // 更新发送的进度
                sent += ret;
            }
        }
        // 客户端关闭连接
        else if (n == 0)
        {
            log_disconnect(client_fd);
            goto cleanup;
        }
        // n==-1,接受失败
        else
        {
            // 失败原因是缓冲区空了，没有数据可以读
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            log_error("recv failde");
            goto cleanup;
        }
    }
    return;
error:
    log_error("handle_client_data error");

cleanup:
    // 不再监听这个socket
    epoll_del_fd(epfd, client_fd);
    close(client_fd);
}