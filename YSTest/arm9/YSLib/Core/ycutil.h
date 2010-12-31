/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycutil.h
\ingroup Core
\brief 核心实用模块。
\version 0.2383;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-23 06:10:59 + 08:00;
\par 修改时间:
	2010-12-31 12:15 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YCoreUtilities;
*/


#ifndef INCLUDED_YCUTIL_H_
#define INCLUDED_YCUTIL_H_

#include "ysdef.h"
#include <cstring>
#include <functional>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Design)

//不可复制对象：继承此类的对象在外部无法调用复制构造函数和复制赋值操作符。
class NonCopyable
{
//保护非多态类。
protected:
	/*!
	\brief 保护构造。
	\note 空实现。
	*/
	NonCopyable()
	{}
	/*!
	\brief 保护析构。
	\note 空实现。
	*/
	~NonCopyable()
	{}

private: 
	/*!
	\brief 禁止复制构造。
	\note 无实现。
	*/
	NonCopyable(const NonCopyable&);

	/*!
	\brief 禁止赋值复制。
	\note 无实现。
	*/
	NonCopyable& operator=(const NonCopyable&);
};

YSL_END_NAMESPACE(Design)

using Design::NonCopyable;


/*!
\brief 判断引用相等关系。
*/
template<typename _type>
inline bool
ReferenceEquals(const _type& a, const _type& b)
{
	return &a == &b;
}


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
	return _type();
}


