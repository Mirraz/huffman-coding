#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <assert.h>
#include <stdio.h>
#include <map>
#include <queue>
#include <vector>
#include <stdexcept>
#include "ibitio.h"
#include "vbit_symbolio.h"

template <typename SYMBOL_TYPE, typename HTREE_IDX_TYPE, HTREE_IDX_TYPE MAX_SYMBOL_COUNT>
class Huffman {
public:
typedef SYMBOL_TYPE symbol_type;
typedef HTREE_IDX_TYPE htree_idx_type;
static_assert(MAX_SYMBOL_COUNT > 0, "MAX_HTREE_SIZE can't be zero");
static_assert((htree_idx_type)((MAX_SYMBOL_COUNT - 1) + MAX_SYMBOL_COUNT) > MAX_SYMBOL_COUNT, "MAX_HTREE_SIZE overflow");
static constexpr htree_idx_type MAX_HTREE_SIZE = (MAX_SYMBOL_COUNT - 1) + MAX_SYMBOL_COUNT;

// decode huffman tree
struct DHTreeNode {
	bool is_leaf;
	union {
		htree_idx_type childs[2];
		symbol_type leaf_value;
	} value;
};

struct DHTree {
	htree_idx_type root;
	htree_idx_type size;
	DHTreeNode htree[MAX_HTREE_SIZE];
};

private:
typedef size_t weight_type;
typedef htree_idx_type hcode_len_type;
static constexpr htree_idx_type MAX_HCODE_LEN = MAX_HTREE_SIZE;

struct HTreeNode {
	weight_type weight;
	htree_idx_type parent_idx;
	bit_type code;
	bool is_leaf;
	symbol_type leaf_value;
	
	HTreeNode() : weight(0), parent_idx(0), code(0), is_leaf(false), leaf_value(0) {}
	HTreeNode(weight_type b_weight, symbol_type b_leaf_value) :
		weight(b_weight), parent_idx(0), code(0), is_leaf(true), leaf_value(b_leaf_value) {}
	HTreeNode(weight_type b_weight) :
		weight(b_weight), parent_idx(0), code(0), is_leaf(false), leaf_value(0) {}
};

class HTreeIdxComparator {
private:
	struct HTreeNode *htree;
public:
	HTreeIdxComparator() {}
	HTreeIdxComparator(struct HTreeNode *b_htree) : htree(b_htree) {}
	
