// unpacker.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdafx.h>
#include "Rofs.h"
#include "FileHandle.h"
#include "tinyxml2.h"
#include "p-exe.hpp"

//std::string DecodeString(u8* data, u8** last = nullptr);
std::string DecodeString(u8* data);
std::string DecodeStringEU(u8* data, bool is_live);

struct RDT_HEADER
{
	char field_0;
	char field_1;
	char field_2;
	char field_3;
	char field_4;
	char field_5;
	char field_6;
	char Sprt;
	int field_8;
	int field_C;
	int field_10;
	int field_14;
	int field_18;
	int field_1C;
	int field_20;
	void* pCut;
	int field_28;
	int field_2C;
	int field_30;
	int field_34;
	int field_38;
	u32 pText;
	int field_40;
	int field_44;
	int field_48;
	int field_4C;
	int field_50;
	int field_54;
	int field_58;
	int field_5C;
};

using namespace tinyxml2;

void StringToXml(const char* out_name, std::vector<std::string> &str)
{
	XMLDocument xml;
	xml.SetBOM(true);
	XMLElement* items = xml.NewElement("Strings");

	for(size_t i = 0, cnt = str.size(); i<cnt; i++)
	{
		XMLElement* sub = xml.NewElement("Text");
		sub->SetText(str[i].c_str());
		items->InsertEndChild(sub);
	}

	xml.InsertEndChild(items);
	xml.SaveFile(out_name);
}

void Extract_rdt_bin(const char* name, const char* xml_name)
{
	CFile f;
	f.Open(name);
	if (!f.IsOpen()) return;

	size_t size = f.GetSize();
	std::vector<u8> data = std::vector<u8>(size);
	f.Read(&data[0], size);
	f.Close();

	u16* ptr = (u16*)&data[0];
	std::vector<std::string> str;
	for (int i = 0; i < ptr[0] / 2; i++)
	{
		std::string s = DecodeStringEU(&((u8*)ptr)[ptr[i]], false);
		str.push_back(s);
	}

	StringToXml(xml_name, str);
}

void Extract_rdt_text(const char* name, const char* xml_name)
{
	CFile f;
	f.Open(name);
	if (!f.IsOpen()) return;

	size_t size = f.GetSize();
	std::vector<u8> data = std::vector<u8>(size);
	f.Read(&data[0], size);
	f.Close();

	RDT_HEADER* p = (RDT_HEADER*)&data[0];
	if (!p->pText) return;

	u16* ptr = (u16*)&data[p->pText];
	std::vector<std::string> str;
	for (int i = 0; i < ptr[0] / 2; i++)
	{
		std::string s = DecodeStringEU(&((u8*)ptr)[ptr[i]], false);
		str.push_back(s);
	}

	StringToXml(xml_name, str);
}

void Extract_rdt_text_bin(const char* name, const char* out_name)
{
	CFile f;
	f.Open(name);
	if (!f.IsOpen()) return;

	size_t size = f.GetSize();
	std::vector<u8> data = std::vector<u8>(size);
	f.Read(&data[0], size);
	f.Close();

	RDT_HEADER* p = (RDT_HEADER*)&data[0];
	if (!p->pText) return;

	u16* ptr = (u16*)&data[p->pText];
	std::vector<std::string> str;
	u8* last = nullptr;
	for (int i = 0; i < ptr[0] / 2; i++)
	{
		std::string s;
		//DecodeString(&((u8*)ptr)[ptr[i]], &last);
	}

	size_t tsize = (size_t)last - (size_t)ptr;
	f.Create(out_name);
	f.Write(ptr, tsize);
	f.Close();
}

bool is_live(u32 Room, size_t index)
{
	//switch (Room)
	//{
	//case 0x10f:
	//}

	return false;
}

void Extract_rofs(const char* rofs)
{
	CRofs f;
	f.Open(rofs);
	for (size_t j = 0, sj = f.GetCount(); j < sj; j++)
	{
		//printf("Extracting %s %d/%d\n", path, j + 1, sj);
		f.ExtractFile(j);
	}
}

void Extract_exe_text(BYTE* exe, u32 _ptr, u32 _text, int count, const char* xml_name)
{
	u16* ptr = (u16*)&exe[_ptr];
	u8* text = &exe[_text];

	std::vector<std::string> str;
	for (int i = 0; i < count; i++)
	{
		std::string s;
		str.push_back(DecodeStringEU(&text[ptr[i]], false));
	}

	StringToXml(xml_name, str);
}

void Extract_bin_text(const char* bin, u32 _ptr, int count, const char* xml_name)
{
	CBufferFile bin_d(bin);
	Extract_exe_text(bin_d.GetBuffer(), _ptr, 0, count, xml_name);
}

