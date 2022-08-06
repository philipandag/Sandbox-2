#pragma once
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include "Randomiser.h"
#include "Color.h"
struct Vector4
{
	int r, g, b, a;
	int& operator[](int pos);

	Vector4& randomise(int min = 0, int max = 255);
	operator Color() {
		return Color(r, g, b, a);
	}
};

