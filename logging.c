#include "headers.h"

errcode logThis(logLevel level, comm whosTalking, const char *format, ...)
{
    FILE *logFile = fopen("interaction@NM.log", "a");
    if (logFile == NULL)
    {
        return FILE_DESCRIPTOR_ERROR;
    }
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    fprintf(logFile, "[%04d-%02d-%02d %02d:%02d:%02d] ",
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    const char *levelPrefix;
    const char *commPrefix;
    switch (level)
    {
    case LOG_INFO:
        levelPrefix = "INFO";
        break;
    case LOG_WARNING:
        levelPrefix = "WARNING";
        break;
    case LOG_ERROR:
        levelPrefix = "ERROR";
        break;
    default:
        levelPrefix = "UNKNOWN";
    }
    switch (whosTalking)
    {
    case CLIENT_NM:
        commPrefix = "CLIENT->NM";
        break;
    case NM_CLIENT:
        commPrefix = " NM->CLIENT";
        break;
    case SS_NM:
        commPrefix = "SS->NM";
        break;
    case NM_SS:
        commPrefix = "NM->SS";
    default:
        commPrefix = "UNKNOWN";
    }

    fprintf(logFile, "[%s] [%s] ", levelPrefix, commPrefix);

    va_list args;
    va_start(args, format);
    vfprintf(logFile, format, args);
    va_end(args);

    fprintf(logFile, "\n");
    fclose(logFile);
    return NO_ERROR;
}
