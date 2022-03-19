#pragma once

#include <Always.h>

#include <Pipe.h>

class Base64Pipe : public Pipe
{
public:
	enum CodeControl 
	{
		ENCODE,
		DECODE
	};

	Base64Pipe(CodeControl control);

	CLASS_NOCOPY(Base64Pipe);
	CLASS_CANMOVE(Base64Pipe);

	virtual ~Base64Pipe() override;

	virtual int Flush() override;
	virtual int Put(const void* source, int slen) override;

private:
	CodeControl Control;
	int Counter;
	char CBuffer[4];
	char PBuffer[3];
};