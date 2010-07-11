// YSLib::Core::YFile by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-24 23:14:51;
// UTime = 2010-5-18 19:44;
// Version = 0.1252;


#include "yfile.h"

YSL_BEGIN

YFile::YFile(CPATH p)
: fp(NULL), fsize(0)
{
	if(OpenFile(p))
	{
		fseek(0, SEEK_END);
		fsize = ftell();
		rewind();
	}
}

YFile::~YFile()
{
	if(IsValid())
		fclose(fp);
}

bool
YFile::OpenFile(CPATH p)
{
	this->~YFile();
	fp = fopen(p, "r");
	return IsValid();
}

YSL_END

