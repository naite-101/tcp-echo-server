#include "../include/common.h"
#include "epoll_manager.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//接受等待中的客户端连接
void handle_accept(int listen_fd ,int epfd){
    //接收accept返回的客户端的地址信息
    //struck sockaddr_in IPV4的地址结构体
    struct  sockaddr_in client_addr;
    //把客户端地址的大小暂时存储到这个变量中
    socklen_t addr_len = sizeof(client_addr);

    //无限循环
    while (1)
    {
        //从连接好的队列中取出一个连接从而创建新的socket
        int client_fd = accept(listen_fd,(struct sockaddr*)&client_addr,&addr_len);
        //检查是否创建成功
        if (client_fd == -1)
        {
            //判断没有连接成功的的原因
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            perror("accept");
            break;
        }
        //获取client_fd当前的状态
        int flags = fcntl(client_fd,F_GETFL,0);
        //判断是否获取成功
        if (flags ==-1)
        {
            perror("fcntl F_GETFL");
            close(client_fd);
            continue;
        }
        //判断是否将客户端socket设置成非阻塞状态
        //F_SETFL - 设置标志命令，O_NONBLOCK-非阻塞标志，
        if (fcntl(client_fd,F_SETFL,flags | O_NONBLOCK) == -1)
        {
            perror("fcntl,F_SETFL");
            close(client_fd);
            continue;
        }
        //判断是否将其添加到epoll中
        //EPOLLIN | EPOLLET - 要监控的事件类型
        if (epoll_add_fd(epfd,client_fd,EPOLLIN | EPOLLET) == -1)
        {
            close(client_fd);
            continue;
        }
        //IPV4地址存放
        char ip[INET_ADDRSTRLEN];
        //把二进制转换成字符串
        inet_ntop(AF_INET,&client_addr.sin_addr,ip,sizeof(ip));
        printf("新客户端连接:fd=%d,IP=%s,端口=%d\n",client_fd, ip ,ntohs(client_addr.sin_port));
    }
    
    
}

void handle_client_data (int client_fd, int epfd)
{
    printf("客户端fd=%d 有数据可读\n",client_fd);
}