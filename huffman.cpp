#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <vector>
#include "chario.h"
#include "binary.h"
#include "huffman.h"

typedef uint_fast8_t symbol_type;
typedef uint_fast16_t htree_idx_type;
#define MAX_SYMBOL_COUNT 256
#define SYMBOL_SIZE 8

typedef std::vector<symbol_type> vector_type;
typedef Huffman<symbol_type, htree_idx_type, MAX_SYMBOL_COUNT> huffman_type;
typedef typename huffman_type::DHTree dhtree_type;

void fread_all(FILE *stream, vector_type &vector) {
	while (true) {
		int c = fgetc(stream);
		if (c == EOF) break;
		vector.push_back(c);
	}
}

void encode(const char *dhtree_fname) {
	FILE *dhtree_file = fopen(dhtree_fname, "w");
	if (dhtree_file == NULL) {perror("fopen"); exit(EXIT_FAILURE);}

	std::vector<symbol_type> input_vector;
	fread_all(stdin, input_vector);
	const symbol_type* input_data = input_vector.data();
	size_t input_data_size = input_vector.size();
	
	dhtree_type dhtree;
	{
		FileStreamCharPrinter char_printer(stdout);
		BinaryEncoder encoder(char_printer);
		huffman_type::encode(input_data, input_data_size, encoder, dhtree);
	}
	{
		//huffman_type::fprint_dhtable(stderr, dhtree);
		FileStreamCharPrinter char_printer(dhtree_file);
		BinaryEncoder encoder(char_printer);
		huffman_type::dhtree_encode(dhtree, encoder, SYMBOL_SIZE);
	}
	
	if (fclose(dhtree_file)) perror("fclose");
}

void decode(const char *dhtree_fname) {
	FILE *dhtree_file = fopen(dhtree_fname, "r");
	if (dhtree_file == NULL) {perror("fopen"); exit(EXIT_FAILURE);}
	
	dhtree_type dhtree;
	{
		FileStreamCharReader char_reader(dhtree_file);
		BinaryDecoder decoder(char_reader);
		bool res = huffman_type::dhtree_decode(decoder, dhtree);
		assert(res);
		//huffman_type::fprint_dhtable(stderr, dhtree);
	}
	if (fclose(dhtree_file)) perror("fclose");
	static huffman_type::symbol_type text[1024*1024];
	size_t text_size;
	{
		FileStreamCharReader char_reader(stdin);
		BinaryDecoder decoder(char_reader);
		text_size = huffman_type::decode(dhtree, decoder, text, sizeof(text)/sizeof(text[0]));
	}
	
	for (size_t i=0; i<text_size; ++i) {
		putchar(text[i]);
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