/*!
\brief 取值类型最小值。
*/
template<typename _type>
inline _type
vmin(_type a, _type b)
{
	return b < a ? b : a;
}
/*!
\brief 取值类型最大值。
*/
template<typename _type>
inline _type
vmax(_type a, _type b)
{
	return a < b ? b : a;
}
/*!
\brief 取值类型最小值。
\note 使用指定判断操作。
*/
template<typename _type, typename _fCompare>
inline _type
vmin(_type a, _type b, _fCompare _comp)
{
	return _comp(b, a) ? b : a;
}
/*!
\brief 取值类型最大值。
\note 使用指定判断操作。
*/
template<typename _type, typename _fCompare>
inline _type
vmax(_type a, _type b, _fCompare _comp)
{
	return _comp(a, b) ? b : a;
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
std::size_t
SwitchInterval(_type v, const _type* a, std::size_t n)
{
	YAssert(a,
		"In function \"template<typename _type>\n"
		"std::size_t\n"
		"SwitchInterval(_type v, const _type* a, std::size_t n)\":\n"
		"The array pointer is null.");
	YAssert(n != 0,
		"In function \"template<typename _type>\n"
		"std::size_t\n"
		"SwitchInterval(_type v, const _type* a, std::size_t n)\":\n"
		"The length of array is zero.");
	YAssert(!(v < *a),
		"In function \"template<typename _type>\n"
		"std::size_t\n"
		"SwitchInterval(_type v, const _type* a, std::size_t n)\":\n"
		"The value is less than lower bound.");

	std::size_t i(0);

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
std::size_t
SwitchAddedInterval(_type v, const _type* a, std::size_t n)
{
	YAssert(a,
		"In function \"template<typename _type>\n"
		"std::size_t\n"
		"SwitchAddedInterval(_type v, const _type* a, std::size_t n)\":\n"
		"The array pointer is null.");
	YAssert(n != 0,
		"In function \"template<typename _type>\n"
		"std::size_t\n"
		"SwitchAddedInterval(_type v, const _type* a, std::size_t n)\":\n"
		"Length of array is zero.");
	YAssert(!(v < *a),
		"In function \"template<typename _type>\n"
		"std::size_t\n"
		"SwitchAddedInterval(_type v, const _type* a, std::size_t n)\":\n"
		"Value is less than lower bound.");

	_type s(*a);
	std::size_t i(0);

	while(!(++i == n || v < (s += a[i])))
		;
	return i - 1;
}

/*!
\brief 约束整数 i 在闭区间 [a, b] 中。
\pre 断言：a < b 。
\post !(i < a || b < i) 。
*/
template<typename _type>
void
RestrictInClosedInterval(_type& i, int a, int b)
{
	YAssert(a < b,
		"In function \"template<typename _type>\n"
		"void\n"
		"RestrictInClosedInterval(_type& i, int a, int b)\":\n"
		"Lower bound is not less than upper bound.");

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
ClearSequence(_type* dst, std::size_t n)
{
	if(dst && n)
		mmbset(dst, 0, sizeof(_type) * n);
}


//! \brief delete 仿函数。
struct delete_obj
{
	/*!
	\brief 删除指针指向的对象。
	*/
	template<typename _type>
	inline void
	operator()(_type* _ptr) ythrow()
	{
		delete _ptr;
	}
};


//! \brief 带置空指针操作的 delete 仿函数。
struct safe_delete_obj
{
	/*!
	\brief 删除指针指向的对象，并置指针为空值。
	*/
	template<typename _tPointer>
	inline void
	operator()(_tPointer& _ptr) ythrow()
	{
		YReset(_ptr);
	}
};


typedef delete_obj delete_obj_ndebug;
typedef safe_delete_obj safe_delete_obj_ndebug;


#ifdef YSL_USE_MEMORY_DEBUG

//! \brief delete 仿函数（调试版本）。
struct delete_obj_debug
{
	/*!
	\brief 删除指针指向的对象。
	*/
	template<typename _type>
	inline void
	operator()(_type* _ptr) ythrow()
	{
		ydelete(_ptr);
	}
};


//! \brief 带置空指针操作的 delete 仿函数（调试版本）。
struct safe_delete_obj_debug
{
	/*!
	\brief 删除指针指向的对象，并置指针为空值。
	*/
	template<typename _tPointer>
	inline void
	operator()(_tPointer& _ptr) ythrow()
	{
		YDelete_Debug(_ptr);
	}
};

#	define delete_obj delete_obj_debug
#	define safe_delete_obj safe_delete_obj_debug

#endif


//! \brief 引用仿函数。
template<typename _type>
struct deref_op : std::unary_function<_type, _type*>
{
	/*!
	\brief 对指定对象使用 operator& 并返回结果。
	*/
	inline _type*
	operator()(_type& _x) const
	{
		return &_x;
	}
};


//! \brief 常量引用仿函数。
template<typename _type>
struct const_deref_op : std::unary_function<const _type, const _type*>
{
	/*!
	\brief 对指定 const 对象使用 operator& 并返回结果。
	*/
	inline const _type*
	operator()(const _type& _x) const
	{
		return &_x;
	}
};


//! \brief 间接访问比较仿函数。
template<
	typename _type, typename _tPointer = _type*,
	template<typename _type> class _fCompare = std::less
>
struct deref_comp : _fCompare<_type>
{
	/*!
	\brief 返回 _fCompare<_type>::operator()(*_x, *_y) 。
	\note 如有空指针则不进行判断，直接返回 false 。
	*/
	bool
	operator()(const _tPointer& _x, const _tPointer& _y) const
	{
		return _x && _y && _fCompare<_type>::operator()(*_x, *_y);
	}
};


//! \brief 间接访问字符串比较仿函数。
template<
	typename _tChar,
	int (*_lexi_cmp)(const _tChar*, const _tChar*) = std::strcmp,
	class _fCompare = std::less<int>
>
struct deref_str_comp : _fCompare
{
	/*!
	\brief 返回 _fCompare::operator()(_lexi_cmp(_x, _y), 0) 。
	\note 如有空指针则不进行判断，直接返回 false 。
	*/
	bool
	operator()(const _tChar* _x, const _tChar* _y) const
	{
		return _x && _y && _fCompare::operator()(_lexi_cmp(_x, _y), 0);
	}
};


/*!	\defgroup Algorithm Gerneral Algorithm
\brief 算法。
*/

/*!
\ingroup Algorithm
\brief 删除指定标准容器中所有相同元素。
*/
template<typename _tContainer>
typename _tContainer::size_type
erase_all(_tContainer& _container,
	const typename _tContainer::value_type& _value)
{
	int n(0);
	typename _tContainer::iterator i;

	while((i = std::find(_container.begin(), _container.end(), _value))
		!= _container.end())
	{
		_container.erase(i);
		++n;
	}
	return n;
}

/*!
\ingroup Algorithm
\brief 删除指定标准容器中所有满足条件元素。
*/
template<typename _tContainer, typename _fPredicate>
typename _tContainer::size_type
erase_all_if(_tContainer& _container,
	const typename _tContainer::value_type& _pred)
{
	int n(0);
	typename _tContainer::iterator i;

	while((i = std::find(_container.begin(), _container.end(), _pred))
		!= _container.end())
		if(_pred(*i))
		{
			_container.erase(i);
			++n;
		}
	return n;
}

/*!
\ingroup Algorithm
\brief 按指定键值搜索指定映射。
\return 一个用于表示结果的 std::pair 对象，其 first 成员为迭代器， second 成员
	表示是否需要插入（行为同 std::map::operator[] ）。
*/
template<class _tMap>
std::pair<typename _tMap::iterator, bool>
search_map(_tMap& m, const typename _tMap::key_type& k)
{
	typename _tMap::iterator i(m.lower_bound(k));

	return std::make_pair(i, (i == m.end() || m.key_comp()(k, i->first)));
}

YSL_END

#endif

