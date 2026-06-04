#ifndef CONNECTION_H
#define CONNECTION_H

// 处理接受连接
void handle_accept(int listen_fd, int epfd);
// 读取客户端的数据
void handle_client_data(int client_fd, int epfd);

#endif