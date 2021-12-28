/*
	© 2009-2015, 2018-2019, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file CharacterProcessing.h
\ingroup CHRLib
\brief 字符编码处理。
\version r2416
\author FrankHB <frankhb1989@gmail.com>
\since build 565
\par 创建时间:
	2009-11-17 17:52:35 +0800
\par 修改时间:
	2021-12-27 03:15 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	CHRLib::CharacterProcessing
*/


#ifndef INC_CHRLib_CharacterProcessing_h_
#define INC_CHRLib_CharacterProcessing_h_ 1

#include "YModules.h"
#include YFM_CHRLib_CharacterMapping
#include <ystdex/cstring.h> // for ystdex::uchar_t;
#include <ystdex/string.hpp> // for ystdex::is_null, ystdex::ntctslen,
//	ystdex::string_traits, std::allocator_arg_t,
//	ystdex::make_obj_using_allocator, std::basic_string, std::allocator_arg;
#include <cstdio> // for std::FILE;
#include <memory> // for std::move;

namespace CHRLib
{

//! \since build 630
using ystdex::uchar_t;


/*!
\brief 判断整数类型字符在 ASCII 字符取值范围内。
\note 截取低 7 位。
*/
template<typename _tChar>
YB_ATTR_nodiscard YB_STATELESS yconstfn bool
IsASCII(_tChar c)
{
	return !(c & ~0x7F);
}

/*!
\brief 任意整数类型字符转换为 ASCII 取值范围兼容的字符。
\note 截取低 7 位。
*/
template<typename _tChar>
YB_ATTR_nodiscard YB_STATELESS yconstfn char
ToASCII(_tChar c)
{
	static_assert(std::is_integral<_tChar>(), "Invalid type found.");

	return c & 0x7F;
}


//! \return 转换的字节数。
//@{
//! \since build 641
//@{
//! \brief 按指定编码和转换状态转换字符串中的字符为 UCS-2 字符。
//@{
YF_API ConversionResult
MBCToUC(char16_t&, const char*&, Encoding, ConversionState&& = {});
YF_API ConversionResult
MBCToUC(char16_t&, const char*&, const char*, Encoding, ConversionState&& = {});
inline PDefH(ConversionResult, MBCToUC, char16_t& uc, const char*& c,
	Encoding enc, ConversionState& st)
	ImplRet(MBCToUC(uc, c, enc, std::move(st)))
//@}
/*!
\brief 按指定编码和转换状态转换字符流中的字符为 UCS-2 字符。
\pre 断言：指针参数非空。
*/
//@{
YF_API YB_NONNULL(2) ConversionResult
MBCToUC(char16_t&, std::FILE*, Encoding, ConversionState&& = {});
inline PDefH(ConversionResult, MBCToUC, char16_t& uc, std::FILE* fp, Encoding enc,
	ConversionState& st)
	ImplRet(MBCToUC(uc, fp, enc, std::move(st)))
//@}
//@}
/*!
\brief 按指定编码和转换状态返回转换字符为 UCS-2 字符的字节数。
\since build 291
*/
//@{
YF_API ConversionResult
MBCToUC(const char*&, Encoding, ConversionState&& = {});
//! \since build 614
YF_API ConversionResult
MBCToUC(const char*&, const char*, Encoding, ConversionState&& = {});
inline PDefH(ConversionResult, MBCToUC, const char*& c, Encoding enc,
	ConversionState& st)
	ImplRet(MBCToUC(c, enc, std::move(st)))
//! \since build 614
inline PDefH(ConversionResult, MBCToUC, const char*& c, const char* e,
	Encoding enc, ConversionState& st)
	ImplRet(MBCToUC(c, e, enc, std::move(st)))
//! \pre 断言：指针参数非空。
//@{
YF_API YB_NONNULL(1) ConversionResult
MBCToUC(std::FILE*, Encoding, ConversionState&& = {});
YB_NONNULL(1) inline PDefH(ConversionResult, MBCToUC, std::FILE* fp,
	Encoding enc, ConversionState& st)
	ImplRet(MBCToUC(fp, enc, std::move(st)))
//@}
//@}

/*!
\brief 按指定编码转换 UCS-2 字符为字符串表示的多字节字符。
\pre 断言：指针参数非空 。
\pre 第一参数指向的缓冲区能容纳转换后字符序列。
\since build 641
*/
YF_API YB_NONNULL(1) size_t
UCToMBC(char*, const char16_t&, Encoding);
//@}


//! \note 编码字节序同实现的 char16_t 存储字节序。
//@{
//! \since build 645
//@{
template<typename _func, typename _tDst, typename _tSrc>
YB_NONNULL(2, 3) size_t
TranscodeWith(_func f, _tDst* d, const _tSrc* s)
{
	yconstraint(d),
	yconstraint(s);

	const auto p(d);

	f(d, s);
	*d = _tDst();
	return size_t(d - p);
}
template<typename _func, typename _tDst, typename _tSrc>
YB_NONNULL(2, 3, 4) size_t
TranscodeWith(_func f, _tDst* d, const _tSrc* s, const _tSrc* e)
{
	yconstraint(d),
	yconstraint(s),
	yconstraint(e),
	yconstraint(s <= e);

	const auto p(d);

	f(d, s, e);
	return size_t(d - p);
}
//@}

//! \since build 644
//@{
template<typename _func, typename _tDst, typename _tSrc>
YB_NONNULL(2, 3) size_t
DecodeWith(_func f, _tDst* d, const _tSrc* s)
{
	return CHRLib::TranscodeWith([&](_tDst*& dst, const _tSrc*& src){
		while(!ystdex::is_null(*src) && f(*dst, src))
			++dst;
	}, d, s);
}
template<typename _func, typename _tDst, typename _tSrc>
YB_NONNULL(2, 3, 4) size_t
DecodeWith(_func f, _tDst* d, const _tSrc* s, const _tSrc* e)
{
	return CHRLib::TranscodeWith(
		[&](_tDst*& dst, const _tSrc*& src, const _tSrc*& end){
		while(!ystdex::is_null(*src) && f(*dst, src, end))
			++dst;
	}, d, s, e);
}

template<typename _func, typename _tDst, typename _tSrc>
YB_NONNULL(2, 3) size_t
EncodeWith(_func f, _tDst* d, const _tSrc* s)
{
	return CHRLib::TranscodeWith([&](char*& dst, const _tSrc*& src){
		while(!ystdex::is_null(*src))
			f(dst, src);
	}, d, s);
}
template<typename _func, typename _tDst, typename _tSrc>
YB_NONNULL(2, 3, 4) size_t
EncodeWith(_func f, _tDst* d, const _tSrc* s, const _tSrc* e)
{
	return CHRLib::TranscodeWith(
		[&](_tDst*& dst, const _tSrc*& src, const _tSrc* end){
		// TODO: Deferred. Use guard for encoding.
		while(src < end)
			f(dst, src, end);
	}, d, s, e);
}
//@}

/*!
\pre 断言：指针参数非空 。
\pre 第一参数指向的缓冲区能容纳转换后的 NTCTS （包括结尾的空字符）。
\pre 指针参数指向的缓冲区不重叠。
\return 转换的串长。
\since build 645
*/
//@{
//! \brief 按指定编码转换 MBCS 字符串为 UCS-2 字符串。
//@{
template<typename _func>
YB_NONNULL(2, 3) size_t
MBCSToUCS2(_func f, char16_t* d, const char* s)
{
	return CHRLib::DecodeWith([&](char16_t& dc, const char*& src) -> bool{
		return f(dc, src, ConversionState()) == ConversionResult::OK;
	}, d, s);
}
template<typename _func>
YB_NONNULL(2, 3, 4) size_t
MBCSToUCS2(_func f, char16_t* d, const char* s, const char* e)
{
	return CHRLib::DecodeWith(
		[&](char16_t& dc, const char*& src, const char*& end) -> bool{
		return f(dc, {src, end}, ConversionState()) == ConversionResult::OK;
	}, d, s, e);
}
YF_API YB_FLATTEN YB_NONNULL(1, 2) size_t
MBCSToUCS2(char16_t*, const char*);
YF_API YB_NONNULL(1, 2) size_t
MBCSToUCS2(char16_t*, const char*, Encoding);
YF_API YB_FLATTEN YB_NONNULL(1, 2, 3) size_t
MBCSToUCS2(char16_t*, const char*, const char* e);
YF_API YB_NONNULL(1, 2, 3) size_t
MBCSToUCS2(char16_t*, const char*, const char* e, Encoding);
//@}

//! \brief 按指定编码转换 MBCS 字符串为 UCS-4 字符串。
//@{
template<typename _func>
YB_NONNULL(2, 3) size_t
MBCSToUCS4(_func f, char32_t* d, const char* s)
{
	return CHRLib::DecodeWith([&](char32_t& dc, const char*& src) -> bool{
		// TODO: Necessary initialization?
		char16_t c;

		if(f(c, src, ConversionState()) == ConversionResult::OK)
		{
			dc = c;
			return true;
		}
		return {};
	}, d, s);
}
template<typename _func>
YB_NONNULL(2, 3, 4) size_t
MBCSToUCS4(_func f, char32_t* d, const char* s, const char* e)
{
	return CHRLib::DecodeWith(
		[&](char32_t& dc, const char*& src, const char*& end) -> bool{
		// TODO: Necessary initialization?
		char16_t c;

		if(f(c, {src, end}, ConversionState()) == ConversionResult::OK)
		{
			dc = c;
			return true;
		}
		return {};
	}, d, s, e);
}
YF_API YB_FLATTEN YB_NONNULL(1, 2) size_t
MBCSToUCS4(char32_t*, const char*);
YF_API YB_NONNULL(1, 2) size_t
MBCSToUCS4(char32_t*, const char*, Encoding);
YF_API YB_FLATTEN YB_NONNULL(1, 2, 3) size_t
MBCSToUCS4(char32_t*, const char*, const char*);
YF_API YB_NONNULL(1, 2, 3) size_t
MBCSToUCS4(char32_t*, const char*, const char*, Encoding);
//@}

//! \brief 按指定编码转换 UCS-2 字符串为 MBCS 字符串。
//@{
//! \since build 644
//@{
template<typename _func>
YB_NONNULL(2, 3) size_t
UCS2ToMBCS(_func f, char* d, const char16_t* s)
{
	return CHRLib::EncodeWith([&](char*& dst, const char16_t*& src){
		dst += f(dst, *src++);
	}, d, s);
}
template<typename _func>
YB_NONNULL(2, 3, 4) size_t
UCS2ToMBCS(_func f, char* d, const char16_t* s, const char16_t* e)
{
	return CHRLib::EncodeWith(
		[&](char*& dst, const char16_t*& src, const char16_t*){
		dst += f(dst, *src++);
	}, d, s, e);
}
//@}
YF_API YB_FLATTEN YB_NONNULL(1, 2) size_t
UCS2ToMBCS(char*, const char16_t*);
YF_API YB_NONNULL(1, 2) size_t
UCS2ToMBCS(char*, const char16_t*, Encoding);
YF_API YB_FLATTEN YB_NONNULL(1, 2, 3) size_t
UCS2ToMBCS(char*, const char16_t*, const char16_t*);
YF_API YB_NONNULL(1, 2, 3) size_t
UCS2ToMBCS(char*, const char16_t*, const char16_t*, Encoding);
//@}

/*!
\brief 转换 UCS-2 字符串为 UCS-4 字符串。
\since build 641
*/
//@{
YF_API YB_NONNULL(1, 2) size_t
UCS2ToUCS4(char32_t*, const char16_t*);
YF_API YB_NONNULL(1, 2, 3) size_t
UCS2ToUCS4(char32_t*, const char16_t*, const char16_t*);
//@}

//! \brief 按指定编码转换 UCS-4 字符串为 MBCS 字符串。
//@{
//! \since build 644
//@{
template<typename _func>
YB_NONNULL(2, 3) size_t
UCS4ToMBCS(_func f, char* d, const char32_t* s)
{
	return CHRLib::EncodeWith([&](char*& dst, const char32_t*& src){
		dst += f(dst, char16_t(*src++));
	}, d, s);
}
template<typename _func>
YB_NONNULL(2, 3, 4) size_t
UCS4ToMBCS(_func f, char* d, const char32_t* s, const char32_t* e)
{
	return CHRLib::EncodeWith(
		[&](char*& dst, const char32_t*& src, const char32_t*){
		dst += f(dst, char16_t(*src++));
	}, d, s, e);
}
//@}
YF_API YB_FLATTEN YB_NONNULL(1, 2) size_t
UCS4ToMBCS(char*, const char32_t*);
YF_API YB_NONNULL(1, 2) size_t
UCS4ToMBCS(char*, const char32_t*, Encoding);
YF_API YB_FLATTEN YB_NONNULL(1, 2, 3) size_t
UCS4ToMBCS(char*, const char32_t*, const char32_t*);
YF_API YB_NONNULL(1, 2, 3) size_t
UCS4ToMBCS(char*, const char32_t*, const char32_t*, Encoding);
//@}

/*!
\brief 转换 UCS-4 字符串为 UCS-2 字符串。
\since build 641
*/
//@{
YF_API YB_NONNULL(1, 2) size_t
UCS4ToUCS2(char16_t*, const char32_t*);
YF_API YB_NONNULL(1, 2, 3) size_t
UCS4ToUCS2(char16_t*, const char32_t*, const char32_t*);
//@}
//@}


//! \since build 645
//@{
template<Encoding, Encoding>
struct DispatchTranscoding;

#define CHRLib_Impl_DispatchTranscoding(_vFrom, _vTo, _n) \
	template<> \
	struct DispatchTranscoding<CharSet::_vFrom, CharSet::_vTo> \
	{ \
		template<typename... _tParams> \
		static size_t \
		Transcode(_tParams&&... args) \
		{ \
			return _n(yforward(args)...); \
		} \
	};

CHRLib_Impl_DispatchTranscoding(Null, ISO_10646_UCS_2, MBCSToUCS2)

CHRLib_Impl_DispatchTranscoding(Null, ISO_10646_UCS_4, MBCSToUCS4)

CHRLib_Impl_DispatchTranscoding(ISO_10646_UCS_2, Null, UCS2ToMBCS)

CHRLib_Impl_DispatchTranscoding(ISO_10646_UCS_2, ISO_10646_UCS_4, UCS2ToUCS4)

CHRLib_Impl_DispatchTranscoding(ISO_10646_UCS_4, Null, UCS4ToMBCS)

CHRLib_Impl_DispatchTranscoding(ISO_10646_UCS_4, ISO_10646_UCS_2, UCS4ToUCS2)

#undef CHRLib_Impl_DispatchTranscoding


template<Encoding _vFrom, Encoding _vTo,
	class _tDispatcher = DispatchTranscoding<_vFrom, _vTo>>
struct Transcoding
{
	template<typename _tDst, typename _tChar, typename... _tParams>
	YB_ATTR_nodiscard YB_PURE static _tDst
	MakeTranscoded(const _tChar* s, _tParams&&... args)
	{
		_tDst str(ystdex::ntctslen(s),
			typename ystdex::string_traits<_tDst>::value_type());

		str.resize(_tDispatcher::Transcode(&str[0], s, yforward(args)...));
		return str;
	}
	//! \since build 861
	template<typename _tDst, typename _tChar, typename... _tParams>
	YB_ATTR_nodiscard YB_PURE static _tDst
	MakeTranscoded(std::allocator_arg_t, const typename
		_tDst::allocator_type& a, const _tChar* s, _tParams&&... args)
	{
		auto str(ystdex::make_obj_using_allocator<_tDst>(a, ystdex::ntctslen(s),
			typename ystdex::string_traits<_tDst>::value_type()));

		str.resize(_tDispatcher::Transcode(&str[0], s, yforward(args)...));
		return str;
	}

