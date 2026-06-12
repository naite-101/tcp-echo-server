#ifndef LOGGER_H
#define LOGGER_H

#include <netinet/in.h>
#include <stddef.h>

// 创建一个枚举类型
typedef enum
{
    // 设置为调试级别
    LOG_LEVEL_DEBUG = 0,
    // 设置为信息级别
    LOG_LEVEL_INFO = 1,
    // 设置为错误级别
    LOG_LEVEL_ERROR = 2
} LogLevel;

// 设置日志的级别
void log_set_level(LogLevel level);

// 记录连接的日志
void log_connect(int fd, struct sockaddr_in *addr);

// 记录接收数据日志
void log_recv(int fd, const char *buf, size_t len);

// 记录断开连接的日志
void log_disconnect(int fd);

// 记录错误的日志
void log_error(const char *fmt, ...);

// 记录信息的日志
void log_info(const char *fmt, ...);

// 只有在编译时定义看DEBUG，才能调试日志代码
#ifdef DEBUG
// 调试日志宏的定义
#define log_debug(fmt, ...) _log_debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
void _log_debug(const char *file, int line, const char *fmt, ...);
#else
#define log_debug(fmt, ...)
#endif

#endif