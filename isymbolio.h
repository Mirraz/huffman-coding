#ifndef ISYMBOLIO_H
#define ISYMBOLIO_H

template <typename SYMBOL_TYPE>
class ISymbolOut {
public:
	typedef SYMBOL_TYPE symbol_type;
	
	virtual ~ISymbolOut() {}
	virtual void put(symbol_type symbol) = 0;
	virtual void finish() = 0;
};

template <typename SYMBOL_TYPE>
class ISymbolIn {
public:
	typedef SYMBOL_TYPE symbol_type;
	
	virtual ~ISymbolIn() {}
	virtual bool get(symbol_type &symbol) = 0;
};

#endif/*ISYMBOLIO_H*/

