#include <assert.h>
#include <stdint.h>
#include "base64_char_bitio.h"

Base64CharBitOut::Base64CharBitOut(ISymbolOut<unsigned char> &b_symbol_out) :
	SymbolBitOut<unsigned char, 6>(b_symbol_out) {}

static const char base64codes[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

unsigned char Base64CharBitOut::code_to_symbol(unsigned char code) {
	assert(code < 64);
	return base64codes[code];
}

Base64CharBitIn::Base64CharBitIn(ISymbolIn<unsigned char> &b_symbol_in) :
	SymbolBitIn<unsigned char, 6>(b_symbol_in) {}

unsigned char Base64CharBitIn::symbol_to_code(unsigned char symbol) {
	char c = symbol;
	if (c >= 'A' && c <= 'Z') return c - 'A';
	if (c >= 'a' && c <= 'z') return c - 'a' + 26;
	if (c >= '0' && c <= '9') return c - '0' + 26*2;
	if (c == '+') return 62;
	if (c == '/') return 63;
	assert(0);
	return 64;
}

