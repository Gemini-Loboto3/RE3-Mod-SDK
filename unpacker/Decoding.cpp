#include "stdafx.h"
#include <string>

size_t u8_wc_toutf8(char* dest, uint32_t ch);

static std::wstring encode =
{
	L" .►@@()@@«»@012345"	// 00
	L"6789:@,\"!?@ABCDEFG"	// 12
	L"HIJKLMNOPQRSTUVWXY"	// 24
	L"Z[/]'-·abcdefghijk"	// 36
	L"lmnopqrstuvwxyz@@@"	// 48
	L"@@@@@@@@@@@@@@@@@@"	// 5A
	L"@@@@@@@@@@@@@@@@@@"	// 6C
	L"@@@@@@@@@@@@@@@@@@"	// 7E
	L"@@@@@@@@@@@@@@@@@@"	// 90
	L"@@@@@@@@@@@@@@@@@@"	// A2
	L"@@@@@@@@@@@@@@@@@@"	// B4
	L"@@@@@@@@@@@@@@@@@@"	// C6
	L"@@@@@@@@@@@@@@@@@@"	// D8
	L"@@@@@@@@@@@@@@@@@@"	// EA
	L"&…@@@@@@@@@@@@@@@@"	// FC
	L"ＳＴＡＲⅠⅡⅢ"		// 10E
};

std::string DecodeString(u8 *data, u8 **last)
{
	std::string str;
	char temp[32];

	while (1)
	{
		u8 c = *data++;

		switch (c)
		{
		case 0xee:	// upper ranges
			//str += encode[*data++ + 0xea];
			u8_wc_toutf8(temp, encode[*data++ + 0xea]);
			str += temp;
			break;
		case 0xef:
		case 0xf0:
		case 0xf1:
		case 0xf2:
		case 0xf3:
		case 0xf4:
		case 0xf5:	// ??
			str += "{p}";
			break;
		case 0xf6:	// ??
			break;
		case 0xf7:	// short EOS for items
			if (last) *last = data;
			return str;
		case 0xf8:
			str += "{string ";
			str += *data + '0';
			str += "}";
			data++;
			break;
		case 0xf9:
			str += "{color ";
			str += *data + '0';
			str += "}";
			data++;
			break;
		case 0xfa:	// non-instant text scrolling
			sprintf_s(temp, sizeof(temp), "{scroll %d}", *data++);
			str += temp;
			break;
		case 0xfb:
			sprintf_s(temp, sizeof(temp), "{branch %d}", *data++);
			str += temp;
			break;
		case 0xfc:
			str += "\\n";
			break;
		case 0xfd:
			str += "{clear ";
			str += *data + '0';
			str += "}";
			data++;
			break;
		case 0xfe:
			if (*data != 0)
			{
				sprintf_s(temp, sizeof(temp), "{close %d}", *data++);
				str += temp;
			}
			if (last) *last = data;
			return str;
		case 0xff: // ??
			break;
		default:
			u8_wc_toutf8(temp, encode[c]);
			str += temp;
			//if (encode[c] & 0x80)
			//	str += "\xC2";
			//str += encode[c];
		}
	}

	if (last) *last = data;
	return str;
}

static std::wstring encode_eu =
{
	L" .►@@()@@‟”@012345"	// 00
	L"6789:@,@!?$ABCDEFG"	// 12
	L"HIJKLMNOPQRSTUVWXY"	// 24
	L"Z+/-'-·abcdefghijk"	// 36
	L"lmnopqrstuvwxyz   "	// 48
	L"@„…@@@@@‹@@@@@@'‟”"	// 5A
	L"@-@@@@›æ@@@@¡@@@@@"	// 6C
	L"@@@ª@@@@@°@@@'@@@@"	// 7E
	L"@@@@@@¿ÀÁÂ@ÄＡ@ÇÈÉÊ"	// 90
	L"ËÌÍÎÏ@ÑÒÓÔ@Ö@@ÙÚÛÜ"	// A2
	L"@@ßàáâ@ä@@çèéêëìíî"	// B4
	L"ï@ñòóô@ö@@ùúûü@@@@"	// C6
	L"@@@@@@@@@@@@@@@@@@"	// D8
	L"@@@@@@@@@@@@@@@@@@"	// EA
	L"&…@@@@@@@@@*@@@@@@"	// FC
	L"ＳＴＡＲⅠⅡⅢ;"		// 10E
};

size_t u8_wc_toutf8(char *dest, uint32_t ch)
{
	if (ch < 0x80) {
		dest[0] = (char)ch;
		dest[1] = '\0';
		return 1;
	}
	if (ch < 0x800) {
		dest[0] = (ch >> 6) | 0xC0;
		dest[1] = (ch & 0x3F) | 0x80;
		dest[2] = '\0';
		return 2;
	}
	if (ch < 0x10000) {
		dest[0] = (ch >> 12) | 0xE0;
		dest[1] = ((ch >> 6) & 0x3F) | 0x80;
		dest[2] = (ch & 0x3F) | 0x80;
		dest[3] = '\0';
		return 3;
	}
	if (ch < 0x110000) {
		dest[0] = (ch >> 18) | 0xF0;
		dest[1] = ((ch >> 12) & 0x3F) | 0x80;
		dest[2] = ((ch >> 6) & 0x3F) | 0x80;
		dest[3] = (ch & 0x3F) | 0x80;
		dest[4] = '\0';
		return 4;
	}
	return 0;
}

