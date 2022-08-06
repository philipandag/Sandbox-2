#pragma once
#include <stdexcept>
#include <iostream>
struct Vector2
{
	int x, y;
	Vector2(int x = 0, int y = 0);

	int operator[](int pos);

	Vector2 operator/(double divisor) {
		return Vector2(x / divisor, y / divisor);
	}
};

