/*
	© 2010-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cast.hpp
\ingroup YStandardEx
\brief C++ 转换模板。
\version r1219
\author FrankHB <frankhb1989@gmail.com>
\since build 175
\par 创建时间:
	2010-12-15 08:13:18 +0800
\par 修改时间:
	2015-11-05 01:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Cast
*/


#ifndef YB_INC_ystdex_cast_hpp_
#define YB_INC_ystdex_cast_hpp_ 1

#include "type_traits.hpp" // for is_object, is_void, is_function, decay_t,
//	is_same, _t;
#include <memory> // for std::addressof;
#include <typeinfo> // for std::bad_cast;
#include "cassert.h" // for yassume;

//! \since build 175
namespace ystdex
{

/*!
\brief 取 \c void* 类型的指针。
\note 适合 std::fprintf 等的 \c %p 转换规格。
\since build 563
*/
//@{
template<typename _type>
yconstfn yimpl(enable_if_t)<or_<is_object<_type>, is_void<_type>>::value, void*>
pvoid(_type* p) ynothrow
{
	return const_cast<void*>(static_cast<const volatile void*>(p));
}
//! \note ISO C++11 指定函数指针 \c reinterpret_cast 为对象指针有条件支持。
template<typename _type>
inline yimpl(enable_if_t)<is_function<_type>::value, void*>
pvoid(_type* p) ynothrow
{
	return const_cast<void*>(reinterpret_cast<const volatile void*>(p));
}

/*!
\return 非空指针。
\since build 552
*/
template<typename _type>
yconstfn void*
pvoid_ref(_type&& ref)
{
	return ystdex::pvoid(std::addressof(ref));
}
//@}


/*!
\brief 转换 const 引用。
\see WG21/N4380 。
\since build 593
*/
template<typename _type>
inline add_const_t<_type>&
as_const(_type& t)
{
	return t;
}


/*!	\defgroup cast Cast
\brief 显式类型转换。
\since build 243
*/
//@{
/*!
\pre 源类型和目标类型退化后相同。
\note 指定源类型优先，可自动推导目标类型。
\sa as_const
\since build 531
*/
//! \brief 允许添加限定符转换。
template<typename _tSrc, typename _tDst = const decay_t<_tSrc>&&>
yconstfn _tDst
qualify(_tSrc&& arg) ynothrow
{
	static_assert(is_same<decay_t<_tSrc>, decay_t<_tDst>>(),
		"Non-qualification conversion found.");

	return static_cast<_tDst>(arg);
}


/*!
\brief 多态类指针类型转换。
\tparam _tSrc 源类型。
\tparam _pDst 目标类型。
\pre 静态断言： _tSrc 是多态类。
\pre 静态断言： _pDst 是内建指针。
\throw std::bad_cast dynamic_cast 失败。
\since build 175
*/
template<typename _pDst, class _tSrc>
inline _pDst
polymorphic_cast(_tSrc* v)
{
	static_assert(is_polymorphic<_tSrc>(), "Non-polymorphic class found.");
	static_assert(is_pointer<_pDst>(), "Non-pointer destination found.");

	if(const auto p = dynamic_cast<_pDst>(v))
		return p;
	throw std::bad_cast();
}

/*!
\brief 多态类指针向派生类指针转换。
\since build 551
*/
//@{
/*!
\tparam _tSrc 源类型。
\tparam _pDst 目标类型。
\pre 静态断言： _tSrc 是多态类。
\pre 静态断言： _pDst 是内建指针。
\pre 静态断言： _tSrc 是 _pDst 指向的类去除修饰符后的基类。
\pre 断言： dynamic_cast 成功。
*/
template<typename _pDst, class _tSrc>
inline _pDst
polymorphic_downcast(_tSrc* v) ynothrow
{
	static_assert(is_polymorphic<_tSrc>(), "Non-polymorphic class found.");
	static_assert(is_pointer<_pDst>(), "Non-pointer destination found.");
	static_assert(is_base_of<_tSrc, remove_cv_t<
		remove_pointer_t<_pDst>>>(), "Wrong destination type found.");

	yassume(dynamic_cast<_pDst>(v) == v);
	return static_cast<_pDst>(v);
}
/*!
\tparam _tSrc 源类型。
\tparam _rDst 目标类型。
\pre 静态断言： _rDst 是左值引用。
*/
template<typename _rDst, class _tSrc>
yconstfn yimpl(enable_if_t)<is_lvalue_reference<_rDst>::value, _rDst>
polymorphic_downcast(_tSrc& v) ynothrow
{
	return *ystdex::polymorphic_downcast<remove_reference_t<_rDst>*>(
		std::addressof(v));
}
/*!
\tparam _tSrc 源类型。
\tparam _rDst 目标类型。
\pre 静态断言： _rDst 是右值引用。
*/
template<typename _rDst, class _tSrc>
yconstfn yimpl(enable_if_t)<is_rvalue_reference<_rDst>::value
	&& !is_reference<_tSrc>::value, _rDst>
polymorphic_downcast(_tSrc&& v) ynothrow
{
	return std::move(ystdex::polymorphic_downcast<_rDst&>(v));
}
/*!
\tparam _tSrc 源的元素类型。
\tparam _tDst 目标的元素类型。
\tparam _tDeleter 删除器类型。
\pre _tDst 可通过 \c release() 取得的指针和删除器右值构造。
\pre 断言：调用 polymorphic_downcast 转换 \c release() 取得的指针保证无异常抛出。
\note 使用 ADL 调用 polymorphic_downcast 转换 \c release() 取得的指针。
*/
template<class _tDst, typename _tSrc, typename _tDeleter>
inline yimpl(enable_if_t)<!is_reference<_tDst>::value
	&& !is_array<_tDst>::value, std::unique_ptr<_tDst, _tDeleter>>
polymorphic_downcast(std::unique_ptr<_tSrc, _tDeleter>&& v) ynothrow
{
	using dst_type = std::unique_ptr<_tDst, _tDeleter>;
	using pointer = typename dst_type::pointer;
	auto ptr(v.release());
	ynoexcept_assert("Invalid cast found.", polymorphic_downcast<pointer>(ptr));

	yassume(bool(ptr));
	return dst_type(polymorphic_downcast<pointer>(ptr),
		std::move(v.get_deleter()));
}
/*!
\tparam _tSrc 源的元素类型。
\tparam _tDst 目标的元素类型。
\pre _tDst 可通过 \c get() 取得的指针和删除器右值构造。
*/
template<class _tDst, typename _tSrc>
inline yimpl(enable_if_t)<!is_reference<_tDst>::value
	&& !is_array<_tDst>::value, std::shared_ptr<_tDst>>
polymorphic_downcast(const std::unique_ptr<_tSrc>& v) ynothrow
{
	yassume(dynamic_cast<_tDst*>(v.get()) == v.get());

	return std::static_pointer_cast<_tDst>(v);
}
//@}

//! \brief 多态类指针交叉转换。
//@{
/*!
\tparam _tSrc 源类型。
\tparam _pDst 目标类型。
\pre 静态断言： _tSrc 是多态类。
\pre 静态断言： _pDst 是内建指针。
\pre 断言： dynamic_cast 成功。
\return 非空结果。
\note 空指针作为参数一定失败。
\since build 179
*/
template<typename _pDst, class _tSrc>
inline _pDst
polymorphic_crosscast(_tSrc* v)
{
	static_assert(is_polymorphic<_tSrc>(), "Non-polymorphic class found.");
	static_assert(is_pointer<_pDst>(), "Non-pointer destination found.");

	auto p(dynamic_cast<_pDst>(v));

	yassume(p);
	return p;
}
/*!
\tparam _tSrc 源类型。
\tparam _rDst 目标类型。
\pre 静态断言： _rDst 是左值引用。
\since build 496
*/
template<typename _rDst, class _tSrc>
yconstfn _rDst
polymorphic_crosscast(_tSrc& v)
{
	static_assert(is_lvalue_reference<_rDst>(),
		"Invalid destination type found.");

	return *ystdex::polymorphic_crosscast<remove_reference_t<_rDst>*>(
		std::addressof(v));
}
/*!
\tparam _tSrc 源类型。
\tparam _rDst 目标类型。
\pre 静态断言： _rDst 是右值引用。
\since build 496
*/
template<typename _rDst, class _tSrc>
yconstfn enable_if_t<!is_reference<_tSrc>::value, _rDst>
polymorphic_crosscast(_tSrc&& v)
{
	static_assert(is_rvalue_reference<_rDst>(),
		"Invalid destination type found.");

	return std::move(ystdex::polymorphic_crosscast<_rDst&>(v));
}
//@}


namespace details
{

//! \since build 496
//@{
template<typename _tFrom, typename _tTo, bool _bNonVirtualDownCast>
struct general_polymorphic_cast_helper
{
	static yconstfn _tTo
	cast(_tFrom v)
	{
		return ystdex::polymorphic_downcast<_tTo>(v);
	}
};

template<typename _tFrom, typename _tTo>
struct general_polymorphic_cast_helper<_tFrom, _tTo, false>
{
	static yconstfn _tTo
	cast(_tFrom v)
	{
		return dynamic_cast<_tTo>(v);
	}
};


template<typename _tFrom, typename _tTo, bool _bUseStaticCast>
struct general_cast_helper
{
	static yconstfn _tTo
	cast(_tFrom v)
	{
		return _tTo(v);
	}
};

template<typename _tFrom, typename _tTo>
struct general_cast_helper<_tFrom, _tTo, false>
{
	static yconstfn _tTo
	cast(_tFrom v)
	{
		return general_polymorphic_cast_helper<_tFrom, _tTo, and_<is_base_of<
			_tFrom, _tTo>, not_<has_common_nonempty_virtual_base<
			_t<remove_rp<_tFrom>>, _t<remove_rp<_tTo>>>>>::value>::cast(v);
	}
};

template<typename _type>
struct general_cast_helper<_type, _type, true>
{
	static inline _type
	cast(_type v)
	{
		return v;
	}
};

template<typename _type>
struct general_cast_helper<_type, _type, false>
{
	static yconstfn _type
	cast(_type v)
	{
		return v;
	}
};

template<typename _tFrom, typename _tTo>
struct general_cast_type_helper
	: bool_constant<is_convertible<_tFrom, _tTo>::value>
{};
//@}

} // namespace details;

/*!
\brief 一般类型转换。
\tparam _tSrc 源类型。
\tparam _tDst 目标类型。
\since build 175
\todo 扩展接受右值引用参数。

能确保安全隐式转换时使用 static_cast ；
除此之外非虚基类向派生类转换使用 polymophic_downcast；
否则使用 dynamic_cast。
*/
//@{
template<typename _tDst, typename _tSrc>
yconstfn _tDst
general_cast(_tSrc* v)
{
	return details::general_cast_helper<_tSrc*, _tDst,
		details::general_cast_type_helper<_tSrc*, _tDst>::value>::cast(v);
}
template<typename _tDst, typename _tSrc>
yconstfn _tDst
general_cast(_tSrc& v)
{
	return details::general_cast_helper<_tSrc&, _tDst,
		details::general_cast_type_helper<_tSrc&, _tDst>::value>::cast(v);
}
template<typename _tDst, typename _tSrc>
yconstfn const _tDst
general_cast(const _tSrc& v)
{
	return details::general_cast_helper<const _tSrc&, _tDst, details
		::general_cast_type_helper<const _tSrc&, const _tDst>::value>::cast(v);
}
//@}
//@}

} // namespace ystdex;

#endif

