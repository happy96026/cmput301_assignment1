#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>


void FATAL(const char *fmt, ...) {
	va_list ap;
	fflush(stdout);
	va_start(ap, fmt); vfprintf(stderr, fmt, ap); va_end(ap);
	fflush(NULL);
	exit(1);
}

void WARNING(const char *fmt, ...) {
	va_list ap;
	fflush(stdout);
	va_start(ap, fmt); vfprintf(stderr, fmt, ap); va_end(ap);
}
