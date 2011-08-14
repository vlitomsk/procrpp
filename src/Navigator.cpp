#include "Navigator.h"

direction get_direction(int tx, int ty, int ax, int ay) {
	direction res;
	res.left = ax - tx;
	res.up = ay - ty;

	return res;
}
