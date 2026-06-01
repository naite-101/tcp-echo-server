#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include "logger.h"
#include <stdio.h>

//静态全局的变量 
static LogLevel current_level = LOG_LEVEL_INFO ;

static void get_time_str(char *buf ,size_t size){
    time_t now = time(NULL);
    struct tm *tm_info =localtime(&now);
    strftime(buf ,size ,"%Y-%m-%d %H:%M:%S",tm_info);
}

static void log_print(LogLevel level, const char *tag, const char *msg)
{
    if(level < current_level)return;
    char time_buf[20];
    get_time_str(time_buf, sizeof(time_buf));
    printf("[%s][%s] %s\n",time_buf, tag, msg);
    fflush(stdout);
}


void log_set_level(LogLevel level)
{
    current_level = level;
}

void log_connect(int fd, struct sockaddr_in *addr)
{
    char ip [INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));

    char msg[256];
    snprintf(msg, sizeof(msg),fd, ip, ntohs(addr->sin_port));
    log_print(LOG_LEVEL_INFO, "CONNECT",msg);
}

void log_recv(int fd, const char *buf, size_t len)
{
    char msg[512];
    int show_len = len < 100 ? (int)len : 100;
    snprintf(msg, sizeof(msg), "收到数据: fd=%d, 长度=%zu, 内容=%.*s",
            fd, len ,show_len, buf);
    log_print(LOG_LEVEL_INFO, "RECV", msg);
}

void log_disconnect(int fd)
{
    char msg[64];
    snprintf(msg, sizeof(msg), "断开连接: fd=%d", fd);
    log_print(LOG_LEVEL_INFO, "DISCONNECT", msg);
}

void log_error(const char *msg)
{
    log_print(LOG_LEVEL_ERROR, "ERROR", msg);
}

void log_info(const char *msg)
{
    log_print(LOG_LEVEL_INFO,"INFO", msg);
}

#ifdef DEBUG
void _log_debug(const char *file, int line, const char *fmt, ...)
{
    if(LOG_LEVEL_DEBUG < current_level)return;

    char time_buf[20];
    get_time_str(time_buf, sizeof(time_buf));

    char msg[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    printf("[%s] [DEBUG] [%s:%d] %s\n", time_buf, file, line, msg);
    fflush(sydout);
}
#endif