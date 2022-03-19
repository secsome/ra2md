#pragma once

#include <Always.h>

#include <BufferIOFile.h>
#include <Buffer.h>

// We don't need to search for CDs, so CDFileClass is abandoned.
// Therefore, CCFileClass just derived from BufferIOFileClass

class CCFileClass : public BufferIOFileClass
{
public:
	explicit CCFileClass(char const* filename) noexcept;
	explicit CCFileClass() noexcept;

	CLASS_NOCOPY(CCFileClass);
	CLASS_CANMOVE(CCFileClass);

	virtual ~CCFileClass() override { Position = 0; };

	virtual bool Is_Available(bool forced = false) override;
	virtual bool Is_Open() const override;
	virtual bool Open(char const* filename, ENUM_FILE_ACCESS rights = FILE_ACCESS_READ) override;
	virtual bool Open(ENUM_FILE_ACCESS rights = FILE_ACCESS_READ) override;
	virtual int Read(void* buffer, int size) override;
	virtual int Seek(int pos, int dir = SEEK_CUR) override;
	virtual int Size() override;
	virtual int Write(void* const buffer, int size) override;
	virtual void Close() override;
	virtual unsigned int Get_Date_Time() override;
	virtual bool Set_Date_Time(unsigned int datetime) override;
	virtual void Error(int error, bool canretry = false, char const* filename = nullptr) override;

	bool Is_Resident() const { return Data.Get_Buffer() != nullptr; }

private:
	::Buffer Data;
	int Position;
};