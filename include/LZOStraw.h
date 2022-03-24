#pragma once

#include <Always.h>

#include <Straw.h>

class LZOStraw : public Straw
{
public:
	enum CompControl 
	{
		COMPRESS,
		DECOMPRESS
	};

	explicit LZOStraw(CompControl control, int blocksize = 1024 * 8) noexcept;

	CLASS_NOCOPY(LZOStraw);
	CLASS_CANMOVE(LZOStraw);

	virtual ~LZOStraw() override;

	virtual int Get(void* buffer, int length) override;

private:
	CompControl Control;
	int Counter;
	char* Buffer;
	char* Buffer2;
	int BlockSize;
	int SafetyMargin;
	struct 
	{
		unsigned short CompCount;
		unsigned short UncompCount;
	} BlockHeader;
};