#include <stdint.h>

void * memset(void * destination, int32_t c, uint64_t length) {
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length) {
	/*
	* memcpy does not support overlapping buffers, so always do it
	* forwards. (Don't change this without adjusting memmove.)
	*
	* For speedy copying, optimize the common case where both pointers
	* and the length are word-aligned, and copy word-at-a-time instead
	* of byte-at-a-time. Otherwise, copy by bytes.
	*
	* The alignment logic below should be portable. We rely on
	* the compiler to be reasonably intelligent about optimizing
	* the divides and modulos out. Fortunately, it is.
	*/
	uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	} else {
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

/**
 * @brief
 *  Concat two strings.
 * @param dest
 * @param src
 * @return char*
 */
char *strcat(char *dest, const char *src) {
	char *aux;
	for (aux = dest; *aux != '\0'; aux++);
	for (; *src != '\0';)
		*(aux++) = *(src++);
	*aux = '\0';
	return aux;
}

/**
 * @brief
 *  Copy the content of src into dest.
 * @param dest
 * @param src
 * @return char*
 */
char *strcpy(char *dest, const char *src) {
	char *aux;
	for (aux = dest; *src != '\0';)
		*(aux++) = *(src++);
	*aux = '\0';
	return aux;
}

long itoa(long number, char *str) {
	int digits = 1;
	for (long n = number / 10; n != 0; digits++, n /= 10);

	if (digits == 1) {
		str[0] = '0';
		str[1] = number + '0';
		str[2] = 0;
		return digits;
	}

	str[digits] = 0;
	for (int i = digits - 1; i >= 0; i--) {
		str[i] = (number % 10) + '0';
		number /= 10;
	}

	return digits;
}

int strcmp(char * str1, char * str2) {
	while(*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

int strlen(char *str) {
	int i = 0;
	for (; str[i] != '\0' && *str != '\0'; i++);
	return i;
}