std::string DecodeStringEU(u8 *data, bool is_live)
{
	std::string str;
	char temp[32];

	int scroll = 0;
	int lives = 0;

	while (1)
	{
		u8 c = *data++;

		switch (c)
		{
		case 0xea:	// upper ranges
			if ((size_t)(*data + 0xea) >= encode_eu.size())
				break;
			else u8_wc_toutf8(temp, encode_eu[*data + 0xea]);
			data++;
			str += temp;
			break;
		case 0xef:
		case 0xf0:
		case 0xf1:
		case 0xf2:
			break;
		case 0xf3:
			sprintf_s(temp, sizeof(temp), "{snd %d}", *data++);
			str += temp;
			break;
		case 0xf4:
			sprintf_s(temp, sizeof(temp), "{cut %d}", *data++);
			str += temp;
			break;
		case 0xf5:	// ??
			str += "{s}";
			break;
		case 0xf6:	// ??
			str += "{t}";
			break;
		case 0xf7:	// short EOS for items
			return str;
		case 0xf8:
			sprintf_s(temp, sizeof(temp), "{string %d}", *data++);
			str += temp;
			break;
		case 0xf9:
			sprintf_s(temp, sizeof(temp), "{color %d}", *data++);
			str += temp;
			break;
			break;
		case 0xfa:	// non-instant text scrolling
			scroll = *data;
			sprintf_s(temp, sizeof(temp), "{scroll %d}", *data++);
			str += temp;
			break;
		case 0xfb:
			lives = (*data >> 4) & 0x7;
			sprintf_s(temp, sizeof(temp), "{branch %d}", *data++);
			str += temp;
			break;
		case 0xfc:
			str += "\\n";
			break;
		case 0xfd:
			sprintf_s(temp, sizeof(temp), "{clear %d}", *data++ * 60 / 50);
			str += temp;
			break;
		case 0xfe:
			if (lives)
			{
				str += "{live}";
				lives--;
				break;
			}
			if (*data != 0)
			{
				sprintf_s(temp, sizeof(temp), "{timed %d}", *data * 60 / 50);
				str += temp;
				return str;
			}
			return str;
		case 0xff: // ??
			break;
		default:
			u8_wc_toutf8(temp, encode_eu[c]);
			str += temp;
		}
	}

	return str;
}

std::string DecodeString(u8* data)
{
	std::string str;
	char temp[32];

	int scroll = 0;
	int lives = 0;

	while (1)
	{
		u8 c = *data++;

		switch (c)
		{
		case 0xea:	// upper ranges
			if ((size_t)(*data + 0xea) >= encode.size())
				break;
			else u8_wc_toutf8(temp, encode[*data + 0xea]);
			data++;
			str += temp;
			break;
		case 0xef:
		case 0xf0:
		case 0xf1:
		case 0xf2:
			break;
		case 0xf3:
			sprintf_s(temp, sizeof(temp), "{snd %d}", *data++);
			str += temp;
			break;
		case 0xf4:
			sprintf_s(temp, sizeof(temp), "{cut %d}", *data++);
			str += temp;
			break;
		case 0xf5:	// ??
			str += "{s}";
			break;
		case 0xf6:	// ??
			str += "{t}";
			break;
		case 0xf7:	// short EOS for items
			return str;
		case 0xf8:
			sprintf_s(temp, sizeof(temp), "{string %d}", *data++);
			str += temp;
			break;
		case 0xf9:
			sprintf_s(temp, sizeof(temp), "{color %d}", *data++);
			str += temp;
			break;
			break;
		case 0xfa:	// non-instant text scrolling
			scroll = *data;
			sprintf_s(temp, sizeof(temp), "{scroll %d}", *data++);
			str += temp;
			break;
		case 0xfb:
			lives = (*data >> 4) & 0x7;
			sprintf_s(temp, sizeof(temp), "{branch %d}", *data++);
			str += temp;
			break;
		case 0xfc:
			str += "\\n";
			break;
		case 0xfd:
			sprintf_s(temp, sizeof(temp), "{clear %d}", *data++);
			str += temp;
			break;
		case 0xfe:
			if (lives)
			{
				str += "{live}";
				lives--;
				break;
			}
			if (*data != 0)
			{
				sprintf_s(temp, sizeof(temp), "{timed %d}", *data);
				str += temp;
				return str;
			}
			return str;
		case 0xff: // ??
			break;
		default:
			u8_wc_toutf8(temp, encode[c]);
			str += temp;
		}
	}

	return str;
}
