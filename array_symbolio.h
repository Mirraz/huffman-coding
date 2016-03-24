#ifndef ARRAY_SYMBOLIO_H
#define ARRAY_SYMBOLIO_H

#include <stddef.h>
#include "isymbolio.h"

template <typename SYMBOL_TYPE>
class ArraySymbolOut : public ISymbolOut<SYMBOL_TYPE> {
public:
	typedef SYMBOL_TYPE symbol_type;
	
private:
	symbol_type *array;
	size_t size;
	size_t idx;
	
public:
	ArraySymbolOut(symbol_type *b_array, size_t b_size) : array(b_array), size(b_size), idx(0) {}
	
	virtual ~ArraySymbolOut() {}
	
	virtual void put(symbol_type symbol) {
		assert(idx < size);
		array[idx++] = symbol;
	}
	
	virtual void finish(){
	}
	
	virtual size_t get_length() const {
		return idx;
	}
};

template <typename SYMBOL_TYPE>
class ArraySymbolIn : public ISymbolIn<SYMBOL_TYPE> {
public:
	typedef SYMBOL_TYPE symbol_type;

private:
	const symbol_type *array;
	size_t size;
	size_t idx;
	
public:
	ArraySymbolIn(const symbol_type *b_array, size_t b_size) : array(b_array), size(b_size), idx(0) {}
	
	virtual ~ArraySymbolIn() {}
	
	virtual bool get(symbol_type &symbol) {
		if (idx == size) return false;
		symbol = array[idx++];
		return true;
	}
};

#endif/*ARRAY_SYMBOLIO_H*/

