#ifndef STRING_CHARIO_H
#define STRING_CHARIO_H

#include <stddef.h>
#include "ichario.h"
#include "array_symbolio.h"

class StringCharOut : public ICharOut, public ArraySymbolOut<char> {
private:
	typedef ArraySymbolOut<char> array_symbol_out_type;
public:
	StringCharOut(char *b_str, size_t b_size);
	void put(char c);
	void finish();
	size_t get_length() const;
};

class StringCharIn : public ICharIn, public ArraySymbolIn<char> {
private:
	typedef ArraySymbolIn<char> array_symbol_in_type;
public:
	StringCharIn(const char *b_str, size_t b_size);
	bool get(char &c);
	int get_with_eof();
};

#endif/*STRING_CHARIO_H*/

