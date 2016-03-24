#ifndef CHAR_BITIO_H
#define CHAR_BITIO_H

#include <stdint.h>
#include "abstract_char_bitio.h"

char code_to_char(uint_fast8_t code);
typedef AbstractCharBitOut<8, code_to_char> CharBitOut;

uint_fast8_t char_to_code(char c);
typedef AbstractCharBitIn<8, char_to_code> CharBitIn;

#endif/*CHAR_BITIO_H*/

