/*
	© 2010-2016, 2018-2019, 2021, 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YString.h
\ingroup Core
\brief 基础字符串管理。
\version r2751
\author FrankHB <frankhb1989@gmail.com>
\since build 594
\par 创建时间:
	2010-03-05 22:06:05 +0800
\par 修改时间:
	2023-04-04 23:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YString
*/


#ifndef YSL_INC_Core_YString_h_
#define YSL_INC_Core_YString_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YShellDefinition // for module
//	YSLib::Core::YShellDefinition, ystdex::enable_if_t, std::is_same,
//	std::allocator_arg;
#include YFM_YSLib_Adaptor_YTextBase // for u16string, std::basic_string,
//	u16string_view, std::initializer_list, MakeUCS2LE, Encoding, CS_Default,
//	basic_string_view, ystdex::make_string_view, MakeMBCS;
#include <ystdex/allocator.hpp> // for ystdex::rebind_alloc_t;
#include <ystdex/swap.hpp> // for ystdex::copy_and_swap;
#include <ystdex/meta.hpp> // for ystdex::enable_if_t, ystdex::and_,
//	ystdex::is_implicitly_constructible, ystdex::exclude_self_t,
//	std::is_nothrow_constructible, ystdex::enable_if_convertible_t,
//	ystdex::enable_if_inconvertible_t;

