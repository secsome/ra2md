#pragma once

#include <Always.h>

#include <Straw.h>
#include <RandomStraw.h>
#include <BlowStraw.h>
#include <PKey.h>

class PKStraw : public Straw
{
public:
	enum CryptControl 
	{
		ENCRYPT,
		DECRYPT
	} ;

	explicit PKStraw(CryptControl control, RandomStraw& rnd) noexcept;

	CLASS_NOCOPY(PKStraw);
	CLASS_CANMOVE(PKStraw);

	virtual ~PKStraw() override;

	virtual void Get_From(Straw* straw) override;
	virtual int Get(void* buffer, int length) override;
	virtual void Get_From(Straw& straw) { Get_From(&straw); }

	void Key(const PKey* key);

private:
	int Encrypted_Key_Length() const;
	int Plain_Key_Length() const;

private:
	enum 
	{
		BLOWFISH_KEY_SIZE = BlowfishEngine::MAX_KEY_LENGTH,
		MAX_KEY_BLOCK_SIZE = 256
	};

	bool IsGettingKey;
	RandomStraw& Rand;
	BlowStraw BF;
	CryptControl Control;
	const PKey* CipherKey;
	char Buffer[256];
	int Counter;
	int BytesLeft;
};