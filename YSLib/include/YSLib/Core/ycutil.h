/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycutil.h
\ingroup Core
\brief 核心实用模块。
\version r2694;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-23 06:10:59 +0800;
\par 修改时间:
	2011-09-10 03:22 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YCoreUtilities;
*/


#ifndef YSL_INC_CORE_YCUTIL_H_
#define YSL_INC_CORE_YCUTIL_H_

#include "ysdef.h"

YSL_BEGIN

/*!
\brief 转换类型选择。

若 \c _type 能隐式转换为 \c _tStrict 则 \c Result 为 \c _tStrict，
	否则 \c Result 为 \c _tWeak 。
*/
template<typename _type, typename _tStrict, typename _tWeak>
struct MoreConvertible
{
	typedef typename std::conditional<std::is_convertible<_type,
		_tStrict>::value, _tStrict, _tWeak>::Result Result;

	inline static Result
	Cast(_type o)
	{
		return static_cast<Result>(o);
	}
};


/*!
\brief 转换类型选择。

若 \c _type 能隐式转换为 \c _tStrict 则 \c Result 为 \c _tStrict，
	否则 \c Result 为 \c _type 。
*/
template<typename _type, typename _tStrict>
struct SelectConvertible : MoreConvertible<_type, _tStrict, _type>
{
	typedef typename MoreConvertible<_type, _tStrict, _type>::Result
		Result;
};


/*!
\brief 取整数类型的零元素。
*/
inline int
GetZeroElement()
{
	return 0;
}
/*!
\brief 取指定类型的零元素。
*/
PDefTH1(_type)
inline _type
GetZeroElement()
{
	return _type(0);
}

/*!
\brief 整数类型符号函数。
\note 若 <tt>a < b</tt> 则返回 -1 ，否则若 <tt>a = b</tt> 则返回 0 ，否则返回 1 。
*/
inline s8
sgn(int a, int b = 0)
{
	return a < b ? -1 : !(a == b);
}
/*!
\brief 符号函数。
\note 若 <tt>a < b</tt> 则返回 -1 ，否则若 <tt>a = b</tt> 则返回 0 ，否则返回 1 。
*/
PDefTH1(_type)
s8
sgn(const _type& a, const _type& b = GetZeroElement<_type>())
{
	return a < b ? -1 : !(a == b);
}

/*!
\brief 判断整数 d 和以 [a, b](a ≤ b) 或 [b, a](a > b) 区间的关系。
\return < 0 ：d 在区间外；
\return = 0 ：d 在区间端点上；
\return > 0 ：d 在区间内。
\note 无精度修正。
*/
inline int
sgnInterval(int d, int a, int b)
{
	return sgn(a, d) * sgn(d, b);
}
/*!
\brief 判断 d 和以 [a, b](a ≤ b) 或 [b, a](a > b) 区间的关系。
\return < 0 ：d 在区间外；
\return = 0 ：d 在区间端点上；
\return > 0 ：d 在区间内。
\note 无精度修正。
*/
PDefTH1(_type)
int
sgnInterval(const _type& d, const _type& a, const _type& b)
{
	return sgn(a, d) * sgn(d, b);
}

/*!
\brief 判断 i 是否在左闭右开区间 [<tt>GetZeroElement<_type>()</tt>, b) 中。
\pre 断言： <tt>GetZeroElement<_type>() < b</tt> 。
*/
PDefTH1(_type)
inline bool
IsInInterval(_type i, _type b)
{
	YAssert(GetZeroElement<_type>() < b,
		"Zero element as lower bound is not less than upper bound"
		" @ IsInterval<_type>(_type, _type);");

	return !(i < GetZeroElement<_type>()) && i < b;
}
/*!
\brief 判断 i 是否在左闭右开区间 [a, b) 中。
\pre 断言： <tt>a < b</tt> 。
*/
PDefTH1(_type)
inline bool
IsInInterval(_type i, _type a, _type b)
{
	YAssert(a < b, "Lower bound is not less than upper bound"
		" @ IsInInterval<_type>(_type, _type, _type);");

	return !(i < a) && i < b;
}

/*!
\brief 判断 i 是否在开区间 (GetZeroElement<_type>(), b) 内。
\pre 断言： <tt>GetZeroElement<_type>() < b</tt> 。
*/
PDefTH1(_type)
inline bool
IsInOpenInterval(_type i, _type b)
{
	YAssert(GetZeroElement<_type>() < b,
		"Zero element as lower bound is not less than upper bound"
		" @ IsInOpenInterval<_type>(_type, _type);");

	return i > GetZeroElement<_type>() && i < b;
}
/*!
\brief 判断 i 是否在开区间 (a, b) 内。
\pre 断言： <tt>a < b</tt> 。
*/
PDefTH1(_type)
inline bool
IsInOpenInterval(_type i, _type a, _type b)
{
	YAssert(a < b,
		"Lower bound is not less than upper bound"
		" @ IsInOpenInterval<_type>(_type, type, _type);");

	return i > a && i < b;
}

