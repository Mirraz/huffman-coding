#include <assert.h>
#include "binary.h"

BinaryEncoder::BinaryEncoder(ICharPrinter &b_char_printer) :
	char_printer(b_char_printer), remainder(0), mask(1) {}

#define CHAR_MASK_MAX (1<<7)

void BinaryEncoder::encode_bit(bit_type bit) {
	if (bit) remainder |= mask;
	else remainder &= ~mask;
	if (mask != CHAR_MASK_MAX) {
		mask <<= 1;
	} else {
		char_printer.putchar(remainder);
		mask = 1;
	}
}

void BinaryEncoder::encode(const bit_type bits[], size_t count) {
	for (size_t i=0; i<count; ++i) encode_bit(bits[i]);
}

void BinaryEncoder::finish() {
	encode_bit(1);
	if (mask != 1) {
		while (true) {
			remainder &= ~mask;
			if (mask == CHAR_MASK_MAX) break;
			mask <<= 1;
		}
		char_printer.putchar(remainder);
		mask = 1;
	}
	char_printer.finish();
}

BinaryDecoder::BinaryDecoder(ICharReader &b_char_reader) :
	char_reader(b_char_reader), remainder(0), rem_counter(0) {}

bit_or_eof_type BinaryDecoder::decode_bit() {
	if (char_reader.is_end() && rem_counter == 0) return BIT_EOF_VALUE;
	if (rem_counter == 0) {
		remainder = char_reader.getchar();
		rem_counter = 8;
		if (char_reader.is_end()) {
			assert(remainder != 0);
			for (uint_fast8_t mask = CHAR_MASK_MAX; !(remainder & mask); mask >>= 1) --rem_counter;
			assert(rem_counter > 0 && rem_counter <= 8);
			--rem_counter;
			if (rem_counter == 0) return BIT_EOF_VALUE;
		}
	}
	bit_type bit = remainder & 1;
	remainder >>= 1;
	--rem_counter;
	return bit;
}

