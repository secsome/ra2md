#include "FilePipe.h"

FilePipe::FilePipe(FileClass* file) noexcept
	: Pipe{}
	, File{ file }
	, HasOpened{ false }
{
}

FilePipe::FilePipe(FileClass& file) noexcept
	: Pipe{}
	, File{ &file }
	, HasOpened{ false }
{
}

FilePipe::~FilePipe()
{
	if (Valid_File() && HasOpened) 
	{
		HasOpened = false;
		File->Close();
		File = nullptr;
	}
}

int FilePipe::Put(const void* source, int slen)
{
	if (Valid_File() && source != nullptr && slen > 0) 
	{
		if (!File->Is_Open()) 
		{
			HasOpened = true;
			File->Open(FILE_ACCESS_WRITE);
		}

		return File->Write((void*)source, slen);
	}
	return 0;
}

int FilePipe::End()
{
	int total = Pipe::End();
	if (Valid_File() && HasOpened)
	{
		HasOpened = false;
		File->Close();
	}
	return total;
}
