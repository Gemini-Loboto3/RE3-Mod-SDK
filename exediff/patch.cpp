#include <stdafx.h>
#include "patch.h"

int ApplyPatch(u8* src, u8*& dest, u8* patch, int entry_size, int src_size, int dst_size)
{
	// create copy buffer
	//dest=new u8[dst_size];
	//memcpy(dest,src,dst_size<=src_size ? dst_size : src_size);
	MEM_STREAM str;
	MemStreamCreate(&str);
	MemStreamWrite(&str, src, src_size);

	// apply patch segments
	for (u8* seek = patch; seek < patch + entry_size;)
	{
		int paddr = *((int*)seek);
		int plen = *((int*)(seek + 4));
		//memcpy(dest+paddr,seek+8,plen);
		MemStreamSeek(&str, paddr, SEEK_SET);
		MemStreamWrite(&str, seek + 8, plen);
		seek += plen + 8;
	}
	dest = str.data;
	return entry_size;
}

void CompareFiles(DWORD* source, DWORD* edit, int size_s, int size_e, CMemStream &patch)
{
	bool different = FALSE;
	//BYTE* buffer = new BYTE[size_e * 9];
	//BYTE* buf2 = buffer;

	int i;
	for (i = 0;; i++)
	{
		if (i >= size_s || i >= size_e) break;
		if (source[i] != edit[i])
		{
			int j;
			different = true;
			//*((ULONG*)buf2) = i;	// position for change
			// count how many different bytes there are in a row
			for (j = 1; ; j++) if (j + i >= size_s || j + i >= size_e || source[j + i] == edit[j + i]) break;
			patch.Write(&i, 3);
			patch.Write(&j, 3);
			patch.Write(&edit[i], j * sizeof(*source));

			//*((ULONG*)(buf2 + 4)) = j;	// lenght for change
			//memcpy(buf2 + 8, edit + i, j);	// copy data into the buffer to compress
			//buf2 += 8 + j;	// increase buffer position
			i += j - 1;		// increase read position
		}
	}
	if (size_e > size_s)	// write all the additional data when new size>original
	{
		int size = size_e - size_s;
		patch.Write(&size_s, 3);
		patch.Write(&size, 3);
		patch.Write(&edit[i], size * sizeof(*source));
		//*((ULONG*)buf2) = size_s;
		//*((ULONG*)(buf2 + 4)) = size;
		//memcpy(buf2 + 8, edit + i, size);
		//buf2 += 8 + size;
		different = true;
	}

	if (different)
	{
		u32 cmd = -1;
		patch.Write(&cmd, 3);
	}
	//delete[] buffer;
}