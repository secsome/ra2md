#pragma once

#include <Always.h>

int Base64_Encode(const void* source, int slen, void* dest, int dlen);
int Base64_Decode(const void* source, int slen, void* dest, int dlen);