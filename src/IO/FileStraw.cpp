#include <FileStraw.h>

FileStraw::FileStraw(FileClass* file) noexcept
	: Straw{}
	, File{ file }
	, HasOpened{ false }
{
}

FileStraw::FileStraw(FileClass& file) noexcept
	: Straw{}
	, File{ &file }
	, HasOpened{ false }
{
}

FileStraw::~FileStraw()
{
	if (Valid_File() && HasOpened) 
	{
		File->Close();
		HasOpened = false;
		File = nullptr;
	}
}

int FileStraw::Get(void* buffer, int length)
{
	if (Valid_File() && buffer != nullptr && length > 0) 
	{
		if (!File->Is_Open()) 
		{
			HasOpened = true;
			if (!File->Is_Available()) 
				return 0;
			
			if (!File->Open(FILE_ACCESS_READ)) 
				return 0;
		}

		return File->Read(buffer, length);
	}
	return 0;
}
