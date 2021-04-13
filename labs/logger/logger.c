#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

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

int infof(const char *format, ...){
    va_list arg;
    va_start (arg, format);
    green();
    vprintf (format, arg);
    va_end (arg);
    reset();
    return 0;
}

int warnf(const char *format, ...){
    va_list arg;
    va_start (arg, format);
    yellow();
    vprintf (format, arg);
    va_end (arg);
    reset();
    return 0;
}

int errorf(const char *format, ...){
    va_list arg;
    va_start (arg, format);
    red();
    vprintf (format, arg);
    va_end (arg);
    reset();
    return 0;
}

int panicf(const char *format, ...){
    va_list arg;
    va_start (arg, format);
    boldred();
    vprintf (format, arg);
    va_end (arg);
    reset();
    return 0;
}
