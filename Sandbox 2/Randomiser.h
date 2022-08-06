#pragma once
#include <time.h>
#include <math.h>

class Randomiser
{
	static unsigned int seed;
	static unsigned long x, y, z;
	static size_t precision;
	
public:
	static void initRandomiser() {
		changeSeed(time(0));
	}
	static void changeSeed(time_t newSeed) {
		seed = (unsigned int)newSeed;
		x = seed;
		y = seed << 2;
		z = seed >> 4 * y;
	}
	
	static bool yesno(double chance = 0.5)
	{
		return rand() % precision < chance* precision;
	}

	static unsigned long rand(unsigned int min, unsigned int max) {
		return rand(max-min) + min;
	}
	static unsigned long rand(unsigned int max) {
		return rand() % (max == 0 ? 1 : max);
	}
	static unsigned long rand() {
		unsigned long t;
		x ^= x << 13;
		x ^= x >> 5;
		x ^= x << 1;

		t = x;
		x = y;
		y = z;
		z = t ^ x ^ y;

		return z;
	}

	static float randomFloat(int precision = 5) {
		int precisionDecimal = static_cast<int>(pow(10, precision));
		float result = static_cast<float>(rand() % precisionDecimal);
		return result / precisionDecimal;
	}
};

