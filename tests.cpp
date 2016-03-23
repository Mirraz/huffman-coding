#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include "file_chario.h"
#include "line_file_chario.h"
#include "string_chario.h"
#include "char_bitio.h"
#include "base64_char_bitio.h"
#include "bit_symbolio.h"
#include "array_symbolio.h"
#include "huffman.h"

static const char test_char_str[] = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
constexpr size_t test_char_str_size = sizeof(test_char_str)/sizeof(test_char_str[0]);

void test_char_out(ICharOut &char_out) {
	for (size_t i=0; i<test_char_str_size; ++i) {
		char c = test_char_str[i];
		char_out.put(c);
	}
	char_out.finish();
}

void test_char_in_stdout(ICharIn &char_in) {
	while (true) {
		int c = char_in.get_with_eof();
		if (c == ICharIn::EOF_VALUE) break;
		else putchar(c);
	}
}

void test_char_in(ICharIn &char_in) {
	for (size_t i=0; i<test_char_str_size; ++i) {
		int c = char_in.get_with_eof();
		assert(c != ICharIn::EOF_VALUE);
		assert(c == test_char_str[i]);
	}
	assert(char_in.get_with_eof() == ICharIn::EOF_VALUE);
}

void test_file_char_out_stdout() {
	FileCharOut file_char_out(stdout);
	test_char_out(file_char_out);
}

void test_line_file_char_out_stdout() {
	LineFileCharOut line_file_char_out(stdout, 8);
	test_char_out(line_file_char_out);
}

void test_file_char_in_stdin() {
	FileCharIn file_char_in(stdin);
	test_char_in_stdout(file_char_in);
}

void test_string_chario() {
	char str[1024];
	StringCharOut string_char_out(str, sizeof(str)/sizeof(str[0]));
	test_char_out(string_char_out);
	size_t str_length = string_char_out.get_length();
	assert(str_length == test_char_str_size);
	for (size_t i=0; i<str_length; ++i) {
		assert(str[i] == test_char_str[i]);
	}
	
	StringCharIn string_char_in(str, str_length);
	test_char_in(string_char_in);
}

void make_test_bits(bit_type bits[], size_t bits_size) {
	for (size_t i=0; i<bits_size; ++i) bits[i] = (i % 5) & 1;
}

void test_char_bit_out_stdout() {
	bit_type bits[512];
	const size_t bit_count = 64;
	make_test_bits(bits, bit_count);

	FileCharOut file_char_out(stdout);
	CharBitOut char_bit_out(file_char_out);
	char_bit_out.put_array(bits, bit_count);
	char_bit_out.finish();
}

void test_char_bit_in_stdin() {
	FileCharIn file_char_in(stdin);
	CharBitIn char_bit_in(file_char_in);
	while (true) {
		IBitIn::bit_or_eof_type bit = char_bit_in.get_with_eof();
		if (bit == IBitIn::EOF_VALUE) break;
		printf("%u", bit & 1);
	}
	printf("\n");
}

void test_char_bitio() {
	bit_type bits[512];
	const size_t bit_count = 64;
	make_test_bits(bits, bit_count);
	
	char str[1024];
	StringCharOut string_char_out(str, sizeof(str)/sizeof(str[0]));
	
	CharBitOut char_bit_out(string_char_out);
	char_bit_out.put_array(bits, bit_count);
	char_bit_out.finish();
	size_t str_length = string_char_out.get_length();
	
	StringCharIn string_char_in(str, str_length);
	
	CharBitIn char_bit_in(string_char_in);
	for (size_t i=0; i<bit_count; ++i) {
		IBitIn::bit_or_eof_type bit = char_bit_in.get_with_eof();
		assert(bit != IBitIn::EOF_VALUE);
		assert(bit == bits[i]);
	}
	assert(char_bit_in.get_with_eof() == IBitIn::EOF_VALUE);
}

void test_base64_char_out_stdout() {
	bit_type bits[64];
	const size_t bit_count = 64;
	make_test_bits(bits, bit_count);

	FileCharOut file_char_out(stdout);
	Base64CharBitOut char_bit_out(file_char_out);
	char_bit_out.put_array(bits, bit_count);
	char_bit_out.finish();
}

void test_base64_char_bit_in_stdin() {
	FileCharIn file_char_in(stdin);
	Base64CharBitIn char_bit_in(file_char_in);
	while (true) {
		IBitIn::bit_or_eof_type bit = char_bit_in.get_with_eof();
		if (bit == IBitIn::EOF_VALUE) break;
		printf("%u", bit & 1);
	}
	printf("\n");
}

void test_base64_char_bitio() {
	bit_type bits[64];
	const size_t bit_count = 64;
	make_test_bits(bits, bit_count);
	
	char str[1024];
	StringCharOut string_char_out(str, sizeof(str)/sizeof(str[0]));
	
	Base64CharBitOut char_bit_out(string_char_out);
	char_bit_out.put_array(bits, bit_count);
	char_bit_out.finish();
	size_t str_length = string_char_out.get_length();
	
	StringCharIn string_char_in(str, str_length);
	
	Base64CharBitIn char_bit_in(string_char_in);
	for (size_t i=0; i<bit_count; ++i) {
		IBitIn::bit_or_eof_type bit = char_bit_in.get_with_eof();
		assert(bit != IBitIn::EOF_VALUE);
		assert(bit == bits[i]);
	}
	assert(char_bit_in.get_with_eof() == IBitIn::EOF_VALUE);
}

