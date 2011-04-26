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
\version 0.2585;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-23 06:10:59 +0800;
\par 修改时间:
	2011-04-25 12:27 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YCoreUtilities;
*/


#ifndef YSL_INC_CORE_YCUTIL_H_
#define YSL_INC_CORE_YCUTIL_H_

#include "ysdef.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Design)

typedef ystdex::noncopyable NonCopyable;


/*!
\brief 转换类型选择。

若 _type 能隐式转换为 _tStrict 则 Result 为 _tStrict，否则 Result 为 _tWeak 。
*/
template<typename _type, typename _tStrict, typename _tWeak>
struct MoreConvertible
{
	typedef typename Select<ystdex::is_convertible<_type, _tStrict>::value,
			_tStrict, _tWeak>::Result Result;

	inline static Result
	Cast(_type o)
	{
		return static_cast<Result>(o);
	}
};


/*!
\brief 转换类型选择。

若 _type 能隐式转换为 _tStrict 则 Result 为 _tStrict，否则 Result 为 _type 。
*/
template<typename _type, typename _tStrict>
struct SelectConvertible : MoreConvertible<_type, _tStrict, _type>
{
	typedef typename MoreConvertible<_type, _tStrict, _type>::Result
		Result;
};

YSL_END_NAMESPACE(Design)

using Design::NonCopyable;

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
template<typename _type>
inline _type
GetZeroElement()
{
	return _type(0);
}

/*!
\brief 整数类型符号函数。
\note 当 a < b 时返回 -1 ，否则当 a = b 时返回 0 ，否则返回 1 。
*/
inline s8
sgn(int a, int b = 0)
{
	return a < b ? -1 : !(a == b);
}
/*!
\brief 符号函数。
\note 当 a < b 时返回 -1 ，否则当 a = b 时返回 0 ，否则返回 1 。
*/
template<typename _type>
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
template<typename _type>
int
sgnInterval(const _type& d, const _type& a, const _type& b)
{
	return sgn(a, d) * sgn(d, b);
}

/*!
\brief 判断 i 是否在左闭右开区间 [GetZeroElement<_type>(), b) 中。
\pre 断言：GetZeroElement<_type>() < b 。
*/
template<typename _type>
inline bool
IsInInterval(_type i, _type b)
{
	YAssert(GetZeroElement<_type>() < b,
		"In function \"template<typename _type>\n"
		"inline bool\n"
		"IsInInterval(_type i, _type b)\":\n"
		"Zero element as lower bound is not less than upper bound.");

	return !(i < GetZeroElement<_type>()) && i < b;
}
/*!
\brief 判断 i 是否在左闭右开区间 [a, b) 中。
\pre 断言：a < b 。
*/
template<typename _type>
inline bool
IsInInterval(_type i, _type a, _type b)
{
	YAssert(a < b,
		"In function \"template<typename _type>\n"
		"inline bool\n"
		"IsInInterval(_type i, _type a, _type b)\":\n"
		"Lower bound is not less than upper bound.");

	return !(i < a) && i < b;
}

/*!
\brief 判断 i 是否在开区间 (GetZeroElement<_type>(), b) 内。
\pre 断言：GetZeroElement<_type>() < b 。
*/
template<typename _type>
inline bool
IsInOpenInterval(_type i, _type b)
{
	YAssert(GetZeroElement<_type>() < b,
		"In function \"template<typename _type>\n"
		"inline bool\n"
		"IsInOpenInterval(_type i, _type b)\":\n"
		"Zero element as lower bound is not less than upper bound.");

	return i > GetZeroElement<_type>() && i < b;
}
/*!
\brief 判断 i 是否在开区间 (a, b) 内。
\pre 断言：a < b 。
*/
template<typename _type>
inline bool
IsInOpenInterval(_type i, _type a, _type b)
{
	YAssert(a < b,
		"In function \"template<typename _type>\n"
		"inline bool\n"
		"IsInOpenInterval(_type i, _type a, _type b)\":\n"
		"Lower bound is not less than upper bound.");

	return i > a && i < b;
}

