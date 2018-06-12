#ifndef _PIPE_H
#define _PIPE_H

#include "define.h"

typedef struct _Pipe_t {
	BYTE * beginPtr;
	BYTE * headPtr;
	BYTE * tailPtr;
	BYTE * endPtr;
}  Pipe_t;

void pipe_init(Pipe_t *pipe, BYTE *buffer, WORD size);
bool pipe_write(Pipe_t *pipe, BYTE value);
bool pipe_read(Pipe_t *pipe, BYTE *value);

#endif
