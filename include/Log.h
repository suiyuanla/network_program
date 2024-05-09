#ifndef LOG_H
#define LOG_H

#include <cstdio>

#ifdef NDEBUG
#define LOG(msg)  // Nothing
#else
#define LOG(msg) fprintf(stderr, "%s(%d): %s\n", __FILE__, __LINE__, msg)
#endif  // NDEBUG

#endif  // LOG_H