/* ===================================================================
 * Copyright (c) 2005-2012 Vadim Druzhin (cdslow@mail.ru).
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

void logprintf(char *format, ...)
    {
#ifndef NDEBUG
    static FILE *log;
    va_list va;

    log=fopen("ntpw.log", "a");
    if(NULL==log)
        return;
    va_start(va, format);
    vfprintf(log, format, va);
    fclose(log);
    va_end(va);
#else
    (void)format;
#endif
    }
