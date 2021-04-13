#define _CRT_SECURE_NO_WARNINGS

#include <stdafx.h>
#include "Rofs.h"

#pragma pack(push, 1)

typedef struct tagRofsHeader
{
	u32 magic[5];	// 3, 1, 4, 0x11000, 0x400
	u8 unk;			// padding?
} ROFS_HEADER;

typedef struct tagRofsFile
{
	u32 offset,
		size;
} ROFS_FILE;

typedef struct tagRofsFileHeader
{
	u16 offset,
		key_cnt;
	u32 size;
	u8 ident[8];	// encrypted NotComp or Hi_Comp
} ROFS_FILE_HEADER;

#pragma pack(pop)

static const u16 base_array[] = {
	0x00e6, 0x01a4, 0x00e6, 0x01c5, 0x0130, 0x00e8, 0x03db, 0x008b,
	0x0141, 0x018e, 0x03ae, 0x0139, 0x00f0, 0x027a, 0x02c9, 0x01b0,
	0x01f7, 0x0081, 0x0138, 0x0285, 0x025a, 0x015b, 0x030f, 0x0335,
	0x02e4, 0x01f6, 0x0143, 0x00d1, 0x0337, 0x0385, 0x007b, 0x00c6,
	0x0335, 0x0141, 0x0186, 0x02a1, 0x024d, 0x0342, 0x01fb, 0x03e5,
	0x01b0, 0x006d, 0x0140, 0x00c0, 0x0386, 0x016b, 0x020b, 0x009a,
	0x0241, 0x00de, 0x015e, 0x035a, 0x025b, 0x0154, 0x0068, 0x02e8,
	0x0321, 0x0071, 0x01b0, 0x0232, 0x02d9, 0x0263, 0x0164, 0x0290
};

static inline unsigned char re3_next_key(unsigned long *key)
{
	*key *= 0x5d588b65;
	*key += 0x8000000b;

	return (*key >> 24);
}

static void re3_decrypt_block(unsigned char *src, unsigned long key, unsigned long length)
{
	unsigned char xor_key, base_index;
	unsigned long i;
	int block_index;

	xor_key = re3_next_key(&key);
	base_index = re3_next_key(&key) % 0x3f;

	block_index = 0;
	for (i = 0; i < length; i++) {
		if (block_index > base_array[base_index]) {
			base_index = re3_next_key(&key) % 0x3f;
			xor_key = re3_next_key(&key);
			block_index = 0;
		}
		src[i] ^= xor_key;
		block_index++;
	}
}

static size_t re3_depack(unsigned char *src, unsigned char *dst, int length)
{
	unsigned char tmp4k[4096 + 256];
	int dstIndex;

	int srcNumBit, srcIndex, tmpIndex;
	int i, value, value2, tmpStart, tmpLength;

	for (i = 0; i < 256; i++) {
		memset(&tmp4k[i * 16], i, 16);
	}
	memset(&tmp4k[4096], 0, 256);

	srcNumBit = 0;
	srcIndex = 0;
	tmpIndex = 0;
	dstIndex = 0;
	while (srcIndex < length) {
		srcNumBit++;

		value = src[srcIndex++] << srcNumBit;
		value |= src[srcIndex] >> (8 - srcNumBit);

		if (srcNumBit == 8) {
			srcIndex++;
			srcNumBit = 0;
		}

		if ((value & (1 << 8)) == 0) {
			dst[dstIndex++] = tmp4k[tmpIndex++] = value;
		}
		else {
			value2 = (src[srcIndex++] << srcNumBit) & 0xff;
			value2 |= src[srcIndex] >> (8 - srcNumBit);

			tmpLength = (value2 & 0x0f) + 2;

			tmpStart = (value2 >> 4) & 0xfff;
			tmpStart |= (value & 0xff) << 4;

			memcpy(&dst[dstIndex], &tmp4k[tmpStart], tmpLength);
			memcpy(&tmp4k[tmpIndex], &dst[dstIndex], tmpLength);

			dstIndex += tmpLength;
			tmpIndex += tmpLength;
		}

		if (tmpIndex >= 4096) {
			tmpIndex = 0;
		}
	}

	return dstIndex - 1;
}

