#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
	uint32_t i;
	for (i = 0; s[i];i++);
	return i;
}

char *strcpy(char *dst, const char *src) {
	int i = 0;
	while (src[i]) {
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
	return dst;
}

char *strncpy(char *dst, const char *src, size_t n) {
  panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
	int i = 0;
	while (dst[i]) i++;
	int j = 0;
	while (src[j]) {
		dst[i] = src[j];
		i++;
		j++;
	}
	dst[i] = '\0';
	return dst;
}

int strcmp(const char *s1, const char *s2) {
	int i = 0;
	while (*s1 && *s2){
		i = *s1 - *s2;
		if (i != 0) return i;
		s1++;
		s2++;
	}
	return (*s1 - *s2);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
	for (int i = 0; i < n; i++) {
		*((int8_t*)s + i) = c;
	}
	return s;
}

void *memmove(void *dst, const void *src, size_t n) {
	char *buf = malloc(n);
	int i;
	for(i = 0; i < n; i++) {
		buf[i] = *(char*)(src + i);
	}
	for(i = 0; i < n; i++) {
		*(char*)(dst + i) = buf[i];
	}
	return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
	for (int i = 0; i < n; i++) {
		*((int8_t*)out + i) = *((int8_t*)in + i);
	}
	return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
	int j = 0;
	for (int i = 0; i < n; i++) {
		j = *((int8_t*)s1 + i) - *((int8_t*)s2 + i);
		if (j) return j;
	}
	return 0;
}

#endif
