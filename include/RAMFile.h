#pragma once

#include <Always.h>

#include <File.h>

class RAMFileClass : public FileClass
{
public:
	explicit RAMFileClass(void* buffer, int len) noexcept;

	CLASS_NOCOPY(RAMFileClass);
	CLASS_CANMOVE(RAMFileClass);

	virtual ~RAMFileClass() override;

	virtual const char* File_Name() const override { return "UNKNOWN"; }
	virtual const char* Set_Name(const char*) override { return File_Name(); }
	virtual bool Create() override;
	virtual bool Delete() override;
	virtual bool Is_Available(bool forced = false) override;
	virtual bool Is_Open() const override;
	virtual bool Open(const char* filename, ENUM_FILE_ACCESS access = FILE_ACCESS_READ) override;
	virtual bool Open(ENUM_FILE_ACCESS access = FILE_ACCESS_READ) override;
	virtual int Read(void* buffer, int size) override;
	virtual int Seek(int pos, int dir = SEEK_CUR) override;
	virtual int Size() override;
	virtual int Write(void* const buffer, int size) override;
	virtual void Close() override;
	virtual unsigned int Get_Date_Time() override { return 0; }
	virtual bool Set_Date_Time(unsigned int) override { return true; }
	virtual void Error(int, bool = false, const char* = nullptr) override {}

	operator const char*() { return File_Name(); }

private:
	char* Buffer;
	int MaxLength;
	int Length;
	int Offset;
	int Access;
	bool IsOpen;
	bool IsAllocated;
};