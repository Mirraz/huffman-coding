#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "chario.h"
#include "base64.h"
#include "huffman.h"

void test_char_printer(ICharPrinter &char_printer) {
	for (char c='!';; ++c) {
		char_printer.putchar(c);
		if (c == '~') break;
	}
	char_printer.finish();
}

void test_char_reader(ICharReader &char_reader) {
	while (!char_reader.is_end()) {
		char c = char_reader.getchar();
		putchar(c);
	}
	printf("\n");
}

void test_chario() {
	LineFileStreamCharPrinter fs_char_printer(stdout, 64);
	test_char_printer(fs_char_printer);
	
	char str[1024];
	StringCharPrinter str_char_printer(str, sizeof(str)/sizeof(str[0]));
	test_char_printer(str_char_printer);
	size_t str_length = str_char_printer.get_length();
	for (size_t i=0; i<str_length; ++i) putchar(str[i]); printf("\n");
	
	StringCharReader str_char_reader(str, str_length);
	test_char_reader(str_char_reader);
}

void test_base64() {
	char str[1024];
	StringCharPrinter str_char_printer(str, sizeof(str)/sizeof(str[0]));
	
	Base64Encoder encoder(str_char_printer);
	bit_type bits[512];
	const size_t bit_count = 64;
	for (size_t i=0; i<bit_count; ++i) bits[i] = (i % 5) & 1;
	encoder.encode(bits, bit_count);
	
	size_t str_length = str_char_printer.get_length();
	for (size_t i=0; i<str_length; ++i) putchar(str[i]); printf("\n");
	
	/////////
	
	StringCharReader str_char_reader(str, str_length);
	Base64Decoder decoder(str_char_reader);
	while (true) {
		bit_or_eof_type bit = decoder.decode_bit();
		if (bit == BIT_EOF_VALUE) break;
		printf("%u", bit);
	}
	printf("\n");
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
	StringCharPrinter str_char_printer(str, sizeof(str)/sizeof(str[0]));
	Base64Encoder encoder(str_char_printer);
	
	symbol_type text[MAX_TEXT_SIZE];
	for (size_t i=0; i<text_size; ++i) {
		symbol_type t = (i | (i % 3) | (i % 7)) & 0x0F;
		//printf("%u ", t);
		text[i] = t;
	}
	//printf("\n");
	
	DHTree dhtree;
	huffman_type::encode(text, text_size, encoder, dhtree);
	size_t str_length = str_char_printer.get_length();
	
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
	
	StringCharReader str_char_reader(str, str_length);
	Base64Decoder decoder(str_char_reader);
	
	symbol_type dtext[256];
	size_t dtext_length = huffman_type::decode(dhtree, decoder, dtext, sizeof(dtext)/sizeof(dtext[0]));
	for (size_t i=0; i<dtext_length; ++i) {
		assert(text[i] == dtext[i]);
		//printf("%u ", dtext[i]);
	}
	//printf("\n");
}

void test_huffman() {
	//test_huffman_(256, true);
	for (uint_fast16_t i=1; i<=256; ++i) test_huffman_(i, false);
	printf("huffman test: OK\n");
}

void tests_suite() {
	//test_chario();
	//test_base64();
	test_huffman();
}

int main() {
	tests_suite();
	return 0;
}

