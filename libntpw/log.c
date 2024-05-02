/* ===================================================================
 * Copyright (c) 2005-2014 Vadim Druzhin (cdslow@mail.ru).
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; 
 * version 2.1 of the License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 * ===================================================================
 */

#include <stdio.h>
#include <stdarg.h>
#include "log.h"

void vlogprintf(char const *format, va_list va)
    {
#ifndef NDEBUG
    static FILE *log = NULL;

    if(NULL == log)
        log = fopen("ntpw.log", "w");

    if(NULL == log)
        return;

    vfprintf(log, format, va);
    fflush(log);
#else
    (void)format;
    (void)va;
#endif
    }

void logprintf(char const *format, ...)
    {
    va_list va;

    va_start(va, format);
    vlogprintf(format, va);
    va_end(va);
    }

void flogprintf(FILE *f, char const *format, ...)
    {
    va_list va;

    va_start(va, format);

    if(f == stderr || f == stdout)
        vlogprintf(format, va);
    else
        vfprintf(f, format, va);

    va_end(va);
    }
