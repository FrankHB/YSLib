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
\version r1074
\author FrankHB <frankhb1989@gmail.com>
\since build 175
\par 创建时间:
	2010-12-15 08:13:18 +0800
\par 修改时间:
	2015-03-21 09:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Cast
*/


#ifndef YB_INC_ystdex_cast_hpp_
#define YB_INC_ystdex_cast_hpp_ 1

#include "type_op.hpp" // for ystdex::decay_t, ystdex::is_same,
//	ystdex::array_ref_decay, ystdex::is_object, ystdex::is_void,
//	ystdex::is_function;
#include <memory> // for std::addressof;
#include "cassert.h"
#include <typeinfo> // for std::bad_cast;
#include <initializer_list> // for std::initializer_list;

namespace ystdex
{

/*!	\defgroup cast Cast
\brief 显式类型转换。
\since build 243
*/


/*!
\ingroup cast
\pre 源类型和目标类型退化后相同。
\note 指定源类型优先，可自动推导目标类型。
\since build 531
*/
//@{
//! \brief 允许添加限定符转换。
template<typename _tSrc, typename _tDst = const decay_t<_tSrc>&&>
yconstfn _tDst
qualify(_tSrc&& arg) ynothrow
{
	static_assert(is_same<decay_t<_tSrc>, decay_t<_tDst>>::value,
		"Non-qualification conversion found.");

	return static_cast<_tDst>(arg);
}

//! \brief 允许去除限定符转换。
template<typename _tSrc,
	typename _tDst = typename array_ref_decay<_tSrc>::reference>
yconstfn _tDst
unqualify(_tSrc&& arg) ynothrow
{
	static_assert(is_same<decay_t<_tSrc>, decay_t<_tDst>>::value,
		"Non-qualification conversion found.");

	return const_cast<_tDst>(arg);
}
//@}


/*!
\brief 取 \c void* 类型的指针。
\note 适合 \c std::fprintf 等的 \c %p 转换规格。
\since build 563
*/
//@{
template<typename _type>
yconstfn
	yimpl(enable_if_t)<is_object<_type>::value || is_void<_type>::value, void*>
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
\ingroup cast
\brief 使用匿名联合体进行的类型转换。
\tparam _tSrc 源类型。
\tparam _tDst 目标类型。
\pre <tt>is_pod<_tDst>::value && sizeof<_tSrc> == sizeof<_tDst></tt> 。
\since build 297
*/
template<typename _tDst, typename _tSrc>
inline _tDst
union_cast(_tSrc x) ynothrow
{
	static_assert(is_pod<_tDst>::value, "Non-POD destination type found.");
	static_assert(sizeof(_tSrc) == sizeof(_tDst), "Incompatible types found.");

	union
	{
		_tSrc x;
		_tDst y;
	} u = {x};
	return u.y;
}


/*!
\ingroup cast
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
polymorphic_cast(_tSrc* x)
{
	static_assert(is_polymorphic<_tSrc>::value, "Non-polymorphic class found.");
	static_assert(is_pointer<_pDst>::value, "Non-pointer destination found.");

	if(const auto p = dynamic_cast<_pDst>(x))
		return p;
	throw std::bad_cast();
}

/*!
\ingroup cast
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
polymorphic_downcast(_tSrc* x) ynothrow
{
	static_assert(is_polymorphic<_tSrc>::value, "Non-polymorphic class found.");
	static_assert(is_pointer<_pDst>::value, "Non-pointer destination found.");
	static_assert(is_base_of<_tSrc, remove_cv_t<
		remove_pointer_t<_pDst>>>::value, "Wrong destination type found.");

	yassume(dynamic_cast<_pDst>(x) == x);
	return static_cast<_pDst>(x);
}
/*!
\tparam _tSrc 源类型。
\tparam _rDst 目标类型。
\pre 静态断言： _rDst 是左值引用。
*/
template<typename _rDst, class _tSrc>
yconstfn yimpl(enable_if_t)<is_lvalue_reference<_rDst>::value, _rDst>
polymorphic_downcast(_tSrc& x) ynothrow
{
	return *ystdex::polymorphic_downcast<remove_reference_t<_rDst>*>(
		std::addressof(x));
}
/*!
\tparam _tSrc 源类型。
\tparam _rDst 目标类型。
\pre 静态断言： _rDst 是右值引用。
*/
template<typename _rDst, class _tSrc>
yconstfn yimpl(enable_if_t)<is_rvalue_reference<_rDst>::value
	&& !is_reference<_tSrc>::value, _rDst>
polymorphic_downcast(_tSrc&& x) ynothrow
{
	return std::move(ystdex::polymorphic_downcast<_rDst&>(x));
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
polymorphic_downcast(std::unique_ptr<_tSrc, _tDeleter>&& x) ynothrow
{
	using dst_type = std::unique_ptr<_tDst, _tDeleter>;
	using pointer = typename dst_type::pointer;
	auto ptr(x.release());
	ynoexcept_assert("Invalid cast found.", polymorphic_downcast<pointer>(ptr));

	yassume(bool(ptr));
	return dst_type(polymorphic_downcast<pointer>(ptr),
		std::move(x.get_deleter()));
}
/*!
\tparam _tSrc 源的元素类型。
\tparam _tDst 目标的元素类型。
\pre _tDst 可通过 \c get() 取得的指针和删除器右值构造。
*/
template<class _tDst, typename _tSrc>
inline yimpl(enable_if_t)<!is_reference<_tDst>::value
	&& !is_array<_tDst>::value, std::shared_ptr<_tDst>>
polymorphic_downcast(const std::unique_ptr<_tSrc>& x) ynothrow
{
	yassume(dynamic_cast<_tDst*>(x.get()) == x.get());

	return std::static_pointer_cast<_tDst>(x);
}
//@}

/*!
\ingroup cast
\brief 多态类指针交叉转换。
*/
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
polymorphic_crosscast(_tSrc* x)
{
	static_assert(is_polymorphic<_tSrc>::value, "Non-polymorphic class found.");
	static_assert(is_pointer<_pDst>::value, "Non-pointer destination found.");

	auto p(dynamic_cast<_pDst>(x));

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
polymorphic_crosscast(_tSrc& x)
{
	static_assert(is_lvalue_reference<_rDst>::value,
		"Invalid destination type found.");

	return *ystdex::polymorphic_crosscast<remove_reference_t<_rDst>*>(
		std::addressof(x));
}
/*!
\tparam _tSrc 源类型。
\tparam _rDst 目标类型。
\pre 静态断言： _rDst 是右值引用。
\since build 496
*/
template<typename _rDst, class _tSrc>
yconstfn enable_if_t<!is_reference<_tSrc>::value, _rDst>
polymorphic_crosscast(_tSrc&& x)
{
	static_assert(is_rvalue_reference<_rDst>::value,
		"Invalid destination type found.");

	return std::move(ystdex::polymorphic_crosscast<_rDst&>(x));
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
	cast(_tFrom x)
	{
		return ystdex::polymorphic_downcast<_tTo>(x);
	}
};

template<typename _tFrom, typename _tTo>
struct general_polymorphic_cast_helper<_tFrom, _tTo, false>
{
	static yconstfn _tTo
	cast(_tFrom x)
	{
		return dynamic_cast<_tTo>(x);
	}
};


template<typename _tFrom, typename _tTo, bool _bUseStaticCast>
struct general_cast_helper
{
	static yconstfn _tTo
	cast(_tFrom x)
	{
		return _tTo(x);
	}
};

template<typename _tFrom, typename _tTo>
struct general_cast_helper<_tFrom, _tTo, false>
{
	static yconstfn _tTo
	cast(_tFrom x)
	{
		return general_polymorphic_cast_helper<_tFrom, _tTo, (is_base_of<
			_tFrom, _tTo>::value && !has_common_nonempty_virtual_base<typename
			remove_rp<_tFrom>::type, typename remove_rp<_tTo>::type>::value)
		>::cast(x);
	}
};

template<typename _type>
struct general_cast_helper<_type, _type, true>
{
	static inline _type
	cast(_type x)
	{
		return x;
	}
};

template<typename _type>
struct general_cast_helper<_type, _type, false>
{
	static yconstfn _type
	cast(_type x)
	{
		return x;
	}
};

template<typename _tFrom, typename _tTo>
struct general_cast_type_helper
	: integral_constant<bool, is_convertible<_tFrom, _tTo>::value>
{};
//@}

} // namespace details;

/*!
\ingroup cast
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
general_cast(_tSrc* x)
{
	return details::general_cast_helper<_tSrc*, _tDst,
		details::general_cast_type_helper<_tSrc*, _tDst>::value>::cast(x);
}
template<typename _tDst, typename _tSrc>
yconstfn _tDst
general_cast(_tSrc& x)
{
	return details::general_cast_helper<_tSrc&, _tDst,
		details::general_cast_type_helper<_tSrc&, _tDst>::value>::cast(x);
}
template<typename _tDst, typename _tSrc>
yconstfn const _tDst
general_cast(const _tSrc& x)
{
	return details::general_cast_helper<const _tSrc&, _tDst, details
		::general_cast_type_helper<const _tSrc&, const _tDst>::value>::cast(x);
}
//@}

} // namespace ystdex;

#endif

