#include <BufferStraw.h>

BufferStraw::BufferStraw(const Buffer& buffer) noexcept
	: Straw{}
	, BufferPtr{ buffer }
	, Index{ 0 }
{
}

BufferStraw::BufferStraw(const void* buffer, int length) noexcept
	: Straw{}
	, BufferPtr{ (void*)buffer, length }
	, Index{ 0 }
{
}

BufferStraw::~BufferStraw()
{
}

int BufferStraw::Get(void* buffer, int length)
{
	int total = 0;

	if (Is_Valid() && buffer != nullptr && length > 0) 
	{
		int len = length;
		if (BufferPtr.Get_Size() != 0) 
			len = MIN(length, BufferPtr.Get_Size() - Index);

		if (len > 0)
			memmove(buffer, ((char*)BufferPtr.Get_Buffer()) + Index, len);

		Index += len;
		total += len;
	}
	return total;
}
