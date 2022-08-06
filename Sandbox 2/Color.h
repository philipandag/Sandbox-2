#pragma once
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include "Randomiser.h"

struct Color
{
	unsigned char r, g, b, a;
	unsigned char& operator[](unsigned char pos);

	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
	Color& randomise();
};
