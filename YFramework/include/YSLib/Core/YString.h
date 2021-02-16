/*
	© 2010-2016, 2018-2019, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YString.h
\ingroup Core
\brief 基础字符串管理。
\version r2370
\author FrankHB <frankhb1989@gmail.com>
\since build 594
\par 创建时间:
	2010-03-05 22:06:05 +0800
\par 修改时间:
	2021-02-06 22:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YString
*/


#ifndef YSL_INC_Core_YString_h_
#define YSL_INC_Core_YString_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YShellDefinition // for module
//	YSLib::Core::YShellDefinition, allocator_arg;
#include YFM_YSLib_Adaptor_YTextBase // for u16string;

namespace YSLib
{

namespace Text
{

/*!
\brief YSLib 标准字符串（使用 UCS-2 作为内部编码）。
\warning 非虚析构。
\since 早于 build 132
*/
class YF_API String : public u16string
{
public:
	/*!
	\brief 无参数构造：默认实现。
	*/
	DefDeCtor(String)
	//! \since build 641
	//@{
	using u16string::u16string;
	//! \brief 构造：使用 YSLib 基本字符串。
	String(const u16string& s)
		: u16string(s)
	{}
	//! \brief 构造：使用 YSLib 基本字符串右值引用。
	String(u16string&& s)
		: u16string(std::move(s))
	{}
	//@}
	/*!
	\pre 间接断言：指针参数非空。
	\since build 836
	*/
	//@{
	//! \brief 构造：使用非 char16_t 的指针类型表示的 NTCTS 和默认编码。
	template<typename _tChar, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	YB_NONNULL(2)
	String(const _tChar* s)
		: u16string(MakeUCS2LE<u16string>(s, CS_Default))
	{}
	//! \brief 构造：使用指针表示的 NTCTS 和指定编码。
	template<typename _tChar>
	YB_NONNULL(2)
	String(const _tChar* s, Encoding enc)
		: u16string(MakeUCS2LE<u16string>(s, enc))
	{}
	//! \brief 构造：使用非 char16_t 的指针类型表示的 NTCTS 和默认编码。
	template<typename _tChar, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	YB_NONNULL(2)
	String(const _tChar* s, size_t n)
		: String(basic_string_view<_tChar>(s, n), CS_Default)
	{}
	//! \brief 构造：使用指针表示的字符范围和指定编码。
	template<typename _tChar>
	YB_NONNULL(2)
	String(const _tChar* s, size_t n, Encoding enc)
		: String(basic_string_view<_tChar>(s, n), enc)
	{}
	//@}
	//! \since build 835
	//@{
	//! \brief 构造：使用非 char16_t 的字符类型的 basic_string_view 和默认编码。
	template<typename _tChar, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	explicit
	String(basic_string_view<_tChar> sv)
		: u16string(MakeUCS2LE<u16string>(sv, CS_Default))
	{}
	//! \brief 构造：使用指定字符类型的 basic_string_view 和指定编码。
	template<typename _tChar>
	explicit
	String(basic_string_view<_tChar> sv, Encoding enc)
		: u16string(MakeUCS2LE<u16string>(sv, enc))
	{}
	//! \brief 构造：使用非 char16_t 的字符类型的 basic_string 和默认编码。
	template<typename _tChar, class _tTraits = std::char_traits<_tChar>,
		class _tAlloc = std::allocator<_tChar>, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	String(const basic_string<_tChar, _tTraits, _tAlloc>& s)
		: String(basic_string_view<_tChar>(s), CS_Default)
	{}
	//! \brief 构造：使用指定字符类型的 basic_string 和指定编码。
	template<typename _tChar, class _tTraits = std::char_traits<_tChar>,
		class _tAlloc = std::allocator<_tChar>>
	String(const basic_string<_tChar, _tTraits, _tAlloc>& s,
		Encoding enc)
		: String(basic_string_view<_tChar>(s), enc)
	{}
	//@}
	//! \since build 861
	//@{
	/*!
	\brief 构造：使用非 char16_t 的字符类型的 basic_string_view 、默认编码
		和分配器。
	*/
	template<typename _tChar, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	explicit
	String(basic_string_view<_tChar> sv, const allocator_type& a)
		: u16string(MakeUCS2LE<u16string>(std::allocator_arg, a, sv,
		CS_Default))
	{}
	//! \brief 构造：使用指定字符类型的 basic_string_view 、指定编码和分配器。
	template<typename _tChar>
	explicit
	String(basic_string_view<_tChar> sv, Encoding enc, const allocator_type& a)
		: u16string(MakeUCS2LE<u16string>(std::allocator_arg, a, sv, enc))
	{}
	//! \brief 构造：使用非 char16_t 的字符类型的 basic_string 、默认编码和分配器。
	template<typename _tChar, class _tTraits = std::char_traits<_tChar>,
		class _tAlloc = std::allocator<_tChar>, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	String(const basic_string<_tChar, _tTraits, _tAlloc>& s,
		const allocator_type& a)
		: String(basic_string_view<_tChar>(s), CS_Default, a)
	{}
	//! \brief 构造：使用指定字符类型的 basic_string 、指定编码和分配器。
	template<typename _tChar, class _tTraits = std::char_traits<_tChar>,
		class _tAlloc = std::allocator<_tChar>>
	String(const basic_string<_tChar, _tTraits, _tAlloc>& s,
		Encoding enc, const allocator_type& a)
		: String(basic_string_view<_tChar>(s), enc, a)
	{}
	//@}
	/*!
	\brief 构造：使用字符的初值符列表。
	\since build 510
	*/
	template<typename _tChar>
	String(std::initializer_list<_tChar> il)
		: u16string(il.begin(), il.end())
	{}
	/*!
	\brief 复制构造：默认实现。
	*/
	DefDeCopyCtor(String)
	/*!
	\brief 转移构造：默认实现。
	*/
	DefDeMoveCtor(String)
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

	//! \since build 834
	friend DefSwap(ynothrow, String, ystdex::swap_dependent(
		static_cast<u16string&>(_x), static_cast<u16string&>(_y)))

	/*!
	\brief 取指定编码的多字节字符串。
	\since build 287
	*/
	PDefH(string, GetMBCS, Encoding enc = CS_Default) const
		ImplRet(MakeMBCS<string>(std::allocator_arg, get_allocator(), *this,
			enc))
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

