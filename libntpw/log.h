#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

void vlogprintf(char const *format, va_list va);
void logprintf(char const *format, ...);
void flogprintf(FILE *f, char const *format, ...);

#ifndef NDEBUG
#define logdebug(a) logprintf a
#else
#define logdebug(a)
#endif /* NDEBUG */

#ifdef _MSC_VER
#define snprintf _snprintf
#endif /* _MSC_VER */

#ifdef _WIN32
#define bzero(a, b) memset((a), 0, (b))
#endif /* _WIN32 */

#undef printf
#undef putchar
#define printf logprintf
#define putchar(x) logprintf("%c", (x))

#ifdef fprintf
#undef fprintf
#endif /* fprintf */
#define fprintf flogprintf

#endif /* __LOG_H__ */
