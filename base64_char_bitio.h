#ifndef BASE64_CHAR_BITIO_H
#define BASE64_CHAR_BITIO_H

#include <stdint.h>
#include "abstract_char_bitio.h"

char base64_code_to_char(uint_fast8_t code);
typedef AbstractCharBitOut<6, base64_code_to_char> Base64CharBitOut;

uint_fast8_t base64_char_to_code(char c);
typedef AbstractCharBitIn<6, base64_char_to_code> Base64CharBitIn;

#endif/*BASE64_CHAR_BITIO_H*/

