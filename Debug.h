#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <assert.h>

void info(const char *fmt, ...);
void warn(const char *fmt, ...);
void error(const char *fmt, ...);
__attribute__((noreturn)) void fatal(const char *fmt, ...);

#endif /* ndef DEBUG_H_INCLUDED */
