// exediff.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <stdafx.h>
#include <vector>
#include "patch.h"

// helper class to read exe segments
class PExe
{
public:
	bool Open(LPCSTR name)
	{
		PIMAGE_DOS_HEADER dosHeader;		//Pointer to DOS Header
		PIMAGE_NT_HEADERS ntHeader;			//Pointer to NT Header
		PIMAGE_SECTION_HEADER pSecHeader;	//Section Header or Section Table Header

		CBufferFile f(name);
		if (f.GetBuffer() == nullptr || f.GetSize() == 0)
			return false;
		dosHeader = (PIMAGE_DOS_HEADER)f.GetBuffer();
		// check for valid DOS file
		if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
			return false;

		ntHeader = (PIMAGE_NT_HEADERS)((DWORD)(dosHeader)+(dosHeader->e_lfanew));
		// identify valid signature
		if (ntHeader->Signature != IMAGE_NT_SIGNATURE)
			return false;

		// gather information about segments
		pSecHeader = IMAGE_FIRST_SECTION(ntHeader);
		for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; i++, pSecHeader++)
		{
			if (_stricmp((char*)pSecHeader->Name, ".data") == 0 ||
				_stricmp((char*)pSecHeader->Name, "data") == 0)
			{
				data = std::vector<BYTE>(pSecHeader->SizeOfRawData);
				memcpy(&data[0], (void*)((DWORD)dosHeader + pSecHeader->PointerToRawData), pSecHeader->SizeOfRawData);
				data_addr = ntHeader->OptionalHeader.ImageBase + pSecHeader->VirtualAddress;
			}
			else if (_stricmp((char*)pSecHeader->Name, ".rdata") == 0 ||
				_stricmp((char*)pSecHeader->Name, "rdata") == 0)
			{
				rdata = std::vector<BYTE>(pSecHeader->SizeOfRawData);
				memcpy(&rdata[0], (void*)((DWORD)dosHeader + pSecHeader->PointerToRawData), pSecHeader->SizeOfRawData);
				rdata_addr = ntHeader->OptionalHeader.ImageBase + pSecHeader->VirtualAddress;
			}
			else if (_stricmp((char*)pSecHeader->Name, ".text") == 0 ||
				_stricmp((char*)pSecHeader->Name, "text") == 0)
			{
				text = std::vector<BYTE>(pSecHeader->SizeOfRawData);
				memcpy(&text[0], (void*)((DWORD)dosHeader + pSecHeader->PointerToRawData), pSecHeader->SizeOfRawData);
				text_addr = ntHeader->OptionalHeader.ImageBase + pSecHeader->VirtualAddress;
			}
		}

		return true;
	}

	std::vector<BYTE> data,
		rdata,
		text;
	DWORD data_addr,
		rdata_addr,
		text_addr;
};

void Write_to_patch(std::vector<BYTE> &old_data, std::vector<BYTE> &new_data, CMemStream &patch, DWORD seg_addr)
{
	patch.Write(&seg_addr, 4);
	patch.Write(&seg_addr, 4);	// reserve size here

	size_t pos0 = patch.Tell();
	CompareFiles((DWORD*)&old_data[0], (DWORD*)&new_data[0], old_data.size() / 4, new_data.size() / 4, patch);
	size_t pos1 = patch.Tell();

	DWORD size = pos1 - pos0;			// calculate patch block size
	patch.Seek(pos0 - 4, SEEK_SET);		// jump back to size
	patch.Write(&size, 4);				// write size

	patch.Seek(0, SEEK_END);			// go back to the end of the patch
}

int main()
{
	PExe exe1, exe2;

	// open exes as segments for comparison
	exe1.Open("D:\\Program Files\\Biohazard 3 PC\\BIOHAZARD(R) 3 PC.exe");
	exe2.Open("D:\\Program Files\\RE3 LOTN\\The Lord of the Necropolis_1_0_0.exe");

	// generate exe patch
	CMemStream patch;
	Write_to_patch(exe1.text, exe2.text,   patch, exe1.text_addr);
	Write_to_patch(exe1.data, exe2.data,   patch, exe1.data_addr);
	Write_to_patch(exe1.rdata, exe2.rdata, patch, exe1.rdata_addr);

	// flush patch
	FILE* fp = nullptr;
	fopen_s(&fp, "exe.patch", "wb+");
	if (fp)
	{
		fwrite(patch.Data(), patch.str.size, 1, fp);
		fclose(fp);
	}
}