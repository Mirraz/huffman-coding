#include "ichario.h"

ICharOut::~ICharOut() {}

ICharIn::~ICharIn() {}

bool ICharIn::get(char &c) {
	int result = get_with_eof();
	if (result == ICharIn::EOF_VALUE) return false;
	c = result;
	return true;
}

