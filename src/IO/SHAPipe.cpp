#include "SHAPipe.h"

SHAPipe::SHAPipe() noexcept
	: Pipe{}
{
}

SHAPipe::~SHAPipe()
{
}

int SHAPipe::Put(const void* source, int slen)
{
	SHA.Hash(source, slen);
	return Pipe::Put(source, slen);
}

int SHAPipe::Result(void* result) const
{
	return SHA.Result(result);
}
