// YSLib::Core::YString by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-5 22:06:05;
// UTime = 2010-9-17 17:09;
// Version = 0.2826;


#ifndef INCLUDED_YSTRING_H_
#define INCLUDED_YSTRING_H_

// String ：基础字符串管理。

#ifndef UNICODE
#define UNICODE
#endif

#include "yobject.h"
#include <string>

namespace stdex
{
	typedef std::basic_string<uchar_t> ustring;
}

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

// YSLib 标准字符串（使用 UTF-16BE ）。
class String : public stdex::ustring
{
private:
	static uchar_t* s_str; //内码转换生成字符串临时指针。

public:
	String();
	String(const uchar_t*);
	template<class _charT>
	String(const _charT*);
	String(const stdex::ustring&);
};

inline
String::String()
: stdex::ustring()
{}
inline
String::String(const uchar_t* s)
: stdex::ustring(s)
{}
template<class _charT>
String::String(const _charT* s)
: stdex::ustring(s_str = ucsdup(s))
{
	free(s_str);
}
inline
String::String(const stdex::ustring& s)
: stdex::ustring(s)
{}


//多字节字符串转化为 YSLib 标准字符串。
String
MBCSToString(const char*, const CSID& = CS_Local);
inline String
MBCSToString(const std::string& s, const CSID& cp = CS_Local)
{
	return MBCSToString(s.c_str(), cp);
}

YSL_END_NAMESPACE(Text)

YSL_END

#endif

