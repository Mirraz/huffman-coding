#ifndef IBITIO_H
#define IBITIO_H

#include <stdint.h>
#include <stddef.h>

typedef uint_fast8_t bit_type;

class IBitOut {
public:
	virtual ~IBitOut();
	virtual void put(bit_type bit) = 0;
	virtual void put_array(const bit_type bits[], size_t count);
	virtual void finish() = 0;
};

class IBitIn {
public:
	typedef uint_fast8_t bit_or_eof_type;
	static constexpr bit_or_eof_type EOF_VALUE = 255;
	
	virtual ~IBitIn();
	virtual bit_or_eof_type get_with_eof() = 0;
};

#endif /*IBITIO_H*/

