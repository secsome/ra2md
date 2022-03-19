#include <INI.h>

#include <FileStraw.h>
#include <FilePipe.h>
#include <CacheStraw.h>
#include <ReadLine.h>
#include <BufferPipe.h>
#include <Base64Pipe.h>
#include <BufferStraw.h>
#include <Base64Straw.h>

INIClass::INIClass() noexcept
{
}

INIClass::~INIClass()
{
	Clear();
}

bool INIClass::Load(FileClass& file)
{
	FileStraw fstraw(file);
	return Load(fstraw);
}

bool INIClass::Load(Straw& straw)
{
	bool end_of_file = false;
	char buffer[MAX_LINE_LENGTH];

	CacheStraw cstraw;
	cstraw.Get_From(straw);

	while (!end_of_file) 
	{
		Read_Line(cstraw, buffer, sizeof(buffer), end_of_file);
		if (end_of_file) 
			return false;

		if (buffer[0] == '[' && strchr(buffer, ']') != nullptr) 
			break;
	}

	while (!end_of_file) 
	{

		buffer[0] = ' ';
		char* ptr = strchr(buffer, ']');
		if (ptr) *ptr = '\0';
		strtrimcpp(buffer);
		INISection* secptr = new INISection(strdup(buffer));
		if (secptr == nullptr) 
		{
			Clear();
			return false;
		}

		while (!end_of_file) 
		{
			int len = Read_Line(cstraw, buffer, sizeof(buffer), end_of_file);
			if (buffer[0] == '[' && strchr(buffer, ']') != nullptr) 
				break;

			Strip_Comments(buffer);
			if (len == 0 || buffer[0] == ';' || buffer[0] == '=')
				continue;

			char* divider = strchr(buffer, '=');
			if (!divider)
				continue;

			*divider++ = '\0';
			strtrimcpp(buffer);
			if (!strlen(buffer))
				continue;

			strtrimcpp(divider);
			if (!strlen(divider)) 
				continue;

			INIEntry* entryptr = new INIEntry(strdup(buffer), strdup(divider));
			if (entryptr == nullptr) 
			{
				delete secptr;
				Clear();
				return false;
			}

			secptr->EntryIndex.Add_Index(entryptr->Index_ID(), entryptr);
			secptr->EntryList.Add_Tail(entryptr);
		}

		if (secptr->EntryList.Is_Empty())
			delete secptr;

		else 
		{
			SectionIndex.Add_Index(secptr->Index_ID(), secptr);
			SectionList.Add_Tail(secptr);
		}
	}
	return true;
}

int INIClass::Save(FileClass& file) const
{
	FilePipe fpipe(file);
	return Save(fpipe);
}

int INIClass::Save(Pipe& pipe) const
{
	int total = 0;

	INISection* secptr = SectionList.First();
	while (secptr && secptr->Is_Valid()) 
	{
		total += pipe.Put("[", 1);
		total += pipe.Put(secptr->Section, strlen(secptr->Section));
		total += pipe.Put("]", 1);
		total += pipe.Put("\r\n", strlen("\r\n"));

		INIEntry* entryptr = secptr->EntryList.First();
		while (entryptr && entryptr->Is_Valid())
		{
			total += pipe.Put(entryptr->Entry, strlen(entryptr->Entry));
			total += pipe.Put("=", 1);
			total += pipe.Put(entryptr->Value, strlen(entryptr->Value));
			total += pipe.Put("\r\n", strlen("\r\n"));

			entryptr = entryptr->Next();
		}

		total += pipe.Put("\r\n", strlen("\r\n"));
		secptr = secptr->Next();
	}
	total += pipe.End();

	return total;
}

