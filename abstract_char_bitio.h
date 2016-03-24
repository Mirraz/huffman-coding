#ifndef ABSTRACT_CHAR_BITIO_H
#define ABSTRACT_CHAR_BITIO_H

#include <assert.h>
#include <stdint.h>
#include "ibitio.h"
#include "ichario.h"

typedef char (*code_to_char_function_type)(uint_fast8_t code);

template <uint_fast8_t CODE_BSIZE, code_to_char_function_type code_to_char>
class AbstractCharBitOut : public IBitOut {
private:
	static_assert(CODE_BSIZE > 0, "CODE_BSIZE can't be zero");
	static_assert(CODE_BSIZE <= 8, "CODE_BSIZE is too big");
	static constexpr uint_fast8_t CODE_MASK_MAX = 1<<(CODE_BSIZE-1);
	
	ICharOut &char_out;
	uint_fast8_t remainder;
	uint_fast8_t mask;
	
public:
	AbstractCharBitOut(ICharOut &b_char_out) : char_out(b_char_out), remainder(0), mask(1) {}
	
	void put(bit_type bit) {
		if (bit) remainder |= mask;
		else remainder &= ~mask;
		if (mask != CODE_MASK_MAX) {
			mask <<= 1;
		} else {
			char_out.put(code_to_char(remainder));
			mask = 1;
		}
	}
	
	void finish() {
		put(1);
		if (mask != 1) {
			while (true) {
				remainder &= ~mask;
				if (mask == CODE_MASK_MAX) break;
				mask <<= 1;
			}
			char_out.put(code_to_char(remainder));
			mask = 1;
		}
		char_out.finish();
	}
};

typedef uint_fast8_t (*char_to_code_function_type)(char c);

template <uint_fast8_t CODE_BSIZE, char_to_code_function_type char_to_code>
class AbstractCharBitIn : public IBitIn {
private:
	static_assert(CODE_BSIZE > 0, "CODE_BSIZE can't be zero");
	static_assert(CODE_BSIZE <= 8, "CODE_BSIZE is too big");
	static constexpr uint_fast8_t CODE_MASK_MAX = 1<<(CODE_BSIZE-1);
	
	ICharIn &char_in;
	uint_fast8_t remainder;
	uint_fast8_t rem_counter;
	int next_c;
	
public:
	AbstractCharBitIn(ICharIn &b_char_in) : char_in(b_char_in), remainder(0), rem_counter(0) {
		next_c = char_in.get_with_eof();
	}
	
	bit_or_eof_type get_with_eof() {
		if (rem_counter == 0) {
			if (next_c == ICharIn::EOF_VALUE) return IBitIn::EOF_VALUE;
			assert((next_c & ~0xFF) == 0);
			remainder = char_to_code(next_c & 0xFF);
			rem_counter = CODE_BSIZE;
			next_c = char_in.get_with_eof();
			if (next_c == ICharIn::EOF_VALUE) {
				assert(remainder != 0);
				for (uint_fast8_t mask = CODE_MASK_MAX; !(remainder & mask); mask >>= 1) --rem_counter;
				assert(rem_counter > 0 && rem_counter <= CODE_BSIZE);
				--rem_counter;
				if (rem_counter == 0) return IBitIn::EOF_VALUE;
			}
		}
		bit_type bit = remainder & 1;
		remainder >>= 1;
		--rem_counter;
		return bit;
	}
};

#endif/*ABSTRACT_CHAR_BITIO_H*/

