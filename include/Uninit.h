#pragma once

#include <Always.h>

#include <MixFile.h>

void Uninit_MixFiles()
{
	MFCD::Free_All();
}