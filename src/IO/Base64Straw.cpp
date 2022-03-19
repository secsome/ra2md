#include <Base64Straw.h>

#include <Base64.h>

Base64Straw::Base64Straw(CodeControl control) noexcept
	: Straw{}
	, Control{ control }
	, Counter{ 0 }
	, CBuffer{ 0 }
	, PBuffer{ 0 }
{
}

Base64Straw::~Base64Straw()
{
}

int Base64Straw::Get(void* buffer, int length)
{
	int total = 0;

	char* from;
	int fromsize;
	char* to;
	int tosize;

	if (Control == ENCODE) 
	{
		from = PBuffer;
		fromsize = sizeof(PBuffer);
		to = CBuffer;
		tosize = sizeof(CBuffer);
	}
	else 
	{
		from = CBuffer;
		fromsize = sizeof(CBuffer);
		to = PBuffer;
		tosize = sizeof(PBuffer);
	}
	
	while (length > 0)
	{
		if (Counter > 0) 
		{
			int len = MIN(length, Counter);
			memmove(buffer, &to[tosize - Counter], len);
			Counter -= len;
			length -= len;
			buffer = ((char*)buffer) + len;
			total += len;
		}
		if (length == 0) 
			break;

		int incount = Straw::Get(from, fromsize);
		if (Control == ENCODE)
			Counter = Base64_Encode(from, incount, to, tosize);
		else
			Counter = Base64_Decode(from, incount, to, tosize);

		if (Counter == 0)
			break;
	}

	return total;
}