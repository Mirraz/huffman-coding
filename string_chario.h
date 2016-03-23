#ifndef STRING_CHARIO_H
#define STRING_CHARIO_H

#include <stddef.h>
#include "ichario.h"

class StringCharOut : public ICharOut {
private:
	char *str;
	size_t size;
	size_t idx;
public:
	StringCharOut(char *b_str, size_t b_size);
	void put(char c);
	void finish();
	size_t get_length() const;
};

class StringCharIn : public ICharIn {
private:
	const char *str;
	size_t size;
	size_t idx;
	
public:
	StringCharIn(const char *b_str, size_t b_size);
	int get_with_eof();
};

#endif/*STRING_CHARIO_H*/

