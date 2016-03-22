#ifndef SYMBOLIO_H
#define SYMBOLIO_H

#include <assert.h>
#include "bitio.h"

template <typename SYMBOL_TYPE>
class SymbolIO {
public:

typedef SYMBOL_TYPE symbol_type;

class SymbolEncoder {
private:
	IBitEncoder &encoder;
	size_t symbol_size;
	
public:
	SymbolEncoder(IBitEncoder &b_encoder, size_t b_symbol_size) : encoder(b_encoder), symbol_size(b_symbol_size) {}
	
	void encode(symbol_type symbol) {
		symbol_type mask = 1;
		for (size_t i=0; i<symbol_size; ++i, mask <<= 1) {
			encoder.encode_bit((symbol & mask) != 0);
		}
	}
	
	void finish() {
		encoder.finish();
	}
};

class SymbolDecoder {
private:
	IBitDecoder &decoder;
	size_t symbol_size;
	
public:
	SymbolDecoder(IBitDecoder &b_decoder, size_t b_symbol_size) : decoder(b_decoder), symbol_size(b_symbol_size) {}

public:
	// returns success
	bool decode(symbol_type &result) {
		symbol_type symbol = 0;
		symbol_type mask = 1;
		for (size_t i=0; i<symbol_size; ++i, mask <<= 1) {
			bit_or_eof_type bit = decoder.decode_bit();
			if (bit == BIT_EOF_VALUE) return false;
			if (bit) symbol |= mask;
			else symbol &= ~mask;
		}
		result = symbol;
		return true;
	}
};

};

#endif/*SYMBOLIO_H*/

