// YSLib::Core::YString by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-5 22:44:04;
// UTime = 2010-9-22 3:54;
// Version = 0.2628;


#include "ystring.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

uchar_t* String::s_str(NULL);


String
MBCSToString(const char* s, const CSID& cp)
{
	uchar_t* t(ucsdup(s, cp));
	String str(t);

	std::free(t);
	return str;
}

std::string
StringToMBCS(const String& s, const CSID& cp)
{
	char* t(static_cast<char*>(std::malloc((s.length() + 1) << 2)));

	if(t != NULL)
		UTF16LEToMBCS(t, s.c_str(), cp);

	std::string str(t);

	std::free(t);
	return str;
}

YSL_END_NAMESPACE(Text)

YSL_END

