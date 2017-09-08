
#include <stdint.h>
#include "kstring.h"

void *kmemset(void *pPtr, int pValue, size_t pLen) {
	unsigned char realVal = (unsigned char)pValue;
	unsigned char *realPtr = (unsigned char*)pPtr;
	
	for(size_t i = 0; i < pLen; i++) {
		realPtr[i] = realVal;
	}
	
	return pPtr;
}

void *kmemcpy(void *pDest, const void *pSrc, size_t pBytes) {
	uint8_t *dst = (uint8_t*)pDest;
	uint8_t *src = (uint8_t*)pSrc;

	for(size_t i = 0; i < pBytes; i++, dst++, src++) {
		*dst = *src;
	}

	return pDest;
}

size_t kstrlen(const char *pStr) {
	size_t len = 0;
	while(pStr[len] != 0) {
		len++;
	}
	return len;
}

char *kstrtoupper(char *pStr) {
	char *ptr = pStr;
	while(*ptr != 0) {
		if(*ptr >= 97 && *ptr <= 122) {
			*ptr -= 32;
		}
		ptr++;
	}
	return pStr;
}

char *kstrtolower(char *pStr) {
	char *ptr = pStr;
	while(*ptr != 0) {
		if(*ptr >= 65 && *ptr <= 90) {
			*ptr += 32;
		}
		ptr++;
	}
	return pStr;
}