	bool operator()(htree_idx_type a, htree_idx_type b) {
		if (htree[a].weight == htree[b].weight) return a > b;
		return htree[a].weight > htree[b].weight;
	}
};

struct hcode {
	hcode_len_type length;
	bit_type bits[MAX_HCODE_LEN];
};

typedef std::map<symbol_type, hcode> code_map_type;

static void make_huffman_codes(const symbol_type array[], size_t array_size, code_map_type &code_map, DHTree &dhtree) {
	assert(code_map.empty());
	
	if (array_size == 0) {
		dhtree.root = 0;
		dhtree.size = 0;
		return;
	}

	typedef std::map<symbol_type, weight_type> frequency_map_type;
	typedef typename frequency_map_type::iterator frequency_map_iterator_type;
	frequency_map_type frequency;
	for (size_t i=0; i<array_size; ++i) {
		++frequency[array[i]];
	}
	if (frequency.size() > MAX_SYMBOL_COUNT) throw std::out_of_range("Symbols count exceeds MAX_SYMBOL_COUNT");
	
	struct HTreeNode htree[MAX_HTREE_SIZE];
	typedef std::priority_queue<htree_idx_type, std::vector<htree_idx_type>, HTreeIdxComparator> heap_type;
	HTreeIdxComparator comparator(htree);
	heap_type heap(comparator);
	
	htree_idx_type htree_count = 0, leaf_count = 0;
	for (frequency_map_iterator_type it=frequency.begin(); it != frequency.end(); ++it) {
		//printf("%u %u\n", it->first, it->second);
		assert(htree_count < MAX_HTREE_SIZE);
		htree[htree_count] = HTreeNode(it->second, it->first);
		heap.push(htree_count);
		++htree_count;
		++leaf_count;
	}
	
	while (heap.size() >= 2) {
		htree_idx_type idx1 = heap.top(); heap.pop();
		htree_idx_type idx0 = heap.top(); heap.pop();
		assert(htree_count < MAX_HTREE_SIZE);
		htree[htree_count] = HTreeNode(htree[idx0].weight + htree[idx1].weight);
		htree[idx0].parent_idx = htree_count;
		htree[idx1].parent_idx = htree_count;
		htree[idx0].code = 0;
		htree[idx1].code = 1;
		heap.push(htree_count);
		++htree_count;
	}
	assert(heap.size() == 1);
	assert(heap.top() == htree_count-1);
	heap.pop();
	assert(heap.empty());
	htree_idx_type root_idx = htree_count-1;
	assert(htree[root_idx].weight == array_size);
	
	for (htree_idx_type i=0; i<htree_count; ++i) {
		if (! htree[i].is_leaf) continue;
		
		bit_type code_bits[MAX_HCODE_LEN]; // reverse at first
		hcode_len_type code_length = 0;
		
		htree_idx_type idx = i;
		// maybe i == root_idx already in case if all input symbols are equal
		do {
			code_bits[code_length++] = htree[idx].code;
			idx = htree[idx].parent_idx;
		} while (idx != root_idx);
		
		hcode &leaf_code = code_map[htree[i].leaf_value];
		leaf_code.length = code_length;
		for (hcode_len_type j=0, k=code_length-1; j < code_length; ++j, --k) {
			leaf_code.bits[j] = code_bits[k];
		}
	}
	
	// htree:
	// | 0 .. leaf_count-1 | leaf_count .. htree_count-2 | htree_count-1 |
	// |       leaves      |         not leaves          |    root       |
	// dhtree (want to):
	// |  0   | 1 .. htree_count-leaf_count-1 | htree_count-leaf_count .. htree_count-1 |
	// | root |           not leaves          |                  leaves                 |
	
	for (htree_idx_type idx=0, didx=htree_count-leaf_count; idx<leaf_count; ++idx, ++didx) {
		assert(didx < htree_count);
		HTreeNode &node = htree[idx];
		DHTreeNode &dnode = dhtree.htree[didx];
		assert(node.is_leaf);
		dnode.is_leaf = true;
		dnode.value.leaf_value = node.leaf_value;
		
		if (idx != root_idx) {
			htree_idx_type parent_idx = node.parent_idx;
			assert(!htree[parent_idx].is_leaf);
			htree_idx_type parent_didx = htree_count-1-parent_idx;
			dhtree.htree[parent_didx].value.childs[node.code] = didx;
		}
	}
	for (htree_idx_type idx=leaf_count, didx=htree_count-leaf_count-1; idx<htree_count; ++idx, --didx) {
		assert(didx < htree_count);
		HTreeNode &node = htree[idx];
		DHTreeNode &dnode = dhtree.htree[didx];
		assert(!node.is_leaf);
		dnode.is_leaf = false;
		
		if (idx != root_idx) {
			htree_idx_type parent_idx = node.parent_idx;
			assert(!htree[parent_idx].is_leaf);
			htree_idx_type parent_didx = htree_count-1-parent_idx;
			dhtree.htree[parent_didx].value.childs[node.code] = didx;
		}
	}
	dhtree.size = htree_count;
	dhtree.root = htree_count-1-root_idx;
	assert(dhtree.root == 0);
}

public:
static void encode(const symbol_type array[], size_t array_size, IBitOut &bit_out, DHTree &dhtree) {
	code_map_type code_map;
	make_huffman_codes(array, array_size, code_map, dhtree);
	//for (code_map_type::iterator it=code_map.begin(); it!=code_map.end(); ++it) {
	//	printf("%u: ", it->first);
	//	hcode &code = it->second;
	//	for (htree_idx_type i=0; i<code.length; ++i) printf("%u", code.bits[i]);
	//	printf("\n");
	//}
	
	for (size_t i=0; i<array_size; ++i) {
		hcode &code = code_map[array[i]];
		bit_out.put_array(code.bits, code.length);
	}
	bit_out.finish();
}

// returns: whether full bit-stream decoded (indicates wrong input data)
static bool decode(const DHTree &dhtree, IBitIn &bit_in, ISymbolOut<SYMBOL_TYPE> &symbol_out) {
	htree_idx_type idx = dhtree.root;
	while (true) {
		IBitIn::bit_or_eof_type bit = bit_in.get_with_eof();
		if (bit == IBitIn::EOF_VALUE) break;
		if (! dhtree.htree[idx].is_leaf) idx = dhtree.htree[idx].value.childs[bit];
		if (dhtree.htree[idx].is_leaf) {
			symbol_out.put(dhtree.htree[idx].value.leaf_value);
			idx = dhtree.root;
		}
	}
	return idx == dhtree.root;
}

static void fprint_dhtree(FILE *stream, const DHTree &dhtree) {
	typedef unsigned long long int lluint_type;
	#define LLUINT_FMT "llu"
	fprintf(stream, "{%" LLUINT_FMT ",%" LLUINT_FMT ",{\n", (lluint_type)dhtree.root, (lluint_type)dhtree.size);
	for (htree_idx_type i=0; i<dhtree.size; ++i) {
		const DHTreeNode &dnode = dhtree.htree[i];
		fprintf(stream, "{%s,{", dnode.is_leaf ? "true" : "false");
		if (dnode.is_leaf) {
			fprintf(stream, "%" LLUINT_FMT, (lluint_type)dnode.value.leaf_value);
		} else {
			fprintf(stream, "{%" LLUINT_FMT ",%" LLUINT_FMT "}", (lluint_type)dnode.value.childs[0], (lluint_type)dnode.value.childs[1]);
		}
		fprintf(stream, "}},\n");
	}
	fprintf(stream, "}}");
	#undef LLUINT_FMT
}

static void print_dhtree(const DHTree &dhtree) {
	fprint_dhtree(stdout, dhtree);
}

// symbol_bsize - size of symbol in bits
static void dhtree_encode(const DHTree &dhtree, IBitOut &bit_out, size_t symbol_bsize) {
	if (dhtree.size == 0) {
		bit_out.finish();
		return;
	}

	// find size in bits of dhtree.size
	if (!(dhtree.size > 0)) throw std::invalid_argument("Invalid dhtree: size can't be zero");
	size_t htree_idx_type_bsize = 0;
	for (htree_idx_type tsize = dhtree.size; tsize > 0; tsize >>= 1) ++htree_idx_type_bsize;
	
	VBitSymbolOut<symbol_type> symbol_out(bit_out, symbol_bsize);
	VBitSymbolOut<htree_idx_type> htree_idx_out(bit_out, htree_idx_type_bsize);

	// encode symbol_bsize-1 by unary coding
	if (!(symbol_bsize > 0)) throw std::invalid_argument("symbol_bsize can't be zero");
	for (size_t i=1; i<symbol_bsize; ++i) bit_out.put(0);
	bit_out.put(1);
	
	// encode htree_idx_type_bsize-1 by unary coding
	assert(htree_idx_type_bsize > 0);
	for (size_t i=1; i<htree_idx_type_bsize; ++i) bit_out.put(0);
	bit_out.put(1);
	
	// encode dhtree
	if (!(dhtree.root == 0)) throw std::invalid_argument("Invalid dhtree: root must be zero");
	htree_idx_out.put(dhtree.size);
	
	for (htree_idx_type i=0; i<dhtree.size; ++i) {
		const DHTreeNode &dnode = dhtree.htree[i];
		bit_out.put(dnode.is_leaf);
		if (dnode.is_leaf) {
			symbol_out.put(dnode.value.leaf_value);
		} else {
			htree_idx_out.put(dnode.value.childs[0]);
			htree_idx_out.put(dnode.value.childs[1]);
		}
	}
	
	bit_out.finish();
}

// returns success
static bool dhtree_decode(IBitIn &bit_in, DHTree &dhtree) {
	IBitIn::bit_or_eof_type bit = bit_in.get_with_eof();
	if (bit == IBitIn::EOF_VALUE) {
		dhtree.root = 0;
		dhtree.size = 0;
		return true;
	}
	
	// decode unary encoded symbol_bsize-1
	size_t symbol_bsize = 0;
	do {
		if (symbol_bsize != 0) bit = bit_in.get_with_eof(); // first bit is read already
		if (bit == IBitIn::EOF_VALUE) return false;
		++symbol_bsize;
	} while (!bit);
	if (!(sizeof(symbol_type)*8 >= symbol_bsize)) return false;
	
	// decode unary encoded htree_idx_type_bsize-1
	size_t htree_idx_type_bsize = 0;
	do {
		bit = bit_in.get_with_eof();
		if (bit == IBitIn::EOF_VALUE) return false;
		++htree_idx_type_bsize;
	} while (!bit);
	if (!(sizeof(htree_idx_type)*8 >= htree_idx_type_bsize)) return false;
	
	VBitSymbolIn<symbol_type> symbol_in(bit_in, symbol_bsize);
	VBitSymbolIn<htree_idx_type> htree_idx_in(bit_in, htree_idx_type_bsize);
	
	// decode dhtree
	dhtree.root = 0;
	if (!htree_idx_in.get(dhtree.size)) return false;
	
	for (htree_idx_type i=0; i<dhtree.size; ++i) {
		DHTreeNode &dnode = dhtree.htree[i];
		bit = bit_in.get_with_eof();
		if (bit == IBitIn::EOF_VALUE) return false;
		dnode.is_leaf = bit;
		if (dnode.is_leaf) {
			if (!symbol_in.get(dnode.value.leaf_value)) return false;
		} else {
			if (!htree_idx_in.get(dnode.value.childs[0])) return false;
			if (!htree_idx_in.get(dnode.value.childs[1])) return false;
		}
	}
	
	return bit_in.get_with_eof() == IBitIn::EOF_VALUE;
}

};

#endif/*HUFFMAN_H*/

