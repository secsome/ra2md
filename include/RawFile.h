#pragma once

#include <Always.h>

#include <File.h>

class RawFileClass : public FileClass
{
public:
	explicit RawFileClass(const char* filename) noexcept;
	explicit RawFileClass() noexcept;

	CLASS_NOCOPY(RawFileClass);
	CLASS_CANMOVE(RawFileClass);

	virtual ~RawFileClass() override;

	virtual const char* File_Name() const override;
	virtual const char* Set_Name(const char* filename) override;
	virtual bool Create() override;
	virtual bool Delete() override;
	virtual bool Is_Available(bool forced = false) override;
	virtual bool Is_Open() const override;
	virtual bool Open(const char* filename, ENUM_FILE_ACCESS rights = FILE_ACCESS_READ) override;
	virtual bool Open(ENUM_FILE_ACCESS rights = FILE_ACCESS_READ) override;
	virtual int Read(void* buffer, int size) override;
	virtual int Seek(int pos, int dir = SEEK_CUR) override;
	virtual int Size() override;
	virtual int Write(void* const buffer, int size) override;
	virtual void Close() override;
	virtual unsigned int Get_Date_Time() override;
	virtual bool Set_Date_Time(unsigned int dt) override;
	virtual void Error(int error, bool canretry = false, const char* filename = nullptr) override;

	void Bias(int start, int length = -1);

	HANDLE_TYPE Get_File_Handle() { return Handle; }

protected:
	int Transfer_Block_Size() { return static_cast<int>(UINT_MAX - 16L); }

	int Raw_Seek(int pos, int dir = SEEK_CUR);

private:

public:
	ENUM_FILE_ACCESS Rights;
	int BiasStart;
	int BiasLength;

protected:
	
private:
	HANDLE_TYPE Handle;

	const char* Filename;

	struct
	{
		unsigned short Day : 5;		// [0, 31]
		unsigned short Month : 4;	// [1, 12]
		unsigned short Year : 7;	// [0, 119], representing [1980, 2099]
	} Date;
	struct
	{
		unsigned short Second : 5;	// [0, 29], second / 2
		unsigned short Minutes : 6;	// [0, 59]
		unsigned short Hours : 5;	// [0, 23]
	} Time;

	/*
	**	Filenames that were assigned as part of the construction process
	**	are not allocated. It is assumed that the filename string is a
	**	constant in that case and thus making duplication unnecessary.
	**	This value will be non-zero if the filename has be allocated
	**	(using strdup()).
	*/
	bool Allocated;
};