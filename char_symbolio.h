#ifndef CHAR_SYMBOLIO_H
#define CHAR_SYMBOLIO_H

#include <assert.h>
#include <stddef.h>
#include "isymbolio.h"
#include "ichario.h"

// SYMBOL_SIZE - in bytes
template <typename SYMBOL_TYPE, size_t SYMBOL_SIZE>
class CharSymbolOut : public ISymbolOut<SYMBOL_TYPE>  {
static_assert((SYMBOL_TYPE)-1 > 0, "SYMBOL_TYPE must be unsigned");
static_assert((SYMBOL_SIZE << 3) >> 3 == SYMBOL_SIZE, "SYMBOL_SIZE overflow");
public:
	typedef SYMBOL_TYPE symbol_type;
	
private:
	ICharOut &char_out;
	
public:
	CharSymbolOut(ICharOut &b_char_out) : char_out(b_char_out) {}
	
	void put(symbol_type symbol) {
		for (size_t i=0; i<SYMBOL_SIZE; ++i) {
			char_out.put(symbol & 0xFF);
			symbol >>= 8;
		}
	}
	
	void finish() {
		char_out.finish();
	}
};

// SYMBOL_SIZE - in bytes
template <typename SYMBOL_TYPE, size_t SYMBOL_SIZE>
class CharSymbolIn : public ISymbolIn<SYMBOL_TYPE> {
static_assert((SYMBOL_TYPE)-1 > 0, "SYMBOL_TYPE must be unsigned");
static_assert((SYMBOL_SIZE << 3) >> 3 == SYMBOL_SIZE, "SYMBOL_SIZE overflow");
public:
	typedef SYMBOL_TYPE symbol_type;
	
private:
	ICharIn &char_in;
	
public:
	CharSymbolIn(ICharIn &b_char_in) : char_in(b_char_in) {}
	
	bool get(symbol_type &symbol) {
		symbol_type result = 0;
		for (size_t i=0; i<SYMBOL_SIZE*8; i+=8) {
			int c = char_in.get_with_eof();
			if (c == ICharIn::EOF_VALUE) return false;
			assert((c & ~0xFF) == 0);
			result |= (c & 0xFF) << i;
		}
		symbol = result;
		return true;
	}
};

#endif/*CHAR_SYMBOLIO_H*/

