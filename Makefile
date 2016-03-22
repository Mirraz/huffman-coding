CC=g++
LD=g++
STRIP=strip -s
#STRIP=true
WARNINGS=-Wall -Wextra
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

tests: $(BUILD_DIR)/tests.o $(BUILD_DIR)/base64.o $(BUILD_DIR)/bitio.o $(BUILD_DIR)/chario.o
	$(LD) -o $@ $^ $(LDFLAGS)
	$(STRIP) $@

$(BUILD_DIR)/tests.o: $(SRC_DIR)/tests.cpp $(SRC_DIR)/huffman.h $(SRC_DIR)/base64.h $(SRC_DIR)/bitio.h $(SRC_DIR)/chario.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)

$(BUILD_DIR)/base64.o: $(SRC_DIR)/base64.cpp $(SRC_DIR)/base64.h $(SRC_DIR)/bitio.h $(SRC_DIR)/chario.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)

$(BUILD_DIR)/bitio.o: $(SRC_DIR)/bitio.cpp $(SRC_DIR)/bitio.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)

$(BUILD_DIR)/chario.o: $(SRC_DIR)/chario.cpp $(SRC_DIR)/chario.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)

$(BUILD_DIR)/binary.o: $(SRC_DIR)/binary.cpp $(SRC_DIR)/binary.h $(SRC_DIR)/bitio.h $(SRC_DIR)/chario.h Makefile
	$(CC) -o $@ $< -c $(CFLAGS)

clean:
	rm -rf $(BUILD_DIR)
	mkdir $(BUILD_DIR)

