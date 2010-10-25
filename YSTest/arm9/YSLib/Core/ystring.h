// YSLib::Core::YString by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-03-05 22:06:05 + 08:00;
// UTime = 2010-10-24 19:55 + 08:00;
// Version = 0.2948;


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
	//********************************
	//名称:		String
	//全名:		YSLib::Text::String::String
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	默认构造。
	//备注:		
	//********************************
	String();
	//********************************
	//名称:		String
	//全名:		YSLib::Text::String::String
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const uchar_t *
	//功能概要:	构造：使用标准字符指针表示的字符串。
	//备注:		
	//********************************
	String(const uchar_t*);
	//********************************
	//名称:		String
	//全名:		YSLib::Text::String::String<_tChar>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const _tChar *
	//功能概要:	构造：使用字符指针表示的字符串。
	//备注:		
	//********************************
	template<class _tChar>
	String(const _tChar*);
	//********************************
	//名称:		String
	//全名:		YSLib::Text::String::String
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const ustring &
	//功能概要:	构造：使用 YSLib 基本字符串。
	//备注:		
	//********************************
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


//********************************
//名称:		MBCSToString
//全名:		YSLib::Text::MBCSToString
//可访问性:	public 
//返回类型:	YSLib::Text::String
//修饰符:	
//形式参数:	const char *
//形式参数:	const CSID &
//功能概要:	多字节字符串转化为 YSLib 标准字符串。
//备注:		
//********************************
String
MBCSToString(const char*, const CSID& = CS_Local);
//********************************
//名称:		MBCSToString
//全名:		YSLib::Text::MBCSToString
//可访问性:	public 
//返回类型:	YSLib::Text::String
//修饰符:	
//形式参数:	const string & s
//形式参数:	const CSID & cp
//功能概要:	多字节字符串转化为 YSLib 标准字符串。
//备注:		
//********************************
inline String
MBCSToString(const string& s, const CSID& cp = CS_Local)
{
	return MBCSToString(s.c_str(), cp);
}

//********************************
//名称:		StringToMBCS
//全名:		YSLib::Text::StringToMBCS
//可访问性:	public 
//返回类型:	YSLib::string
//修饰符:	
//形式参数:	const ustring &
//形式参数:	const CSID &
//功能概要:	 YSLib 基本字符串转化为多字节字符串。
//备注:		
//********************************
string
StringToMBCS(const ustring&, const CSID& = CS_Local);

YSL_END_NAMESPACE(Text)

YSL_END

#endif

