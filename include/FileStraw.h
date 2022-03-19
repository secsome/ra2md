#pragma once

#include <Always.h>

#include <Straw.h>
#include <File.h>

class FileStraw : public Straw
{
public:
	explicit FileStraw(FileClass* file) noexcept;
	explicit FileStraw(FileClass& file) noexcept;

	CLASS_NOCOPY(FileStraw);
	CLASS_CANMOVE(FileStraw);

	virtual ~FileStraw() override;

	virtual int Get(void* buffer, int length) override;

private:
	bool Valid_File() { return File != nullptr; }

private:
	FileClass* File;
	bool HasOpened;
};