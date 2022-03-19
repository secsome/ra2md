#include <RAMFile.h>

RAMFileClass::RAMFileClass(void* buffer, int len) noexcept
	: Buffer{ (char*)buffer }
	, MaxLength{ len }
	, Length{ len }
	, Offset{ 0 }
	, Access{ FILE_ACCESS_READ }
	, IsOpen{ false }
	, IsAllocated{ false }
{
	if (buffer == nullptr && len > 0) 
	{
		Buffer = new char[len];
		IsAllocated = true;
	}
}

RAMFileClass::~RAMFileClass()
{
	Close();
	if (IsAllocated) 
	{
		delete[] Buffer;
		Buffer = NULL;
		IsAllocated = false;
	}
}

bool RAMFileClass::Create()
{
	if (!Is_Open()) 
	{
		Length = 0;
		return true;
	}
	return false;
}

bool RAMFileClass::Delete()
{
	if (!Is_Open())
	{
		Length = 0;
		return true;
	}
	return false;
}

bool RAMFileClass::Is_Available(bool forced)
{
	return true;
}

bool RAMFileClass::Is_Open() const
{
	return IsOpen;
}

bool RAMFileClass::Open(const char* filename, ENUM_FILE_ACCESS access)
{
	return Open(access);
}

bool RAMFileClass::Open(ENUM_FILE_ACCESS access)
{
	if (Buffer == nullptr || Is_Open())
		return false;

	Offset = 0;
	Access = access;
	IsOpen = true;

	switch (access) 
	{
	default:
	case FILE_ACCESS_READ:
		break;

	case FILE_ACCESS_WRITE:
		Length = 0;
		break;

	case FILE_ACCESS_READWRITE:
		break;
	}

	return Is_Open();
}

int RAMFileClass::Read(void* buffer, int size)
{
	if (Buffer == nullptr || buffer == nullptr || size == 0)
		return 0;

	bool hasopened = false;
	if (!Is_Open()) 
	{
		Open(FILE_ACCESS_READ);
		hasopened = true;
	}
	else 
	{
		if ((Access & FILE_ACCESS_READ) == 0)
			return 0;
	}

	int tocopy = MIN(size, Length - Offset);
	memmove(buffer, &Buffer[Offset], tocopy);
	Offset += tocopy;

	if (hasopened)
		Close();

	return tocopy;
}

int RAMFileClass::Seek(int pos, int dir)
{
	if (Buffer == nullptr || !Is_Open())
		return Offset;

	int maxoffset = Length;
	if ((Access & FILE_ACCESS_WRITE) != 0)
		maxoffset = MaxLength;

	switch (dir) 
	{
	case SEEK_CUR:
		Offset += pos;
		break;

	case SEEK_SET:
		Offset = 0 + pos;
		break;

	case SEEK_END:
		Offset = maxoffset + pos;
		break;
	}

	if (Offset < 0) 
		Offset = 0;
	if (Offset > maxoffset) 
		Offset = maxoffset;

	Offset = CLAMP(Offset, 0, maxoffset);

	if (Offset > Length)
		Length = Offset;

	return Offset;
}

int RAMFileClass::Size()
{
	return Length;
}

int RAMFileClass::Write(void* const buffer, int size)
{
	if (Buffer == nullptr || buffer == nullptr || size == 0)
		return 0;

	bool hasopened = false;
	if (!Is_Open()) 
	{
		Open(FILE_ACCESS_WRITE);
		hasopened = true;
	}
	else
	{
		if ((Access & FILE_ACCESS_WRITE) == 0)
			return 0;
	}

	int towrite = MIN(size, MaxLength - Offset);
	memmove(&Buffer[Offset], buffer, towrite);
	Offset += towrite;

	if (Offset > Length)
		Length = Offset;

	if (hasopened)
		Close();

	return towrite;
}

void RAMFileClass::Close()
{
	IsOpen = false;
}
