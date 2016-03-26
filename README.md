# huffman-coding
Huffman coding implementation and utility classes for abstract binary coding including base64 coding implementation

##### Notation:
`symbol` - elementary part of data, mostly unsigned integer type<br />
`dhtree` - decode Huffman tree

Before calling `make` create `build` directory (`mkdir build`)

## Main code and tests

#### `huffman.h`
`huffman.h` - `Huffman` template class with Huffman coding implementation
##### `Huffman` methods (all static):
`encode` - encode data (input: symbol array, output: bit stream and dhtree)<br />
`decode` - decode data (input: bit stream and dhtree, output: symbol stream)<br />
`dhtree_encode` - encode dhtree (input: dhtree, output: bit stream)<br />
`dhtree_decode` - decode dhtree (input: bit stream, output: dhtree)

#### `tests.cpp`
`tests.cpp` - tests and usage examples, **compile** by `make tests`

#### `huffman.cpp`
`huffman.cpp` - command line utility: Huffman encode or decode from stdin to stdout<br />
**compile** by `make`
##### Usage:
```
./huffman -h
./huffman (-e|-d) [-b] -t <file> [-k]
```
##### Options:
```
-h          print help and exit
-e or -d    encode or decode (must be exactly one of them)
-b          encode using base64 or decode as base64
-t <file>   huffman tree file (output if encode or input if decode)
-k          encode huffman tree file using base64 or decode as base64
```

## Utility classes

#### `ichario`
`ichario.{h,cpp}` - `IChar{In,Out}` interfaces (`ICharIn` is actually an abstract class): char streams, inherited from `ISymbol{In,Out}<unsigned char>`
##### Implementations:
`file_chario.{h,cpp}` - `FileChar{In,Out}` classes: wrap stdio FILE streams<br />
`line_file_chario.{h,cpp}` - `LineFileCharOut` class: like `FileCharOut` with splitting into lines<br />
`string_chario.{h,cpp}` - `StringChar{In,Out}` classes: wrap char string (array of chars)<br />
Actually I'd like to get rid of all `*char*` classes and use only `symbol`, but file io is implemented easier with chars.

#### `isymbolio`
`isymbolio.h` - `ISymbol{In,Out}` template interfaces: symbol streams
##### Implementations:
`array_symbolio.h` - `ArraySymbol{In,Out}` template classes: wrap array of symbols<br />
`char_symbolio.h` - `CharSymbol{In,Out}` template classes: wrap `IChar{In,Out}`, encode symbol stream to or decode from char stream<br />
`bit_symbolio.h` - `BitSymbol{In,Out}` template classes: wrap `IBit{In,Out}`, encode symbol stream to or decode from bit stream, symbol size is set with template parameter<br />
`vbit_symbolio.h` - `VBitSymbol{In,Out}` template classes: wrap `IBit{In,Out}`, encode symbol stream to or decode from bit stream, symbol size is set with constructor parameter

#### `ibitio`
`ibitio.{h,cpp}` - `IBit{In,Out}` interfaces (`IBitOut` is actually an abstract class): bit streams
##### Implementations:
`symbol_bitio.h` - `SymbolBit{In,Out}` template classes: wrap `ISymbol{In,Out}`, encode bit stream to or decode from symbol stream, support various bit coding, for example base64 coding classes are inherited from them<br />
`base64_char_bitio.{h,cpp}` - `Base64CharBit{In,Out}` classes (inherited from `SymbolBit{In,Out}`): wrap `ISymbol{In,Out}<unsigned char>`, encode bit stream to or decode from base64 char stream

