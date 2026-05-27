#ifndef COMMON_H
#define COMMON_H

// 网络参数
#define MAX_EVENTS 1024
#define BUFFER_SIZE 4096
#define DEFAULT_PORT 8888
#define BACKLOG 128

// 调试宏
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) \
        printf("[DEBUG] %s:%d " fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
#endif

#endif
