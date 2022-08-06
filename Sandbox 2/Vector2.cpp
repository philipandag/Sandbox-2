#include "Vector2.h"

Vector2::Vector2(int x, int y) :
	x(x), y(y)
{}

int Vector2::operator[](int pos) {
	try {
		return pos == 0 ? x : pos == 1 ? y : throw std::out_of_range("Out of vector range");
	}
	catch (const std::out_of_range& e)
	{
		std::cout << e.what() << '\n';
	}
}