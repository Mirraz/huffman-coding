#include <assert.h>
#include <stdint.h>
#include "base64.h"

static const char base64codes[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static uint_fast8_t base64_char_to_idx(char c) {
	if (c >= 'A' && c <= 'Z') return c - 'A';
	if (c >= 'a' && c <= 'z') return c - 'a' + 26;
	if (c >= '0' && c <= '9') return c - '0' + 26*2;
	if (c == '+') return 62;
	if (c == '/') return 63;
	assert(0);
	return 64;
}

#define BASE64_MASK_END (1<<6)

Base64Encoder::Base64Encoder(ICharPrinter &b_char_printer) :
	char_printer(b_char_printer), remainder(0), mask(1) {}

void Base64Encoder::encode(const bit_type bits[], size_t count) {
	size_t i = 0;
	while (i < count) {
		for (; i<count && mask != BASE64_MASK_END; i++, mask <<= 1) {
			if (bits[i]) remainder |= mask;
			else remainder &= ~mask;
		}
		if (mask != BASE64_MASK_END) break;
		assert(remainder < 64);
		char_printer.putchar(base64codes[remainder]);
		mask = 1;
	}
}

void Base64Encoder::finish() {
	bit_type term_bit = 1;
	encode(&term_bit, 1);
	if (mask != 1) {
		for (; mask != BASE64_MASK_END; mask <<= 1) {
			remainder &= ~mask;
		}
		assert(remainder < 64);
		char_printer.putchar(base64codes[remainder]);
		mask = 1;
	}
	char_printer.finish();
}

Base64Decoder::Base64Decoder(ICharReader &b_char_reader) :
	char_reader(b_char_reader), remainder(0), rem_counter(0) {}

bit_or_eof_type Base64Decoder::decode_bit() {
	if (char_reader.is_end() && rem_counter == 0) return BIT_EOF_VALUE;
	if (rem_counter == 0) {
		remainder = base64_char_to_idx(char_reader.getchar());
		assert(remainder < 64);
		rem_counter = 6;
		if (char_reader.is_end()) {
			assert(remainder != 0);
			for (uint_fast8_t mask = 1 << 5; !(remainder & mask); mask >>= 1) --rem_counter;
			assert(rem_counter > 0 && rem_counter <= 6);
			--rem_counter;
			if (rem_counter == 0) return BIT_EOF_VALUE;
		}
	}
	bit_type bit = remainder & 1;
	remainder >>= 1;
	--rem_counter;
	return bit;
}

