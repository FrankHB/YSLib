// YSLib::Core::YString by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-5 22:06:05;
// UTime = 2010-8-25 20:11;
// Version = 0.2812;


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
class MString : public stdex::ustring
{
private:
	static uchar_t* s_str; //内码转换生成字符串临时指针。

public:
	MString();
	MString(const uchar_t*);
	template<class _charT>
	MString(const _charT*);
	MString(const stdex::ustring&);
};

inline
MString::MString()
: stdex::ustring()
{}
inline
MString::MString(const uchar_t* s)
: stdex::ustring(s)
{}
template<class _charT>
MString::MString(const _charT* s)
: stdex::ustring(s_str = ucsdup(s))
{
	free(s_str);
}
inline
MString::MString(const stdex::ustring& s)
: stdex::ustring(s)
{}


//多字节字符串转化为 YSLib 标准字符串。
MString
MBCSToMString(const char*, const CSID& = CS_Local);
inline MString
MBCSToString(const std::string& s, const CSID& cp = CS_Local)
{
	return MBCSToMString(s.c_str(), cp);
}

YSL_END_NAMESPACE(Text)

YSL_END

#endif

