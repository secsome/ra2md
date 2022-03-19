#include <Base64Pipe.h>

#include <Base64.h>

Base64Pipe::Base64Pipe(CodeControl control)
	: Pipe{}
	, Control{ control }
	, Counter{ 0 }
	, CBuffer{ 0 }
	, PBuffer{ 0 }
{
}

Base64Pipe::~Base64Pipe()
{
}

int Base64Pipe::Flush()
{
	int len = 0;

	if (Counter) 
	{
		if (Control == ENCODE) 
		{
			int chars = Base64_Encode(PBuffer, Counter, CBuffer, sizeof(CBuffer));
			len += Pipe::Put(CBuffer, chars);
		}
		else 
		{
			int chars = Base64_Decode(CBuffer, Counter, PBuffer, sizeof(PBuffer));
			len += Pipe::Put(PBuffer, chars);
		}
		Counter = 0;
	}
	len += Pipe::Flush();
	return len;
}

int Base64Pipe::Put(const void* source, int slen)
{
	if (source == nullptr || slen < 1)
		return Pipe::Put(source, slen);

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

	if (Counter > 0) 
	{
		int len = (slen < (fromsize - Counter)) ? slen : (fromsize - Counter);
		memmove(&from[Counter], source, len);
		Counter += len;
		slen -= len;
		source = ((char*)source) + len;

		if (Counter == fromsize) 
		{
			int outcount;
			if (Control == ENCODE)
				outcount = Base64_Encode(from, fromsize, to, tosize);
			else
				outcount = Base64_Decode(from, fromsize, to, tosize);

			total += Pipe::Put(to, outcount);
			Counter = 0;
		}
	}

	while (slen >= fromsize) 
	{
		int outcount;
		if (Control == ENCODE)
			outcount = Base64_Encode(source, fromsize, to, tosize);
		else
			outcount = Base64_Decode(source, fromsize, to, tosize);

		source = ((char*)source) + fromsize;
		total += Pipe::Put(to, outcount);
		slen -= fromsize;
	}

	if (slen > 0) 
	{
		memmove(from, source, slen);
		Counter = slen;
	}

	return total;
}