#pragma once

#include <Always.h>

class RandomClass
{
public:
	explicit RandomClass(unsigned seed = 0) noexcept;

	CLASS_CANDEFAULTCOPY(RandomClass);
	CLASS_CANMOVE(RandomClass);

	~RandomClass() = default;

	operator int() { return(operator()()); };
	int operator()();
	int operator()(int minval, int maxval);

	enum { SIGNIFICANT_BITS = 15 };

	unsigned int Seed;

protected:
	enum 
	{
		MULT_CONSTANT = 0x41C64E6D,
		ADD_CONSTANT = 0x00003039,
		THROW_AWAY_BITS = 10
	};
};