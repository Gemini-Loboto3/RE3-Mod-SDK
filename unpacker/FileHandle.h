#pragma once
#include <string>

class CFile
{
public:
	CFile() { handle = INVALID_HANDLE_VALUE; }
	CFile(LPCSTR name, bool bOpen = true)
	{
		handle = INVALID_HANDLE_VALUE;
		if (bOpen) Open(name);
		else Create(name);
	}
	~CFile() { Close(); }

	int Open(LPCSTR filename);
	int Create(LPCTSTR filename);
	void Close();

	DWORD GetSize();
	void GetFinalPath();

	__inline const char* GetOpenPath() { return open_path.c_str(); }

	DWORD Read(LPVOID buffer, size_t size);
	DWORD Write(LPVOID buffer, size_t size);
	DWORD Seek(__int64 position, size_t from);

	__inline bool IsOpen() { return handle != INVALID_HANDLE_VALUE; }

private:
	HANDLE handle;
	std::string open_path;
};
