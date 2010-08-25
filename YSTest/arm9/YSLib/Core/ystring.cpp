// YSLib::Core::YString by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-5 22:44:04;
// UTime = 2010-8-25 20:14;
// Version = 0.2595;


#include "ystring.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

uchar_t* MString::s_str(NULL);


MString
MBCSToMString(const char* s, const CSID& cp)
{
	uchar_t* t(ucsdup(s, cp));
	MString str(t);

	free(t);
	return str;
}

YSL_END_NAMESPACE(Text)

YSL_END

