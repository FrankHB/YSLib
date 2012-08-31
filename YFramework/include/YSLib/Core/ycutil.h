/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycutil.h
\ingroup Core
\brief 核心实用模块。
\version r1895;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-05-23 06:10:59 +0800;
\par 修改时间:
	2012-08-30 20:09 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YCoreUtilities;
*/


#ifndef YSL_INC_CORE_YCUTIL_H_
#define YSL_INC_CORE_YCUTIL_H_ 1

#include "ysdef.h"

YSL_BEGIN

/*!
\brief 转换类型选择。

若 \c _type 能隐式转换为 \c _tStrict 则 \c Result 为 \c _tStrict，
	否则 \c Result 为 \c _tWeak 。
\since build 201 。
*/
template<typename _type, typename _tStrict, typename _tWeak>
struct MoreConvertible
{
	typedef typename std::conditional<std::is_convertible<_type,
		_tStrict>::value, _tStrict, _tWeak>::Result Result;

	static inline Result
	Cast(_type o)
	{
		return Result(o);
	}
};


/*!
\brief 转换类型选择。

若 \c _type 能隐式转换为 \c _tStrict 则 \c Result 为 \c _tStrict，
	否则 \c Result 为 \c _type 。
\since build 201 。
*/
template<typename _type, typename _tStrict>
struct SelectConvertible : MoreConvertible<_type, _tStrict, _type>
{
	typedef typename MoreConvertible<_type, _tStrict, _type>::Result
		Result;
};


/*!
\brief 取整数类型的零元素。
\since build 319 。
*/
yconstfn int
FetchZero() ynothrow
{
	return 0;
}
/*!
\brief 取指定类型的零元素。
\since build 319 。
*/
template<typename _type>
yconstfn _type
FetchZero() ynothrow
{
	return _type(0);
}

/*!
\brief 整数类型符号函数。
\note 若 <tt>a < b</tt> 则返回 -1 ，否则若 <tt>a = b</tt> 则返回 0 ，否则返回 1 。
\since build 319 。
*/
yconstfn s8
FetchSign(int a, int b = 0) ynothrow
{
	return a < b ? -1 : !(a == b);
}
/*!
\brief 符号函数。
\note 若 <tt>a < b</tt> 则返回 -1 ，否则若 <tt>a = b</tt> 则返回 0 ，否则返回 1 。
\since build 319 。
*/
template<typename _type>
yconstfn s8
FetchSign(const _type& a, const _type& b = FetchZero<_type>()) ynothrow
{
	return a < b ? -1 : !(a == b);
}

/*!
\brief 判断整数 d 和以 [a, b](a ≤ b) 或 [b, a](a > b) 区间的关系。
\return < 0 ：d 在区间外；
\return = 0 ：d 在区间端点上；
\return > 0 ：d 在区间内。
\note 无精度修正。
\since build 319 。
*/
yconstfn int
FetchSignFromInterval(int d, int a, int b) ynothrow
{
	return FetchSign(a, d) * FetchSign(d, b);
}
/*!
\brief 判断 d 和以 [a, b](a ≤ b) 或 [b, a](a > b) 区间的关系。
\return < 0 ：d 在区间外；
\return = 0 ：d 在区间端点上；
\return > 0 ：d 在区间内。
\note 无精度修正。
\since build 319 。
*/
template<typename _type>
yconstfn int
FetchSignFromInterval(const _type& d, const _type& a, const _type& b) ynothrow
{
	return FetchSign(a, d) * FetchSign(d, b);
}

/*!
\brief 判断 i 是否在左闭右开区间 [<tt>FetchZero<_type>()</tt>, b) 中。
\pre 断言： <tt>FetchZero<_type>() < b</tt> 。
\since build 319 。
*/
template<typename _type>
inline bool
IsInInterval(_type i, _type b) ynothrow
{
	YAssert(FetchZero<_type>() < b,
		"Zero element as lower bound is not less than upper bound.");

	return !(i < FetchZero<_type>()) && i < b;
}
/*!
\brief 判断 i 是否在左闭右开区间 [a, b) 中。
\pre 断言： <tt>a < b</tt> 。
\since build 319 。
*/
template<typename _type>
inline bool
IsInInterval(_type i, _type a, _type b) ynothrow
{
	YAssert(a < b, "Lower bound is not less than upper bound.");

	return !(i < a) && i < b;
}

