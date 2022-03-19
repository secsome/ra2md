#include <PKStraw.h>

PKStraw::PKStraw(CryptControl control, RandomStraw& rnd) noexcept
	: Straw{}
	, IsGettingKey{ true }
	, Rand{ rnd }
	, BF{ (control == ENCRYPT) ? BlowStraw::ENCRYPT : BlowStraw::DECRYPT }
	, Control{ control }
	, CipherKey{ nullptr }
	, Counter{ 0 }
	, BytesLeft{ 0 }
	, Buffer{ 0 }
{
	Straw::Get_From(BF);
}

PKStraw::~PKStraw()
{
}

void PKStraw::Get_From(Straw* straw)
{
	if (BF.ChainTo != straw) 
	{
		if (straw != nullptr && straw->ChainFrom != nullptr) 
		{
			straw->ChainFrom->Get_From(nullptr);
			straw->ChainFrom = nullptr;
		}

		if (BF.ChainTo != nullptr)
			BF.ChainTo->ChainFrom = nullptr;

		BF.ChainTo = straw;
		BF.ChainFrom = this;
		ChainTo = &BF;
		if (BF.ChainTo != nullptr)
			BF.ChainTo->ChainFrom = this;
	}
}

int PKStraw::Get(void* buffer, int length)
{
	if (buffer == nullptr || length < 1 || CipherKey == nullptr)
		return Straw::Get(buffer, length);

	int total = 0;

	if (IsGettingKey) 
	{
		if (Control == DECRYPT) 
		{
			char cbuffer[MAX_KEY_BLOCK_SIZE];
			int got = Straw::Get(cbuffer, Encrypted_Key_Length());

			if (got != Encrypted_Key_Length()) 
				return 0;

			CipherKey->Decrypt(cbuffer, got, Buffer);
			BF.Key(Buffer, BLOWFISH_KEY_SIZE);
		}
		else 
		{
			char buffer[MAX_KEY_BLOCK_SIZE];
			memset(buffer, 0, sizeof(buffer));
			Rand.Get(buffer, BLOWFISH_KEY_SIZE);

			Counter = BytesLeft = CipherKey->Encrypt(buffer, Plain_Key_Length(), Buffer);
			BF.Key(buffer, BLOWFISH_KEY_SIZE);
		}

		IsGettingKey = false;
	}

	if (BytesLeft > 0) {
		int tocopy = (length < BytesLeft) ? length : BytesLeft;
		memmove(buffer, &Buffer[Counter - BytesLeft], tocopy);
		buffer = (char*)buffer + tocopy;
		BytesLeft -= tocopy;
		length -= tocopy;
		total += tocopy;
	}

	total += Straw::Get(buffer, length);

	return total;
}

void PKStraw::Key(const PKey* key)
{
	CipherKey = key;
	if (key != nullptr)
		IsGettingKey = true;

	Counter = 0;
	BytesLeft = 0;
}

int PKStraw::Encrypted_Key_Length() const
{
	return CipherKey == nullptr ? 0 : CipherKey->Block_Count(BLOWFISH_KEY_SIZE) * CipherKey->Crypt_Block_Size();
}

int PKStraw::Plain_Key_Length() const
{
	return CipherKey == nullptr ? 0 : CipherKey->Block_Count(BLOWFISH_KEY_SIZE) * CipherKey->Plain_Block_Size();
}
