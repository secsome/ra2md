#pragma once

#include <Always.h>

class CRCEngine
{
public:
	explicit CRCEngine(int initial = 0) noexcept;

	CLASS_CANDEFAULTCOPY(CRCEngine);
	CLASS_CANMOVE(CRCEngine);

	~CRCEngine() = default;

	int operator()() const;

	void operator()(char datum);
	int operator()(void* buffer, int length);
	int operator()(const void* buffer, int length);

	operator int() const;

protected:
	bool Buffer_Needs_Data() const;
	int Value() const;

	static int Memory(const void* data, int bytes, int crc);

protected:
	int CRC;
	int Index;
	union
	{
		int Composite;
		char Buffer[sizeof(int)];
	} StagingBuffer;

	static unsigned int Table[256];
};