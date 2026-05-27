#ifndef SERVER_INIT_H
#define SERVER_INIT_H

//初始化TCP
int init_server(int port);
//设置为非阻塞模式
int set_nonblocking(int fd);

#endif
