#pragma once

#include <Always.h>

#include <Straw.h>
#include <Random.h>

class RandomStraw : public Straw
{
public:
	static RandomStraw CryptRandom;

	explicit RandomStraw() noexcept;

	CLASS_NOCOPY(RandomStraw);
	CLASS_CANMOVE(RandomStraw);

	virtual ~RandomStraw() override;

	virtual int Get(void* buffer, int length) override;

	void Reset();
	void Seed_Bit(int seed);
	void Seed_Byte(char seed);
	void Seed_Short(short seed);
	void Seed_Int(int seed);
	int Seed_Bits_Needed() const;

private:
	void Scramble_Seed();

private:
	int SeedBits;
	int Current;
	RandomClass Random[32];
};