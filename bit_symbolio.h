#ifndef BIT_SYMBOLIO_H
#define BIT_SYMBOLIO_H

#include "isymbolio.h"
#include "ibitio.h"

// SYMBOL_BSIZE - symbol size in bits
template <typename SYMBOL_TYPE, size_t SYMBOL_BSIZE = sizeof(SYMBOL_TYPE)*8>
class BitSymbolOut : public ISymbolOut<SYMBOL_TYPE> {
	static_assert((SYMBOL_TYPE)-1 > 0, "SYMBOL_TYPE must be unsigned");
	static_assert(SYMBOL_BSIZE > 0, "SYMBOL_BSIZE can't be zero");
	static_assert(SYMBOL_BSIZE <= sizeof(SYMBOL_TYPE)*8, "SYMBOL_BSIZE bits don't fit in SYMBOL_TYPE");
	
public:
	typedef SYMBOL_TYPE symbol_type;
	
private:
	IBitOut &bit_out;
	
public:
	
	BitSymbolOut(IBitOut &b_bit_out) : bit_out(b_bit_out) {}
	
	void put(symbol_type symbol) {
		// TODO: use bit_out.put_array
		symbol_type mask = 1;
		for (size_t i=0; i<SYMBOL_BSIZE; ++i, mask <<= 1) {
			bit_out.put((symbol & mask) != 0);
		}
	}
	
	void finish() {
		bit_out.finish();
	}
};

// SYMBOL_BSIZE - symbol size in bits
template <typename SYMBOL_TYPE, size_t SYMBOL_BSIZE = sizeof(SYMBOL_TYPE)*8>
class BitSymbolIn : public ISymbolIn<SYMBOL_TYPE> {
	static_assert((SYMBOL_TYPE)-1 > 0, "SYMBOL_TYPE must be unsigned");
	static_assert(SYMBOL_BSIZE > 0, "SYMBOL_BSIZE can't be zero");
	static_assert(SYMBOL_BSIZE <= sizeof(SYMBOL_TYPE)*8, "SYMBOL_BSIZE bits don't fit in SYMBOL_TYPE");
	
public:
	typedef SYMBOL_TYPE symbol_type;

private:
	IBitIn &bit_in;
	
public:
	BitSymbolIn(IBitIn &b_bit_in) : bit_in(b_bit_in) {}
	
	bool get(symbol_type &symbol) {
		symbol_type result = 0;
		symbol_type mask = 1;
		for (size_t i=0; i<SYMBOL_BSIZE; ++i, mask <<= 1) {
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

