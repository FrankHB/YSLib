// YSLib::Core::YFile by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-24 23:14:51;
// UTime = 2010-7-25 10:37;
// Version = 0.1326;


#include "yfile.h"

YSL_BEGIN

YFile::YFile(CPATH p)
: fp(NULL), fsize(0)
{
	if(Open(p))
	{
		fseek(0, SEEK_END);
		fsize = ftell();
		rewind();
	}
}

YFile::~YFile()
{
	Release();
}

void
YFile::Release()
{
	if(IsValid())
		fclose(fp);
}

bool
YFile::Open(CPATH p)
{
	Release();
	fp = fopen(p, "r");
	return IsValid();
}

YSL_END

