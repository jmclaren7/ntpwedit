#ifndef __LOG_H__
#define __LOG_H__

void logprintf(char *format, ...);
#ifndef NDEBUG
#define logdebug(a) logprintf a
#else
#define logdebug(a)
#endif /* NDEBUG */

#endif /* __LOG_H__ */
