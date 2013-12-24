/*
	© 2009-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file chrmap.h
\ingroup CHRLib
\brief 字符映射。
\version r1282
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-17 17:52:35 +0800
\par 修改时间:
	2013-12-24 09:44 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::CharacterMapping
*/


#ifndef INC_CHRLib_chrmap_h_
#define INC_CHRLib_chrmap_h_ 1

#include "YModules.h"
#include YFM_CHRLib_Encoding
#include <ystdex/cassert.h>

namespace CHRLib
{

using CharSet::Encoding;


/*!
\brief 默认字符编码。
\since build 287
*/
yconstexpr Encoding CS_Default = CharSet::UTF_8;


/*!
\brief 取 c_ptr 指向的大端序双字节字符。
\pre 断言： \c c_ptr 。
*/
inline ucs2_t
FetchBiCharBE(const char* c_ptr)
{
	yconstraint(c_ptr);

	return (*c_ptr << CHAR_BIT) | c_ptr[1];
}

/*!
\brief 取 c_ptr 指向的小端序双字节字符。
\pre 断言： \c c_ptr 。
*/
inline ucs2_t
FetchBiCharLE(const char* c_ptr)
{
	yconstraint(c_ptr);

	return (c_ptr[1] << CHAR_BIT) | *c_ptr;
}


/*!
\brief 编码转换结果。
\since build 273
*/
enum class ConversionResult
{
	OK = 0, //!< 转换成功。
	BadState, //!< 转换状态错误。
	BadSource, //!< 源数据不可达（如越界）。
	Invalid, //!< 数据校验失败（如不构成代码点的字节序列）。
	Unhandled //!< 未处理（超过被处理的界限）。
};


/*!
\brief 编码转换状态。
\since build 249
*/
struct ConversionState
{
	/*!
	\brief 当前已转换字符计数。
	*/
	std::uint_fast8_t Count;
	union
	{
		ucsint_t Wide;
		byte Sequence[4];
	} Value;

	yconstfn
	ConversionState(size_t n= 0)
		: Count(n), Value()
	{}
};

//! \relates ConversionState
//@{
yconstfn std::uint_fast8_t&
GetCountOf(ConversionState& st)
{
	return st.Count;
}
yconstfn ucsint_t&
GetWideOf(ConversionState& st)
{
	return st.Value.Wide;
}
yconstfn byte*
GetSequenceOf(ConversionState& st)
{
	return st.Value.Sequence;
}
//@}


/*!
\brief 一般类型计数。
\since build 273
*/
template<typename _type>
yconstfn _type&
GetCountOf(_type& st)
{
	return st;
}


/*!
\brief 取指定固定编码的固定字符宽度。
\return 未定义编码或变长编码返回 0 ，否则为指定编码中每个字符占用的字节数。
\note UTF-16 视为 UCS-2 。
\since build 273
*/
YF_API size_t
FetchFixedCharWidth(Encoding);

/*!
\brief 取指定编码的最大字符宽度。
\return 未定义编码返回 0 ，否则为指定编码中每个字符最大可能占用的字节数。
\note UTF-16 视为 UCS-2 。
\since build 273
*/
YF_API size_t
FetchMaxCharWidth(Encoding);

/*!
\brief 取指定变长编码的最大字符宽度。
\return 未定义编码或固定编码返回 0 ，否则为指定编码中每个字符最大可能占用的字节数。
\note UTF-16 视为 UCS-2 。
\since build 273
*/
YF_API size_t
FetchMaxVariantCharWidth(Encoding);

} // namespace CHRLib;

#endif

