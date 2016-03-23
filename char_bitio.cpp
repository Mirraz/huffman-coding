#include <assert.h>
#include "char_bitio.h"

#define CHAR_MASK_MAX (1<<7)
#define CHAR_BSIZE 8

CharBitOut::CharBitOut(ICharOut &b_char_out) : char_out(b_char_out), remainder(0), mask(1) {}

void CharBitOut::put(bit_type bit) {
	if (bit) remainder |= mask;
	else remainder &= ~mask;
	if (mask != CHAR_MASK_MAX) {
		mask <<= 1;
	} else {
		char_out.put(remainder);
		mask = 1;
	}
}

void CharBitOut::finish() {
	put(1);
	if (mask != 1) {
		while (true) {
			remainder &= ~mask;
			if (mask == CHAR_MASK_MAX) break;
			mask <<= 1;
		}
		char_out.put(remainder);
		mask = 1;
	}
	char_out.finish();
}

CharBitIn::CharBitIn(ICharIn &b_char_in) : char_in(b_char_in), remainder(0), rem_counter(0) {
	next_c = char_in.get_with_eof();
}

IBitIn::bit_or_eof_type CharBitIn::get_with_eof() {
	if (rem_counter == 0) {
		if (next_c == ICharIn::EOF_VALUE) return IBitIn::EOF_VALUE;
		assert((next_c & ~0xFF) == 0);
		remainder = next_c & 0xFF;
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