namespace YSLib
{

namespace Text
{

/*!
\brief 对应 YSLib 基本字符串的标准库类型。
\note 可隐式初始化 YSLib 基本字符串。
\note 视配置可能和 YSLib 基本字符串类型 u16string 相同。
\since build 929
*/
using std_u16string = std::basic_string<char16_t, u16string::traits_type,
	u16string::allocator_type>;


/*!
\brief YSLib 标准字符串（使用 UCS-2 作为内部编码）。
\note 和 u16string 及 std_u16string 互变。
\note 和 \c char16_t 以外的字符类型的字符串或 basic_string_view 实例不互变。
\warning 非虚析构。
\since 早于 build 132
*/
class YF_API String : public u16string
{
private:
	//! \since build 929
	template<typename _tChar>
	using ralloc_t = ystdex::rebind_alloc_t<allocator_type, _tChar>;
	//! \since build 971
	//!@{
	using base = u16string;
	using sv_type = u16string_view;
	template<typename _tParam>
	using enable_bivariant_t = ystdex::enable_if_t<
		ystdex::and_<ystdex::is_implicitly_constructible<String, _tParam>,
		ystdex::is_implicitly_constructible<_tParam, String>>{}, String&>;
	//!@}

public:
	//! \brief 无参数构造：默认实现。
	DefDeCtor(String)
	// NOTE: Both %base and %std_u16string are bivariant to %String.
	//! \since build 641
	using base::base;
	//! \since build 929
	//!@{
	//! \brief 构造：使用字符的初值符列表和分配器。
	template<typename _tChar>
	String(std::initializer_list<_tChar> il,
		const allocator_type& a = allocator_type())
		: base(il.begin(), il.end(), a)
	{}
	//! \brief 构造：使用 YSLib 基本字符串。
	String(const std_u16string& str)
		: base(str)
	{}
	//! \brief 构造：使用 YSLib 基本字符串和分配器。
	String(const std_u16string& str, const allocator_type& a)
		: base(str, a)
	{}
	//! \brief 构造：使用 YSLib 基本字符串右值引用。
	String(std_u16string&& str) ynothrow
		: base(std::move(str))
	{}
	//! \brief 构造：使用 YSLib 基本字符串右值引用和分配器。
	String(std_u16string&& str, const allocator_type& a)
		: base(std::move(str), a)
	{}
	//!@}
	//! \pre 间接断言：指针参数非空。
	//!@{
	/*!
	\brief 构造：使用非 \c char16_t 的指针类型表示的 NTCTS 和默认编码。
	\since build 836
	*/
	template<typename _tChar, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	YB_NONNULL(2) inline
	String(const _tChar* s)
		: base(MakeUCS2LE<base>(s, CS_Default))
	{}
	/*!
	\brief 构造：使用非 \c char16_t 的指针类型表示的 NTCTS 和默认编码和分配器。
	\since build 929
	*/
	template<typename _tChar, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	YB_NONNULL(2) inline
	String(const _tChar* s, const allocator_type& a)
		: base(MakeUCS2LE<base>(std::allocator_arg, a, s, CS_Default))
	{}
	/*!
	\brief 构造：使用指针表示的 NTCTS 和指定编码。
	\since build 836
	*/
	template<typename _tChar>
	YB_NONNULL(2) inline
	String(const _tChar* s, Encoding enc)
		: base(MakeUCS2LE<base>(s, enc))
	{}
	/*!
	\brief 构造：使用指针表示的 NTCTS 、指定编码和分配器。
	\since build 929
	*/
	template<typename _tChar>
	YB_NONNULL(2) inline
	String(const _tChar* s, Encoding enc, const allocator_type& a)
		: base(MakeUCS2LE<base>(std::allocator_arg, a, s, enc))
	{}
	/*!
	\brief 构造：使用非 \c char16_t 的指针类型表示的 NTCTS 和默认编码。
	\since build 836
	*/
	template<typename _tChar, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	YB_NONNULL(2) inline
	String(const _tChar* s, size_t n)
		: String(basic_string_view<_tChar>(s, n), CS_Default)
	{}
	/*!
	\brief 构造：使用非 \c char16_t 的指针类型表示的 NTCTS 、默认编码和分配器。
	\since build 929
	*/
	template<typename _tChar, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	YB_NONNULL(2) inline
	String(const _tChar* s, size_t n, const allocator_type& a)
		: String(basic_string_view<_tChar>(s, n), CS_Default, a)
	{}
	/*!
	\brief 构造：使用指针表示的字符范围和指定编码。
	\since build 836
	*/
	template<typename _tChar>
	YB_NONNULL(2) inline
	String(const _tChar* s, size_t n, Encoding enc)
		: String(basic_string_view<_tChar>(s, n), enc)
	{}
	/*!
	\brief 构造：使用指针表示的字符范围、指定编码和分配器。
	\since build 929
	*/
	template<typename _tChar>
	YB_NONNULL(2) inline
	String(const _tChar* s, size_t n, Encoding enc, const allocator_type& a)
		: String(basic_string_view<_tChar>(s, n), enc, a)
	{}
	//!@}
	// NOTE: As %basic_string, the constructor having %basic_string_view
	//	instance as the only one of its parameter has 'explicit'.
	/*!
	\brief 构造：使用非 \c char16_t 的字符类型的 basic_string_view 和默认编码。
	\since build 835
	*/
	template<typename _tChar, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	explicit inline
	String(basic_string_view<_tChar> sv)
		: base(MakeUCS2LE<base>(sv, CS_Default))
	{}
	/*!
	\brief 构造：使用非 \c char16_t 的字符类型的 basic_string_view 、默认编码
		和分配器。
	\since build 861
	*/
	template<typename _tChar, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	inline
	String(basic_string_view<_tChar> sv, const allocator_type& a)
		: base(
		MakeUCS2LE<base>(std::allocator_arg, a, sv, CS_Default))
	{}
	/*!
	\brief 构造：使用指定字符类型的 basic_string_view 和指定编码。
	\since build 835
	*/
	template<typename _tChar>
	inline
	String(basic_string_view<_tChar> sv, Encoding enc)
		: base(MakeUCS2LE<base>(sv, enc))
	{}
	/*!
	\brief 构造：使用指定字符类型的 basic_string_view 、指定编码和分配器。
	\since build 861
	*/
	template<typename _tChar>
	inline
	String(basic_string_view<_tChar> sv, Encoding enc, const allocator_type& a)
		: base(MakeUCS2LE<base>(std::allocator_arg, a, sv, enc))
	{}
	// NOTE: Instance of %std::basic_string are used instead of %basic_string
	//	for more conversions. They are not bivariant to %String, because the
	//	implied encoding conversion (by calling to %MakeUCS2LE) is considered
	//	costly and it should not used unless explicitly specified. So there is
	//	'explicit' on the overload of one parameter.
	//! \since build 929
	//!@{
	/*!
	\brief 构造：使用非 \c char16_t 的字符类型的 \c std::basic_string 和默认编码。
	*/
	template<typename _tChar, class _tTraits = std::char_traits<_tChar>,
		class _tAlloc = yimpl(ralloc_t)<_tChar>, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	explicit inline
	String(const std::basic_string<_tChar, _tTraits, _tAlloc>& str)
		: String(str, ConvAlloc(str.get_allocator()))
	{}
	/*!
	\brief 构造：使用非 \c char16_t 的字符类型的 \c std::basic_string 、默认编码
		和分配器。
	*/
	template<typename _tChar, class _tTraits = std::char_traits<_tChar>,
		class _tAlloc = yimpl(ralloc_t)<_tChar>, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	inline
	String(const std::basic_string<_tChar, _tTraits, _tAlloc>& str,
		const allocator_type& a)
		: String(ystdex::make_string_view(str), CS_Default, a)
	{}
	//! \brief 构造：使用指定字符类型的 \c std::basic_string 和指定编码。
	template<typename _tChar, class _tTraits = std::char_traits<_tChar>,
		class _tAlloc = yimpl(ralloc_t)<_tChar>>
	inline
	String(const std::basic_string<_tChar, _tTraits, _tAlloc>& str,
		Encoding enc)
		: String(str, enc, ConvAlloc(str.get_allocator()))
	{}
	//! \brief 构造：使用指定字符类型的 \c std::basic_string 、指定编码和分配器。
	template<typename _tChar, class _tTraits = std::char_traits<_tChar>,
		class _tAlloc = yimpl(ralloc_t)<_tChar>>
	inline
	String(const std::basic_string<_tChar, _tTraits, _tAlloc>& s, Encoding enc,
		const allocator_type& a)
		: String(ystdex::make_string_view(s), enc, a)
	{}
	//!@}
	//! \since build 971
	//!@{
	//! \brief 复制构造：传播分配器。
	String(const String& str)
		: String(str, str.get_allocator())
	{}
	//! \brief 复制构造：使用参数和参数指定的分配器。
	String(const String& str, const allocator_type& a)
		: base(str, a)
	{}
	//!@}
	//! \brief 转移构造：默认实现。
	DefDeMoveCtor(String)
	/*!
	\brief 转移构造：使用参数和参数指定的分配器。
	\since build 971
	*/
	String(String&& str, const allocator_type& a) ynothrow
		: base(std::move(str), a)
	{}
	DefDeDtor(String)

