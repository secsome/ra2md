#pragma once

#include <Always.h>

#include <RawFile.h>

class BufferIOFileClass : public RawFileClass
{
public:
	explicit BufferIOFileClass(const char * filename) noexcept;
	explicit BufferIOFileClass() noexcept;

	CLASS_NOCOPY(BufferIOFileClass);
	CLASS_CANMOVE(BufferIOFileClass);

	virtual ~BufferIOFileClass() override;

	virtual const char * Set_Name(const char * filename) override;
	virtual bool Is_Available(bool forced = false) override;
	virtual bool Is_Open() const override;
	virtual bool Open(const char* filename, ENUM_FILE_ACCESS rights = FILE_ACCESS_READ) override;
	virtual bool Open(ENUM_FILE_ACCESS rights = FILE_ACCESS_READ) override;
	virtual int Read(void* buffer, int size) override;
	virtual int Seek(int pos, int dir = SEEK_CUR) override;
	virtual int Size() override;
	virtual int Write(void* const buffer, int size) override;
	virtual void Close() override;

	bool Cache(int size = 0, void* ptr = nullptr);
	void Free();
	bool Commit();

	static constexpr int MINIMUM_BUFFER_SIZE = 1024;

private:
	bool IsAllocated;
	bool IsOpen;
	bool IsDiskOpen;
	bool IsCached;
	bool IsChanged;
	bool UseBuffer;
	ENUM_FILE_ACCESS BufferRights;
	void* Buffer;
	int BufferSize;
	int BufferPos;
	int BufferFilePos;
	int BufferChangeBegin;
	int BufferChangeEnd;
	int FileSize;
	int FilePos;
	int TrueFileStart;
};