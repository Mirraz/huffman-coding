#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include "file_chario.h"
#include "line_file_chario.h"
#include "array_symbolio.h"
#include "string_chario.h"
#include "char_symbolio.h"
#include "symbol_bitio.h"
#include "base64_char_bitio.h"
#include "bit_symbolio.h"
#include "vbit_symbolio.h"
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

void test_file_char_out_stdout() {
	FileCharOut file_char_out(stdout);
	test_char_out(file_char_out);
}

void test_file_char_in_stdin() {
	FileCharIn char_in(stdin);
	while (true) {
		int c = char_in.get_with_eof();
		if (c == ICharIn::EOF_VALUE) break;
		else putchar(c);
	}
}

void test_line_file_char_out_stdout() {
	LineFileCharOut line_file_char_out(stdout, 8);
	test_char_out(line_file_char_out);
}

template <typename symbol_type, size_t symbol_bsize>
void make_test_data(symbol_type data[], size_t data_size) {
	for (size_t i=0; i<data_size; ++i) data[i] = 1 << (i % symbol_bsize);
}

void test_array_symbolio() {
	typedef uint32_t symbol_type;
	symbol_type data[256];
	const size_t data_size = sizeof(data) / sizeof(data[0]);
	make_test_data<symbol_type, 32>(data, data_size);
	
	symbol_type storage[256];
	ArraySymbolOut<symbol_type> array_symbol_out(storage, sizeof(storage)/sizeof(storage[0]));
	for (size_t i=0; i<data_size; ++i) array_symbol_out.put(data[i]);
	size_t storage_length = array_symbol_out.get_length();
	assert(storage_length == data_size);
	for (size_t i=0; i<data_size; ++i) assert(storage[i] == data[i]);
	
	ArraySymbolIn<symbol_type> symbol_in(storage, storage_length);
	for (size_t i=0; i<data_size; ++i) {
		symbol_type symbol;
		assert(symbol_in.get(symbol));
		assert(symbol == data[i]);
	}
	symbol_type symbol;
	assert(!symbol_in.get(symbol));
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
	
	StringCharIn char_in(str, str_length);
	for (size_t i=0; i<test_char_str_size; ++i) {
		int c = char_in.get_with_eof();
		assert(c != ICharIn::EOF_VALUE);
		assert(c == test_char_str[i]);
	}
	assert(char_in.get_with_eof() == ICharIn::EOF_VALUE);
}

void test_char_symbolio() {
	typedef uint32_t symbol_type;
	symbol_type data[256];
	const size_t data_size = sizeof(data) / sizeof(data[0]);
	make_test_data<symbol_type, 32>(data, data_size);
	
	char str[1024];
	StringCharOut string_char_out(str, sizeof(str)/sizeof(str[0]));
	CharSymbolOut<symbol_type, 4> symbol_out(string_char_out);
	for (size_t i=0; i<data_size; ++i) symbol_out.put(data[i]);
	size_t str_length = string_char_out.get_length();
	
	StringCharIn string_char_in(str, str_length);
	CharSymbolIn<symbol_type, 4> symbol_in(string_char_in);
	for (size_t i=0; i<data_size; ++i) {
		symbol_type symbol;
		assert(symbol_in.get(symbol));
		assert(symbol == data[i]);
	}
	symbol_type symbol;
	assert(!symbol_in.get(symbol));
}

void make_test_bits(bit_type bits[], size_t bits_size) {
	for (size_t i=0; i<bits_size; ++i) bits[i] = (i % 5) & 1;
}

void test_symbol_bit_out_stdout() {
	bit_type bits[64];
	const size_t bit_count = 64;
	make_test_bits(bits, bit_count);
	
	FileCharOut file_char_out(stdout);
	CharSymbolOut<uint_fast8_t, 1> char_symbol_out(file_char_out);
	SymbolBitOut<uint_fast8_t, 8> symbol_bit_out(char_symbol_out);
	symbol_bit_out.put_array(bits, bit_count);
	symbol_bit_out.finish();
}

void test_symbol_bit_in_stdin() {
	FileCharIn file_char_in(stdin);
	CharSymbolIn<uint_fast8_t, 1> char_symbol_in(file_char_in);
	SymbolBitIn<uint_fast8_t, 8> symbol_bit_in(char_symbol_in);
	while (true) {
		IBitIn::bit_or_eof_type bit = symbol_bit_in.get_with_eof();
		if (bit == IBitIn::EOF_VALUE) break;
		printf("%u", bit & 1);
	}
	printf("\n");
}

void test_symbol_bitio() {
	bit_type bits[64];
	const size_t bit_count = 64;
	make_test_bits(bits, bit_count);
	
	typedef uint_fast8_t symbol_type;
	symbol_type storage[256];
	ArraySymbolOut<symbol_type> array_symbol_out(storage, sizeof(storage)/sizeof(storage[0]));
	
	SymbolBitOut<symbol_type, 8> symbol_bit_out(array_symbol_out);
	symbol_bit_out.put_array(bits, bit_count);
	symbol_bit_out.finish();
	size_t storage_length = array_symbol_out.get_length();
	
	ArraySymbolIn<symbol_type> array_symbol_in(storage, storage_length);
	
	SymbolBitIn<symbol_type, 8> symbol_bit_in(array_symbol_in);
	for (size_t i=0; i<bit_count; ++i) {
		IBitIn::bit_or_eof_type bit = symbol_bit_in.get_with_eof();
		assert(bit != IBitIn::EOF_VALUE);
		assert(bit == bits[i]);
	}
	assert(symbol_bit_in.get_with_eof() == IBitIn::EOF_VALUE);
}

