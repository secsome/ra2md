#include <LZOStraw.h>

#include <LZO.h>

LZOStraw::LZOStraw(CompControl control, int blocksize) noexcept
	: Control{ control }
	, Counter{ 0 }
	, Buffer{ nullptr }
	, Buffer2{ nullptr }
	, BlockSize{ blocksize }
	, BlockHeader{ 0,0 }
{
	SafetyMargin = BlockSize;
	Buffer = new char[BlockSize + SafetyMargin];
	if (control == COMPRESS)
		Buffer2 = new char[BlockSize + SafetyMargin];
}

LZOStraw::~LZOStraw()
{
	delete[] Buffer;
	Buffer = nullptr;

	if (Buffer2)
	{
		delete[] Buffer2;
		Buffer2 = nullptr;
	}
}

int LZOStraw::Get(void* buffer, int length)
{
	if (buffer == nullptr || length < 1)
		return 0;
	
	int total = 0;
	while (length > 0) 
	{
		if (Counter) 
		{
			int len = MIN(length, Counter);
			if (Control == DECOMPRESS)
				memmove(buffer, &Buffer[BlockHeader.UncompCount - Counter], len);
			else
				memmove(buffer, &Buffer2[(BlockHeader.CompCount + sizeof(BlockHeader)) - Counter], len);

			buffer = ((char*)buffer) + len;
			length -= len;
			Counter -= len;
			total += len;
		}
		if (length == 0) 
			break;

		if (Control == DECOMPRESS) 
		{
			int incount = Straw::Get(&BlockHeader, sizeof(BlockHeader));
			if (incount != sizeof(BlockHeader)) 
				break;

			char* staging_buffer = new char[BlockHeader.CompCount];
			incount = Straw::Get(staging_buffer, BlockHeader.CompCount);
			if (incount != BlockHeader.CompCount) 
				break;
			unsigned int length = sizeof(Buffer);
			lzo1x_decompress((unsigned char*)staging_buffer, BlockHeader.CompCount, (unsigned char*)Buffer, &length, NULL);
			delete[] staging_buffer;
			Counter = BlockHeader.UncompCount;
		}
		else 
		{
			BlockHeader.UncompCount = (unsigned short)Straw::Get(Buffer, BlockSize);
			if (BlockHeader.UncompCount == 0) 
				break;
			char* dictionary = new char[64 * 1024];
			unsigned int length = sizeof(Buffer2) - sizeof(BlockHeader);
			lzo1x_1_compress((unsigned char*)Buffer, BlockHeader.UncompCount, (unsigned char*)(&Buffer2[sizeof(BlockHeader)]), &length, dictionary);
			BlockHeader.CompCount = (unsigned short)length;
			delete[] dictionary;
			memmove(Buffer2, &BlockHeader, sizeof(BlockHeader));
			Counter = BlockHeader.CompCount + sizeof(BlockHeader);
		}
	}

	return total;
}
