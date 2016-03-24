#ifndef BIT_SYMBOLIO_H
#define BIT_SYMBOLIO_H

#include "isymbolio.h"
#include "ibitio.h"

template <typename SYMBOL_TYPE>
class BitSymbolOut : public ISymbolOut<SYMBOL_TYPE> {
static_assert((SYMBOL_TYPE)-1 > 0, "SYMBOL_TYPE must be unsigned");
public:
	typedef SYMBOL_TYPE symbol_type;
	
private:
	IBitOut &bit_out;
	size_t symbol_bsize;
	
public:
	// symbol_bsize - symbol size in bits
	BitSymbolOut(IBitOut &b_bit_out, size_t b_symbol_bsize) : bit_out(b_bit_out), symbol_bsize(b_symbol_bsize) {
		assert(symbol_bsize <= sizeof(symbol_type) * 8);
	}
	
	void put(symbol_type symbol) {
		symbol_type mask = 1;
		for (size_t i=0; i<symbol_bsize; ++i, mask <<= 1) {
			bit_out.put((symbol & mask) != 0);
		}
	}
	
	void finish() {
		bit_out.finish();
	}
};

template <typename SYMBOL_TYPE>
class BitSymbolIn : public ISymbolIn<SYMBOL_TYPE> {
static_assert((SYMBOL_TYPE)-1 > 0, "SYMBOL_TYPE must be unsigned");
public:
	typedef SYMBOL_TYPE symbol_type;

private:
	IBitIn &bit_in;
	size_t symbol_bsize;
	
public:
	BitSymbolIn(IBitIn &b_bit_in, size_t b_symbol_bsize) : bit_in(b_bit_in), symbol_bsize(b_symbol_bsize) {
		assert(symbol_bsize <= sizeof(symbol_type) * 8);
	}
	
	bool get(symbol_type &symbol) {
		symbol_type result = 0;
		symbol_type mask = 1;
		for (size_t i=0; i<symbol_bsize; ++i, mask <<= 1) {
			IBitIn::bit_or_eof_type bit = bit_in.get_with_eof();
			if (bit == IBitIn::EOF_VALUE) return false;
			if (bit) result |= mask;
			else result &= ~mask;
		}
		symbol = result;
		return true;
	}
};

#endif/*BIT_SYMBOLIO_H*/