void test_base64_char_out_stdout() {
	bit_type bits[64];
	const size_t bit_count = 64;
	make_test_bits(bits, bit_count);

	FileCharOut file_char_out(stdout);
	CharSymbolOut<unsigned char, 1> char_symbol_out(file_char_out);
	Base64CharBitOut bit_out(char_symbol_out);
	bit_out.put_array(bits, bit_count);
	bit_out.finish();
}

void test_base64_char_bit_in_stdin() {
	FileCharIn file_char_in(stdin);
	CharSymbolIn<unsigned char, 1> char_symbol_in(file_char_in);
	Base64CharBitIn bit_in(char_symbol_in);
	while (true) {
		IBitIn::bit_or_eof_type bit = bit_in.get_with_eof();
		if (bit == IBitIn::EOF_VALUE) break;
		printf("%u", bit & 1);
	}
	printf("\n");
}

void test_base64_char_bitio() {
	bit_type bits[64];
	const size_t bit_count = 64;
	make_test_bits(bits, bit_count);
	
	typedef unsigned char symbol_type;
	symbol_type storage[256];
	ArraySymbolOut<symbol_type> array_symbol_out(storage, sizeof(storage)/sizeof(storage[0]));
	
	Base64CharBitOut bit_out(array_symbol_out);
	bit_out.put_array(bits, bit_count);
	bit_out.finish();
	size_t storage_length = array_symbol_out.get_length();
	
	ArraySymbolIn<symbol_type> array_symbol_in(storage, storage_length);
	
	Base64CharBitIn bit_in(array_symbol_in);
	for (size_t i=0; i<bit_count; ++i) {
		IBitIn::bit_or_eof_type bit = bit_in.get_with_eof();
		assert(bit != IBitIn::EOF_VALUE);
		assert(bit == bits[i]);
	}
	assert(bit_in.get_with_eof() == IBitIn::EOF_VALUE);
}

void test_vbit_symbol_out_stdout() {
	FileCharOut file_char_out(stdout);
	CharSymbolOut<uint_fast8_t, 1> file_symbol_out(file_char_out);
	SymbolBitOut<uint_fast8_t, 8> bit_out(file_symbol_out);
	
	typedef uint32_t symbol_type;
	VBitSymbolOut<symbol_type> symbol_out(bit_out, sizeof(symbol_type)*8);
	for (symbol_type i = UINT32_MAX; i>= UINT32_MAX-16; --i) {
		symbol_out.put(i);
	}
	symbol_out.finish();
}

void test_vbit_symbol_in_stdin() {
	FileCharIn file_char_in(stdin);
	CharSymbolIn<uint_fast8_t, 1> file_symbol_in(file_char_in);
	SymbolBitIn<uint_fast8_t, 8> bit_in(file_symbol_in);
	
	typedef uint32_t symbol_type;
	VBitSymbolIn<symbol_type> symbol_in(bit_in, sizeof(symbol_type)*8);
	symbol_type symbol;
	while (symbol_in.get(symbol)) {
		printf("%" PRIX32 " ", symbol);
	}
	printf("\n");
}

void test_vbit_symbolio() {
	typedef uint32_t symbol_type;
	const size_t symbol_bsize = sizeof(symbol_type) * 8;
	symbol_type data[16];
	const size_t data_size = 16;
	make_test_data<symbol_type, 32>(data, data_size);
	
	uint_fast8_t storage[256];
	ArraySymbolOut<uint_fast8_t> array_symbol_out(storage, sizeof(storage)/sizeof(storage[0]));
	SymbolBitOut<uint_fast8_t, 8> bit_out(array_symbol_out);
	
	VBitSymbolOut<symbol_type> bit_symbol_out(bit_out, symbol_bsize);
	for (size_t i=0; i<data_size; ++i) bit_symbol_out.put(data[i]);
	bit_symbol_out.finish();
	
	size_t storage_length =  array_symbol_out.get_length();
	ArraySymbolIn<uint_fast8_t> array_symbol_in(storage, storage_length);
	SymbolBitIn<uint_fast8_t, 8> bit_in(array_symbol_in);
	
	VBitSymbolIn<symbol_type> bit_symbol_in(bit_in, symbol_bsize);
	for (size_t i=0; i<data_size; ++i) {
		symbol_type symbol;
		assert(bit_symbol_in.get(symbol));
		assert(symbol == data[i]);
	}
	symbol_type symbol;
	assert(!bit_symbol_in.get(symbol));
}