/*!
\brief 判断 i 是否在开区间 (FetchZero<_type>(), b) 内。
\pre 断言： <tt>FetchZero<_type>() < b</tt> 。
\since build 319 。
*/
template<typename _type>
inline bool
IsInOpenInterval(_type i, _type b) ynothrow
{
	YAssert(FetchZero<_type>() < b,
		"Zero element as lower bound is not less than upper bound.");

	return i > FetchZero<_type>() && i < b;
}
/*!
\brief 判断 i 是否在开区间 (a, b) 内。
\pre 断言： <tt>a < b</tt> 。
\since build 319 。
*/
template<typename _type>
inline bool
IsInOpenInterval(_type i, _type a, _type b) ynothrow
{
	YAssert(a < b,
		"Lower bound is not less than upper bound.");

	return i > a && i < b;
}

/*!
\brief 计算满足指定的值 v 在区间 [<tt>a[i]</tt>, <tt>a[i + 1]</tt>) 内最小的 i 。
\pre 断言： <tt>a</tt> 。
\pre 断言： <tt>n != 0</tt> 。
\pre 断言： <tt>!(v < *a)</tt> 。
\since build 319 。
*/
template<typename _type>
size_t
SwitchInterval(_type v, const _type* a, size_t n) ynothrow
{
	YAssert(a, "Null array pointer found."),
	YAssert(n != 0, "Zero length of array found.");
	YAssert(!(v < *a), "Value less than lower bound found.");

	size_t i(0);

	while(!(++i == n || v < a[i]))
		;
	return i - 1;
}

/*!
\brief 计算满足指定的值 v 在区间 [b(i), b(i + 1)) 内的最小的 i ；
			其中 b(i) 是 <tt>a[i]</tt> 前 i 项的和。
\pre 断言： <tt>a</tt> 。
\pre 断言： <tt>n != 0</tt> 。
\pre 断言： <tt>!(v < *a)</tt> 。
\since build 319 。
*/
template<typename _type>
size_t
SwitchAddedInterval(_type v, const _type* a, size_t n) ynothrow
{
	YAssert(a, "Null array pointer found."),
	YAssert(n != 0, "Zero length of array found.");
	YAssert(!(v < *a), "Value less than lower bound found.");

	_type s(*a);
	size_t i(0);

	while(!(++i == n || v < (s += a[i])))
		;
	return i - 1;
}

/*!
\brief 约束整数 i 在闭区间 [a, b] 中。
\pre 断言： <tt>!(b < a)</tt> 。
\post <tt>!(i < a || b < i)</tt> 。
\since build 319 。
*/
template<typename _type>
void
RestrictInClosedInterval(_type& i, int a, int b) ynothrow
{
	YAssert(!(b < a), "Upper bound is less than lower bound.");

	if(i < a)
		i = a;
	else if(b < i)
		i = b;
}

/*!
\brief 约束整数 i 在左闭右开区间 [a, b) 中。
\pre 断言： <tt>a < b</tt> 。
\post <tt>!(i < a) && i < b</tt> 。
\since build 319 。
*/
template<typename _type>
void
RestrictInInterval(_type& i, int a, int b) ynothrow
{
	YAssert(a < b, "Lower bound is not less than upper bound.");

	if(i < a)
		i = a;
	else if(!(i < b))
		i = b - 1;
}

/*!
\brief 约束无符号整数 u 在区间上界 b 内。
\post <tt>!(b < u)</tt>。
\since build 319 。
*/
template<typename _type>
void
RestrictUnsignedStrict(_type& u, unsigned b) ynothrow
{
	if(b < u)
		u = b;
}

/*!
\brief 约束无符号整数 u 在左闭右开区间 [0, b) 中。
\pre 断言： <tt>b != FetchZero<_type>()</tt> 。
\post <tt>!(u < FetchZero<_type>()) && u < b</tt> 。
\since build 319 。
*/
template<typename _type>
void
RestrictUnsigned(_type& u, unsigned b) ynothrow
{
	YAssert(b != FetchZero<_type>(), "Zero upper bound found.");

	if(!(u < b))
		u = b - 1;
}

