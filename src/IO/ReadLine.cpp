#include <ReadLine.h>

#include <FileStraw.h>

void strtrimcpp(char* buffer)
{
	if (buffer)
	{
		char* source = buffer;
		while (isspace(*source))
			++source;

		if (source != buffer)
			strcpy(buffer, source);

		for (int index = strlen(buffer) - 1; index >= 0; --index) 
		{
			if (isspace(buffer[index]))
				buffer[index] = '\0';
			else
				break;
		}
	}
}

int Read_Line(FileClass& file, char* buffer, int len, bool& eof)
{
	FileStraw fstraw(file);
	return Read_Line(fstraw, buffer, len, eof);
}

int Read_Line(Straw& file, char* buffer, int len, bool& eof)
{
	if (len == 0 || buffer == nullptr)
		return 0;

	int count = 0;
	for (;;) 
	{
		char c;
		if (file.Get(&c, sizeof(c)) != sizeof(c))
		{
			eof = true;
			buffer[0] = '\0';
			break;
		}

		if (c == '\x0A')
			break;
		if (c != '\x0D' && count + 1 < len)
			buffer[count++] = c;
	}
	buffer[count] = '\0';

	strtrimcpp(buffer);
	return strlen(buffer);
}
