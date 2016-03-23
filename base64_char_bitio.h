#ifndef BASE64_CHAR_BITIO_H
#define BASE64_CHAR_BITIO_H

#include <stdint.h>
#include "ibitio.h"
#include "ichario.h"

class Base64CharBitOut : public IBitOut {
private:
	ICharOut &char_out;
	uint_fast8_t remainder;
	uint_fast8_t mask;
	
public:
	Base64CharBitOut(ICharOut &b_char_out);
	void put(bit_type bit);
	void finish();
};

class Base64CharBitIn : public IBitIn {
private:
	ICharIn &char_in;
	uint_fast8_t remainder;
	uint_fast8_t rem_counter;
	int next_c;
	
public:
	Base64CharBitIn(ICharIn &b_char_in);
	bit_or_eof_type get_with_eof();
};

#endif/*BASE64_CHAR_BITIO_H*/

