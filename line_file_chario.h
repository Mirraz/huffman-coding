#ifndef LINE_FILE_CHARIO_H
#define LINE_FILE_CHARIO_H

#include "file_chario.h"

class LineFileCharOut : public FileCharOut {
private:
	size_t line_length;
	size_t line_counter;
	
public:
	LineFileCharOut(FILE *b_stream);
	LineFileCharOut(FILE *b_stream, size_t b_line_length);
	void put(unsigned char c);
	void finish();
};

#endif/*LINE_FILE_CHARIO_H*/