	template<typename _tDst, typename _type, typename... _tParams>
	YB_ATTR_nodiscard YB_PURE static _tDst
	MakeViewTranscoded(_type sv, _tParams&&... args)
	{
		const auto s(sv.data());

		yconstraint(s);

		const auto l(sv.length());
		_tDst str(l, typename ystdex::string_traits<_tDst>::value_type());

		str.resize(_tDispatcher::Transcode(&str[0], s, s + l,
			yforward(args)...));
		return str;
	}
	//! \since build 861
	template<typename _tDst, typename _type, typename... _tParams>
	YB_ATTR_nodiscard YB_PURE static _tDst
	MakeViewTranscoded(std::allocator_arg_t,
		const typename _tDst::allocator_type& a, _type sv, _tParams&&... args)
	{
		const auto s(sv.data());

		yconstraint(s);

		const auto l(sv.length());
		auto str(ystdex::make_obj_using_allocator<_tDst>(a, l,
			typename ystdex::string_traits<_tDst>::value_type()));

		str.resize(_tDispatcher::Transcode(&str[0], s, s + l,
			yforward(args)...));
		return str;
	}
};

template<Encoding _vFrom, class _tDispatcher>
struct Transcoding<_vFrom, CharSet::Null, _tDispatcher>
{
	template<typename _tDst, typename _tChar, typename... _tParams>
	YB_ATTR_nodiscard YB_PURE static _tDst
	MakeTranscoded(const _tChar* s)
	{
		const auto w(FetchMaxCharWidth(CS_Default));
		_tDst str(ystdex::ntctslen(s) * (w == 0 ? sizeof(ucsint_t) : w),
			typename ystdex::string_traits<_tDst>::value_type());

		str.resize(_tDispatcher::Transcode(&str[0], s));
		return str;
	}
	template<typename _tDst, typename _tChar, typename... _tParams>
	YB_ATTR_nodiscard YB_PURE static _tDst
	MakeTranscoded(const _tChar* s, Encoding enc, _tParams&&... args)
	{
		const auto w(FetchMaxCharWidth(enc));
		_tDst str(ystdex::ntctslen(s) * (w == 0 ? sizeof(ucsint_t) : w),
			typename ystdex::string_traits<_tDst>::value_type());

		str.resize(_tDispatcher::Transcode(&str[0], s, enc, yforward(args)...));
		return str;
	}
	//! \since build 861
	template<typename _tDst, typename _tChar, typename... _tParams>
	YB_ATTR_nodiscard YB_PURE static _tDst
	MakeTranscoded(std::allocator_arg_t,
		const typename _tDst::allocator_type& a, const _tChar* s)
	{
		const auto w(FetchMaxCharWidth(CS_Default));
		auto str(ystdex::make_obj_using_allocator<_tDst>(a,
			ystdex::ntctslen(s) * (w == 0 ? sizeof(ucsint_t) : w),
			typename ystdex::string_traits<_tDst>::value_type()));

		str.resize(_tDispatcher::Transcode(&str[0], s));
		return str;
	}
	//! \since build 861
	template<typename _tDst, typename _tChar, typename... _tParams>
	YB_ATTR_nodiscard YB_PURE static _tDst
	MakeTranscoded(std::allocator_arg_t, const typename _tDst::allocator_type&
		a, const _tChar* s, Encoding enc, _tParams&&... args)
	{
		const auto w(FetchMaxCharWidth(enc));
		auto str(ystdex::make_obj_using_allocator<_tDst>(a, ystdex::ntctslen(s)
			* (w == 0 ? sizeof(ucsint_t) : w),
			typename ystdex::string_traits<_tDst>::value_type()));

		str.resize(_tDispatcher::Transcode(&str[0], s, enc, yforward(args)...));
		return str;
	}

