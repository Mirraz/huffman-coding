#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include "file_chario.h"
#include "char_symbolio.h"
#include "symbol_bitio.h"
#include "huffman.h"

typedef uint_fast8_t symbol_type;
typedef uint_fast16_t htree_idx_type;
#define MAX_SYMBOL_COUNT 256
#define SYMBOL_SIZE 1
#define SYMBOL_BSIZE (SYMBOL_SIZE*8)

typedef Huffman<symbol_type, htree_idx_type, MAX_SYMBOL_COUNT> huffman_type;
typedef typename huffman_type::DHTree dhtree_type;

void encode(const char *dhtree_fname) {
	FILE *dhtree_file = fopen(dhtree_fname, "w");
	if (dhtree_file == NULL) {perror("fopen"); exit(EXIT_FAILURE);}

	FileCharIn char_in(stdin);
	CharSymbolIn<symbol_type, SYMBOL_SIZE> symbol_in(char_in);
	std::vector<symbol_type> input_vector;
	symbol_type symbol;
	while (symbol_in.get(symbol)) input_vector.push_back(symbol);
	const symbol_type* input_data = input_vector.data();
	size_t input_data_size = input_vector.size();
	
	dhtree_type dhtree;
	{
		FileCharOut char_out(stdout);
		SymbolBitOut<unsigned char, 8> bit_out(char_out);
		huffman_type::encode(input_data, input_data_size, bit_out, dhtree);
	}
	{
		//huffman_type::fprint_dhtree(stderr, dhtree);
		FileCharOut char_out(dhtree_file);
		SymbolBitOut<unsigned char, 8> bit_out(char_out);
		huffman_type::dhtree_encode(dhtree, bit_out, SYMBOL_BSIZE);
	}
	
	if (fclose(dhtree_file)) perror("fclose");
}

void decode(const char *dhtree_fname) {
	FILE *dhtree_file = fopen(dhtree_fname, "r");
	if (dhtree_file == NULL) {perror("fopen"); exit(EXIT_FAILURE);}
	
	dhtree_type dhtree;
	{
		FileCharIn char_in(dhtree_file);
		SymbolBitIn<unsigned char, 8> bit_in(char_in);
		bool res = huffman_type::dhtree_decode(bit_in, dhtree);
		assert(res);
		//huffman_type::fprint_dhtree(stderr, dhtree);
	}
	if (fclose(dhtree_file)) perror("fclose");
	{
		FileCharIn char_in(stdin);
		SymbolBitIn<unsigned char, 8> bit_in(char_in);
		
		FileCharOut char_out(stdout);
		CharSymbolOut<symbol_type, SYMBOL_SIZE> symbol_out(char_out);
		huffman_type::decode(dhtree, bit_in, symbol_out);
	}
}

int main(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "error: not exactly 2 args\n");
		exit(EXIT_FAILURE);
	}
	const char *dhtree_fname = argv[2];
	if (!strcmp(argv[1], "-e")) {
		encode(dhtree_fname);
	} else if (!strcmp(argv[1], "-d")) {
		decode(dhtree_fname);
	} else {
		fprintf(stderr, "error: first arg != [-e|-d]\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}
