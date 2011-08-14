#ifndef NAVIGATOR_H_INCLUDED
#define NAVIGATOR_H_INCLUDED

#include "types.h"

struct direction {
	u8 left, up;
};

extern direction get_direction(int tx, int ty, int ax, int ay);

#endif