/*!
\brief 约束关系：a ≤ b 。
\post <tt>a <= b</tt> 。
\since build 319 。
*/
template<typename _type>
inline void
RestrictLessEqual(_type& a, _type& b) ynothrow
{
	if(b < a)
		std::swap(a, b);
}


/*!
\brief 清除指定的连续对象。
\pre 设类型 T 为 <tt>std::remove_reference<decltype(*dst)>::type</tt>， 则应满足
	<tt>std::is_pod<T> || (std::is_nothrow_default_constructible<T>::value
		&& std::is_nothrow_assignable<T, T>::value)</tt> 。
\note 忽略空指针和零长度。
\since build 322 。
*/
template<typename _tOut>
inline void
ClearSequence(_tOut dst, size_t n) ynothrow
{
	typedef typename std::remove_reference<decltype(*dst)>::type _type;

	static_assert(std::is_pod<_type>::value
		|| (std::is_nothrow_default_constructible<_type>::value
		&& std::is_nothrow_assignable<_type, _type>::value),
		"Invalid type found.");

	if(YB_LIKELY(dst && n))
		std::fill_n(dst, n, _type());
}


/*!
\brief delete 仿函数。
\since build 174 。
*/
struct delete_obj_ndebug
{
	/*!
	\brief 删除指针指向的对象。
	*/
	template<typename _type>
	inline void
	operator()(_type* _ptr) ynothrow
	{
		delete _ptr;
	}
};


/*!
\brief delete 第二成员仿函数。
\since build 281 。
\todo 删除数组成员。
*/
struct delete_second_mem_ndebug
{
	/*!
	\brief 删除第二成员指向的对象。
	*/
	template<typename _type>
	inline void
	operator()(const _type& _pr) ynothrow
	{
		delete _pr.second;
	}
};


#ifdef YSL_USE_MEMORY_DEBUG

/*!
\brief delete 仿函数（调试版本）。
\since build 174 。
*/
struct delete_obj_debug
{
	/*!
	\brief 删除指针指向的对象。
	*/
	template<typename _type>
	inline void
	operator()(_type* _ptr) ynothrow
	{
		ydelete(_ptr);
	}
};


/*!
\brief delete 第二成员仿函数。
\since build 281 。
\todo 删除数组成员。
*/
struct delete_second_mem_debug
{
	/*!
	\brief 删除第二成员指向的对象。
	*/
	template<typename _type>
	inline void
	operator()(const _type& _pr) ynothrow
	{
		ydelete(_pr.second);
	}
};


#	define delete_obj delete_obj_debug
#	define delete_second_mem delete_second_mem_debug

#else

#	define delete_obj delete_obj_ndebug
#	define delete_second_mem delete_second_mem_ndebug

#endif

/*!
\brief 带置空指针操作的 delete 仿函数。
\since build 154 。
*/
struct safe_delete_obj
{
	/*!
	\brief 删除指针指向的对象，并置指针为空值。
	*/
	template<typename _tPointer>
	inline void
	operator()(_tPointer& _ptr) ynothrow
	{
		reset(_ptr);
	}
};


/*!
\brief 使用 new 复制指定指针指向的对象。
\since build 240 。
*/
template<typename _type>
yconstfn auto
CloneNonpolymorphic(const _type& p) -> decltype(&*p)
{
	return new typename std::remove_reference<decltype(*p)>::type(*p);
}

/*!
\brief 使用 Clone 成员函数复制指定指针指向的多态类类型对象。
\pre 断言： std::is_polymorphic<decltype(*p)>::value 。
\since build 240 。
*/
template<class _type>
auto
ClonePolymorphic(const _type& p) -> decltype(&*p)
{
	static_assert(std::is_polymorphic<typename
		std::remove_reference<decltype(*p)>::type>::value,
		"Non-polymorphic class type found.");

	return p->Clone();
}

YSL_END

#endif