bool INIClass::Clear(const char* section, const char* entry)
{
	if (section == nullptr) 
	{
		SectionList.Delete();
		SectionIndex.Clear();
	}
	else 
	{
		INISection* secptr = Find_Section(section);
		if (secptr != nullptr) 
		{
			if (entry != nullptr) 
			{
				INIEntry* entptr = secptr->Find_Entry(entry);
				if (entptr != nullptr) 
				{
					secptr->EntryIndex.Remove_Index(entptr->Index_ID());
					delete entptr;
				}
			}
			else 
			{
				SectionIndex.Remove_Index(secptr->Index_ID());
				delete secptr;
			}
		}
	}
	return true;
}

bool INIClass::Is_Loaded() const
{
	return !SectionList.Is_Empty();
}

bool INIClass::Is_Present(const char* section, const char* entry) const
{
	return entry == nullptr ? Find_Section(section) != nullptr : Find_Entry(section, entry) != nullptr;
}

int INIClass::Section_Count() const
{
	return SectionIndex.Count();
}

bool INIClass::Section_Present(const char* section) const
{
	return Find_Section(section) != nullptr;
}

int INIClass::Entry_Count(const char* section) const
{
	INISection* secptr = Find_Section(section);
	return secptr != nullptr ? secptr->EntryIndex.Count() : 0;
}

const char* INIClass::Get_Entry(const char* section, int index) const
{
	INISection* secptr = Find_Section(section);

	if (secptr != nullptr && index < secptr->EntryIndex.Count()) 
	{
		INIEntry* entryptr = secptr->EntryList.First();

		while (entryptr != NULL && entryptr->Is_Valid()) 
		{
			if (index == 0) 
				return entryptr->Entry;
			--index;
			entryptr = entryptr->Next();
		}
	}
	return nullptr;
}

int INIClass::Get_String(const char* section, const char* entry, const char* defvalue, char* buffer, int size) const
{
	if (section == nullptr || entry == nullptr) 
	{
		if (buffer != nullptr && size > 0)
			buffer[0] = '\0';

		return 0;
	}

	INIEntry* entryptr = Find_Entry(section, entry);
	if (entryptr) 
	{
		if (entryptr->Value)
			defvalue = entryptr->Value;
	}

	if (buffer == nullptr || !size)
		return 0;
	else if (defvalue == nullptr) 
	{
		buffer[0] = '\0';
		return 0;
	}
	else if (buffer == defvalue)
		return strlen(buffer);
	else 
	{
		strncpy(buffer, defvalue, size);
		buffer[size - 1] = '\0';
		strtrimcpp(buffer);
		return strlen(buffer);
	}
}

int INIClass::Get_Int(const char* section, const char* entry, int defvalue) const
{
	if (section == nullptr || entry == nullptr) 
		return defvalue;

	INIEntry* entryptr = Find_Entry(section, entry);
	if (entryptr && entryptr->Value != nullptr)
	{
		if (*entryptr->Value == '$')
			sscanf(entryptr->Value, "$%x", &defvalue);

		else 
		{
			if (tolower(entryptr->Value[strlen(entryptr->Value) - 1]) == 'h')
				sscanf(entryptr->Value, "%xh", &defvalue);
			else
				defvalue = atoi(entryptr->Value);
		}
	}
	return defvalue;
}

int INIClass::Get_Hex(const char* section, const char* entry, int defvalue) const
{
	if (section == nullptr || entry == nullptr) 
		return defvalue;

	INIEntry* entryptr = Find_Entry(section, entry);
	if (entryptr && entryptr->Value != nullptr)
		sscanf(entryptr->Value, "%x", &defvalue);

	return defvalue;
}

bool INIClass::Get_Bool(const char* section, const char* entry, bool defvalue) const
{
	if (section == nullptr || entry == nullptr) 
		return defvalue;

	INIEntry* entryptr = Find_Entry(section, entry);
	if (entryptr && entryptr->Value != nullptr) 
	{
		switch (toupper(*entryptr->Value)) 
		{
		case 'Y':
		case 'T':
		case '1':
			return true;

		case 'N':
		case 'F':
		case '0':
			return false;
		}
	}
	return defvalue;
}

