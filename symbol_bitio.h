#ifndef SYMBOL_BITIO_H
#define SYMBOL_BITIO_H

#include <assert.h>
#include "ibitio.h"
#include "isymbolio.h"

template <typename SYMBOL_TYPE, size_t CODE_BSIZE = sizeof(SYMBOL_TYPE)*8>
class SymbolBitOut : public IBitOut {
	static_assert((SYMBOL_TYPE)-1 > 0, "SYMBOL_TYPE must be unsigned");
	static_assert(CODE_BSIZE > 0, "CODE_BSIZE can't be zero");
	static_assert(CODE_BSIZE <= sizeof(SYMBOL_TYPE)*8, "CODE_BSIZE bits don't fit in SYMBOL_TYPE");
	
public:
	typedef SYMBOL_TYPE symbol_type;
	
private:
	typedef ISymbolOut<symbol_type> symbol_out_type;
	static constexpr symbol_type CODE_MASK_MAX = 1<<(CODE_BSIZE-1);
	
	symbol_out_type &symbol_out;
	symbol_type remainder;
	symbol_type mask;
	
public:
	SymbolBitOut(symbol_out_type &b_symbol_out) : symbol_out(b_symbol_out), remainder(0), mask(1) {}
	
protected:
	virtual symbol_type code_to_symbol(symbol_type code) {
		return code;
	}
	
public:
	void put(bit_type bit) {
		if (bit) remainder |= mask;
		else remainder &= ~mask;
		if (mask != CODE_MASK_MAX) {
			mask <<= 1;
		} else {
			symbol_out.put(code_to_symbol(remainder));
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
			symbol_out.put(code_to_symbol(remainder));
			mask = 1;
		}
		symbol_out.finish();
	}
};

template <typename SYMBOL_TYPE, size_t CODE_BSIZE = sizeof(SYMBOL_TYPE)*8>
class SymbolBitIn : public IBitIn {
private:
	static_assert((SYMBOL_TYPE)-1 > 0, "SYMBOL_TYPE must be unsigned");
	static_assert(CODE_BSIZE > 0, "CODE_BSIZE can't be zero");
	static_assert(CODE_BSIZE <= sizeof(SYMBOL_TYPE)*8, "CODE_BSIZE bits don't fit in SYMBOL_TYPE");
	
public:
	typedef SYMBOL_TYPE symbol_type;

private:
	typedef ISymbolIn<symbol_type> symbol_in_type;
	static constexpr symbol_type CODE_MASK_MAX = 1<<(CODE_BSIZE-1);
	
	symbol_in_type &symbol_in;
	symbol_type remainder;
	symbol_type rem_counter;
	symbol_type next_symbol;
	bool next_result;
	
public:
	SymbolBitIn(symbol_in_type &b_symbol_in) : symbol_in(b_symbol_in), remainder(0), rem_counter(0) {
		next_result = symbol_in.get(next_symbol);
	}
	
protected:
	virtual symbol_type symbol_to_code(symbol_type symbol) {
		return symbol;
	}
	
public:
	bit_or_eof_type get_with_eof() {
		if (rem_counter == 0) {
			if (!next_result) return IBitIn::EOF_VALUE;
			remainder = symbol_to_code(next_symbol);
			rem_counter = CODE_BSIZE;
			next_result = symbol_in.get(next_symbol);
			if (!next_result) {
				assert(remainder != 0);
				for (symbol_type mask = CODE_MASK_MAX; !(remainder & mask); mask >>= 1) --rem_counter;
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

#endif/*SYMBOL_BITIO_H*/

