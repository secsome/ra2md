#include <RandomStraw.h>

#include <SHA.h>

RandomStraw RandomStraw::CryptRandom;

RandomStraw::RandomStraw() noexcept
	: Straw{}
	, SeedBits{ 0 }
	, Current { 0 }
{
	Reset();
}

RandomStraw::~RandomStraw()
{
	Reset();
}

int RandomStraw::Get(void* buffer, int length)
{
	if (buffer == nullptr || length < 1) 
		return Straw::Get(buffer, length);

	int total = 0;
	while (length > 0)
	{
		*(char*)buffer = (char)Random[Current++];
		Current = Current % (sizeof(Random) / sizeof(Random[0]));
		buffer = (char*)buffer + sizeof(char);
		--length;
		++total;
	}
	return total;
}

void RandomStraw::Reset()
{
	SeedBits = 0;
	Current = 0;
	memset(Random, 0, sizeof(Random));
}

void RandomStraw::Seed_Bit(int seed)
{
	char* ptr = ((char*)&Random[0]) + ((SeedBits / CHAR_BIT) % sizeof(Random));
	char frac = (char)(1 << (SeedBits & (CHAR_BIT - 1)));

	if (seed & 0x01)
		*ptr ^= frac;

	++SeedBits;

	if (SeedBits == (sizeof(Random) * CHAR_BIT))
		Scramble_Seed();
}

void RandomStraw::Seed_Byte(char seed)
{
	for (int index = 0; index < CHAR_BIT; ++index)
	{
		Seed_Bit(seed);
		seed >>= 1;
	}
}

void RandomStraw::Seed_Short(short seed)
{
	for (int index = 0; index < (sizeof(seed) * CHAR_BIT); ++index) 
	{
		Seed_Bit(seed);
		seed >>= 1;
	}
}

void RandomStraw::Seed_Int(int seed)
{
	for (int index = 0; index < (sizeof(seed) * CHAR_BIT); ++index)
	{
		Seed_Bit(seed);
		seed >>= 1;
	}
}

int RandomStraw::Seed_Bits_Needed(void) const
{
	const int total = sizeof(Random) * CHAR_BIT;
	
	return SeedBits < total ? total - SeedBits : 0;
}

void RandomStraw::Scramble_Seed()
{
	SHAEngine sha;

	for (int index = 0; index < sizeof(Random); ++index)
	{
		char digest[20];

		sha.Hash(&Random[0], sizeof(Random));
		sha.Result(digest);

		int tocopy = sizeof(digest) < (sizeof(Random) - index) ? sizeof(digest) : (sizeof(Random) - index);
		memmove(((char*)&Random[0]) + index, digest, tocopy);
	}
}
