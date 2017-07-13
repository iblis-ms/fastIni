#ifndef FAST_INI_H_
#define FAST_INI_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int(*DParseListener)(char *, int, char *, int, char *, int);

int fastIniParse(const char *aPath, char *aBufferBeg, size_t aBufferSize, DParseListener aListener);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // FAST_INI_H_
