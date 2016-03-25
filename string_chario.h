#ifndef STRING_CHARIO_H
#define STRING_CHARIO_H

#include <stddef.h>
#include "ichario.h"
#include "array_symbolio.h"

class StringCharOut : public ICharOut, public ArraySymbolOut<unsigned char> {
private:
	typedef ArraySymbolOut<unsigned char> array_symbol_out_type;
public:
	StringCharOut(unsigned char *b_str, size_t b_size);
	void put(unsigned char c);
	void finish();
	size_t get_length() const;
};

class StringCharIn : public ICharIn, public ArraySymbolIn<unsigned char> {
private:
	typedef ArraySymbolIn<unsigned char> array_symbol_in_type;
public:
	StringCharIn(const unsigned char *b_str, size_t b_size);
	bool get(unsigned char &c);
	int get_with_eof();
};

#endif/*STRING_CHARIO_H*/