/*!
\brief 计算满足指定的值 v 在区间 [<tt>a[i]</tt>, <tt>a[i + 1]</tt>) 内最小的 i 。
\pre 断言： <tt>a</tt> 。
\pre 断言： <tt>n != 0</tt> 。
\pre 断言： <tt>!(v < *a)</tt> 。

*/
PDefTH1(_type)
size_t
SwitchInterval(_type v, const _type* a, size_t n)
{
	YAssert(a, "Null array pointer found"
		" @ SwitchInterval<_type>(_type, const _type*, size_t);");
	YAssert(n != 0, "Zero length of array found"
		" @ SwitchInterval<_type>(_type, const _type*, size_t);");
	YAssert(!(v < *a), "Value less than lower bound found"
		" @ SwitchInterval<_type>(_type, const _type*, size_t);");

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
*/
PDefTH1(_type)
size_t
SwitchAddedInterval(_type v, const _type* a, size_t n)
{
	YAssert(a, "Null array pointer found"
		" @ SwitchAddedInterval<_type>(_type, const _type*, size_t);");
	YAssert(n != 0, "Zero length of array found"
		" @ SwitchAddedInterval<_type>(_type, const _type*, size_t);");
	YAssert(!(v < *a), "Value less than lower bound found"
		" @ SwitchAddedInterval<_type>(_type, const _type*, size_t);");

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
*/
PDefTH1(_type)
void
RestrictInClosedInterval(_type& i, int a, int b)
{
	YAssert(!(b < a), "Upper bound is less than lower bound"
		" @ estrictInClosedInterval<_type>(_type&, int, int);");

	if(i < a)
		i = a;
	else if(b < i)
		i = b;
}

/*!
\brief 约束整数 i 在左闭右开区间 [a, b) 中。
\pre 断言： <tt>a < b</tt> 。
\post <tt>!(i < a) && i < b</tt> 。
*/
PDefTH1(_type)
void
RestrictInInterval(_type& i, int a, int b)
{
	YAssert(a < b, "Lower bound is not less than upper bound"
		" @ RestrictInInterval<_type>(_type&, int, int);");

	if(i < a)
		i = a;
	else if(!(i < b))
		i = b - 1;
}

/*!
\brief 约束无符号整数 u 在区间上界 b 内。
\post <tt>!(b < u)</tt>。
*/
PDefTH1(_type)
void
RestrictUnsignedStrict(_type& u, unsigned b)
{
	if(b < u)
		u = b;
}

/*!
\brief 约束无符号整数 u 在左闭右开区间 [0, b) 中。
\pre 断言： <tt>b != GetZeroElement<_type>()</tt> 。
\post <tt>!(u < GetZeroElement<_type>()) && u < b</tt> 。
*/
PDefTH1(_type)
void
RestrictUnsigned(_type& u, unsigned b)
{
	YAssert(b != GetZeroElement<_type>(), "Zero upper bound found"
		" @ RestrictUnsigned<_type>(_type& u, unsigned b);");

	if(!(u < b))
		u = b - 1;
}

/*!
\brief 约束关系：a ≤ b 。
\post <tt>a <= b</tt> 。
*/
PDefTH1(_type)
inline void
RestrictLessEqual(_type& a, _type& b)
{
	if(b < a)
		std::swap(a, b);
}


/*!
\brief 清除指定的连续区域。
\note 忽略空指针和零长度。
*/
PDefTH1(_type)
void
ClearSequence(_type* dst, size_t n)
{
	if(dst && n)
		mmbset(dst, 0, sizeof(_type) * n);
}


//! \brief delete 仿函数。
struct delete_obj_ndebug
{
	/*!
	\brief 删除指针指向的对象。
	*/
	PDefTH1(_type)
	inline void
	operator()(_type* _ptr) ynothrow
	{
		delete _ptr;
	}
};


#ifdef YSL_USE_MEMORY_DEBUG

//! \brief delete 仿函数（调试版本）。
struct delete_obj_debug
{
	/*!
	\brief 删除指针指向的对象。
	*/
	PDefTH1(_type)
	inline void
	operator()(_type* _ptr) ynothrow
	{
		ydelete(_ptr);
	}
};

#	define delete_obj delete_obj_debug

#else

#	define delete_obj delete_obj_ndebug

#endif

//! \brief 带置空指针操作的 delete 仿函数。
struct safe_delete_obj
{
	/*!
	\brief 删除指针指向的对象，并置指针为空值。
	*/
	PDefTH1(_tPointer)
	inline void
	operator()(_tPointer& _ptr) ynothrow
	{
		reset(_ptr);
	}
};


/*!
\brief 使用 new 复制指定指针指向的对象。
*/
PDefTH1(_type)
auto
CloneNonpolymorphic(const _type& p) -> decltype(&*p)
{
	return new typename std::remove_reference<decltype(*p)>::type(*p);
}

/*!
\brief 使用 Clone 成员函数复制指定指针指向的多态类类型对象。
\pre 断言： std::is_polymorphic<decltype(*p)>::value 。
*/
template<class _type>
auto
ClonePolymorphic(const _type& p) -> decltype(&*p)
{
	static_assert(std::is_polymorphic<typename
		std::remove_reference<decltype(*p)>::type>::value,
		"Non-polymorphic class type found @ ClonePolymorphic;");

	return p->Clone();
}

YSL_END

#endif

