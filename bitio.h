#ifndef BITIO_H
#define BITIO_H

#include <stdint.h>
#include <stddef.h>

typedef uint_fast8_t bit_type;
typedef uint_fast8_t bit_or_eof_type;
#define BIT_EOF_VALUE 255

class IBitEncoder {
public:
	virtual ~IBitEncoder();
	virtual void encode(const bit_type bits[], size_t count) = 0;
	virtual void finish() = 0;
};

class IBitDecoder {
public:
	virtual ~IBitDecoder();
	virtual bit_or_eof_type decode_bit() = 0;
};

#endif /*BITIO_H*/

