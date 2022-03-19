#include <BufferPipe.h>

BufferPipe::BufferPipe(Buffer const& buffer) noexcept
	: Pipe{}
	, BufferPtr{ buffer }
	, Index{ 0 }
{
}

BufferPipe::BufferPipe(void* buffer, int length) noexcept
	: Pipe{}
	, BufferPtr{ buffer, length }
	, Index{ 0 }
{
}

BufferPipe::~BufferPipe()
{
}

int BufferPipe::Put(const void* source, int slen)
{
	int total = 0;

	if (Is_Valid() && source != nullptr && slen > 0) 
	{
		int len = slen;
		if (BufferPtr.Get_Size() != 0) 
			len = MIN(slen, BufferPtr.Get_Size() - Index);

		if (len > 0)
			memmove(((char*)BufferPtr.Get_Buffer()) + Index, source, len);

		Index += len;
		total += len;
	}

	return total;
}
