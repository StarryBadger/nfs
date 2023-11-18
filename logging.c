#include "headers.h"
errcode logThis(const char *format, ...)
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

    va_list args;
    va_start(args, format);
    vfprintf(logFile, format, args);
    va_end(args);
    fclose(logFile);
    return NO_ERROR;
}
