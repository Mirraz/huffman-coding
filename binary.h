#ifndef BINARY_H
#define BINARY_H

#include "bitio.h"
#include "chario.h"

class BinaryEncoder : public IBitEncoder {
private:
	ICharPrinter &char_printer;
	uint_fast8_t remainder;
	uint_fast8_t mask;
public:
	BinaryEncoder(ICharPrinter &b_char_printer);
	void encode_bit(bit_type bit);
	void encode(const bit_type bits[], size_t count);
	void finish();
};

class BinaryDecoder : public IBitDecoder {
private:
	ICharReader &char_reader;
	uint_fast8_t remainder;
	uint_fast8_t rem_counter;
public:
	BinaryDecoder(ICharReader &b_char_reader);
	bit_or_eof_type decode_bit();
};

#endif/*BINARY_H*/

