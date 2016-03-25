#ifndef BASE64_CHAR_BITIO_H
#define BASE64_CHAR_BITIO_H

#include <stdint.h>
#include "symbol_bitio.h"

class Base64CharBitOut : public SymbolBitOut<unsigned char, 6> {
public:
	Base64CharBitOut(ISymbolOut<unsigned char> &b_symbol_out);
	
protected:
	unsigned char code_to_symbol(unsigned char code);
};

class Base64CharBitIn : public SymbolBitIn<unsigned char, 6> {
public:
	Base64CharBitIn(ISymbolIn<unsigned char> &b_symbol_in);
	
protected:
	unsigned char symbol_to_code(unsigned char symbol);
};

#endif/*BASE64_CHAR_BITIO_H*/

