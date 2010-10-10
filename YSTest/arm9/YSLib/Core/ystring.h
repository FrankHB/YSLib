// YSLib::Core::YString by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-03-05 22:06:05 + 08:00;
// UTime = 2010-10-09 17:45 + 08:00;
// Version = 0.2896;


#ifndef INCLUDED_YSTRING_H_
#define INCLUDED_YSTRING_H_

// String ：基础字符串管理。



#include "yobject.h"
//#include <string>
#include "../Adaptor/cont.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

// YSLib 标准字符串（使用 UTF-16LE ）。
class String : public ustring
{
private:
	static uchar_t* s_str; //内码转换生成字符串临时指针。

public:
	String();
	String(const uchar_t*);
	template<class _tChar>
	String(const _tChar*);
	String(const ustring&);
};

inline
String::String()
	: ustring()
{}
inline
String::String(const uchar_t* s)
	: ustring(s)
{}
template<class _tChar>
String::String(const _tChar* s)
	: ustring(s_str = ucsdup(s))
{
	std::free(s_str);
}
inline
String::String(const ustring& s)
	: ustring(s)
{}


//多字节字符串转化为 YSLib 标准字符串。
String
MBCSToString(const char*, const CSID& = CS_Local);
inline String
MBCSToString(const string& s, const CSID& cp = CS_Local)
{
	return MBCSToString(s.c_str(), cp);
}

// YSLib 基本字符串转化为多字节字符串。
string
StringToMBCS(const ustring&, const CSID& = CS_Local);

YSL_END_NAMESPACE(Text)

YSL_END

#endif