static std::vector<u8> re3_readfile(unsigned char *src)
{
	ROFS_FILE_HEADER *header = (ROFS_FILE_HEADER*)src;
	int i, offset, compressed;
	unsigned long *array_keys, *array_length;

	/* Decrypt ident */
	for (i = 0; i < 8; i++) {
		header->ident[i] ^= header->ident[7];
	}
	compressed = (strcmp("Hi_Comp", (char*)header->ident) == 0);

	offset = header->offset;

	/* array_keys follows the file header */
	array_keys = (unsigned long *)&src[sizeof(ROFS_FILE_HEADER)];
	/* and array_length follows array_keys */
	array_length = &array_keys[header->key_cnt];

	std::vector<u8> dstBuffer = std::vector<u8>(header->size + 0x1000);
	u8 *dst = &dstBuffer[0];
	/* Decrypt each block with its specific key */
	for (i = 0; i < header->key_cnt; i++) {
		re3_decrypt_block(&src[offset], array_keys[i], array_length[i]);
		if (compressed)
			dst += re3_depack(&src[offset], dst, array_length[i]);
		else
		{
			memcpy(dst, &src[offset], array_length[i]);
			dst += array_length[i];
		}
		offset += array_length[i];
	}

	return dstBuffer;
}

void CreateDirectoryRecursively(std::string path)
{
	unsigned int pos = 0;
	do
	{
		pos = path.find_first_of("\\/", pos + 1);
		CreateDirectoryA(path.substr(0, pos).c_str(), NULL);
	} while (pos != std::string::npos);
}

/////////////////////////////////////////
CRofs::CRofs()
{
}

CRofs::~CRofs()
{
	Close();
}

bool CRofs::Open(LPCSTR filename)
{
	handle.Open(filename);

	if (handle.IsOpen())
	{
		ReadInfo();
		return true;
	}

	return false;
}

void CRofs::ReadInfo()
{
	std::string dir;
	ROFS_HEADER head;

	handle.Read(&head, sizeof(head));
	root = ReadString();
	// contained folder
	ROFS_FILE f;
	handle.Read(&f, sizeof(f));
	dir = ReadString();
	
	// a bit hackish, but let's assume there's never more than one subfolder
	root += "\\";
	root += dir;
	root += "\\";

	CreateDirectoryRecursively(root);

	handle.Seek(f.offset * 8, SEEK_SET);
	size_t count = 0;
	handle.Read(&count, 4);
	for (size_t i = 0; i < count; i++)
	{
		ROFS_FILE ff;
		handle.Read(&ff, sizeof(ff));
		std::string name = ReadString();

		CRofs::FILE in;
		in.offset = ff.offset * 8;
		in.size = ff.size;
		in.name = name;
		info.push_back(in);
	}
}

std::string CRofs::ReadString()
{
	char ch;
	std::string ret;

	while (1)
	{
		handle.Read(&ch, 1);
		if (ch == '\0') break;

		ret += ch;
	}

	return ret;
}

void CRofs::ExtractFile(size_t index)
{
	if (index >= GetCount())
		return;

	CRofs::FILE &file = info[index];
	if (file.size == 0)
		return;
	handle.Seek(file.offset, SEEK_SET);

	u8 *buffer = new u8[file.size];
	handle.Read(buffer, file.size);
	std::vector<u8> data = re3_readfile(buffer);
	delete[] buffer;

	//CFile out;
	char outname[MAX_PATH];
	sprintf_s(outname, MAX_PATH, "%s%s", root.c_str(), file.name.c_str());
	::FILE *out = fopen(outname, "wb+");
	//out.Create(outname);
	//out.Write(&data[0], data.size());
	//out.Close();
	fwrite(&data[0], data.size() - 0x1000, 1, out);
	fclose(out);
}

void CRofs::Close()
{
	root.clear();
	info.clear();
	handle.Close();
}