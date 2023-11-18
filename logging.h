#ifndef __LOGGING_H
#define __LOGGING_H
typedef enum logLevel
{
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} logLevel;
typedef enum comm
{
    CLIENT_NM,
    NM_CLIENT,
    SS_NM,
    NM_SS
} comm;
errcode logThis(logLevel level,comm whosTalking, const char *format, ...);
#endif