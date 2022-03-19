#include <RawFile.h>

RawFileClass::RawFileClass(const char* filename) noexcept
	: Rights{ FILE_ACCESS_READ }
	, BiasStart{ 0 }
	, BiasLength{ -1 }
	, Handle{ NULL_HANDLE }
	, Filename{ filename }
	, Date{ 0 }
	, Time{ 0 }
	, Allocated{ false }
{
}

RawFileClass::RawFileClass() noexcept
	: Rights{ FILE_ACCESS_READ }
	, BiasStart{ 0 }
	, BiasLength{ -1 }
	, Handle{ NULL_HANDLE }
	, Filename{ nullptr }
	, Date{ 0 }
	, Time{ 0 }
	, Allocated{ false }
{
}

RawFileClass::~RawFileClass()
{
	Close();
	if (Allocated && Filename)
	{
		free((char*)Filename);
		Filename = nullptr;
		Allocated = false;
	}
}

const char* RawFileClass::File_Name() const
{
	return Filename;
}

const char* RawFileClass::Set_Name(const char* filename)
{
	if (Filename != nullptr && Allocated) 
	{
		free((char*)Filename);
		Filename = nullptr;
		Allocated = false;
	}

	if (filename == nullptr) 
		return nullptr;

	Bias(0);

	Filename = _strdup(filename);

	if (!Filename)
		Error(ENOMEM, false, filename);

	Allocated = true;
	
	return Filename;
}

bool RawFileClass::Create()
{
	Close();

	if (Open(FILE_ACCESS_WRITE))
	{
		if (BiasLength != -1)
			Seek(0, SEEK_CUR);

		Close();
		return true;
	}

	return false;
}

bool RawFileClass::Delete()
{
	Close();

	if (Filename == nullptr)
	{
		Error(ENOENT, false);
		return false;
	}

	for (;;)
	{
		if (!Is_Available())
			return false;

		if (!DeleteFile(Filename))
		{
			Error(GetLastError(), false, Filename);
			return false;
		}
	}

	return true;
}

