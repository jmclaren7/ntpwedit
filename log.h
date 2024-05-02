void WriteLog(char *format, ...);
#ifdef DEBUG
#define DebugLog(a) WriteLog a
#else
#define DebugLog(a)
#endif
