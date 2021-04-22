#pragma once
#include "memstream.h"

void CompareFiles(DWORD* source, DWORD* edit, int size_s, int size_e, CMemStream& patch);
