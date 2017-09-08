
#ifndef _KSTRING_H
#define _KSTRING_H

#include <stddef.h>

void *kmemset(void *pPtr, int pValue, size_t pLen);
void *kmemcpy(void *pDest, const void *pSrc, size_t pBytes);
size_t kstrlen(const char *pStr);
char *kstrtoupper(char *pStr);
char *kstrtolower(char *pStr);

#endif

