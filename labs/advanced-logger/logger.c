#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>
#include "logger.h"

#define STDOUT 0
#define SYSLOG 1

int tipoDeLog;

void boldred () {
  printf("\033[1;31m");
}

void red () {
  printf("\033[0;31m");
}

void green(){
    printf("\033[0;32m");
}

void yellow(){
  printf("\033[0;33m");
}

void reset () {
  printf("\033[0m");
}

int printStdOut(const char *format, va_list arg){
    vprintf(format, arg);
    vprintf ("\n",arg);
    return 0;
}


int printSysLog(const char *format, va_list arg){
    vsyslog(LOG_INFO, format, arg);
    return 0;
}

int write(const char *format, va_list arg){
    if(tipoDeLog == STDOUT){
        printStdOut(format,arg);
    } else if(tipoDeLog == SYSLOG){
        printSysLog(format,arg);
    }
}


int initLogger(char *logType){
    printf("Initializing Logger on: %s\n", logType);

    if (strcmp(logType, "stdout") == 0){
        tipoDeLog = STDOUT;
    }
    else if (strcmp(logType, "syslog") == 0){

        tipoDeLog= SYSLOG;
    }
    else{
        printf("Invalid log type");
    }
    return 0;
}

int infof(const char *format, ...) {
    va_list arg;
    va_start (arg, format);
    green();
    write(format, arg);
    va_end (arg);
    reset();
    return 0;
}

int warnf(const char *format, ...) {
    va_list arg;
    va_start (arg, format);
    yellow();
    write (format, arg);
    va_end (arg);
    reset();
    return 0;
}

int errorf(const char *format, ...) {
    va_list arg;
    va_start (arg, format);
    red();
    write(format, arg);
    va_end (arg);
    reset();
    return 0;
}

int panicf(const char *format, ...) {
    va_list arg;
    va_start (arg, format);
    boldred();
    write (format, arg);
    va_end (arg);
    reset();
    return 0;
}