/*!
\brief 计算满足指定的值 v 在区间 [a[i], a[i + 1]) 内的最小的 i 。
\pre 断言：a 。
\pre 断言：n != 0 。
\pre 断言：!(v < *a) 。

*/
template<typename _type>
size_t
SwitchInterval(_type v, const _type* a, size_t n)
{
	YAssert(a,
		"In function \"template<typename _type>\n"
		"size_t\n"
		"SwitchInterval(_type v, const _type* a, size_t n)\":\n"
		"The array pointer is null.");
	YAssert(n != 0,
		"In function \"template<typename _type>\n"
		"size_t\n"
		"SwitchInterval(_type v, const _type* a, size_t n)\":\n"
		"The length of array is zero.");
	YAssert(!(v < *a),
		"In function \"template<typename _type>\n"
		"size_t\n"
		"SwitchInterval(_type v, const _type* a, size_t n)\":\n"
		"The value is less than lower bound.");

	size_t i(0);

	while(!(++i == n || v < a[i]))
		;
	return i - 1;
}

/*!
\brief 计算满足指定的值 v 在区间 [b[i], b[i + 1]) 内的最小的 i ；
			其中 b[i] 是 a[i] 前 i 项的和。
\pre 断言：a 。
\pre 断言：n != 0 。
\pre 断言：!(v < *a) 。
*/
template<typename _type>
size_t
SwitchAddedInterval(_type v, const _type* a, size_t n)
{
	YAssert(a,
		"In function \"template<typename _type>\n"
		"size_t\n"
		"SwitchAddedInterval(_type v, const _type* a, size_t n)\":\n"
		"The array pointer is null.");
	YAssert(n != 0,
		"In function \"template<typename _type>\n"
		"size_t\n"
		"SwitchAddedInterval(_type v, const _type* a, size_t n)\":\n"
		"Length of array is zero.");
	YAssert(!(v < *a),
		"In function \"template<typename _type>\n"
		"size_t\n"
		"SwitchAddedInterval(_type v, const _type* a, size_t n)\":\n"
		"Value is less than lower bound.");

	_type s(*a);
	size_t i(0);

	while(!(++i == n || v < (s += a[i])))
		;
	return i - 1;
}

/*!
\brief 约束整数 i 在闭区间 [a, b] 中。
\pre 断言：!(b < a) 。
\post !(i < a || b < i) 。
*/
template<typename _type>
void
RestrictInClosedInterval(_type& i, int a, int b)
{
	YAssert(!(b < a),
		"In function \"template<typename _type>\n"
		"void\n"
		"RestrictInClosedInterval(_type& i, int a, int b)\":\n"
		"Upper bound is less than lower bound.");

	if(i < a)
		i = a;
	else if(b < i)
		i = b;
}

/*!
\brief 约束整数 i 在左闭右开区间 [a, b) 中。
\pre 断言：a < b 。
\post !(i < a) && i < b 。
*/
template<typename _type>
void
RestrictInInterval(_type& i, int a, int b)
{
	YAssert(a < b,
		"In function \"template<typename _type>\n"
		"void\n"
		"RestrictInInterval(_type& i, int a, int b)\":\n"
		"Lower bound is not less than upper bound.");

	if(i < a)
		i = a;
	else if(!(i < b))
		i = b - 1;
}

/*!
\brief 约束无符号整数 u 在区间上界 b 内。
\post !(b < u)。
*/
template<typename _type>
void
RestrictUnsignedStrict(_type& u, unsigned b)
{
	if(b < u)
		u = b;
}

/*!
\brief 约束无符号整数 u 在左闭右开区间 [0, b) 中。
\pre 断言：b != GetZeroElement<_type>() 。
\post !(u < GetZeroElement<_type>()) && u < b 。
*/
template<typename _type>
void
RestrictUnsigned(_type& u, unsigned b)
{
	YAssert(b != GetZeroElement<_type>(),
		"In function \"template<typename _type>\n"
		"void\n"
		"RestrictUnsigned(_type& u, unsigned b)\":\n"
		"Upper bound is zero.");

	if(!(u < b))
		u = b - 1;
}

/*!
\brief 约束关系：a ≤ b 。
\post a <= b 。
*/
template<typename _type>
inline void
RestrictLessEqual(_type& a, _type& b)
{
	if(b < a)
		std::swap(a, b);
}


/*!
\brief 清除指定的连续区域。
*/
template<typename _type>
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
	template<typename _type>
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
	template<typename _type>
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
	template<typename _tPointer>
	inline void
	operator()(_tPointer& _ptr) ynothrow
	{
		reset_pointer(_ptr);
	}
};

YSL_END

#endif

