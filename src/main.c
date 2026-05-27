#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/common.h"
#include "server_init.h"

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;
    
    // 如果命令行指定了端口，使用指定的端口
    if (argc > 1) {
        port = atoi(argv[1]);
    }
    
    // 初始化服务器
    int listen_fd = init_server(port);
    if (listen_fd < 0) {
        printf("❌ 服务器初始化失败\n");
        return 1;
    }
    
    printf("监听套接字 fd = %d\n", listen_fd);
    printf("按回车键退出...\n");
    getchar();  // 等待用户按回车
    
    // 清理
    close(listen_fd);
    printf("服务器已关闭\n");
    
    return 0;
}