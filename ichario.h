#ifndef ICHARIO_H
#define ICHARIO_H

#include <stdio.h>		// EOF
#include "isymbolio.h"

class ICharOut : public ISymbolOut<char> {
public:
	virtual ~ICharOut();
	virtual void put(char c) = 0;
	virtual void finish() = 0;
};

class ICharIn : public ISymbolIn<char> {
public:
	static constexpr int EOF_VALUE = EOF;

	virtual ~ICharIn();
	virtual bool get(char &c);
	virtual int get_with_eof() = 0;
};

#endif/*ICHARIO_H*/

