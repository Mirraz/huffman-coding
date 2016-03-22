#ifndef CHARIO_H
#define CHARIO_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

class ICharPrinter {
public:
	virtual ~ICharPrinter();
	virtual void putchar(char c) = 0;
	virtual void finish() = 0;
};

class FileStreamCharPrinter : public ICharPrinter {
private:
	FILE *stream;
public:
	FileStreamCharPrinter(FILE *b_stream);
	void putchar(char c);
	void finish();
};

class LineFileStreamCharPrinter : public ICharPrinter {
private:
	FILE *stream;
	uint_fast16_t line_length;
	uint_fast16_t line_counter;
public:
	LineFileStreamCharPrinter(FILE *b_stream);
	LineFileStreamCharPrinter(FILE *b_stream, uint_fast16_t b_line_length);
	void putchar(char c);
	void finish();
};

class StringCharPrinter : public ICharPrinter {
private:
	size_t length;
	size_t str_size;
	char *str;
public:
	StringCharPrinter(char *b_str, size_t b_str_size);
	void putchar(char c);
	void finish();
	size_t get_length() const;
};



class ICharReader {
public:
	virtual ~ICharReader();
	virtual char getchar() = 0;
	virtual bool is_end() const = 0;
};

class StringCharReader : public ICharReader {
private:
	size_t length;
	size_t idx;
	const char *str;
public:
	StringCharReader(const char *b_str, size_t b_length);
	char getchar();
	bool is_end() const;
};

#endif/*CHARIO_H*/

