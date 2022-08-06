#pragma once
#include "Vector2.h"
struct Vector3
{
	int x, y, z;
	int& r = x;
	int& g = y;
	int& b = z;

	int operator[](int pos);
};

