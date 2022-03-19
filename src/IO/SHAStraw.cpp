#include <SHAStraw.h>

SHAStraw::~SHAStraw()
{
}

int SHAStraw::Get(void* buffer, int length)
{
	if (buffer == nullptr || length < 1)
		return 0;

	int counter = Straw::Get(buffer, length);
	SHA.Hash(buffer, counter);
	return counter;
}

int SHAStraw::Result(void* result) const
{
	return SHA.Result(result);
}
