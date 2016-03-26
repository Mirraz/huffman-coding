#include <assert.h>
#include <stdio.h>
#include <stdexcept>
#include "file_chario.h"

FileCharOut::FileCharOut(FILE *b_stream) : stream(b_stream) {}

FileCharOut::~FileCharOut() {}

void FileCharOut::put(unsigned char c) {
	if (fputc(c, stream) != c) throw std::runtime_error("fputc error");
}

void FileCharOut::finish() {
}

FileCharIn::FileCharIn(FILE *b_stream) : stream(b_stream) {}

int FileCharIn::get_with_eof() {
	int c = fgetc(stream);
	if (c == EOF) return ICharIn::EOF_VALUE;
	assert((c & ~0xFF) == 0);
	return c & 0xFF;
}

