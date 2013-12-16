//
//  log.c
//  CCanOpenStack
//
//  Created by Timo Jääskeläinen on 20.10.2013.
//  Copyright (c) 2013 Timo Jääskeläinen. All rights reserved.
//

#include <stdio.h>
#include <stdarg.h>
#include "log.h"

/****************************** Global Functions *****************************/
extern void log_write_ln(const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start (args, format);
    vsnprintf (buffer, 256, format, args);
    printf("%s\n", buffer);
    va_end (args);
}
extern void log_write(const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start (args, format);
    vsnprintf (buffer, 256, format, args);
    printf("%s", buffer);
    va_end (args);
}