void test_bit_symbolio() {
	typedef uint32_t symbol_type;
	symbol_type data[16];
	const size_t data_size = 16;
	make_test_data<symbol_type, 32>(data, data_size);
	
	uint_fast8_t storage[256];
	ArraySymbolOut<uint_fast8_t> array_symbol_out(storage, sizeof(storage)/sizeof(storage[0]));
	SymbolBitOut<uint_fast8_t, 8> bit_out(array_symbol_out);
	
	BitSymbolOut<symbol_type, 32> bit_symbol_out(bit_out);
	for (size_t i=0; i<data_size; ++i) bit_symbol_out.put(data[i]);
	bit_symbol_out.finish();
	
	size_t storage_length =  array_symbol_out.get_length();
	ArraySymbolIn<uint_fast8_t> array_symbol_in(storage, storage_length);
	SymbolBitIn<uint_fast8_t, 8> bit_in(array_symbol_in);
	
	BitSymbolIn<symbol_type, 32> bit_symbol_in(bit_in);
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

typedef uint_fast8_t symbol_type;
#define SYMBOL_BSIZE 8
typedef Huffman<symbol_type, uint_fast16_t, ((uint_fast16_t)1)<<SYMBOL_BSIZE> huffman_type;
typedef huffman_type::DHTree DHTree;

void test_dhtreeio(const DHTree &dhtree) {
	unsigned char storage[512];
	ArraySymbolOut<unsigned char> array_symbol_out(storage, sizeof(storage)/sizeof(storage[0]));
	SymbolBitOut<unsigned char, 8> bit_out(array_symbol_out);
	huffman_type::dhtree_encode(dhtree, bit_out, SYMBOL_BSIZE);
	size_t storage_length = array_symbol_out.get_length();
	
	ArraySymbolIn<unsigned char> symbol_in(storage, storage_length);
	SymbolBitIn<unsigned char, 8> bit_in(symbol_in);
	DHTree d_dhtree;
	bool res = huffman_type::dhtree_decode(bit_in, d_dhtree);
	assert(res);
	
	assert(dhtree.root == d_dhtree.root);
	assert(dhtree.size == d_dhtree.size);
	for (huffman_type::htree_idx_type i=0; i<dhtree.size; ++i) {
		assert(dhtree.htree[i].is_leaf == d_dhtree.htree[i].is_leaf);
		if (dhtree.htree[i].is_leaf) {
			assert(dhtree.htree[i].value.leaf_value == d_dhtree.htree[i].value.leaf_value);
		} else {
			assert(dhtree.htree[i].value.childs[0] == d_dhtree.htree[i].value.childs[0]);
			assert(dhtree.htree[i].value.childs[1] == d_dhtree.htree[i].value.childs[1]);
		}
	}
}

#define MAX_TEXT_SIZE 256
void test_huffman_(size_t text_size, bool print_encoded, bool check_dhtree) {
	assert(text_size <= MAX_TEXT_SIZE);
	unsigned char storage[MAX_TEXT_SIZE*4/3+1];
	ArraySymbolOut<unsigned char> storage_symbol_out(storage, sizeof(storage)/sizeof(storage[0]));
	Base64CharBitOut bit_out(storage_symbol_out);
	
	symbol_type text[MAX_TEXT_SIZE];
	for (size_t i=0; i<text_size; ++i) {
		symbol_type t = (i | (i % 3) | (i % 7)) & 0x0F;
		text[i] = t;
	}
	
	DHTree dhtree;
	huffman_type::encode(text, text_size, bit_out, dhtree);
	size_t storage_length = storage_symbol_out.get_length();
	
	if (print_encoded) {
		printf("dhtree =\n");
		huffman_type::print_dhtree(dhtree);
		printf("\n");
		printf("base64_str_length = %u\n", (unsigned int)storage_length);
		printf("base64_str =\n");
		print_base64_string((char *)storage, storage_length, 64);
		printf("\n");
	}
	
	if (check_dhtree) test_dhtreeio(dhtree);
	
	/////////
	
	ArraySymbolIn<unsigned char> storage_symbol_in(storage, storage_length);
	Base64CharBitIn bit_in(storage_symbol_in);
	
	symbol_type dtext[256];
	ArraySymbolOut<symbol_type> array_symbol_out(dtext, sizeof(dtext)/sizeof(dtext[0]));
	
	huffman_type::decode(dhtree, bit_in, array_symbol_out);
	
	size_t dtext_length = array_symbol_out.get_length();
	for (size_t i=0; i<dtext_length; ++i) {
		assert(text[i] == dtext[i]);
	}
}

void test_huffman() {
	//test_huffman_(256, true, false);
	for (uint_fast16_t i=1; i<=256; ++i) test_huffman_(i, false, false);
}

void test_huffman_with_dhtree() {
	for (uint_fast16_t i=1; i<=256; ++i) test_huffman_(i, false, true);
}

void tests_suite() {
	test_array_symbolio();
	test_string_chario();
	test_char_symbolio();
	test_symbol_bitio();
	test_base64_char_bitio();
	test_vbit_symbolio();
	test_bit_symbolio();
	test_huffman();
	test_huffman_with_dhtree();
}

int main() {
	tests_suite();
	return 0;
}

