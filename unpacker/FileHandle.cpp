#include <stdafx.h>
#include "FileHandle.h"
#include <psapi.h>
#include <strsafe.h>

int CFile::Open(LPCSTR filename)
{
	handle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	// successful
	if (handle == INVALID_HANDLE_VALUE)
		return 0;
	// failure
	return 1;
}

int CFile::Create(LPCTSTR filename)
{
	handle = CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	// successful
	if (handle == INVALID_HANDLE_VALUE)
		return 0;
	// failure
	return 1;
}

void CFile::Close()
{
	if (handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(handle);
		handle = INVALID_HANDLE_VALUE;
	}
}

DWORD CFile::GetSize()
{
	return GetFileSize(handle, NULL);
}

void CFile::GetFinalPath()
{
	if (!IsOpen()) return;

	char buffer[MAX_PATH + 1];
	DWORD ret = GetFinalPathNameByHandleA(handle, buffer, MAX_PATH, FILE_NAME_OPENED);
	open_path = std::string(buffer);
	if (ret < MAX_PATH)
	{
		if (open_path.substr(0, 8).compare("\\\\?\\UNC\\") == 0)
			open_path = "\\" + open_path.substr(7);
		else if (open_path.substr(0, 4).compare("\\\\?\\") == 0)
			open_path = open_path.substr(4);
	}
}

DWORD CFile::Read(LPVOID buffer, size_t size)
{
	DWORD read;
	ReadFile(handle, buffer, (DWORD)size, &read, NULL);
	return read;
}

DWORD CFile::Write(LPVOID buffer, size_t size)
{
	DWORD wrote;
	WriteFile(handle, buffer, (DWORD)size, &wrote, NULL);
	return wrote;
}

DWORD CFile::Seek(__int64 position, size_t from)
{
	LARGE_INTEGER li;
	li.QuadPart = position;

 	return SetFilePointer(handle, li.LowPart, &li.HighPart, (DWORD)from);
}
