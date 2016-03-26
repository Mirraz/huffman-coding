#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>		// getopt
#include <vector>
#include "file_chario.h"
#include "char_symbolio.h"
#include "symbol_bitio.h"
#include "base64_char_bitio.h"
#include "huffman.h"

typedef uint_fast8_t symbol_type;
typedef uint_fast16_t htree_idx_type;
#define MAX_SYMBOL_COUNT 256
#define SYMBOL_SIZE 1
#define SYMBOL_BSIZE (SYMBOL_SIZE*8)

typedef Huffman<symbol_type, htree_idx_type, MAX_SYMBOL_COUNT> huffman_type;
typedef typename huffman_type::DHTree dhtree_type;

void encode(bool is_base64, const char *dhtree_fname, bool is_dhtree_base64) {
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
		if (is_base64) {
			Base64CharBitOut bit_out(char_out);
			huffman_type::encode(input_data, input_data_size, bit_out, dhtree);
		} else {
			SymbolBitOut<unsigned char, 8> bit_out(char_out);
			huffman_type::encode(input_data, input_data_size, bit_out, dhtree);
		}
	}
	{
		//huffman_type::fprint_dhtree(stderr, dhtree);
		FileCharOut char_out(dhtree_file);
		if (is_dhtree_base64) {
			Base64CharBitOut bit_out(char_out);
			huffman_type::dhtree_encode(dhtree, bit_out, SYMBOL_BSIZE);
		} else {
			SymbolBitOut<unsigned char, 8> bit_out(char_out);
			huffman_type::dhtree_encode(dhtree, bit_out, SYMBOL_BSIZE);
		}
	}
	
	if (fclose(dhtree_file)) perror("fclose");
}

void decode(bool is_base64, const char *dhtree_fname, bool is_dhtree_base64) {
	FILE *dhtree_file = fopen(dhtree_fname, "r");
	if (dhtree_file == NULL) {perror("fopen"); exit(EXIT_FAILURE);}
	
	dhtree_type dhtree;
	{
		FileCharIn char_in(dhtree_file);
		bool res;
		if (is_dhtree_base64) {
			Base64CharBitIn bit_in(char_in);
			res = huffman_type::dhtree_decode(bit_in, dhtree);
		} else {
			SymbolBitIn<unsigned char, 8> bit_in(char_in);
			res = huffman_type::dhtree_decode(bit_in, dhtree);
		}
		if (!res) {fprintf(stderr, "dhtree decode error\n"); exit(EXIT_FAILURE);}
		//huffman_type::fprint_dhtree(stderr, dhtree);
	}
	if (fclose(dhtree_file)) perror("fclose");
	{
		FileCharOut char_out(stdout);
		CharSymbolOut<symbol_type, SYMBOL_SIZE> symbol_out(char_out);
		
		FileCharIn char_in(stdin);
		bool res;
		if (is_base64) {
			Base64CharBitIn bit_in(char_in);
			res = huffman_type::decode(dhtree, bit_in, symbol_out);
		} else {
			SymbolBitIn<unsigned char, 8> bit_in(char_in);
			res = huffman_type::decode(dhtree, bit_in, symbol_out);
		}
		if (!res) {fprintf(stderr, "decode error\n"); exit(EXIT_FAILURE);}
	}
}

void print_help(const char *prog_name) {
	printf(
		"Usage:\n"
		"    %s -h\n"
		"    %s (-e|-d) [-b] -t <file> [-k]\n"
		"Huffman encode or decode from stdin to stdout"
		"\n"
		"\n"
		"Options:\n"
		"    -h          print this help and exit\n"
		"    -e or -d    encode or decode (must be exactly one of them)\n"
		"    -b          encode using base64 or decode as base64\n"
		"    -t <file>   huffman tree file (output if encode or input if decode)\n"
		"    -k          encode huffman tree file using base64 or decode as base64\n",
		prog_name, prog_name
	);
}

int main(int argc, char* argv[]) {
	bool is_encode = false, is_decode = false, is_base64 = false, is_dhtree_base64 = false;
	const char *dhtree_fname = NULL;
	
	static const char optstring[] = "hedbt:k";
	while (true) {
		int c = getopt(argc, argv, optstring);
		if (c == -1) break;
		switch (c) {
			case 'h':
				print_help(argv[0]);
				exit(EXIT_SUCCESS);
				break;
			case 'e':
				if (is_decode) goto print_help_and_exit_err;
				is_encode = true;
				break;
			case 'd':
				if (is_encode) goto print_help_and_exit_err;
				is_decode = true;
				break;
			case 'b':
				is_base64 = true;
				break;
			case 't':
				dhtree_fname = optarg;
				break;
			case 'k':
				is_dhtree_base64 = true;
				break;
			default:
				goto print_help_and_exit_err;
		}
	}
	if (optind < argc) goto print_help_and_exit_err;
	if (is_encode == is_decode) goto print_help_and_exit_err;
	if (dhtree_fname == NULL) goto print_help_and_exit_err;
	
	if (is_encode) encode(is_base64, dhtree_fname, is_dhtree_base64);
	else decode(is_base64, dhtree_fname, is_dhtree_base64);
	
	return 0;
	
print_help_and_exit_err:
	print_help(argv[0]);
	exit(EXIT_FAILURE);
}

