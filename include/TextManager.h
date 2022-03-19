#pragma once

#include <Always.h>

#include <Search.h>

class TextManagerClass
{
public:
	static TextManagerClass StringTable;

	explicit TextManagerClass() noexcept = default;

	CLASS_NOCOPY(TextManagerClass);
	CLASS_NOMOVE(TextManagerClass);

	virtual ~TextManagerClass() = default;

	bool Load(const char* file);
	void Clear();

	const wchar_t* Fetch(const char* label, const char* file = __FILE__, int line = __LINE__);

	const wchar_t* operator[](const char* label);

private:
	struct labelcomp
	{
		bool operator()(const std::string& k1, const std::string& k2) const
		{
			return _strcmpi(k1.c_str(), k2.c_str()) < 0;
		};
	};

private:
	std::map<std::string, std::wstring, labelcomp> Texts;
};