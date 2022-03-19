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
