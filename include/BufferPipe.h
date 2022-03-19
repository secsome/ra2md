#pragma once

#include <Always.h>

#include <Pipe.h>
#include <Buffer.h>

class BufferPipe : public Pipe
{
public:
	explicit BufferPipe(Buffer const& buffer) noexcept;
	explicit BufferPipe(void* buffer, int length) noexcept;

	CLASS_NOCOPY(BufferPipe);
	CLASS_CANMOVE(BufferPipe);

	virtual ~BufferPipe() override;

	virtual int Put(const void* source, int slen) override;

private:
	bool Is_Valid() { return BufferPtr.Is_Valid(); }

private:
	Buffer BufferPtr;
	int Index;
};