	//! \since build 971
	//!@{
	PDefHOp(String&, =, sv_type sv)
		ImplRet(*this = String(sv, get_allocator()))
	YB_NONNULL(2) PDefHOp(String&, =, const char16_t* s)
		ImplRet(*this = String(s, get_allocator()))
	PDefHOp(String&, =, char16_t c)
		ImplRet(*this = String(1, c, get_allocator()))
	PDefHOp(String&, =, std::initializer_list<char16_t> il)
		ImplRet(*this = String(il, get_allocator()))
	template<class _tParam,
		yimpl(typename = ystdex::exclude_self_t<String, _tParam>)>
	enable_bivariant_t<_tParam>
	operator=(_tParam&& str) ynoexcept(std::is_nothrow_constructible<
		std_u16string, _tParam, const allocator_type&>())
	{
		static_cast<std_u16string&>(*this)
			= std_u16string(yforward(str), get_allocator());
		return *this;
	}
	template<typename _tChar, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	inline String&
	operator=(basic_string_view<_tChar> sv)
	{
		static_cast<base&>(*this) = MakeUCS2LE<base>(sv, CS_Default);
		return *this;
	}
	template<typename _tChar, class _tTraits = std::char_traits<_tChar>,
		class _tAlloc = yimpl(ralloc_t)<_tChar>, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	inline String&
	operator=(const std::basic_string<_tChar, _tTraits, _tAlloc>& str)
	{
		return *this = String(str, ConvAlloc(str.get_allocator()));
	}
	//!@}
	//! \since build 295
	//!@{
	//! \brief 复制赋值：使用参数副本和交换操作。
	PDefHOp(String&, =, const String& str)
		ImplRet(ystdex::copy_and_swap(*this, str))
	//! \brief 转移赋值：默认实现。
	DefDeMoveAssignment(String)
	//!@}

	//! \since build 971
	//!@{
	PDefHOp(String&, +=, sv_type sv)
		ImplRet(*this += String(sv, get_allocator()))
	YB_NONNULL(2) PDefHOp(String&, +=, const char16_t* s)
		ImplRet(static_cast<base&>(*this) += s, *this)
	PDefHOp(String&, +=, char16_t c)
		ImplRet(static_cast<base&>(*this) += c, *this)
	PDefHOp(String&, +=, std::initializer_list<char16_t> il)
		ImplRet(static_cast<base&>(*this) += il, *this)
	template<class _tParam>
	enable_bivariant_t<_tParam>
	operator+=(const _tParam& str)
	{
		static_cast<std_u16string&>(*this)
			+= std_u16string(str, get_allocator());
		return *this;
	}
	template<typename _tChar, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	inline String&
	operator+=(basic_string_view<_tChar> sv)
	{
		static_cast<base&>(*this) += MakeUCS2LE<base>(sv, CS_Default);
		return *this;
	}
	template<typename _tChar, class _tTraits = std::char_traits<_tChar>,
		class _tAlloc = yimpl(ralloc_t)<_tChar>, yimpl(typename
		= ystdex::enable_if_t<!std::is_same<_tChar, char16_t>::value>)>
	inline String&
	operator+=(const std::basic_string<_tChar, _tTraits, _tAlloc>& str)
	{
		return *this += ystdex::make_obj_using_allocator<String>(
			str.get_allocator(), str);
	}
	//!@}

	/*!
	\brief 重复串接。
	\since build 413
	*/
	String&
	operator*=(size_t);

private:
	//! \since build 971
	template<class _tAlloc, yimpl(typename = ystdex::enable_if_convertible_t<
		const _tAlloc&, const allocator_type&>)>
	YB_ATTR_nodiscard YB_STATELESS yconstfn static const _tAlloc&
	ConvAlloc(const _tAlloc& a) ynothrow
	{
		return a;
	}
	//! \since build 971
	template<class _tAlloc, yimpl(ystdex::enable_if_inconvertible_t<
		const _tAlloc&, const allocator_type&, int> = 0)>
	YB_ATTR_nodiscard YB_STATELESS static inline allocator_type
	ConvAlloc(const _tAlloc&) ynothrow
	{
		return {};
	}

public:
	/*!
	\brief 取指定编码的多字节字符串。
	\since build 287
	*/
	PDefH(string, GetMBCS, Encoding enc = CS_Default) const
		ImplRet(
			MakeMBCS<string>(std::allocator_arg, get_allocator(), *this, enc))

	//! \since build 834
	friend DefSwap(ynothrow, String, ystdex::swap_dependent(
		static_cast<base&>(_x), static_cast<base&>(_y)))
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

