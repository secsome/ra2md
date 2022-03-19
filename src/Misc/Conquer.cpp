#include <Conquer.h>

int Calculate_CRC(void* buffer, int length)
{
	return CRCEngine()(buffer, length);
}

RawFileClass LogFile;
void Log_Prepare(const char* const dir, const char* const logname)
{
	if (dir == nullptr || logname == nullptr)
		return;

	// Prepare directory
	CreateDirectory(dir, nullptr);

	// Prepare log file
	char path[_MAX_PATH];
	_makepath(path, "", dir, logname, ".log");

	LogFile.Open(path, FILE_ACCESS_WRITE);
}

void Log_Begin()
{
	Log_Prepare("Debug", "DEBUG");
}

void Log_String(const char* const format, ...)
{
	char buffer[0x400];

	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	if (LogFile.Is_Open())
		LogFile.Write(buffer, strlen(buffer));
	OutputDebugString(buffer);
}

void Log_Close()
{
	if (LogFile.Is_Open())
	{
		char buffer[_MAX_PATH];
		unsigned int dt = LogFile.Get_Date_Time();

		sprintf(buffer, "debug-%04d%02d%02d-%02d%02d%02d.log",
			YEAR(dt), MONTH(dt), DAY(dt), HOUR(dt), MINUTE(dt), SECOND(dt));
		
		LogFile.Close();

		MoveFile(LogFile.File_Name(), buffer);
	}
}

HANDLE AppMutex;
bool Setup_Mutex()
{
	AppMutex = CreateMutex(nullptr, FALSE, "4D01B245-DAA6-4351-9CD3-C2A172516EB7");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		HWND hWnd = FindWindow("4D01B245-DAA6-4351-9CD3-C2A172516EB7", nullptr);
		if (hWnd != NULL)
		{
			SetForegroundWindow(hWnd);
			ShowWindow(hWnd, SW_RESTORE);
		}
		if (AppMutex != NULL)
		{
			CloseHandle(AppMutex);
			AppMutex = NULL;
		}
		Log_String("RA2MD is already running...Bail!\n");
		return false;
	}
	Log_String("Create AppMutex okay.\n");
	return true;
}

void Release_Mutex()
{
	if (AppMutex != NULL)
	{
		CloseHandle(AppMutex);
		AppMutex = NULL;
	}
}