__inline LPCSTR SAFE_STR(BYTE* str, const char *ptr, DWORD base)
{
	if (ptr == 0)
		return nullptr;

	return (LPCSTR)&str[(DWORD)ptr - base];
}

struct MUSIC_DATA
{
	char* name;
	__int16 field_4;
	__int16 flag;
	int loop_start;
	int loop_end;
};

XMLElement* TrackToXml(XMLDocument &xml, BYTE *data, DWORD base, MUSIC_DATA *d)
{
	static MUSIC_DATA dummy = { 0 };
	if (memcmp(&dummy, d, sizeof(dummy))==0)
		return nullptr;
	auto name = SAFE_STR(data, d->name, base);
	if (name == nullptr || name[0] == '\0')
		return nullptr;

	XMLElement* bb = xml.NewElement("Track");
	bb->SetAttribute("name", name);
	bb->SetAttribute("index", d->field_4);
	bb->SetAttribute("has_loop", d->flag);
	if (d->flag)
	{
		bb->SetAttribute("l_start", d->loop_start);
		bb->SetAttribute("l_end", d->loop_end);
	}

	return bb;
}

void Extract_music_data(BYTE *data, DWORD base)
{
	MUSIC_DATA *m = (MUSIC_DATA*)(&data[0x518AE0 - base]);
	XMLDocument xml;
	xml.SetBOM(true);
	XMLElement* bgm = xml.NewElement("BgmAttributes");

	char comm[32];

	XMLElement* main = xml.NewElement("Main");
	// main
	for (int i = 0; i < 192; i += 3)
	{
		XMLElement* track = xml.NewElement("Tracks");
		sprintf_s(comm, sizeof(comm), " MAIN%02X ", i / 3);
		auto c = xml.NewComment(comm);
		main->InsertEndChild(c);
		auto b0 = TrackToXml(xml, data, base, m++); if(b0) track->InsertEndChild(b0);
		auto b1 = TrackToXml(xml, data, base, m++); if(b1) track->InsertEndChild(b1);
		auto b2 = TrackToXml(xml, data, base, m++); if(b2) track->InsertEndChild(b2);
		main->InsertEndChild(track);
	}
	bgm->InsertEndChild(main);

	// sub0
	XMLElement* sbgm0 = xml.NewElement("SBgm0");
	for (int i = 0; i < 135; i += 3)
	{
		XMLElement* track = xml.NewElement("Tracks");
		sprintf_s(comm, sizeof(comm), " SBGM0_%02X ", i / 3);
		auto c = xml.NewComment(comm);
		sbgm0->InsertEndChild(c);
		auto b0 = TrackToXml(xml, data, base, m++); if (b0) track->InsertEndChild(b0);
		auto b1 = TrackToXml(xml, data, base, m++); if (b1) track->InsertEndChild(b1);
		auto b2 = TrackToXml(xml, data, base, m++); if (b2) track->InsertEndChild(b2);
		sbgm0->InsertEndChild(track);
	}
	bgm->InsertEndChild(sbgm0);

	// sub1
	XMLElement* sbgm1 = xml.NewElement("SBgm1");
	for (int i = 0; i < 135; i += 3)
	{
		XMLElement* track = xml.NewElement("Tracks");
		sprintf_s(comm, sizeof(comm), " SBGM1_%02X ", i / 3);
		auto c = xml.NewComment(comm);
		sbgm1->InsertEndChild(c);
		auto b0 = TrackToXml(xml, data, base, m++); if (b0) track->InsertEndChild(b0);
		auto b1 = TrackToXml(xml, data, base, m++); if (b1) track->InsertEndChild(b1);
		auto b2 = TrackToXml(xml, data, base, m++); if (b2) track->InsertEndChild(b2);
		sbgm1->InsertEndChild(track);
	}
	bgm->InsertEndChild(sbgm1);

	xml.InsertEndChild(bgm);
	xml.SaveFile("bgm_attr.xml");
}

