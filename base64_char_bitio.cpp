#include <assert.h>
#include <stdint.h>
#include "base64_char_bitio.h"

static const char base64codes[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char base64_code_to_char(uint_fast8_t code) {
	assert(code < 64);
	return base64codes[code];
}

uint_fast8_t base64_char_to_code(char c) {
	if (c >= 'A' && c <= 'Z') return c - 'A';
	if (c >= 'a' && c <= 'z') return c - 'a' + 26;
	if (c >= '0' && c <= '9') return c - '0' + 26*2;
	if (c == '+') return 62;
	if (c == '/') return 63;
	assert(0);
	return 64;
}

