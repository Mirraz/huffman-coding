#include <assert.h>
#include "line_file_chario.h"

LineFileCharOut::LineFileCharOut(FILE *b_stream) : LineFileCharOut(b_stream, 64) {}

LineFileCharOut::LineFileCharOut(FILE *b_stream, size_t b_line_length) :
	FileCharOut(b_stream), line_length(b_line_length), line_counter(0) {}

void LineFileCharOut::put(unsigned char c) {
	FileCharOut::put(c);
	++line_counter;
	if (line_counter == line_length) {
		FileCharOut::put('\n');
		line_counter = 0;
	}
}

void LineFileCharOut::finish() {
	if (line_counter != 0) {
		FileCharOut::put('\n');
		line_counter = 0;
	}
}
