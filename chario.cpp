#include <assert.h>
#include <stdio.h>
#include "chario.h"

ICharPrinter::~ICharPrinter() {}

FileStreamCharPrinter::FileStreamCharPrinter(FILE *b_stream) : stream(b_stream) {}

void FileStreamCharPrinter::putchar(char c) {
	fputc(c, stream);
}

void FileStreamCharPrinter::finish() {
}

LineFileStreamCharPrinter::LineFileStreamCharPrinter(FILE *b_stream) :
	stream(b_stream), line_length(64), line_counter(0) {}

LineFileStreamCharPrinter::LineFileStreamCharPrinter(FILE *b_stream, uint_fast16_t b_line_length) :
	stream(b_stream), line_length(b_line_length), line_counter(0) {}

void LineFileStreamCharPrinter::putchar(char c) {
	fputc(c, stream);
	++line_counter;
	if (line_counter == line_length) {
		fputc('\n', stream);
		line_counter = 0;
	}
}

void LineFileStreamCharPrinter::finish() {
	if (line_counter != 0) {
		fputc('\n', stream);
		line_counter = 0;
	}
}

StringCharPrinter::StringCharPrinter(char *b_str, size_t b_str_size) :
	length(0), str_size(b_str_size), str(b_str) {}

void StringCharPrinter::putchar(char c) {
	assert(length < str_size);
	str[length++] = c;
}

void StringCharPrinter::finish() {
}

size_t StringCharPrinter::get_length() const {
	return length;
}



ICharReader::~ICharReader() {}

FileStreamCharReader::FileStreamCharReader(FILE *b_stream) : stream(b_stream) {
	next_c = fgetc(stream);
}

char FileStreamCharReader::getchar() {
	assert(!is_end());
	int prev_c = next_c;
	next_c = fgetc(stream);
	return prev_c;
}

bool FileStreamCharReader::is_end() const {
	return next_c == EOF;
}

StringCharReader::StringCharReader(const char *b_str, size_t b_length) :
	length(b_length), idx(0), str(b_str) {}

char StringCharReader::getchar() {
	assert(!is_end());
	return str[idx++];
}

bool StringCharReader::is_end() const {
	return idx == length;
}

