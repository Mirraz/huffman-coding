#ifndef FILECHARIO_H
#define FILECHARIO_H

#include <stdio.h>
#include "ichario.h"

class FileCharOut : public ICharOut {
private:
	FILE *stream;
	
public:
	FileCharOut(FILE *b_stream);
	virtual ~FileCharOut();
	virtual void put(char c);
	virtual void finish();
};

class FileCharIn : public ICharIn {
private:
	FILE *stream;
	
public:
	FileCharIn(FILE *b_stream);
	int get_with_eof();
};

#endif/*FILECHARIO_H*/

