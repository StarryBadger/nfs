#ifndef __LOGGING_H
#define __LOGGING_H
#include "communicationProtocols.h"
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
errcode initLog(char *filename);
errcode logOperation(const char *filename, MessageClient2NM message);
errcode logThis(const char *filename, logLevel level, comm whosTalking, const char *format, ...);
#endif