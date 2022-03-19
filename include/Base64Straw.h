#pragma once

#include <Always.h>

#include <Straw.h>
#include <Base64.h>

class Base64Straw : public Straw
{
public:
	enum CodeControl 
	{
		ENCODE,
		DECODE
	};

	explicit Base64Straw(CodeControl control) noexcept;

	CLASS_NOCOPY(Base64Straw);
	CLASS_CANMOVE(Base64Straw);

	virtual ~Base64Straw() override;

	virtual int Get(void* buffer, int length) override;

private:
	CodeControl Control;
	int Counter;
	char CBuffer[4];
	char PBuffer[3];
};