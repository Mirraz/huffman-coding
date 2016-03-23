CC=g++
LD=g++
STRIP=strip -s
#STRIP=true
WARNINGS=-Wall -Wextra -pedantic
DEBUG=
#DEBUG=-g -ggdb
COPTIM=-march=native -O2
#COPTIM=-O0
DEFINES=
INCLUDES=
CSTD=-std=c++11
CFLAGS=$(WARNINGS) $(DEBUG) $(COPTIM) $(DEFINES) $(INCLUDES) $(CSTD) -pipe
LDOPTIM=-Wl,-O1 -Wl,--as-needed
#LDOPTIM=
LIBFILES=
LDFLAGS=$(WARNINGS) $(DEBUG) $(LDOPTIM) $(LIBFILES)
SRC_DIR=.
BUILD_DIR=build

tests: $(BUILD_DIR)/tests.o $(BUILD_DIR)/ibitio.o $(BUILD_DIR)/ichario.o $(BUILD_DIR)/file_chario.o $(BUILD_DIR)/line_file_chario.o $(BUILD_DIR)/string_chario.o $(BUILD_DIR)/char_bitio.o $(BUILD_DIR)/base64_char_bitio.o
	$(LD) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@

$(BUILD_DIR)/tests.o: $(SRC_DIR)/tests.cpp $(SRC_DIR)/ibitio.h $(SRC_DIR)/ichario.h $(SRC_DIR)/isymbolio.h $(SRC_DIR)/file_chario.h $(SRC_DIR)/line_file_chario.h $(SRC_DIR)/string_chario.h $(SRC_DIR)/char_bitio.h $(SRC_DIR)/base64_char_bitio.h $(SRC_DIR)/bit_symbolio.h $(SRC_DIR)/array_symbolio.h $(SRC_DIR)/char_symbolio.h $(SRC_DIR)/huffman.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)

$(BUILD_DIR)/ibitio.o: $(SRC_DIR)/ibitio.cpp $(SRC_DIR)/ibitio.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)

$(BUILD_DIR)/ichario.o: $(SRC_DIR)/ichario.cpp $(SRC_DIR)/ichario.h $(SRC_DIR)/isymbolio.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)

$(BUILD_DIR)/file_chario.o: $(SRC_DIR)/file_chario.cpp $(SRC_DIR)/file_chario.h $(SRC_DIR)/ichario.h $(SRC_DIR)/isymbolio.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)

$(BUILD_DIR)/line_file_chario.o: $(SRC_DIR)/line_file_chario.cpp $(SRC_DIR)/line_file_chario.h $(SRC_DIR)/file_chario.h $(SRC_DIR)/ichario.h $(SRC_DIR)/isymbolio.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)

$(BUILD_DIR)/string_chario.o: $(SRC_DIR)/string_chario.cpp $(SRC_DIR)/string_chario.h $(SRC_DIR)/ichario.h $(SRC_DIR)/isymbolio.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)

$(BUILD_DIR)/char_bitio.o: $(SRC_DIR)/char_bitio.cpp $(SRC_DIR)/char_bitio.h $(SRC_DIR)/ibitio.h $(SRC_DIR)/ichario.h $(SRC_DIR)/isymbolio.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)

$(BUILD_DIR)/base64_char_bitio.o: $(SRC_DIR)/base64_char_bitio.cpp $(SRC_DIR)/base64_char_bitio.h $(SRC_DIR)/ibitio.h $(SRC_DIR)/ichario.h $(SRC_DIR)/isymbolio.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)


huffman: $(BUILD_DIR)/huffman.o $(BUILD_DIR)/ibitio.o $(BUILD_DIR)/ichario.o $(BUILD_DIR)/file_chario.o $(BUILD_DIR)/char_bitio.o
	$(LD) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@

$(BUILD_DIR)/huffman.o: $(SRC_DIR)/huffman.cpp $(SRC_DIR)/huffman.h $(SRC_DIR)/file_chario.h $(SRC_DIR)/char_symbolio.h $(SRC_DIR)/char_bitio.h $(SRC_DIR)/ichario.h $(SRC_DIR)/isymbolio.h $(SRC_DIR)/ibitio.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)


clean:
	rm -rf $(BUILD_DIR)
	mkdir $(BUILD_DIR)