	template<typename _tDst, typename _type, typename... _tParams>
	YB_ATTR_nodiscard YB_PURE static _tDst
	MakeViewTranscoded(_type sv)
	{
		const auto s(sv.data());

		yconstraint(s);

		const auto l(sv.length());
		const auto w(FetchMaxCharWidth(CS_Default));
		_tDst str(l * (w == 0 ? sizeof(ucsint_t) : w),
			typename ystdex::string_traits<_tDst>::value_type());

		str.resize(_tDispatcher::Transcode(&str[0], s, s + l));
		return str;
	}
	template<typename _tDst, typename _type, typename... _tParams>
	YB_ATTR_nodiscard YB_PURE static _tDst
	MakeViewTranscoded(_type sv, Encoding enc, _tParams&&... args)
	{
		const auto s(sv.data());

		yconstraint(s);

		const auto l(sv.length());
		const auto w(FetchMaxCharWidth(enc));
		_tDst str(l * (w == 0 ? sizeof(ucsint_t) : w),
			typename ystdex::string_traits<_tDst>::value_type());

		str.resize(_tDispatcher::Transcode(&str[0], s, s + l, enc,
			yforward(args)...));
		return str;
	}
	//! \since build 861
	template<typename _tDst, typename _type, typename... _tParams>
	YB_ATTR_nodiscard YB_PURE static _tDst
	MakeViewTranscoded(std::allocator_arg_t, const typename _tDst::allocator_type& a, _type sv)
	{
		const auto s(sv.data());

		yconstraint(s);

		const auto l(sv.length());
		const auto w(FetchMaxCharWidth(CS_Default));
		auto str(ystdex::make_obj_using_allocator<_tDst>(a,
			l * (w == 0 ? sizeof(ucsint_t) : w),
			typename ystdex::string_traits<_tDst>::value_type()));

		str.resize(_tDispatcher::Transcode(&str[0], s, s + l));
		return str;
	}
	//! \since build 861
	template<typename _tDst, typename _type, typename... _tParams>
	YB_ATTR_nodiscard YB_PURE static _tDst
	MakeViewTranscoded(std::allocator_arg_t, const typename
		_tDst::allocator_type& a, _type sv, Encoding enc, _tParams&&... args)
	{
		const auto s(sv.data());

		yconstraint(s);

		const auto l(sv.length());
		const auto w(FetchMaxCharWidth(enc));
		auto str(ystdex::make_obj_using_allocator<_tDst>(a,
			l * (w == 0 ? sizeof(ucsint_t) : w),
			typename ystdex::string_traits<_tDst>::value_type()));

		str.resize(_tDispatcher::Transcode(&str[0], s, s + l, enc,
			yforward(args)...));
		return str;
	}
};
//@}


/*!
\pre 输入字符串的每个字符不超过 \c sizeof(ucsint_t) 字节。
\pre 断言：指针参数非空。
\since build 645
*/
//@{
#define CHRLib_Impl_Transcode(_vFrom, _vTo, ...) \
	return Transcoding<CharSet::_vFrom, CharSet::_vTo> \
		::MakeTranscoded<_tDst>(__VA_ARGS__);
#define CHRLib_Impl_TranscodeView(_vFrom, _vTo, ...) \
	return Transcoding<CharSet::_vFrom, CharSet::_vTo> \
		::MakeViewTranscoded<_tDst>(__VA_ARGS__);
#define CHRLib_Impl_MakeTrans(_n, _tSrcChar, _tDstChar, _vFrom, _vTo, _vDeEnc) \
	template<class _tDst = std::basic_string<_tDstChar>, typename... _tParams> \
	YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE _tDst \
	Make##_n(const _tSrcChar* s, _tParams&&... args) \
	{ \
		CHRLib_Impl_Transcode(_vFrom, _vTo, s, yforward(args)...); \
	} \
	template<class _tDst = std::basic_string<_tDstChar>, typename... _tParams> \
	YB_ATTR_nodiscard YB_PURE _tDst \
	Make##_n(basic_string_view<_tSrcChar> sv, _tParams&&... args) \
	{ \
		CHRLib_Impl_TranscodeView(_vFrom, _vTo, sv, yforward(args)...); \
	} \
	template<class _tDst = std::basic_string<_tDstChar>, typename... _tParams> \
	YB_ATTR_nodiscard YB_NONNULL(3) YB_PURE _tDst \
	Make##_n(std::allocator_arg_t, const typename _tDst::allocator_type& a, \
		const _tSrcChar* s, _tParams&&... args) \
	{ \
		CHRLib_Impl_Transcode(_vFrom, _vTo, std::allocator_arg, a, s, \
			yforward(args)...); \
	} \
	template<class _tDst = std::basic_string<_tDstChar>, typename... _tParams> \
	YB_ATTR_nodiscard YB_PURE _tDst \
	Make##_n(std::allocator_arg_t, const typename _tDst::allocator_type& a, \
		basic_string_view<_tSrcChar> sv, _tParams&&... args) \
	{ \
		CHRLib_Impl_TranscodeView(_vFrom, _vTo, std::allocator_arg, a, sv, \
			yforward(args)...); \
	}
