// YSLib::Core::YString by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-5 22:06:05;
// UTime = 2010-9-22 4:28;
// Version = 0.2842;


#ifndef INCLUDED_YSTRING_H_
#define INCLUDED_YSTRING_H_

// String ：基础字符串管理。



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
	template<class _tChar>
	String(const _tChar*);
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
template<class _tChar>
String::String(const _tChar* s)
: stdex::ustring(s_str = ucsdup(s))
{
	std::free(s_str);
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

// YSLib 标准字符串转化为多字节字符串。
std::string
StringToMBCS(const String&, const CSID& = CS_Local);

YSL_END_NAMESPACE(Text)

YSL_END

#endif

