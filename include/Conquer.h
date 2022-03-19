#pragma once

#include <Always.h>

#include <CRC.h>

#include <RawFile.h>

int Calculate_CRC(void* buffer, int length);

extern RawFileClass LogFile;
void Log_Prepare(const char* const dir, const char* const logname);
void Log_Begin();
void Log_String(const char* const format, ...);
void Log_Close();