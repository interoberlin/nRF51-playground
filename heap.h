
#include <stdint.h>

#define NULL 0

void* malloc(uint32_t size);
void  free(void* ptr);

void* memset(void* s, uint8_t c, uint32_t n);
void* memcpy(void* str1, void* str2, uint32_t n);
