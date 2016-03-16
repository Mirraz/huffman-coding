#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <map>
#include <queue>
#include <vector>
#include <functional>

typedef uint_fast32_t count_type;
typedef uint_fast8_t num_type;
typedef uint_fast16_t htree_idx_type;
typedef uint_fast16_t hcode_len_type;
typedef uint_fast8_t bit_type;
// count of all different possible num_type values
#define MAX_NUM_COUNT 256
#define MAX_HTREE_SIZE (MAX_NUM_COUNT*2)
#define MAX_HCODE_LEN MAX_HTREE_SIZE

struct HTreeNode {
	count_type weight;
	htree_idx_type parent_idx;
	bit_type code;
	bool is_leaf;
	num_type leaf_value;
	
	HTreeNode() : weight(0), parent_idx(0), code(0), is_leaf(false), leaf_value(0) {}
	HTreeNode(count_type b_weight, num_type b_leaf_value) :
		weight(b_weight), parent_idx(0), code(0), is_leaf(true), leaf_value(b_leaf_value) {}
	HTreeNode(count_type b_weight) :
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

// decode
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

void make_huffman_codes(const num_type array[], count_type array_size, code_map_type &code_map, DHTree &dhtree) {
	assert(code_map.empty());

	typedef std::map<num_type, count_type> frequency_map_type;
	frequency_map_type frequency;
	for (count_type i=0; i<array_size; ++i) {
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

/*
class CharPrinter {
private:
	FILE *stream;
	uint_fast16_t line_length;
	uint_fast16_t line_counter;
	
public:
	CharPrinter(FILE *b_stream) : stream(b_stream), line_length(64), line_counter(0) {}
	CharPrinter(FILE *b_stream, uint_fast16_t b_line_length) :
		stream(b_stream), line_length(b_line_length), line_counter(0) {}
	CharPrinter(const CharPrinter &b) :
		stream(b.stream), line_length(b.line_length), line_counter(b. line_counter) {}
	
	void putchar(char c) {
		if (line_counter == 0) fputc('"', stream);
		fputc(c, stream);
		++line_counter;
		if (line_counter == line_length) {
			fputc('"', stream);
			fputc('\n', stream);
			line_counter = 0;
		}
	}
	
	void finish() {
		if (line_counter != 0) {
			fputc('"', stream);
			fputc('\n', stream);
			line_counter = 0;
		}
	}
};
*/

class CharPrinter {
private:
	size_t length;
	size_t str_size;
	char *str;
public:
	CharPrinter(char *b_str, size_t b_str_size) : length(0), str_size(b_str_size), str(b_str) {}
	CharPrinter(const CharPrinter &b) : length(b.length), str_size(b.str_size), str(b.str) {}
	
	void putchar(char c) {
		assert(length < str_size);
		str[length++] = c;
	}
	
	void finish() {
	}
	
	size_t get_length() const {
		return length;
	}
};

static const char base64codes[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

uint_fast8_t base64_char_to_idx(char c) {
	if (c >= 'A' && c <= 'Z') return c - 'A';
	if (c >= 'a' && c <= 'z') return c - 'a' + 26;
	if (c >= '0' && c <= '9') return c - '0' + 26*2;
	if (c == '+') return 62;
	if (c == '/') return 63;
	assert(0);
	return 64;
}

class Base64Encoder {
private:
	CharPrinter *char_printer;
	uint_fast8_t remainder;
	uint_fast8_t mask;
	#define BASE64_MASK_END (1<<6)
public:
	Base64Encoder(CharPrinter *b_char_printer) :
		char_printer(b_char_printer), remainder(0), mask(1) {}
	
	void encode(const bit_type bits[], hcode_len_type count) {
		hcode_len_type i = 0;
		while (i < count) {
			for (; i<count && mask != BASE64_MASK_END; i++, mask <<= 1) {
				if (bits[i] == 1) remainder |= mask;
				else remainder &= ~mask;
			}
			if (mask != BASE64_MASK_END) break;
			assert(remainder < 64);
			char_printer->putchar(base64codes[remainder]);
			mask = 1;
		}
	}
	
	void finish() {
		bit_type term_bit = 1;
		encode(&term_bit, 1);
		if (mask != 1) {
			for (; mask != BASE64_MASK_END; mask <<= 1) {
				remainder &= ~mask;
			}
			assert(remainder < 64);
			char_printer->putchar(base64codes[remainder]);
			mask = 1;
		}
		char_printer->finish();
	}
};

class CharReader {
private:
	size_t length;
	size_t idx;
	const char *str;
public:
	CharReader(const char *b_str, size_t b_length) : length(b_length), idx(0), str(b_str) {}
	
	char getchar() {
		assert(!is_end());
		return str[idx++];
	}
	
	bool is_end() const {
		return idx == length;
	}
};

class Base64Decoder {
private:
	CharReader *char_reader;
	uint_fast8_t remainder;
	uint_fast8_t rem_counter;
public:
	Base64Decoder(CharReader *b_char_reader) :
		char_reader(b_char_reader), remainder(0), rem_counter(0) {}
	
	// returns: true if success, false if eof
	bool decode_bit(bit_type *bit) {
		if (char_reader->is_end() && rem_counter == 0) return false;
		if (rem_counter == 0) {
			remainder = base64_char_to_idx(char_reader->getchar());
			assert(remainder < 64);
			rem_counter = 6;
			if (char_reader->is_end()) {
				assert(remainder != 0);
				for (uint_fast8_t mask = 1 << 5; !(remainder & mask) ; mask >>= 1) --rem_counter;
				assert(rem_counter > 0 && rem_counter <= 6);
				--rem_counter;
				if (rem_counter == 0) return false;
			}
		}
		*bit = remainder & 1;
		remainder >>= 1;
		--rem_counter;
		return true;
	}
};

void encode(const num_type array[], count_type array_size, CharPrinter *char_printer, DHTree &dhtree) {
	code_map_type code_map;
	make_huffman_codes(array, array_size, code_map, dhtree);
	/*for (code_map_type::iterator it=code_map.begin(); it!=code_map.end(); ++it) {
		printf("%u: ", it->first);
		hcode &code = it->second;
		for (htree_idx_type i=0; i<code.length; ++i) printf("%u", code.bits[i]);
		printf("\n");
	}*/
	
	Base64Encoder encoder(char_printer);
	for (count_type i=0; i<array_size; ++i) {
		hcode &code = code_map[array[i]];
		encoder.encode(code.bits, code.length);
	}
	encoder.finish();
}

count_type decode(const DHTree &dhtree, CharReader *char_reader, num_type array[], count_type array_size) {
	count_type array_length = 0;
	Base64Decoder decoder(char_reader);
	
	htree_idx_type idx = dhtree.root;
	while (true) {
		bit_type bit;
		bool decode_bit_res = decoder.decode_bit(&bit);
		if (!decode_bit_res) break;
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

void print_dhtable(const DHTree &dhtree) {
	printf("{%u,%u,{\n", (unsigned int)dhtree.root, (unsigned int)dhtree.size);
	for (htree_idx_type i=0; i<dhtree.size; ++i) {
		const DHTreeNode &dnode = dhtree.htree[i];
		printf("{%s,{", dnode.is_leaf ? "true" : "false");
		if (dnode.is_leaf) {
			printf("%u", (unsigned int)dnode.value.leaf_value);
		} else {
			printf("{%u,%u}", (unsigned int)dnode.value.childs[0], (unsigned int)dnode.value.childs[1]);
		}
		printf("}},\n");
	}
	printf("}}");
}

void print_base64_string(const char base64_str[], size_t base64_str_length, size_t max_line_length) {
	size_t i = 0;
	while (i < base64_str_length) {
		putchar(base64_str[i]);
		++i;
		if (i % max_line_length == 0 && i != base64_str_length) putchar('\n');
	}
}

#define MAX_TEXT_SIZE 200000

void run() {
	static num_type text[MAX_TEXT_SIZE];
	count_type text_length = 0;
	
	// input
	while (true) {
		int c = getchar();
		if (c == EOF) break;
		assert(c>=0 && c<=255);
		text[text_length++] = c;
	}
	assert(text_length > 0);
	assert(text_length <= MAX_TEXT_SIZE);
	
	// algo
	#define base64_str_size (MAX_TEXT_SIZE*4/3+1)
	static char base64_str[base64_str_size];
	CharPrinter char_printer(base64_str, base64_str_size);
	DHTree dhtree;
	encode(text, text_length, &char_printer, dhtree);
	size_t base64_str_length = char_printer.get_length();
	
	// checking
	CharReader char_reader(base64_str, base64_str_length);
	#define da_size MAX_TEXT_SIZE
	static num_type da[da_size];
	count_type dn = decode(dhtree, &char_reader, da, da_size);
	assert(dn == text_length);
	for (count_type i=0; i<text_length; ++i) assert(text[i] == da[i]);
	
	// output
	printf("dhtree =\n");
	print_dhtable(dhtree);
	printf("\n");
	printf("base64_str_length = %u\n", (unsigned int)base64_str_length);
	printf("base64_str =\n");
	print_base64_string(base64_str, base64_str_length, 64);
	printf("\n");
}

int main() {
	run();
	return 0;
}

