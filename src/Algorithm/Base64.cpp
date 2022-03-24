#include <Base64.h>

static char const* const _pad = "=";
static char const* const _encoder = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define	BAD	0xFE
#define	END	0xFF
static unsigned char const _decoder[256] = 
{
	BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,
	BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,
	BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,62,BAD,BAD,BAD,63,
	52,53,54,55,56,57,58,59,60,61,BAD,BAD,BAD,END,BAD,BAD,
	BAD,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,
	15,16,17,18,19,20,21,22,23,24,25,BAD,BAD,BAD,BAD,BAD,
	BAD,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
	41,42,43,44,45,46,47,48,49,50,51,BAD,BAD,BAD,BAD,BAD,
	BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,
	BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,
	BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,
	BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,
	BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,
	BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,
	BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,
	BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD,BAD
};

int const PacketChars = 4;

union PacketType
{
	struct
	{
		unsigned char C3;
		unsigned char C2;
		unsigned char C1;
		unsigned char pad;
	} Char;
	struct 
	{
		unsigned O4 : 6;
		unsigned O3 : 6;
		unsigned O2 : 6;
		unsigned O1 : 6;
		unsigned pad : 8;
	} SubCode;
	unsigned int Raw;
};

int Base64_Encode(const void* source, int slen, void* dest, int dlen)
{
	if (source == nullptr || slen == 0 || dest == nullptr || dlen == 0)
		return 0;

	int total = 0;
	unsigned char const* sptr = (unsigned char const*)source;
	unsigned char* dptr = (unsigned char*)dest;
	while (slen > 0 && dlen >= PacketChars) 
	{
		PacketType packet;

		int pad = 0;
		packet.Raw = 0;
		packet.Char.C1 = *sptr++;
		slen--;
		if (slen) 
		{
			packet.Char.C2 = *sptr++;
			slen--;
		}
		else
			pad++;
		if (slen) 
		{
			packet.Char.C3 = *sptr++;
			slen--;
		}
		else
			pad++;

		*dptr++ = _encoder[packet.SubCode.O1];
		*dptr++ = _encoder[packet.SubCode.O2];
		if (pad < 2)
			*dptr++ = _encoder[packet.SubCode.O3];
		else
			*dptr++ = _pad[0];
		if (pad < 1)
			*dptr++ = _encoder[packet.SubCode.O4];
		else
			*dptr++ = _pad[0];

		dlen -= PacketChars;
		total += PacketChars;
	}

	if (dlen > 0)
		*dptr = '\0';

	return total;
}

int Base64_Decode(void const* source, int slen, void* dest, int dlen)
{
	if (source == nullptr || slen == 0 || dest == nullptr || dlen == 0)
		return 0;

	int total = 0;
	unsigned char const* sptr = (unsigned char const*)source;
	unsigned char* dptr = (unsigned char*)dest;
	while (slen > 0 && dlen > 0)
	{

		PacketType packet;
		packet.Raw = 0;

		int pcount = 0;
		while (pcount < PacketChars && slen > 0)
		{
			unsigned char c = *sptr++;
			--slen;

			unsigned char code = _decoder[c];

			if (code == BAD) 
				continue;

			if (code == END) 
			{
				slen = 0;
				break;
			}

			switch (pcount)
			{
			case 0:
				packet.SubCode.O1 = code;
				break;
			case 1:
				packet.SubCode.O2 = code;
				break;
			case 2:
				packet.SubCode.O3 = code;
				break;
			case 3:
				packet.SubCode.O4 = code;
				break;
			}
			++pcount;
		}

		*dptr++ = packet.Char.C1;
		--dlen;
		++total;
		if (dlen > 0 && pcount > 2) 
		{
			*dptr++ = packet.Char.C2;
			--dlen;
			++total;
		}
		if (dlen > 0 && pcount > 3) 
		{
			*dptr++ = packet.Char.C3;
			--dlen;
			++total;
		}
	}

	return total;
}
