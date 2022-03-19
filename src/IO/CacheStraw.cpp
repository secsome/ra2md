#include <CacheStraw.h>

CacheStraw::CacheStraw(Buffer const& buffer)
	: Straw{}
	, BufferPtr{ buffer }
	, Index{ 0 }
	, Length{ 0 }
{
}

CacheStraw::CacheStraw(int length)
	: Straw{}
	, BufferPtr{ length }
	, Index{ 0 }
	, Length{ 0 }
{
}

CacheStraw::~CacheStraw()
{
}

int CacheStraw::Get(void* buffer, int length)
{
	int total = 0;

	if (Is_Valid() && buffer != nullptr && length > 0) 
	{
		while (length > 0)
		{
			if (Length > 0) 
			{
				int tocopy = MIN(Length, length);
				memmove(buffer, ((char*)BufferPtr.Get_Buffer()) + Index, tocopy);
				length -= tocopy;
				Index += tocopy;
				total += tocopy;
				Length -= tocopy;
				buffer = (char*)buffer + tocopy;
			}
			if (length == 0) 
				break;

			Length = Straw::Get(BufferPtr, BufferPtr.Get_Size());
			Index = 0;
			if (Length == 0) 
				break;
		}
	}
	return total;
}