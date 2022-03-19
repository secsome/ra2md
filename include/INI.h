#pragma once

#include <Always.h>

#include <File.h>
#include <Straw.h>
#include <Pipe.h>
#include <PKey.h>
#include <CRC.h>
#include <ListNode.h>
#include <Search.h>

class INIClass
{
public:
	explicit INIClass() noexcept;

	CLASS_NOCOPY(INIClass);
	CLASS_CANMOVE(INIClass);

	virtual ~INIClass();

	bool Load(FileClass& file);
	bool Load(Straw& straw);
	int Save(FileClass& file) const;
	int Save(Pipe& pipe) const;

	bool Clear(const char* section = nullptr, const char* entry = nullptr);
	bool Is_Loaded() const;
	bool Is_Present(const char* section, const char* entry = nullptr) const;

	int Section_Count() const;
	bool Section_Present(const char* section) const;

	int Entry_Count(const char* section) const;
	const char* Get_Entry(const char* section, int index) const;

	int Get_String(const char* section, const char* entry, const char* defvalue, char* buffer, int size) const;
	int Get_Int(const char* section, const char* entry, int defvalue = 0) const;
	int Get_Hex(const char* section, const char* entry, int defvalue = 0) const;
	bool Get_Bool(const char* section, const char* entry, bool defvalue = false) const;
	int Get_TextBlock(const char* section, char* buffer, int len) const;
	int Get_UUBlock(const char* section, void* buffer, int len) const;
	PKey Get_PKey(bool fast) const;
	double Get_Double(const char* section, const char* entry, double defvalue) const;

	bool Put_Double(const char* section, const char* entry, double value);
	bool Put_String(const char* section, const char* entry, const char* string);
	bool Put_Hex(const char* section, const char* entry, int number);
	bool Put_Int(const char* section, const char* entry, int number, int format = 0);
	bool Put_Bool(const char* section, const char* entry, bool value);
	bool Put_TextBlock(const char* section, const char* text);
	bool Put_UUBlock(const char* section, void const* block, int len);
	bool Put_PKey(PKey const& key);

protected:
	enum { MAX_LINE_LENGTH = 2048 };

	struct INIEntry : public Node<INIEntry> 
	{
		INIEntry(char* entry = 0, char* value = 0);
		
		CLASS_NOCOPY(INIEntry);
		CLASS_CANMOVE(INIEntry);
		
		virtual ~INIEntry() override;

		int Index_ID() const;

		char* Entry;
		char* Value;
	};

	struct INISection : public Node<INISection> 
	{
		INISection(char* section);

		CLASS_NOCOPY(INISection);
		CLASS_CANMOVE(INISection);

		virtual ~INISection() override;
		
		INIEntry* Find_Entry(const char* entry) const;
		int Index_ID() const;

		char* Section;
		List<INIEntry> EntryList;
		IndexClass<INIEntry*> EntryIndex;
	};

private:
	static void Strip_Comments(char* buffer);
	INISection* Find_Section(const char* section) const;
	INIEntry* Find_Entry(const char* section, const char* entry) const;

private:
	List<INISection> SectionList;
	IndexClass<INISection*> SectionIndex;
};