//! \brief 转换指定编码的多字节字符串为指定类型的 UCS-2 字符串。
//@{
CHRLib_Impl_MakeTrans(UCS2LE, char, char16_t, Null, ISO_10646_UCS_2, CS_Default)
//@}
/*!
\warning 假定本机字节序为小端序。
\todo 支持字节序转换。
*/
//@{
/*!
\brief 构造指定类型的 UCS-2 字符串。
\since build 641
*/
template<class _tDst = std::basic_string<char16_t>>
YB_ATTR_nodiscard YB_PURE inline _tDst
MakeUCS2LE(u16string_view sv, Encoding = CharSet::ISO_10646_UCS_2)
{
	const auto s(sv.data());

	yconstraint(s);
	// TODO: Support correct conversion for encoding other than UCS-2LE.
	return {s, sv.length()};
}
/*!
\brief 使用指定的分配器构造指定类型的 UCS-2 字符串。
\since build 861
*/
template<class _tDst = std::basic_string<char16_t>>
YB_ATTR_nodiscard YB_PURE inline _tDst
MakeUCS2LE(std::allocator_arg_t, const typename _tDst::allocator_type& a,
	u16string_view sv, Encoding = CharSet::ISO_10646_UCS_2)
{
	const auto s(sv.data());

	yconstraint(s);
	// TODO: Support correct conversion for encoding other than UCS-2LE.
	return ystdex::make_obj_using_allocator<_tDst>(a, s, sv.length());
}
//! \brief 转换 UCS-4 字符串为指定类型的 UCS-2 字符串。
//@{
CHRLib_Impl_MakeTrans(UCS2LE, char32_t, char16_t, ISO_10646_UCS_4,
	ISO_10646_UCS_2, CharSet::ISO_10646_UCS_4)
