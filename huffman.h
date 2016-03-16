#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include "bitio.h"

typedef uint_fast8_t num_type;
// count of all different possible num_type values
#define MAX_NUM_COUNT 256

typedef uint_fast16_t htree_idx_type;
#define MAX_HTREE_SIZE (MAX_NUM_COUNT*2)

// decode huffman tree
struct DHTreeNode {
	bool is_leaf;
	union {
		htree_idx_type childs[2];
		num_type leaf_value;
	} value;
};

struct DHTree {
	htree_idx_type root;
	htree_idx_type size;
	DHTreeNode htree[MAX_HTREE_SIZE];
};

void encode(const num_type array[], size_t array_size, IBitEncoder &encoder, DHTree &dhtree);

size_t decode(const DHTree &dhtree, IBitDecoder &decoder, num_type array[], size_t array_size);

#endif/*HUFFMAN_H*/