void test_bit_symbol_out_stdout() {
	FileCharOut char_out(stdout);
	CharBitOut bit_out(char_out);
	typedef uint32_t symbol_type;
	typedef BitSymbolOut<symbol_type> bit_symbol_out_type;
	bit_symbol_out_type bit_symbol_out(bit_out, sizeof(symbol_type)*8);
	for (symbol_type i = UINT32_MAX; i>= UINT32_MAX-16; --i) {
		bit_symbol_out.put(UINT32_MAX);
	}
	bit_symbol_out.finish();
}

void test_bit_symbol_in_stdin() {
	FileCharIn char_in(stdin);
	CharBitIn bit_in(char_in);
	typedef uint32_t symbol_type;
	typedef BitSymbolIn<symbol_type> bit_symbol_in_type;
	bit_symbol_in_type bit_symbol_in(bit_in, sizeof(symbol_type)*8);
	symbol_type symbol;
	while (bit_symbol_in.get(symbol)) {
		printf("%" PRIX32 " ", symbol);
	}
	printf("\n");
}

void test_bit_symbolio() {
	char str[1024];
	typedef uint32_t symbol_type;
	const size_t symbol_bsize = sizeof(symbol_type) * 8;
	symbol_type data[256];
	const size_t data_size = 16;
	for (size_t i=0; i<data_size; ++i) data[i] = i * i;

	StringCharOut string_char_out(str, sizeof(str)/sizeof(str[0]));
	CharBitOut bit_out(string_char_out);
	BitSymbolOut<symbol_type> bit_symbol_out(bit_out, symbol_bsize);
	for (size_t i=0; i<data_size; ++i) bit_symbol_out.put(data[i]);
	bit_symbol_out.finish();
	size_t str_length = string_char_out.get_length();
	
	StringCharIn string_char_in(str, str_length);
	CharBitIn bit_in(string_char_in);
	BitSymbolIn<symbol_type> bit_symbol_in(bit_in, symbol_bsize);
	for (size_t i=0; i<data_size; ++i) {
		symbol_type symbol;
		assert(bit_symbol_in.get(symbol));
		assert(symbol == data[i]);
	}
	symbol_type symbol;
	assert(!bit_symbol_in.get(symbol));
}

void print_base64_string(const char base64_str[], size_t base64_str_length, size_t max_line_length) {
	size_t i = 0;
	while (i < base64_str_length) {
		putchar(base64_str[i]);
		++i;
		if (i % max_line_length == 0 && i != base64_str_length) putchar('\n');
	}
}

#define MAX_TEXT_SIZE 256
void test_huffman_(size_t text_size, bool print_encoded) {
	typedef uint_fast8_t symbol_type;
	typedef Huffman<symbol_type, uint_fast16_t, 256> huffman_type;
	typedef huffman_type::DHTree DHTree;

	assert(text_size <= MAX_TEXT_SIZE);
	char str[MAX_TEXT_SIZE*4/3+1];
	StringCharOut string_char_out(str, sizeof(str)/sizeof(str[0]));
	Base64CharBitOut bit_out(string_char_out);
	
	symbol_type text[MAX_TEXT_SIZE];
	for (size_t i=0; i<text_size; ++i) {
		symbol_type t = (i | (i % 3) | (i % 7)) & 0x0F;
		text[i] = t;
	}
	
	DHTree dhtree;
	huffman_type::encode(text, text_size, bit_out, dhtree);
	size_t str_length = string_char_out.get_length();
	
	if (print_encoded) {
		printf("dhtree =\n");
		huffman_type::print_dhtable(dhtree);
		printf("\n");
		printf("base64_str_length = %u\n", (unsigned int)str_length);
		printf("base64_str =\n");
		print_base64_string(str, str_length, 64);
		printf("\n");
	}
	
	/////////
	
	StringCharIn string_char_in(str, str_length);
	Base64CharBitIn bit_in(string_char_in);
	
	symbol_type dtext[256];
	ArraySymbolOut<symbol_type> array_symbol_out(dtext, sizeof(dtext)/sizeof(dtext[0]));
	
	huffman_type::decode(dhtree, bit_in, array_symbol_out);
	
	size_t dtext_length = array_symbol_out.get_length();
	for (size_t i=0; i<dtext_length; ++i) {
		assert(text[i] == dtext[i]);
	}
}

void test_huffman() {
	//test_huffman_(256, true);
	for (uint_fast16_t i=1; i<=256; ++i) test_huffman_(i, false);
}

void tests_suite() {
	//test_string_chario();
	//test_char_bitio();
	//test_bit_symbolio();
	//test_base64_char_bitio();
	test_huffman();
}

int main() {
	tests_suite();
	return 0;
}

