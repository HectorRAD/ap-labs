// Logger
#include <stdarg.h>
int initLogger(char *logType);
int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);
void red();
void boldred();
void green();
void yellow();
void reset();
int printStdOut(const char *format, va_list);
int printSysLog(const char *format, va_list);
int writeLog(const char *format, va_list arg);