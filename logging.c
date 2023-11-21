#include "headers.h"
errcode initLog(char *filename)
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d-%H:%M:%S", timeinfo);
    snprintf(filename, 30, "logs/%s.log", timestamp);
    FILE *logFile = fopen(filename, "w");
    if (logFile == NULL)
    {
        return FILE_DESCRIPTOR_ERROR;
    }
    fprintf(logFile, "Logfile created at %s\n", timestamp);
    fclose(logFile);
    return NO_ERROR;
}
errcode logThis(const char *filename, logLevel level, comm whosTalking, const char *format, ...)
{
    FILE *logFile = fopen(filename, "a");
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
        break;
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
errcode logOperation(const char *filename, MessageClient2NM message)
{
    switch (message.operation)
    {
    case CREATE:
        return logThis(filename, LOG_INFO, CLIENT_NM, "Operation: CREATE - Create a new file/folder. Buffer: %s", message.buffer);
    case READ:
        return logThis(filename, LOG_INFO, CLIENT_NM, "Operation: READ - Read the content of a file. Buffer: %s", message.buffer);
    case WRITE:
        return logThis(filename, LOG_INFO, CLIENT_NM, "Operation: WRITE - Write data to a file. Buffer: %s", message.buffer);
    case DELETE:
        return logThis(filename, LOG_INFO, CLIENT_NM, "Operation: DELETE - Delete a file/folder. Buffer: %s", message.buffer);
    case COPY:
        return logThis(filename, LOG_INFO, CLIENT_NM, "Operation: OPEN - Open a file for reading/writing. Buffer: %s", message.buffer);
    case METADATA:
        return logThis(filename, LOG_INFO, CLIENT_NM, "Operation: METADATA - Get metadata information about a file. Buffer: %s", message.buffer);
    case TERMINATE:
        return logThis(filename, LOG_INFO, CLIENT_NM, "Operation: TERMINATE - Terminate connection");
    default:
        return logThis(filename, LOG_INFO, CLIENT_NM, "Unknown operation. Buffer: %s", message.buffer);
    }
}