int INIClass::Get_TextBlock(const char* section, char* buffer, int len) const
{
	if (len <= 0) 
		return 0;

	buffer[0] = '\0';
	if (len <= 1) 
		return 0;

	int elen = Entry_Count(section);
	int total = 0;
	for (int index = 0; index < elen; ++index)
	{
		if (index > 0) 
		{
			*buffer++ = ' ';
			--len;
			++total;
		}

		Get_String(section, Get_Entry(section, index), "", buffer, len);

		int partial = strlen(buffer);
		total += partial;
		buffer += partial;
		len -= partial;
		if (len <= 1)
			break;
	}
	return total;
}

int INIClass::Get_UUBlock(const char* section, void* buffer, int len) const
{
	if (section == nullptr) 
		return 0;

	Base64Pipe b64pipe(Base64Pipe::DECODE);
	BufferPipe bpipe(buffer, len);

	b64pipe.Put_To(&bpipe);

	int total = 0;
	int counter = Entry_Count(section);
	for (int index = 0; index < counter; ++index)
	{
		char buffer[128];

		int length = Get_String(section, Get_Entry(section, index), "=", buffer, sizeof(buffer));
		int outcount = b64pipe.Put(buffer, length);
		total += outcount;
	}
	total += b64pipe.End();
	return total;
}

PKey INIClass::Get_PKey(bool fast) const
{
	PKey key;
	char buffer[512];

	if (fast)
	{
		BigInt exp(PKey::FAST_EXPONENT);
		exp.DEREncode((unsigned char*)buffer);
		key.Decode_Exponent(buffer);
	}
	else 
	{
		Get_UUBlock("PrivateKey", buffer, sizeof(buffer));
		key.Decode_Exponent(buffer);
	}

	Get_UUBlock("PublicKey", buffer, sizeof(buffer));
	key.Decode_Modulus(buffer);

	return key;
}

double INIClass::Get_Double(const char* section, const char* entry, double defvalue) const
{
	if (section == nullptr || entry == nullptr)
		return defvalue;

	INIEntry* entryptr = Find_Entry(section, entry);
	if (entryptr && entryptr->Value != nullptr)
		sscanf(entryptr->Value, "%lf", &defvalue);

	return defvalue;
}

bool INIClass::Put_Double(const char* section, const char* entry, double value)
{
	return false;
}

bool INIClass::Put_String(const char* section, const char* entry, const char* string)
{
	if (section == nullptr || entry == nullptr) 
		return false;

	INISection* secptr = Find_Section(section);

	if (secptr == nullptr) 
	{
		secptr = new INISection(strdup(section));
		if (secptr == nullptr) 
			return false;

		SectionList.Add_Tail(secptr);
		SectionIndex.Add_Index(secptr->Index_ID(), secptr);
	}

	INIEntry* entryptr = secptr->Find_Entry(entry);
	if (entryptr != nullptr)
	{
		secptr->EntryIndex.Remove_Index(entryptr->Index_ID());
		delete entryptr;
	}

	if (string != nullptr && strlen(string) > 0)
	{
		entryptr = new INIEntry(strdup(entry), strdup(string));

		if (entryptr == nullptr)
			return false;

		secptr->EntryList.Add_Tail(entryptr);
		secptr->EntryIndex.Add_Index(entryptr->Index_ID(), entryptr);
	}
	return true;
}

bool INIClass::Put_Hex(const char* section, const char* entry, int number)
{
	char buffer[MAX_LINE_LENGTH];

	sprintf(buffer, "%X", number);
	return Put_String(section, entry, buffer);
}

bool INIClass::Put_Int(const char* section, const char* entry, int number, int format)
{
	char buffer[MAX_LINE_LENGTH];

	switch (format) 
	{
	default:
	case 0:
		sprintf(buffer, "%d", number);
		break;

	case 1:
		sprintf(buffer, "%Xh", number);
		break;

	case 2:
		sprintf(buffer, "$%X", number);
		break;
	}
	return Put_String(section, entry, buffer);
}

