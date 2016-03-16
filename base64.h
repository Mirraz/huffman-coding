#ifndef BASE64_H
#define BASE64_H

#include "bitio.h"
#include "chario.h"

class Base64Encoder : public IBitEncoder {
private:
	ICharPrinter &char_printer;
	uint_fast8_t remainder;
	uint_fast8_t mask;
public:
	Base64Encoder(ICharPrinter &b_char_printer);
	void encode(const bit_type bits[], size_t count);
	void finish();
};

class Base64Decoder : public IBitDecoder {
private:
	ICharReader &char_reader;
	uint_fast8_t remainder;
	uint_fast8_t rem_counter;
public:
	Base64Decoder(ICharReader &b_char_reader);
	bit_or_eof_type decode_bit();
};

#endif/*BASE64_H*/

