#include <CCINI.h>

#include <FileStraw.h>
#include <FilePipe.h>
#include <SHAPipe.h>

CCINIClass::CCINIClass() noexcept
	: INIClass{}
	, IsDigestPresent{ false }
	, Digest{ 0 }
{
}

CCINIClass::~CCINIClass()
{
}

bool CCINIClass::Load(FileClass& file, bool withdigest)
{
	FileStraw fstraw(file);
	return Load(fstraw, withdigest);
}

int CCINIClass::Load(Straw& file, bool withdigest)
{
	bool ok = INIClass::Load(file);

	Invalidate_Message_Digest();
	if (ok && withdigest)
	{
		unsigned char digest[20];
		int len = Get_UUBlock("Digest", digest, sizeof(digest));
		if (len > 0) 
		{
			Clear("Digest");
			Calculate_Message_Digest();
			if (memcmp(digest, Digest, sizeof(digest)) != 0)
				return 2;
		}
	}
	return ok;
}

int CCINIClass::Save(FileClass& file, bool withdigest) const
{
	FilePipe fpipe(file);
	return Save(fpipe, withdigest);
}

int CCINIClass::Save(Pipe& pipe, bool withdigest) const
{
	if (!withdigest)
		return INIClass::Save(pipe);

	((CCINIClass*)this)->Clear("Digest");
	((CCINIClass*)this)->Calculate_Message_Digest();
	((CCINIClass*)this)->Put_UUBlock("Digest", Digest, sizeof(Digest));
	int length = INIClass::Save(pipe);
	((CCINIClass*)this)->Clear("Digest");
	return length;
}

void CCINIClass::Calculate_Message_Digest()
{
	SHAPipe sha;
	INIClass::Save(sha);
	sha.Result(Digest);
	IsDigestPresent = true;
}

void CCINIClass::Invalidate_Message_Digest()
{
	IsDigestPresent = false;
}