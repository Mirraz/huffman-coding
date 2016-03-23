#include "ibitio.h"

IBitOut::~IBitOut() {}

void IBitOut::put_array(const bit_type bits[], size_t count) {
	for (size_t i=0; i<count; ++i) put(bits[i]);
}

IBitIn::~IBitIn() {}

