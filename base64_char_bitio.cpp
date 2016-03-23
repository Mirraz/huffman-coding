#include <assert.h>
#include <stdint.h>
#include "base64_char_bitio.h"

static const char base64codes[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char idx_to_char(uint_fast8_t idx) {
	assert(idx < 64);
	return base64codes[idx];
}

static uint_fast8_t char_to_idx(char c) {
	if (c >= 'A' && c <= 'Z') return c - 'A';
	if (c >= 'a' && c <= 'z') return c - 'a' + 26;
	if (c >= '0' && c <= '9') return c - '0' + 26*2;
	if (c == '+') return 62;
	if (c == '/') return 63;
	assert(0);
	return 64;
}

#define CHAR_MASK_MAX (1<<5)
#define CHAR_BSIZE 6

Base64CharBitOut::Base64CharBitOut(ICharOut &b_char_out) : char_out(b_char_out), remainder(0), mask(1) {}

void Base64CharBitOut::put(bit_type bit) {
	if (bit) remainder |= mask;
	else remainder &= ~mask;
	if (mask != CHAR_MASK_MAX) {
		mask <<= 1;
	} else {
		char_out.put(idx_to_char(remainder));
		mask = 1;
	}
}

void Base64CharBitOut::finish() {
	put(1);
	if (mask != 1) {
		while (true) {
			remainder &= ~mask;
			if (mask == CHAR_MASK_MAX) break;
			mask <<= 1;
		}
		char_out.put(idx_to_char(remainder));
		mask = 1;
	}
	char_out.finish();
}

Base64CharBitIn::Base64CharBitIn(ICharIn &b_char_in) : char_in(b_char_in), remainder(0), rem_counter(0) {
	next_c = char_in.get_with_eof();
}

IBitIn::bit_or_eof_type Base64CharBitIn::get_with_eof() {
	if (rem_counter == 0) {
		if (next_c == ICharIn::EOF_VALUE) return IBitIn::EOF_VALUE;
		assert((next_c & ~0xFF) == 0);
		remainder = char_to_idx(next_c & 0xFF);
		rem_counter = CHAR_BSIZE;
		next_c = char_in.get_with_eof();
		if (next_c == ICharIn::EOF_VALUE) {
			assert(remainder != 0);
			for (uint_fast8_t mask = CHAR_MASK_MAX; !(remainder & mask); mask >>= 1) --rem_counter;
			assert(rem_counter > 0 && rem_counter <= CHAR_BSIZE);
			--rem_counter;
			if (rem_counter == 0) return IBitIn::EOF_VALUE;
		}
	}
	bit_type bit = remainder & 1;
	remainder >>= 1;
	--rem_counter;
	return bit;
}

