#pragma once
#include <stdafx.h>
#include <string>
#include <vector>

class CRofs
{
public:
	CRofs();
	~CRofs();

	bool Open(LPCSTR filename);
	void ReadInfo();
	std::string ReadString();
	__inline size_t GetCount() { return info.size(); }
	void Close();

	void ExtractFile(size_t index);

	typedef struct tagFile
	{
		u32 offset;
		size_t size;
		std::string name;
	} FILE;

	CFile handle;
	std::string root;
	std::vector<FILE> info;
};

