#pragma once

#include <Always.h>

#include <Straw.h>
#include <Buffer.h>

class CacheStraw : public Straw
{
public:
	CacheStraw(Buffer const& buffer);
	CacheStraw(int length = 4096);

	CLASS_NOCOPY(CacheStraw);
	CLASS_CANMOVE(CacheStraw);

	virtual ~CacheStraw() override;

	virtual int Get(void* buffer, int length) override;

private:
	bool Is_Valid() { return BufferPtr.Is_Valid(); }

private:
	Buffer BufferPtr;
	int Index;
	int Length;
};