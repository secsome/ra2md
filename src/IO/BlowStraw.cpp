#include <BlowStraw.h>

BlowStraw::BlowStraw(CryptControl control) noexcept
	: Straw{}
	, BF{ nullptr }
	, Counter{ 0 }
	, Control{ control }
	, Buffer{ 0 }
{
}

BlowStraw::~BlowStraw()
{
	delete BF;
	BF = nullptr;
}

int BlowStraw::Get(void* buffer, int length)
{
	if (buffer == nullptr || length <= 0)
		return 0;

	if (BF == nullptr)
		return Straw::Get(buffer, length);

	int total = 0;

	while (length > 0) 
	{
		if (Counter > 0)
		{
			int sublen = MIN(length, Counter);
			memmove(buffer, &Buffer[sizeof(Buffer) - Counter], sublen);
			Counter -= sublen;
			buffer = ((char*)buffer) + sublen;
			length -= sublen;
			total += sublen;
		}
		if (length == 0) 
			break;

		int incount = Straw::Get(Buffer, sizeof(Buffer));
		if (incount == 0) 
			break;

		if (incount == sizeof(Buffer))
		{
			if (Control == DECRYPT)
				BF->Decrypt(Buffer, incount, Buffer);
			else
				BF->Encrypt(Buffer, incount, Buffer);
		}
		else
			memmove(&Buffer[sizeof(Buffer) - incount], Buffer, incount);

		Counter = incount;
	}

	return total;
}

void BlowStraw::Key(void const* key, int length)
{
	if (BF == nullptr)
		BF = new BlowfishEngine;

	if (BF != nullptr)
		BF->Submit_Key(key, length);
}