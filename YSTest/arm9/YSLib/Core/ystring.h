// YSLib::Core::YString by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-5 22:06:05;
// UTime = 2010-8-15 9:15;
// Version = 0.2774;


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

class MString : public stdex::ustring
{
private:
	static uchar_t* S_str; //内码转换生成字符串临时指针。

public:
	MString();
	template<class _charT>
	MString(const _charT*);
	MString(const stdex::ustring&);
};

inline
MString::MString()
: stdex::ustring()
{}
template<class _charT>
MString::MString(const _charT* s)
: stdex::ustring(S_str = ucsdup(s))
{
	free(S_str);
}
inline
MString::MString(const stdex::ustring& s)
: stdex::ustring(s)
{}

YSL_END_NAMESPACE(Text)

YSL_END

#endif

