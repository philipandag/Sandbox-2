#include "Vector3.h"

int Vector3::operator[](int pos) {
	try {
		return pos == 0 ? x : pos == 1 ? y : pos == 2 ? z : throw std::out_of_range("Out of vector range");
	}
	catch (const std::out_of_range& e)
	{
		std::cout << e.what() << '\n';
	}
}