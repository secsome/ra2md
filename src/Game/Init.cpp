#include <Init.h>

#include <Conquer.h>
#include <RandomStraw.h>
#include <RAMFile.h>
#include <PKey.h>
#include <INI.h>
#include <TextManager.h>
#include <MixFile.h>
#include <Direct2DApp.h>

void Init_Global()
{
	char filename[_MAX_PATH];
	GetModuleFileName(static_cast<HMODULE>(ProgramInstance), filename, _MAX_PATH);
	
	char drive[_MAX_DRIVE];
	char dir[_MAX_PATH];
	_splitpath(filename, drive, dir, nullptr, nullptr);
	
	char directory[_MAX_PATH];
	_makepath(directory, drive, dir, nullptr, nullptr);

	SetCurrentDirectory(directory);
}

bool Init_Parse_Command_Line(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i)
	{
		char* param = argv[i];
		// Process params here
	}

	return true;
}

void Init_Random()
{
	SYSTEMTIME t;
	GetSystemTime(&t);

	RandomStraw::CryptRandom.Seed_Short(t.wMilliseconds);
	RandomStraw::CryptRandom.Seed_Bit(t.wSecond);
	RandomStraw::CryptRandom.Seed_Bit(t.wSecond >> 1);
	RandomStraw::CryptRandom.Seed_Bit(t.wSecond >> 2);
	RandomStraw::CryptRandom.Seed_Bit(t.wSecond >> 3);
	RandomStraw::CryptRandom.Seed_Bit(t.wSecond >> 4);
	RandomStraw::CryptRandom.Seed_Bit(t.wMinute);
	RandomStraw::CryptRandom.Seed_Bit(t.wMinute >> 1);
	RandomStraw::CryptRandom.Seed_Bit(t.wMinute >> 2);
	RandomStraw::CryptRandom.Seed_Bit(t.wMinute >> 3);
	RandomStraw::CryptRandom.Seed_Bit(t.wMinute >> 4);
	RandomStraw::CryptRandom.Seed_Bit(t.wHour);
	RandomStraw::CryptRandom.Seed_Bit(t.wDay);
	RandomStraw::CryptRandom.Seed_Bit(t.wDayOfWeek);
	RandomStraw::CryptRandom.Seed_Bit(t.wMonth);
	RandomStraw::CryptRandom.Seed_Bit(t.wYear);
}

void Init_Keys()
{
	Log_String("Init_Keys - declarations\n");
	const char* pkey = "[PublicKey]\n1=AihRvNoIbTn85FZRYNZRcT+i6KpU+maCsEqr3Q5q+LDB5tH7Tz2qQ38V\n\n";
	RAMFileClass ramfile((void*)pkey, strlen(pkey));
	INIClass ini;

	Log_String("Init_Keys - Load\n");
	ini.Load(ramfile);

	Log_String("Init_Keys - Init fast key\n");
	PKey::FastKey = ini.Get_PKey(true);
}

void Init_MixFiles()
{
	for (int i = 99; i >= 0; --i)
	{
		char buffer[30];
		sprintf(buffer, "EXPANDMD%02d.MIX", i);
		new MFCD(buffer, &PKey::FastKey);
	}

	new MFCD("RA2.MIX", &PKey::FastKey);
	new MFCD("RA2MD.MIX", &PKey::FastKey);
	new MFCD("CACHE.MIX", &PKey::FastKey);
	new MFCD("CACHEMD.MIX", &PKey::FastKey);
	new MFCD("LOCALMD.MIX", &PKey::FastKey);
	new MFCD("LOCAL.MIX", &PKey::FastKey);
}

void Init_Language()
{
	new MFCD("LANGMD.MIX", &PKey::FastKey);
	new MFCD("LANGUAGE.MIX", &PKey::FastKey);

	TextManagerClass::StringTable.Load("ra2md.csf");
	for (int i = 0; i < 100; ++i)
	{
		char buffer[20];
		sprintf(buffer, "stringtable%02d.csf", i);
		TextManagerClass::StringTable.Load(buffer);
	}

	CCFileClass ccfile("rulesmd.ini");
	if (ccfile.Open())
	{
		INIClass ini;
		ini.Load(ccfile);
		int x = ini.Get_Int("E1", "Cost");
	}
}

bool Init_Check()
{
	Log_String("Init_Check\n");

	if (!GetSystemMetrics(SM_MOUSEPRESENT) || !GetSystemMetrics(SM_CMOUSEBUTTONS))
	{
		Log_String("Mouse Required!\n");
		return false;
	}

	Log_String("Init_Check done!\n");
	return true;
}

void Init_App()
{
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, nullptr, 0);

	if (SUCCEEDED(CoInitialize(nullptr)))
	{
		{
			Direct2DApp app;

			if (SUCCEEDED(app.Initialize()))
				app.RunMessageLoop();

		}
		CoUninitialize();
	}
}