bool RawFileClass::Is_Available(bool forced)
{
	if (Filename == nullptr)
		return false;

	if (Is_Open())
		return true;

	if (forced)
	{
		RawFileClass::Open(FILE_ACCESS_READ);
		RawFileClass::Close();
		return true;
	}

	for (;;)
	{
		Handle = CreateFile(Filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (Handle == NULL_HANDLE)
			return false;

		break;
	}

	if (!CloseHandle(Handle))
		Error(GetLastError(), false, Filename);

	Handle = NULL_HANDLE;

	return true;
}

bool RawFileClass::Is_Open() const
{
	return Handle != NULL_HANDLE;
}

bool RawFileClass::Open(const char* filename, ENUM_FILE_ACCESS rights)
{
	Set_Name(filename);
	return Open(rights);
}

bool RawFileClass::Open(ENUM_FILE_ACCESS rights)
{
	Close();

	if (Filename == nullptr)
	{
		Error(ENOENT, false);
		return false;
	}

	Rights = rights;

	for (;;)
	{
		switch (rights)
		{
		case FILE_ACCESS_READ:
			Handle = CreateFile(Filename, GENERIC_READ, FILE_SHARE_READ, 
				nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			break;

		case FILE_ACCESS_WRITE:
			Handle = CreateFile(Filename, GENERIC_WRITE, NULL, 
				nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			break;

		case FILE_ACCESS_READWRITE:
			Handle = CreateFile(Filename, GENERIC_READ | GENERIC_WRITE, NULL, 
				nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			break;

		default:
			errno = EINVAL;
			break;
		}

		if (BiasStart != 0 || BiasLength != -1)
			Seek(0, SEEK_SET);

		if (Handle == NULL_HANDLE)
		{
			Error(GetLastError(), false, Filename);
			return false;
		}

		break;
	}

	return true;
}

int RawFileClass::Read(void* buffer, int size)
{
	int bytesread = 0;
	bool opened = false;

	if (!Is_Open())
	{
		if (!Open(FILE_ACCESS_READ))
			return 0;

		opened = true;
	}

	if (BiasLength != -1)
	{
		int remainder = BiasLength - Seek(0);
		if (size > remainder)
			size = remainder;
	}

	int total = 0;
	while (size > 0)
	{
		bytesread = 0;
		SetErrorMode(SEM_FAILCRITICALERRORS);
		if (!ReadFile(Handle, buffer, size, reinterpret_cast<LPDWORD>(&bytesread), nullptr))
		{
			size -= bytesread;
			total += bytesread;
			Error(GetLastError(), true, Filename);
			SetErrorMode(NULL);
			continue;
		}
		SetErrorMode(NULL);
		size -= bytesread;
		total += bytesread;
		if (bytesread == 0)
			break;
	}
	bytesread = total;

	if (opened)
		Close();

	return bytesread;
}

int RawFileClass::Seek(int pos, int dir)
{
	if (BiasLength != -1)
	{
		switch (dir)
		{
		case SEEK_SET:
			if (pos > BiasLength)
				pos = BiasLength;

			pos += BiasStart;
			break;

		case SEEK_CUR:
			break;

		case SEEK_END:
			dir = SEEK_SET;
			pos += BiasStart + BiasLength;
			break;
		}

		int newpos = Raw_Seek(pos, dir) - BiasStart;
		if (newpos < 0)
			newpos = Raw_Seek(BiasStart, SEEK_SET) - BiasStart;
		if (newpos > BiasLength)
			newpos = Raw_Seek(BiasStart + BiasLength, SEEK_SET) - BiasStart;

		return newpos;
	}

	return Raw_Seek(pos, dir);
}

int RawFileClass::Size()
{
	int size = 0;

	if (BiasLength != -1)
		return BiasLength;

	if (Is_Open())
	{
		size = GetFileSize(Handle, nullptr);

		if (size == 0xFFFFFFFF)
			Error(GetLastError(), false, Filename);
	}
	else
	{
		if (Open())
		{
			size = Size();
			Close();
		}
	}

	BiasLength = size - BiasLength;
	return BiasLength;
}

int RawFileClass::Write(void* const buffer, int size)
{
	int bytesread = 0;
	bool opened = false;

	if (!Is_Open())
	{
		if (!Open(FILE_ACCESS_WRITE))
			return 0;

		opened = true;
	}

	if (!WriteFile(Handle, buffer, size, reinterpret_cast<LPDWORD>(&bytesread), nullptr))
		Error(GetLastError(), false, Filename);

	if (BiasLength != -1)
	{
		if (Raw_Seek(0) > BiasStart + BiasLength)
			BiasLength = Raw_Seek(0) - BiasStart;
	}

	if (opened)
		Close();

	return bytesread;
}

void RawFileClass::Close()
{
	if (Is_Open())
	{
		if (!CloseHandle(Handle))
			Error(GetLastError(), false, Filename);

		Handle = NULL_HANDLE;
	}
}

unsigned int RawFileClass::Get_Date_Time()
{
	BY_HANDLE_FILE_INFORMATION info;
	
	if (GetFileInformationByHandle(Handle, &info))
	{
		WORD dosdate;
		WORD dostime;
		FileTimeToDosDateTime(&info.ftLastWriteTime, &dosdate, &dostime);
		return (dosdate << 16) | dostime;
	}

	return 0;
}

bool RawFileClass::Set_Date_Time(unsigned int dt)
{
	if (RawFileClass::Is_Open())
	{
		BY_HANDLE_FILE_INFORMATION info;

		if (GetFileInformationByHandle(Handle, &info))
		{
			FILETIME filetime;
			if (DosDateTimeToFileTime(HIWORD(dt), LOWORD(dt), &filetime))
				return SetFileTime(Handle, &info.ftCreationTime, &filetime, &filetime);
		}
	}

	return false;
}

void RawFileClass::Error(int error, bool canretry, const char* filename)
{
}

void RawFileClass::Bias(int start, int length)
{
	if (start == 0)
	{
		BiasStart = 0;
		BiasLength = -1;
		return;
	}

	BiasLength = RawFileClass::Size();
	BiasStart += start;
	if (length != -1 && length < BiasLength)
		BiasLength = length;

	if (BiasLength < 0)
		BiasLength = 0;

	if (Is_Open())
		RawFileClass::Seek(0, SEEK_SET);
}

int RawFileClass::Raw_Seek(int pos, int dir)
{
	if (!Is_Open())
		Error(EBADF, false, Filename);

	switch (dir)
	{
	case SEEK_SET:
		dir = FILE_BEGIN;
		break;

	case SEEK_CUR:
		dir = FILE_CURRENT;
		break;

	case SEEK_END:
		dir = FILE_END;
		break;
	}

	pos = SetFilePointer(Handle, pos, nullptr, dir);

	if (pos == 0xFFFFFFFF)
		Error(GetLastError(), false, Filename);

	return pos;
}


