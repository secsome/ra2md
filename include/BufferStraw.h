#pragma once

#include <Always.h>

#include <Straw.h>
#include <Buffer.h>

class BufferStraw : public Straw
{
public:
	explicit BufferStraw(const Buffer& buffer) noexcept;
	explicit BufferStraw(const void* buffer, int length) noexcept;

	CLASS_NOCOPY(BufferStraw);
	CLASS_CANMOVE(BufferStraw);

	virtual ~BufferStraw() override;

	virtual int Get(void* buffer, int length) override;

private:
	bool Is_Valid() { return(BufferPtr.Is_Valid()); }

private:
	Buffer BufferPtr;
	int Index;
};