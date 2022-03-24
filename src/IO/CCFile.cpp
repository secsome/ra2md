#include <CCFile.h>

#include <MixFile.h>

CCFileClass::CCFileClass(char const* filename) noexcept
	: BufferIOFileClass{ filename }
	, Position{ 0 }
{
	CCFileClass::Set_Name(filename);
}

CCFileClass::CCFileClass() noexcept
	: BufferIOFileClass{}
	, Position{ 0 }
{
}

bool CCFileClass::Is_Available(bool forced)
{
	return Is_Open() || MFCD::Offset(File_Name()) ? true : BufferIOFileClass::Is_Available();
}

bool CCFileClass::Is_Open() const
{
	return Is_Resident() ? true : BufferIOFileClass::Is_Open();
}

bool CCFileClass::Open(char const* filename, ENUM_FILE_ACCESS rights)
{
	Set_Name(filename);
	return Open(rights);
}

bool CCFileClass::Open(ENUM_FILE_ACCESS rights)
{
	Close();

	if ((rights & FILE_ACCESS_WRITE) || BufferIOFileClass::Is_Available())
		return BufferIOFileClass::Open(rights);

	MFCD* mixfile = NULL;
	void* pointer = NULL;
	int length = 0;
	int start = 0;
	if (MFCD::Offset(File_Name(), &pointer, &mixfile, &start, &length))
	{
		if (pointer == nullptr && mixfile != nullptr)
		{
			char* dupfile = _strdup(File_Name());
			Open(mixfile->Filename, FILE_ACCESS_READ);
			Set_Name(dupfile);
			free(dupfile);
			Bias(0);
			Bias(start, length);
			Seek(0, SEEK_SET);
		}
		else
		{
			Data.Reset(pointer, length);
			Position = 0;
		}

	}
	else
		return BufferIOFileClass::Open(rights);
	
	return true;
}

int CCFileClass::Read(void* buffer, int size)
{
	bool opened = false;

	if (!Is_Open()) 
	{
		if (Open())
			opened = true;
	}

	if (Is_Resident())
	{
		int maximum = Data.Get_Size() - Position;
		size = MIN(maximum, size);

		if (size) 
		{
			memmove(buffer, (char*)Data + Position, size);
			Position += size;
		}

		if (opened) 
			Close();

		return size;
	}

	int s = BufferIOFileClass::Read(buffer, size);

	if (opened) 
		Close();

	return s;
}

int CCFileClass::Seek(int pos, int dir)
{
	if (Is_Resident()) 
	{
		switch (dir) 
		{
		case SEEK_END:
			Position = Data.Get_Size();
			break;

		case SEEK_SET:
			Position = 0;
			break;

		case SEEK_CUR:
		default:
			break;
		}

		return CLAMP(Position + pos, 0, Data.Get_Size());
	}
	return BufferIOFileClass::Seek(pos, dir);
}

int CCFileClass::Size()
{
	if (Is_Resident()) 
		return(Data.Get_Size());

	if (!BufferIOFileClass::Is_Available())
	{
		int length = 0;
		MFCD::Offset(File_Name(), nullptr, nullptr, nullptr, &length);
		return length;
	}

	return BufferIOFileClass::Size();
}

int CCFileClass::Write(void* const buffer, int size)
{
	if (Is_Resident())
		Error(EACCES, false, File_Name());

	return BufferIOFileClass::Write(buffer, size);
}

void CCFileClass::Close()
{
	Data.Reset();
	Position = 0;
	BufferIOFileClass::Close();
}

unsigned int CCFileClass::Get_Date_Time()
{
	unsigned int datetime;
	MFCD* mixfile;

	datetime = BufferIOFileClass::Get_Date_Time();

	if (!datetime) 
	{
		if (MFCD::Offset(File_Name(), nullptr, &mixfile, nullptr, nullptr))
			return CCFileClass(mixfile->Filename).Get_Date_Time();
	}

	return datetime;
}

bool CCFileClass::Set_Date_Time(unsigned int datetime)
{
	bool status;
	MFCD* mixfile;

	status = BufferIOFileClass::Set_Date_Time(datetime);

	if (!status)
	{
		if (MFCD::Offset(File_Name(), nullptr, &mixfile, nullptr, nullptr))
			return CCFileClass(mixfile->Filename).Set_Date_Time(datetime);
	}

	return(status);
}

void CCFileClass::Error(int error, bool canretry, char const* filename)
{
}
