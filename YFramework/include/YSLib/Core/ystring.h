/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ystring.h
\ingroup Core
\brief 基础字符串管理。
\version r2169
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-05 22:06:05 +0800
\par 修改时间:
	2014-07-09 08:07 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YString
*/


#ifndef YSL_INC_Core_ystring_h_
#define YSL_INC_Core_ystring_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YObject
#include YFM_YSLib_Adaptor_YContainer
#include YFM_YSLib_Adaptor_YTextBase

namespace YSLib
{

namespace Text
{

/*!
\brief YSLib 标准字符串（使用 UCS-2 作为内部编码）。
\warning 非虚析构。
\since 早于 build 132
*/
class YF_API String : public ucs2string
{
public:
	/*!
	\brief 无参数构造：默认实现。
	*/
	DefDeCtor(String)
	/*!
	\brief 复制构造：默认实现。
	*/
	DefDeCopyCtor(String)
	/*!
	\brief 转移构造：默认实现。
	*/
	DefDeMoveCtor(String)
	/*!
	\brief 构造：使用 UCS-2 字符指针表示的等宽 NTCTS 。
	*/
	String(const ucs2_t* s)
		: ucs2string(s)
	{}
	/*!
	\brief 构造：使用字符指针表示的 NTCTS 和指定编码。
	\since build 281
	*/
	template<typename _tChar>
	String(const _tChar* s, Encoding enc = CS_Default)
		: ucs2string(MakeUCS2LEString<ucs2string>(s, enc))
	{}
	/*!
	\brief 构造：使用字符的初值符列表。
	\since build 510
	*/
	template<typename _tChar>
	String(std::initializer_list<_tChar> il)
		: ucs2string(il.begin(), il.end())
	{}
	/*!
	\brief 构造：使用 YSLib 基本字符串。
	*/
	String(const ucs2string& s)
		: ucs2string(s)
	{}
	/*!
	\brief 构造：使用 YSLib 基本字符串右值引用。
	\since build 285
	*/
	String(ucs2string&& s)
		: ucs2string(std::move(s))
	{}
	/*!
	\brief 构造：使用指定字符类型的 std::basic_string 和指定编码。
	\since build 281
	*/
	template<typename _tChar>
	String(const std::basic_string<_tChar>& s, Encoding enc = CS_Default)
		: String(s.c_str(), enc)
	{}
	DefDeDtor(String)

	/*!
	\brief 复制赋值：默认实现。
	\since build 295
	*/
	DefDeCopyAssignment(String)
	/*!
	\brief 转移赋值：默认实现。
	\since build 295
	*/
	DefDeMoveAssignment(String)

	/*!
	\brief 重复串接。
	\since build 413
	*/
	String&
	operator*=(size_t);

	/*!
	\brief 取指定编码的多字节字符串。
	\since build 287
	*/
	PDefH(string, GetMBCS, Encoding enc = CS_Default) const
		ImplRet(GetMBCSOf<string>(*this, enc))
};

/*!
\relates String
\since build 516
*/
inline PDefHOp(String, *, const String& str, size_t n)
	ImplRet(String(str) *= n)

} // namespace Text;

} // namespace YSLib;

#endif

