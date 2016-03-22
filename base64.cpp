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

Base64Encoder::Base64Encoder(ICharPrinter &b_char_printer) :
	char_printer(b_char_printer), remainder(0), mask(1) {}

#define BASE64_MASK_MAX (1<<5)

void Base64Encoder::encode_bit(bit_type bit) {
	if (bit) remainder |= mask;
	else remainder &= ~mask;
	if (mask != BASE64_MASK_MAX) {
		mask <<= 1;
	} else {
		char_printer.putchar(base64codes[remainder]);
		mask = 1;
	}
}

void Base64Encoder::encode(const bit_type bits[], size_t count) {
	for (size_t i=0; i<count; ++i) encode_bit(bits[i]);
}

void Base64Encoder::finish() {
	encode_bit(1);
	if (mask != 1) {
		while (true) {
			remainder &= ~mask;
			if (mask == BASE64_MASK_MAX) break;
			mask <<= 1;
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
			for (uint_fast8_t mask = BASE64_MASK_MAX; !(remainder & mask); mask >>= 1) --rem_counter;
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

