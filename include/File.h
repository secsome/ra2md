#pragma once

#include <Always.h>

#define HANDLE_TYPE HANDLE
#define NULL_HANDLE INVALID_HANDLE_VALUE

#define YEAR(dt)	(((dt & 0xFE000000) >> (9 + 16)) + 1980)
#define MONTH(dt)	 ((dt & 0x01E00000) >> (5 + 16))
#define DAY(dt)	 ((dt & 0x001F0000) >> (0 + 16))
#define HOUR(dt)	 ((dt & 0x0000F800) >> 11)
#define MINUTE(dt) ((dt & 0x000007E0) >> 5)
#define SECOND(dt) ((dt & 0x0000001F) << 1)

enum ENUM_FILE_ACCESS
{
	FILE_ACCESS_NONE = 0,
	FILE_ACCESS_READ = 1,
	FILE_ACCESS_WRITE = 2,
	FILE_ACCESS_READWRITE = FILE_ACCESS_READ | FILE_ACCESS_WRITE
};

class FileClass
{
public:
	virtual ~FileClass() = default;
	virtual const char* File_Name() const = 0;
	virtual const char* Set_Name(const char* filename) = 0;
	virtual bool Create() = 0;
	virtual bool Delete() = 0;
	virtual bool Is_Available(bool forced = false) = 0;
	virtual bool Is_Open() const = 0;
	virtual bool Open(const char* filename, ENUM_FILE_ACCESS rights = FILE_ACCESS_READ) = 0;
	virtual bool Open(ENUM_FILE_ACCESS rights = FILE_ACCESS_READ) = 0;
	virtual int Read(void* buffer, int size) = 0;
	virtual int Seek(int pos, int dir = SEEK_CUR) = 0;
	virtual int Size() = 0;
	virtual int Write(void* const buffer, int size) = 0;
	virtual void Close() = 0;
	virtual unsigned int Get_Date_Time() { return 0; }
	virtual bool Set_Date_Time(unsigned int dt) { return false; }
	virtual void Error(int error, bool canretry = false, const char* filename = nullptr) = 0;

	operator const char* () { return File_Name(); }
};