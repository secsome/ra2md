#include <BufferIOFile.h>

BufferIOFileClass::BufferIOFileClass(const char* filename) noexcept
	: RawFileClass{ filename }
	, IsAllocated{ false }
	, IsOpen{ false }
	, IsDiskOpen{ false }
	, IsCached{ false }
	, IsChanged{ false }
	, UseBuffer{ false }
	, BufferRights{ FILE_ACCESS_NONE }
	, Buffer{ 0 }
	, BufferSize{ 0 }
	, BufferPos{ 0 }
	, BufferFilePos{ 0 }
	, BufferChangeBegin{ -1 }
	, BufferChangeEnd{ -1 }
	, FileSize{ 0 }
	, FilePos{ 0 }
	, TrueFileStart{ 0 }
{
	BufferIOFileClass::Set_Name(filename);
}

BufferIOFileClass::BufferIOFileClass() noexcept
	: RawFileClass{}
	, IsAllocated{ false }
	, IsOpen{ false }
	, IsDiskOpen{ false }
	, IsCached{ false }
	, IsChanged{ false }
	, UseBuffer{ false }
	, BufferRights{ FILE_ACCESS_NONE }
	, Buffer{ 0 }
	, BufferSize{ 0 }
	, BufferPos{ 0 }
	, BufferFilePos{ 0 }
	, BufferChangeBegin{ -1 }
	, BufferChangeEnd{ -1 }
	, FileSize{ 0 }
	, FilePos{ 0 }
	, TrueFileStart{ 0 }
{
}

BufferIOFileClass::~BufferIOFileClass()
{
	Free();
}

const char* BufferIOFileClass::Set_Name(const char* filename)
{
	if (File_Name() && UseBuffer) 
	{
		if (strcmp(filename, File_Name()) == 0) 
			return File_Name();
		else
		{
			Commit();
			IsCached = false;
		}
	}

	RawFileClass::Set_Name(filename);
	return File_Name();
}

bool BufferIOFileClass::Is_Available(bool forced)
{
	return UseBuffer ? true : RawFileClass::Is_Available();
}

bool BufferIOFileClass::Is_Open() const
{
	return IsOpen && UseBuffer ? true : RawFileClass::Is_Open();
}

bool BufferIOFileClass::Open(const char* filename, ENUM_FILE_ACCESS rights)
{
	Set_Name(filename);
	return BufferIOFileClass::Open(rights);
}

bool BufferIOFileClass::Open(ENUM_FILE_ACCESS rights)
{
	BufferIOFileClass::Close();

	if (UseBuffer) 
	{
		BufferRights = rights;
		if (rights != FILE_ACCESS_READ || (rights == FILE_ACCESS_READ && FileSize > BufferSize)) 
		{
			if (rights == FILE_ACCESS_WRITE) 
			{
				RawFileClass::Open(rights);
				RawFileClass::Close();
				rights = FILE_ACCESS_READWRITE;
				TrueFileStart = 0;
			}

			if (TrueFileStart)
			{
				UseBuffer = false;
				Open(rights);
				UseBuffer = true;
			}
			else
				return RawFileClass::Open(rights);

			IsDiskOpen = true;

			if (BufferRights == FILE_ACCESS_WRITE)
				FileSize = 0;
		}
		else
			IsDiskOpen = false;

		BufferPos = 0;
		BufferFilePos = 0;
		BufferChangeBegin = -1;
		BufferChangeEnd = -1;
		FilePos = 0;
		IsOpen = true;
	}
	else
		return RawFileClass::Open(rights);

	return true;
}

