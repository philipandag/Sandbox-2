#include "Color.h"

unsigned char& Color::operator[](unsigned char pos) {
	try {
		return pos == 0 ? r : pos == 1 ? g : pos == 2 ? b : pos == 3 ? a : throw std::out_of_range("Out of vector range");
	}
	catch (const std::out_of_range& e)
	{
		std::cout << e.what() << '\n';
		exit(-1);
	}
}

Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a):
	r(r),
	g(g),
	b(b),
	a(a)
{}

Color& Color::randomise()
{
	r = Randomiser::rand(255);
	g = Randomiser::rand(255);
	b = Randomiser::rand(255);
	a = Randomiser::rand(255);
	return *this;
}