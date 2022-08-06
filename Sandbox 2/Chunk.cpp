#include "Chunk.h"

int clamp(int number, int min, int max)
{
	if (number < min)
		return min;
	if (number > max)
		return max;
	return number;
}