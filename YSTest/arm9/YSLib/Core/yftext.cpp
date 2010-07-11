// YSLib::Core::YFile_(Text) by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-24 23:14:51;
// UTime = 2010-7-9 10:14;
// Version = 0.1725;


#include "yftext.h"

YSL_BEGIN

using namespace Text;

YTextFile::YTextFile(CPATH p)
: YFile(p), bl(0), cp(CharSet::Null)
{
	if(IsValid())
	{
		fseek(0, SEEK_END);
		bl = CheckBOM(cp);
		Rewind();
	}
	if(!bl)
	{
		cp = CS_Local;
	}
}
YTextFile::~YTextFile()
{
}

u8
YTextFile::CheckBOM(CSID& cp)
{
	rewind();
	if(fsize < 2)
		return 0;
	char tmp[4];
	fread(tmp, 1, 4);

	const char BOM_UTF_16LE[2] = {0xFF, 0xFE};

	if(!memcmp(tmp, BOM_UTF_16LE, 2))
	{
		cp = CharSet::UTF_16LE;
		return 2;
	}

	const char BOM_UTF_16BE[2] = {0xFE, 0xFF};

	if(!memcmp(tmp, BOM_UTF_16BE, 2))
	{
		cp = CharSet::UTF_16BE;
		return 2;
	}

	const char BOM_UTF_8[3] = {0xEF, 0xBB, 0xBF};

	if(!memcmp(tmp, BOM_UTF_8, 3))
	{
		cp = CharSet::UTF_8;
		return 3;
	}

	const char BOM_UTF_32LE[4] = {0xFF, 0xFE, 0x00, 0x00};

	if(!memcmp(tmp, BOM_UTF_32LE, 4))
	{
		cp = CharSet::UTF_32LE;
		return 4;
	}

	const char BOM_UTF_32BE[4] = {0x00, 0x00, 0xFE, 0xFF};

	if(!memcmp(tmp, BOM_UTF_32BE, 4))
	{
		cp = CharSet::UTF_32BE;
		return 4;
	}
	return 0;
}

void
YTextFile::Rewind() const
{
	fseek(bl, SEEK_SET);
}

void
YTextFile::SetPos(u32 pos) const
{
	fseek(bl + pos, SEEK_SET);
}

void
YTextFile::Seek(s32 offset, int origin) const
{
	if(origin == SEEK_SET)
		SetPos(offset);
	else
		fseek(offset, origin);
}

u32
YTextFile::Read(void* s, u32 n)
{
	return fread(s, n, 1);
}

u32
YTextFile::ReadS(uchar_t* s, u32 n) const
{
	u32 l(0);

	if(IsValid())
	{
		u32 i(0);

		while(i < n)
		{
			if(feof(fp))
				break;
			i += ToUTF(fp, s[l++], cp);
		}
	}
	return l;
}

YSL_END