//@}

//! \brief 转换指定编码的多字节字符串为指定类型的 UCS-4 字符串。
//@{
CHRLib_Impl_MakeTrans(UCS4LE, char, char32_t, Null, ISO_10646_UCS_4, CS_Default)
//@}
//! \brief 转换 UCS-2 字符串为指定类型的 UCS-4 字符串。
//@{
CHRLib_Impl_MakeTrans(UCS4LE, char16_t, char32_t, ISO_10646_UCS_2,
	ISO_10646_UCS_4, CharSet::ISO_10646_UCS_2)
//@}
/*!
\brief 构造指定类型的 UCS-4 字符串。
\since build 641
*/
template<class _tDst = std::basic_string<char32_t>>
YB_ATTR_nodiscard YB_PURE inline _tDst
MakeUCS4LE(u32string_view sv, Encoding = CharSet::ISO_10646_UCS_4)
{
	const auto s(sv.data());

	yconstraint(s);
	// TODO: Support correct conversion for encoding other than UCS-4LE.
	return {s, sv.length()};
}
/*!
\brief 使用指定的分配器构造指定类型的 UCS-4 字符串。
\since build 861
*/
template<class _tAlloc, class _tDst = std::basic_string<char32_t>>
YB_ATTR_nodiscard YB_PURE inline _tDst
MakeUCS4LE(std::allocator_arg_t, const typename _tDst::allocator_type& a,
	u32string_view sv, Encoding = CharSet::ISO_10646_UCS_4)
{
	const auto s(sv.data());

	yconstraint(s);
	// TODO: Support correct conversion for encoding other than UCS-4LE.
	return ystdex::make_obj_using_allocator<_tDst>(a, s, sv.length());
}
//@}

