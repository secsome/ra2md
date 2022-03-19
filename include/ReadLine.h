#pragma once

#include <Always.h>

#include <File.h>
#include <Straw.h>

void strtrimcpp(char* buffer);
int Read_Line(FileClass& file, char* buffer, int len, bool& eof);
int Read_Line(Straw& file, char* buffer, int len, bool& eof);