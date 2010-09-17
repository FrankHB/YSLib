// YSLib::Core::YString by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-5 22:44:04;
// UTime = 2010-9-17 17:10;
// Version = 0.2601;


#include "ystring.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

uchar_t* String::s_str(NULL);


String
MBCSToString(const char* s, const CSID& cp)
{
	uchar_t* t(ucsdup(s, cp));
	String str(t);

	free(t);
	return str;
}

YSL_END_NAMESPACE(Text)

YSL_END

