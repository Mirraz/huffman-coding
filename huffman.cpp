#include <assert.h>
#include <stdint.h>
#include <map>
#include <queue>
#include <vector>
#include <functional>
#include "huffman.h"

typedef size_t weight_type;
typedef uint_fast16_t hcode_len_type;
#define MAX_HCODE_LEN MAX_HTREE_SIZE

struct HTreeNode {
	weight_type weight;
	htree_idx_type parent_idx;
	bit_type code;
	bool is_leaf;
	num_type leaf_value;
	
	HTreeNode() : weight(0), parent_idx(0), code(0), is_leaf(false), leaf_value(0) {}
	HTreeNode(weight_type b_weight, num_type b_leaf_value) :
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

typedef std::map<num_type, hcode> code_map_type;

static void make_huffman_codes(const num_type array[], size_t array_size, code_map_type &code_map, DHTree &dhtree) {
	assert(code_map.empty());

	typedef std::map<num_type, weight_type> frequency_map_type;
	frequency_map_type frequency;
	for (size_t i=0; i<array_size; ++i) {
		++frequency[array[i]];
	}
	
	struct HTreeNode htree[MAX_HTREE_SIZE];
	typedef std::priority_queue<htree_idx_type, std::vector<htree_idx_type>, HTreeIdxComparator> heap_type;
	HTreeIdxComparator comparator(htree);
	heap_type heap(comparator);
	
	htree_idx_type htree_count = 0, leaf_count = 0;
	for (frequency_map_type::iterator it=frequency.begin(); it != frequency.end(); ++it) {
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
		// maybe i == root_idx already in case if all input numbers are equal
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

void encode(const num_type array[], size_t array_size, IBitEncoder &encoder, DHTree &dhtree) {
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
		encoder.encode(code.bits, code.length);
	}
	encoder.finish();
}

size_t decode(const DHTree &dhtree, IBitDecoder &decoder, num_type array[], size_t array_size) {
	size_t array_length = 0;
	
	htree_idx_type idx = dhtree.root;
	while (true) {
		bit_or_eof_type bit = decoder.decode_bit();
		if (bit == BIT_EOF_VALUE) break;
		if (! dhtree.htree[idx].is_leaf) idx = dhtree.htree[idx].value.childs[bit];
		if (dhtree.htree[idx].is_leaf) {
			assert(array_length < array_size);
			array[array_length++] = dhtree.htree[idx].value.leaf_value;
			idx = dhtree.root;
		}
	}
	assert(idx == dhtree.root);
	return array_length;
}