//@{
//! \brief 转换 UTF-8 字符串为指定编码的多字节字符串。
//@{
//! \since build 544
template<class _tDst = std::string>
YB_ATTR_nodiscard YB_NONNULL(1) YB_PURE inline _tDst
MakeMBCS(const char* s, Encoding enc)
{
	return enc = CS_Default ? MakeMBCS<_tDst>(s)
		: MakeMBCS<_tDst>(MakeUCS2LE(s, CS_Default), enc);
}
template<class _tDst = std::string>
YB_ATTR_nodiscard YB_PURE inline _tDst
MakeMBCS(string_view sv, Encoding enc)
{
	return enc = CS_Default ? MakeMBCS<_tDst>(sv)
		: MakeMBCS<_tDst>(MakeUCS2LE(sv, CS_Default), enc);
}
//! \since build 861
template<class _tDst = std::string>
YB_ATTR_nodiscard YB_NONNULL(3) YB_PURE inline _tDst
MakeMBCS(std::allocator_arg_t, const typename _tDst::allocator_type& a, char* s,
	Encoding enc)
{
	return enc = CS_Default ? MakeMBCS<_tDst>(s)
		: MakeMBCS<_tDst>(std::allocator_arg, a,
		MakeUCS2LE(std::allocator_arg, a, s, CS_Default), enc);
}
//! \since build 861
template<class _tDst = std::string>
YB_ATTR_nodiscard YB_PURE inline _tDst
MakeMBCS(std::allocator_arg_t, const typename _tDst::allocator_type& a,
	string_view sv, Encoding enc)
{
	return enc = CS_Default ? MakeMBCS<_tDst>(sv)
		: MakeMBCS<_tDst>(std::allocator_arg, a,
		MakeUCS2LE(std::allocator_arg, a, sv, CS_Default), enc);
}
//@}
//! \brief 转换 UCS-2LE 字符串为指定编码的多字节字符串。
//@{
CHRLib_Impl_MakeTrans(MBCS, char16_t, char, ISO_10646_UCS_2, Null, CS_Default)
//@}
//! \brief 转换 UCS-4LE 字符串为指定编码的多字节字符串。
//@{
CHRLib_Impl_MakeTrans(MBCS, char32_t, char, ISO_10646_UCS_4, Null, CS_Default)
//@}
//@}
#undef CHRLib_Impl_MakeTrans
#undef CHRLib_Impl_Transcode
#undef CHRLib_Impl_TranscodeView
//@}

} // namespace CHRLib;

#endif

