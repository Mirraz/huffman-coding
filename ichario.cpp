#include <assert.h>
#include "ichario.h"

ICharOut::~ICharOut() {}

ICharIn::~ICharIn() {}

bool ICharIn::get(unsigned char &c) {
	int result = get_with_eof();
	if (result == ICharIn::EOF_VALUE) return false;
	assert((result & ~0xFF) == 0);
	c = result & 0xFF;
	return true;
}