int BufferIOFileClass::Read(void* buffer, int size)
{
	bool opened = false;

	if (!Is_Open()) 
	{
		if (Open()) 
		{
			TrueFileStart = RawFileClass::Seek(0);
			opened = true;
		}
	}

	if (UseBuffer) 
	{
		int sizeread = 0;

		if (BufferRights != FILE_ACCESS_WRITE) 
		{
			while (size) 
			{
				int sizetoread = MIN(size, BufferSize - BufferPos);

				if (!IsCached) 
				{
					int readsize;

					if (FileSize < BufferSize) 
					{
						readsize = FileSize;
						BufferFilePos = 0;
					}
					else 
					{
						readsize = BufferSize;
						BufferFilePos = FilePos;
					}

					if (TrueFileStart) 
					{
						UseBuffer = false;
						Seek(FilePos, SEEK_SET);
						Read(Buffer, BufferSize);
						Seek(FilePos, SEEK_SET);
						UseBuffer = true;
					}
					else 
					{
						RawFileClass::Seek(BufferFilePos, SEEK_SET);
						RawFileClass::Read(Buffer, readsize);
					}

					BufferPos = 0;
					BufferChangeBegin = -1;
					BufferChangeEnd = -1;

					IsCached = true;
				}

				memmove((char*)buffer + sizeread, (char*)Buffer + BufferPos, sizetoread);

				sizeread += sizetoread;
				size -= sizetoread;
				BufferPos += sizetoread;
				FilePos = BufferFilePos + BufferPos;

				if (BufferPos == BufferSize) 
				{
					Commit();

					BufferPos = 0;
					BufferFilePos = FilePos;
					BufferChangeBegin = -1;
					BufferChangeEnd = -1;

					if (size && FileSize > FilePos) 
					{
						if (TrueFileStart) 
						{
							UseBuffer = false;
							Seek(FilePos, SEEK_SET);
							Read(Buffer, BufferSize);
							Seek(FilePos, SEEK_SET);
							UseBuffer = true;
						}
						else 
						{
							RawFileClass::Seek(FilePos, SEEK_SET);
							RawFileClass::Read(Buffer, BufferSize);
						}
					}
					else
						IsCached = false;
				}
			}
		}
		else
			Error(EACCES);

		size = sizeread;
	}
	else
		size = RawFileClass::Read(buffer, size);

	if (opened)
		Close();

	return size;
}

int BufferIOFileClass::Seek(int pos, int dir)
{
	if (UseBuffer) 
	{
		bool adjusted = false;

		switch (dir) 
		{
		case SEEK_END:
			FilePos = FileSize;
			break;

		case SEEK_SET:
			FilePos = 0;
			break;

		case SEEK_CUR:
		default:
			break;
		}

		if (TrueFileStart) 
		{
			if (pos >= TrueFileStart)
			{
				pos -= TrueFileStart;
				adjusted = true;
			}
		}

		FilePos += pos;

		FilePos = CLAMP(FilePos, 0, FileSize);

		if (FileSize <= BufferSize)
			BufferPos = FilePos;
		else 
		{
			if (FilePos >= BufferFilePos && FilePos < BufferFilePos + BufferSize)
				BufferPos = FilePos - BufferFilePos;
			else
			{
				Commit();
				
				if (TrueFileStart) 
				{
					UseBuffer = false;
					Seek(FilePos, SEEK_SET);
					UseBuffer = true;
				}
				else
					RawFileClass::Seek(FilePos, SEEK_SET);

				IsCached = false;
			}
		}

		if (TrueFileStart && adjusted)
			return FilePos + TrueFileStart;

		return FilePos;
	}

	return RawFileClass::Seek(pos, dir);
}

int BufferIOFileClass::Size()
{
	return IsOpen && UseBuffer ? FileSize : RawFileClass::Size();
}

int BufferIOFileClass::Write(void* const buffer, int size)
{
	bool opened = false;

	if (!Is_Open()) 
	{
		if (!Open(FILE_ACCESS_WRITE)) 
			return 0;

		TrueFileStart = RawFileClass::Seek(0);
		opened = true;
	}

	if (UseBuffer) 
	{
		int sizewritten = 0;

		if (BufferRights != FILE_ACCESS_READ) 
		{
			while (size) 
			{
				int sizetowrite = MIN(BufferSize - BufferPos, size);

				if (sizetowrite != BufferSize) 
				{
					if (!IsCached) 
					{
						int readsize;

						if (FileSize < BufferSize) 
						{
							readsize = FileSize;
							BufferFilePos = 0;
						}
						else 
						{
							readsize = BufferSize;
							BufferFilePos = FilePos;
						}

						if (TrueFileStart) 
						{
							UseBuffer = false;
							Seek(FilePos, SEEK_SET);
							Read(Buffer, BufferSize);
							Seek(FilePos, SEEK_SET);
							UseBuffer = true;
						}
						else 
						{
							RawFileClass::Seek(BufferFilePos, SEEK_SET);
							RawFileClass::Read(Buffer, readsize);
						}

						BufferPos = 0;
						BufferChangeBegin = -1;
						BufferChangeEnd = -1;

						IsCached = true;
					}
				}

				memmove((char*)Buffer + BufferPos, (char*)buffer + sizewritten, sizetowrite);

				IsChanged = true;
				sizewritten += sizetowrite;
				size -= sizetowrite;

				if (BufferChangeBegin == -1)
				{
					BufferChangeBegin = BufferPos;
					BufferChangeEnd = BufferPos;
				}
				else 
				{
					if (BufferChangeBegin > BufferPos)
						BufferChangeBegin = BufferPos;
				}

				BufferPos += sizetowrite;

				if (BufferChangeEnd < BufferPos)
					BufferChangeEnd = BufferPos;

				FilePos = BufferFilePos + BufferPos;

				if (FileSize < FilePos)
					FileSize = FilePos;

				if (BufferPos == BufferSize) 
				{
					Commit();

					BufferPos = 0;
					BufferFilePos = FilePos;
					BufferChangeBegin = -1;
					BufferChangeEnd = -1;

					if (size && FileSize > FilePos) 
					{
						if (TrueFileStart) 
						{
							UseBuffer = false;
							Seek(FilePos, SEEK_SET);
							Read(Buffer, BufferSize);
							Seek(FilePos, SEEK_SET);
							UseBuffer = true;
						}
						else 
						{
							RawFileClass::Seek(FilePos, SEEK_SET);
							RawFileClass::Read(Buffer, BufferSize);
						}
					}
					else
						IsCached = false;
				}
			}
		}
		else
			Error(EACCES);

		size = sizewritten;
	}
	else
		size = RawFileClass::Write(buffer, size);

	if (opened)
		Close();

	return size;
}

