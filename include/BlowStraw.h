#pragma once

#include <Always.h>

#include <Straw.h>
#include <Blowfish.h>

class BlowStraw : public Straw
{
public:
	enum CryptControl 
	{
		ENCRYPT,
		DECRYPT
	};

	explicit BlowStraw(CryptControl control) noexcept;

	CLASS_NOCOPY(BlowStraw);
	CLASS_CANMOVE(BlowStraw);

	virtual ~BlowStraw() override;

	virtual int Get(void* buffer, int length) override;

	void Key(void const* key, int length);

protected:
	BlowfishEngine* BF;

private:
	char Buffer[8];
	int Counter;
	CryptControl Control;
};