#pragma once

#include <Always.h>

#include <Pipe.h>
#include <File.h>

class FilePipe : public Pipe
{
public:
	explicit FilePipe(FileClass* file) noexcept;
	explicit FilePipe(FileClass& file) noexcept;

	CLASS_NOCOPY(FilePipe);
	CLASS_CANMOVE(FilePipe);

	virtual ~FilePipe() override;

	virtual int Put(const void* source, int slen) override;
	virtual int End() override;

private:
	bool Valid_File() { return File != nullptr; }

private:
	FileClass* File;
	bool HasOpened;
};