int main()
{
	PExe sn_exe;
	sn_exe.Open("D:\\Program Files\\Biohazard 3 PC\\BIOHAZARD(R) 3 PC.exe");
	Extract_music_data(&sn_exe.data[0], sn_exe.data_addr);

	Extract_rofs("rofs2i.dat");

	CBufferFile exe_i("I:\\RE3ITA\\ResidentEvil3.exe");
	Extract_exe_text(exe_i.GetBuffer(), 0x119D98, 0x119400, 388 / 2, "xml_ita\\items_simple.xml");
	Extract_exe_text(exe_i.GetBuffer(), 0x1192d0, 0x116cc8, 302 / 2, "xml_ita\\system.xml");
	Extract_exe_text(exe_i.GetBuffer(), 0x116c38, 0x1164a8, 144 / 2, "xml_ita\\prompt.xml");
	Extract_exe_text(exe_i.GetBuffer(), 0x1218ec, 0x121780, 14 / 2,  "xml_ita\\music.xml");
	Extract_exe_text(exe_i.GetBuffer(), 0x121a50, 0x121960, 30 / 2,  "xml_ita\\power.xml");
	Extract_exe_text(exe_i.GetBuffer(), 0x121af4, 0x121ae0, 10 / 2,  "xml_ita\\gas.xml");
	Extract_exe_text(exe_i.GetBuffer(), 0x123860, 0x1236c8, 30 / 2,  "xml_ita\\mercs.xml");
	Extract_exe_text(exe_i.GetBuffer(), 0x125280, 0x125030, 4,       "xml_ita\\prologue.xml");
	Extract_exe_text(exe_i.GetBuffer(), 0x1234a2, 0x123210, 1,       "xml_ita\\epilogue.xml");
	Extract_exe_text(exe_i.GetBuffer(), 0x128860, 0x1285c0, 86 / 2,  "xml_ita\\pc.xml");

	// english stuff extraction
	Extract_bin_text("data_eng\\item.bin",   0x8dc,  388 / 2, "xml_eng\\items_simple.xml");
	Extract_bin_text("data_eng\\sys.bin",    0x26d0, 302 / 2, "xml_eng\\system.xml");
	Extract_bin_text("data_eng\\prompt.bin", 0x7a8,  144 / 2, "xml_eng\\prompt.xml");
	Extract_bin_text("data_eng\\music.bin",  0x10c,  14 / 2,  "xml_eng\\music.xml");
	Extract_bin_text("data_eng\\power.bin",  0xaa,   10 / 2,  "xml_eng\\power.xml");
	Extract_bin_text("data_eng\\gas.bin",    0x14,   10 / 2,  "xml_eng\\gas.xml");
	Extract_bin_text("data_eng\\mercs.bin",  0x13c,  26 / 2,  "xml_eng\\mercs.xml");
	Extract_bin_text("data_eng\\end.bin",    0x22c,  1,       "xml_eng\\epilogue.xml");
	Extract_bin_text("data_eng\\map.bin",    0x64,   20 / 2,  "xml_eng\\map.xml");

	for (int Stage = 1; Stage < 8; Stage++)
	{
		for (int Room = 0; Room < 0x26; Room++)
		{
			char path[MAX_PATH], xml[MAX_PATH];
			sprintf_s(path, MAX_PATH, "data_eng\\rdt\\R%d%02X.bin", Stage, Room);
			sprintf_s(xml, MAX_PATH, "xml_eng\\rdt\\R%d%02X.xml", Stage, Room);
			Extract_rdt_bin(path, xml);
		}
	}

	//Extract_rofs("rofs4i.dat");

	for (int Stage = 1; Stage < 8; Stage++)
	{
		for (int Room = 0; Room < 0x26; Room++)
		{
			char path[MAX_PATH], xml[MAX_PATH];
			sprintf_s(path, MAX_PATH, "DATA_I\\RDT\\R%d%02X.RDT", Stage, Room);
			sprintf_s(xml, MAX_PATH, "xml_ita\\rdt\\R%d%02X.xml", Stage, Room);
			Extract_rdt_text(path, xml);
		}
	}

	char path[MAX_PATH], xml[MAX_PATH];
	for (int Stage = 1; Stage < 9; Stage++)
	{
		for (int Room = 0; Room < 32; Room++)
		{
			printf("Dumping %X%02X\n", Stage, Room);

			sprintf_s(path, MAX_PATH, "RDT US\\R%d%02X.RDT", Stage, Room);
#if 0
			sprintf_s(xml,  MAX_PATH, "xml\\R%d%02X.xml",    Stage, Room);
			Extract_rdt_text(path, xml);
#else
			sprintf_s(xml, MAX_PATH, "xml\\R%d%02X.bin", Stage, Room);
			Extract_rdt_text_bin(path, xml);
#endif
		}
	}


	//for (int i = 1; i <= 15; i++)
	//{
	//	CRofs f;
	//	char path[MAX_PATH];
	//	sprintf_s(path, MAX_PATH, "D:\\Program Files\\Biohazard 3 PC\\Rofs%d.dat", i);
	//	f.Open(path);
	//	for (size_t j = 0, sj = f.GetCount(); j < sj; j++)
	//	{
	//		printf("Extracting %s %d/%d\n", path, j + 1, sj);
	//		f.ExtractFile(j);
	//	}
	//}
}