void BufferIOFileClass::Close()
{
	if (UseBuffer)
	{
		Commit();

		if (IsDiskOpen) 
		{
			if (TrueFileStart) 
			{
				UseBuffer = false;
				Close();
				UseBuffer = true;
			}
			else
				RawFileClass::Close();

			IsDiskOpen = false;
		}

		IsOpen = false;
	}
	else
		RawFileClass::Close();
}

bool BufferIOFileClass::Cache(int size, void* ptr)
{
	if (Buffer)
	{
		if (size || ptr)
			return false;
		else
			return true;
	}

	FileSize = Is_Available() ? Size() : 0;
	
	if (size)
	{
		if (size < MINIMUM_BUFFER_SIZE)
		{
			size = MINIMUM_BUFFER_SIZE;

			if (ptr)
				Error(EINVAL);
		}

		BufferSize = size;
	}
	else
		BufferSize = FileSize;

	Buffer = ptr ? ptr : new char[BufferSize];

	if (Buffer)
	{
		IsAllocated = true;
		IsDiskOpen = false;
		BufferPos = 0;
		BufferFilePos = 0;
		BufferChangeBegin = -1;
		BufferChangeEnd = -1;
		FilePos = 0;
		TrueFileStart = 0;

		if (FileSize)
		{
			int readsize;
			bool opened = false;
			int prevpos = 0;

			readsize = MIN(FileSize, BufferSize);

			if (Is_Open())
			{
				prevpos = Seek(0);

				TrueFileStart = RawFileClass::Is_Open() ? RawFileClass::Seek(0) : prevpos;

				if (FileSize <= BufferSize)
				{
					if (prevpos)
						Seek(0, SEEK_SET);

					BufferPos = prevpos;
				}
				else
					BufferFilePos = prevpos;
				
				FilePos = prevpos;
			}
			else
			{
				if (Open())
				{
					TrueFileStart = RawFileClass::Seek(0);
					opened = true;
				}
			}

			int actual = Read(Buffer, readsize);

			if (actual != readsize)
				Error(EIO);

			if (opened)
				Close();
			else
				Seek(prevpos, SEEK_SET);

			IsCached = true;
		}
		
		UseBuffer = true;
		return true;
	}

	Error(ENOMEM);
	return false;
}

void BufferIOFileClass::Free()
{
	if (Buffer) 
	{
		if (IsAllocated) 
		{
			delete[] Buffer;
			IsAllocated = false;
		}

		Buffer = nullptr;
	}

	BufferSize = 0;
	IsOpen = false;
	IsCached = false;
	IsChanged = false;
	UseBuffer = false;
}

bool BufferIOFileClass::Commit()
{
	if (UseBuffer) 
	{
		if (IsChanged)
		{
			if (IsDiskOpen)
			{
				RawFileClass::Seek(TrueFileStart + BufferFilePos + BufferChangeBegin, SEEK_SET);
				RawFileClass::Write(Buffer, BufferChangeEnd - BufferChangeBegin);
				RawFileClass::Seek(TrueFileStart + FilePos, SEEK_SET);
			}
			else
			{
				RawFileClass::Open();
				RawFileClass::Seek(TrueFileStart + BufferFilePos + BufferChangeBegin, SEEK_SET);
				RawFileClass::Write(Buffer, BufferChangeEnd - BufferChangeBegin);
				RawFileClass::Close();
			}

			IsChanged = false;
			return true;
		}
	}

	return false;
}