#pragma once

#include <Always.h>

#include <INI.h>
#include <PKey.h>

class CCINIClass : public INIClass
{
public:
	explicit CCINIClass() noexcept;

	CLASS_NOCOPY(CCINIClass);
	CLASS_NOMOVE(CCINIClass);

	virtual ~CCINIClass() override;

	bool Load(FileClass& file, bool withdigest);
	int Load(Straw& file, bool withdigest);
	int Save(FileClass& file, bool withdigest) const;
	int Save(Pipe& pipe, bool withdigest) const;

private:
	void Calculate_Message_Digest();
	void Invalidate_Message_Digest();

	bool IsDigestPresent;
	unsigned char Digest[20];
};