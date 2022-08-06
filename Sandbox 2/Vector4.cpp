#include "Vector4.h"
int& Vector4::operator[](int pos) {
	try {
		return pos == 0 ? r : pos == 1 ? g : pos == 2 ? b : pos == 3 ? a : throw std::out_of_range("Out of vector range");
	}
	catch (const std::out_of_range& e)
	{
		std::cout << e.what() << '\n';
		exit(-1);
	}
}

Vector4& Vector4::randomise(int min, int max) 
{
	r = Randomiser::rand(min, max);
	g = Randomiser::rand(min, max);
	b = Randomiser::rand(min, max);
	a = Randomiser::rand(min, max);
	return *this;
}