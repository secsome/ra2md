#include <TextManager.h>

#include <Conquer.h>

#include <CCFile.h>

TextManagerClass TextManagerClass::StringTable;

bool TextManagerClass::Load(const char* file)
{
	CCFileClass ccfile;

	struct CSFHeader
	{
		int ID;
		int Version;
		int LabelCount;
		int StringCount;
		int Reversed;
		int Language;
	} header;

	if (ccfile.Open(file))
	{
		if (ccfile.Read(&header, sizeof(header)) == sizeof(header))
		{
			if (header.ID == 'CSF ')
			{
				for (int i = 0; i < header.LabelCount; ++i)
				{
					std::string label;
					std::wstring value;

					int lbl;
					ccfile.Read(&lbl, sizeof(lbl));
					if (lbl == 'LBL ')
					{
						int numPairs;
						int lblLength;

						ccfile.Read(&numPairs, sizeof(numPairs));
						ccfile.Read(&lblLength, sizeof(lblLength));
						label.resize(lblLength);
						ccfile.Read(&label[0], lblLength);

						int strw;
						int strLength;
						ccfile.Read(&strw, sizeof(strw));
						if (strw == 'STR ' || strw == 'STRW')
						{
							ccfile.Read(&strLength, sizeof(strLength));
							value.resize(strLength);
							ccfile.Read(&value[0], strLength * 2);
							for (char* p = (char*)&value[0], *e = (char*)&value[strLength]; p != e; ++p)
								*p = ~*p;
							
							if (strw == 'STRW')
							{
								int extraLength;
								ccfile.Read(&extraLength, sizeof(extraLength));
								ccfile.Seek(extraLength);
							}

							Texts[label] = value;
						}

						for (int i = 1; i < numPairs; ++i)
						{
							ccfile.Read(&strw, sizeof(strw));
							ccfile.Read(&strLength, sizeof(strLength));
							ccfile.Seek(strLength * 2);
							if (strw == 'STRW')
							{
								int extraLength;
								ccfile.Read(&extraLength, sizeof(extraLength));
								ccfile.Seek(extraLength);
							}
						}
					}
				}

				return true;
			}
		}
	}

	return false;
}

void TextManagerClass::Clear()
{
	Texts.clear();
}

const wchar_t* TextManagerClass::Fetch(const char* label, const char* file, int line)
{
	auto itr = Texts.find(label);
	if (itr == Texts.end())
	{
		wchar_t buffer[0x104];
		swprintf(buffer, L"MISSING:%hs", label);
		itr = Texts.emplace_hint(itr, label, buffer);
		Log_String("***NO_STRING*** '%s' -> file:%s line:%d\n", label, file, line);
	}
	return itr->second.c_str();
}

const wchar_t* TextManagerClass::operator[](const char* label)
{
	return Fetch(label);
}