bool INIClass::Put_Bool(const char* section, const char* entry, bool value)
{
	return Put_String(section, entry, value ? "yes" : "no");
}

bool INIClass::Put_TextBlock(const char* section, const char* text)
{
	if (section == nullptr) 
		return false;

	Clear(section);

	int index = 1;
	while (text != nullptr && *text != '\0')
	{
		char buffer[128];

		strncpy(buffer, text, 75);
		buffer[75] = '\0';

		char b[32];
		sprintf(b, "%d", index);

		int count = strlen(buffer);
		if (count > 0) 
		{
			if (count >= 75) 
			{
				while (count) 
				{
					char c = buffer[count];

					if (isspace(c)) 
						break;
					--count;
				}

				if (count == 0)
					break;
				else
					buffer[count] = '\0';
			}

			strtrimcpp(buffer);
			Put_String(section, b, buffer);
			++index;
			text = ((char*)text) + count;
		}
		else
			break;
	}
	return true;
}

bool INIClass::Put_UUBlock(const char* section, void const* block, int len)
{
	if (section == nullptr || block == nullptr || len < 1) 
		return false;

	Clear(section);

	BufferStraw straw(block, len);
	Base64Straw bstraw(Base64Straw::ENCODE);
	bstraw.Get_From(straw);

	int counter = 1;

	for (;;) 
	{
		char buffer[71];
		char sbuffer[32];

		int length = bstraw.Get(buffer, sizeof(buffer) - 1);
		buffer[length] = '\0';
		if (length == 0)
			break;

		sprintf(sbuffer, "%d", counter);
		Put_String(section, sbuffer, buffer);
		++counter;
	}
	return true;
}

bool INIClass::Put_PKey(PKey const& key)
{
	char buffer[512];

	int len = key.Encode_Modulus(buffer);
	Put_UUBlock("PublicKey", buffer, len);

	len = key.Encode_Exponent(buffer);
	Put_UUBlock("PrivateKey", buffer, len);

	return true;
}

void INIClass::Strip_Comments(char* buffer)
{
	if (buffer != nullptr) 
	{
		char* comment = strchr(buffer, ';');
		if (comment) 
		{
			*comment = '\0';
			strtrimcpp(buffer);
		}
	}
}

INIClass::INISection* INIClass::Find_Section(const char* section) const
{
	if (section != nullptr) 
	{
		int crc = CRCEngine()(section, strlen(section));

		if (SectionIndex.Is_Present(crc))
			return SectionIndex.Fetch_Index(crc);
	}

	return nullptr;
}

INIClass::INIEntry* INIClass::Find_Entry(const char* section, const char* entry) const
{
	INISection* secptr = Find_Section(section);
	return secptr != nullptr ? secptr->Find_Entry(entry) : nullptr;
}

INIClass::INIEntry::INIEntry(char* entry, char* value)
	: Node<INIEntry>{}
	, Entry{ entry }
	, Value{ value }
{
}

INIClass::INIEntry::~INIEntry()
{
	free(Entry); 
	Entry = nullptr; 
	
	free(Value); 
	Value = nullptr;
}

int INIClass::INIEntry::Index_ID() const
{
	return CRCEngine()(Entry, strlen(Entry));
}

INIClass::INISection::INISection(char* section)
	: Node<INISection>{}
	, Section{ section }
{
}

INIClass::INISection::~INISection()
{
	free(Section); 
	Section = nullptr; 
	
	EntryList.Delete();
}

INIClass::INIEntry* INIClass::INISection::Find_Entry(const char* entry) const
{
	if (entry != nullptr) 
	{
		int crc = CRCEngine()(entry, strlen(entry));
		if (EntryIndex.Is_Present(crc))
			return EntryIndex.Fetch_Index(crc);
	}
	return nullptr;
}

int INIClass::INISection::Index_ID() const
{
	return CRCEngine()(Section, strlen(Section));
}
