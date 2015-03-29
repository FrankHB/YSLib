/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YCoreUtilities.h
\ingroup Core
\brief 核心实用模块。
\version r2155
\author FrankHB <frankhb1989@gmail.com>
\since build 539
\par 创建时间:
	2010-05-23 06:10:59 +0800
\par 修改时间:
	2015-03-25 10:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YCoreUtilities
*/


#ifndef YSL_INC_Core_YCoreUtilities_h_
#define YSL_INC_Core_YCoreUtilities_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YException // for YSLib::LoggedEvent;
#include YFM_YSLib_Adaptor_YContainer // for YSLib::string;

namespace YSLib
{

/*!
\brief 转换类型选择。
\since build 201

若 \c _type 能隐式转换为 \c _tStrict 则 \c Result 为 \c _tStrict，
	否则 \c Result 为 \c _tWeak 。
*/
template<typename _type, typename _tStrict, typename _tWeak>
struct MoreConvertible
{
	using Result = ystdex::conditional_t<
		std::is_convertible<_type, _tStrict>::value, _tStrict, _tWeak>;

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
\since build 201
*/
template<typename _type, typename _tStrict>
struct SelectConvertible : MoreConvertible<_type, _tStrict, _type>
{
	using Result = typename MoreConvertible<_type, _tStrict, _type>::Result;
};


/*!
\brief 取指定类型的零元素。
\since build 319
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
\since build 319
*/
yconstfn std::int8_t
FetchSign(int a, int b = 0) ynothrow
{
	return a < b ? -1 : !(a == b);
}
/*!
\brief 符号函数。
\note 若 <tt>a < b</tt> 则返回 -1 ，否则若 <tt>a = b</tt> 则返回 0 ，否则返回 1 。
\since build 319
*/
template<typename _type>
yconstfn std::int8_t
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
\since build 319
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
\since build 319
*/
template<typename _type>
yconstfn int
FetchSignFromInterval(const _type& d, const _type& a, const _type& b) ynothrow
{
	return FetchSign(a, d) * FetchSign(d, b);
}

/*!
\brief 计算指定类型的差值的一半。
\since build 554
*/
template<typename _type>
yconstfn _type
HalfDifference(_type x, _type y)
{
	return (x - y) / 2;
}

/*!
\brief 判断 i 是否在左闭右开区间 [<tt>FetchZero<_type>()</tt>, b) 中。
\pre 断言： <tt>FetchZero<_type>() < b</tt> 。
\since build 319
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
\since build 319
*/
template<typename _type>
inline bool
IsInInterval(_type i, _type a, _type b) ynothrow
{
	YAssert(a < b, "Lower bound is not less than upper bound.");
	return !(i < a) && i < b;
}

/*!
\brief 判断 i 是否在闭区间 [FetchZero<_type>(), b] 中。
\pre 断言： <tt>FetchZero<_type>() < b</tt> 。
\since build 470
*/
template<typename _type>
inline bool
IsInClosedInterval(_type i, _type b) ynothrow
{
	YAssert(FetchZero<_type>() < b,
		"Zero element as lower bound is not less than upper bound.");
	return !(i < FetchZero<_type>() || b < i);
}
/*!
\brief 判断 i 是否在闭区间 [a, b] 中。
\pre 断言： <tt>a < b</tt> 。
\since build 470
*/
template<typename _type>
inline bool
IsInClosedInterval(_type i, _type a, _type b) ynothrow
{
	YAssert(a < b, "Lower bound is not less than upper bound.");
	return !(i < a || b < i);
}

/*!
\brief 判断 i 是否在开区间 (FetchZero<_type>(), b) 内。
\pre 断言： <tt>FetchZero<_type>() < b</tt> 。
\since build 319
*/
template<typename _type>
inline bool
IsInOpenInterval(_type i, _type b) ynothrow
{
	YAssert(FetchZero<_type>() < b,
		"Zero element as lower bound is not less than upper bound.");
	return FetchZero<_type>() < i && i < b;
}
/*!
\brief 判断 i 是否在开区间 (a, b) 内。
\pre 断言： <tt>a < b</tt> 。
\since build 319
*/
template<typename _type>
inline bool
IsInOpenInterval(_type i, _type a, _type b) ynothrow
{
	YAssert(a < b,
		"Lower bound is not less than upper bound.");
	return a < i && i < b;
}

/*!
\brief 计算满足指定的值 v 在区间 [<tt>a[i]</tt>, <tt>a[i + 1]</tt>) 内最小的 i 。
\pre 断言： <tt>a</tt> 。
\pre 断言： <tt>n != 0</tt> 。
\pre 断言： <tt>!(v < *a)</tt> 。
\since build 319
*/
template<typename _type>
size_t
SwitchInterval(_type v, const _type* a, size_t n) ynothrow
{
	YAssert(n != 0, "Zero length of array found.");
	YAssert(!(v < Deref(a)), "Value less than lower bound found.");

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
\since build 319
*/
template<typename _type>
size_t
SwitchAddedInterval(_type v, const _type* a, size_t n) ynothrow
{
	YAssert(n != 0, "Zero length of array found.");
	YAssert(!(v < Deref(a)), "Value less than lower bound found.");

	_type s(*a);
	size_t i(0);

	while(!(++i == n || v < (s += a[i])))
		;
	return i - 1;
}

/*!
\brief 约束 v 在闭区间 [a, b] 中。
\pre 断言： <tt>!(b < a)</tt> 。
\post <tt>!(i < a || b < i)</tt> 。
\since build 448
*/
template<typename _type>
void
RestrictInClosedInterval(_type& v, const _type& a, const _type& b) ynothrow
{
	YAssert(!(b < a), "Upper bound is less than lower bound.");
	if(v < a)
		v = a;
	else if(b < v)
		v = b;
}

/*!
\brief 约束整数 i 在左闭右开区间 [a, b) 中。
\pre 断言： <tt>a < b</tt> 。
\post <tt>!(i < a) && i < b</tt> 。
\since build 519
*/
template<typename _type>
void
RestrictInInterval(_type& i, const _type& a, const _type& b) ynothrow
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
\since build 586
*/
template<typename _type>
void
RestrictUnsignedStrict(_type& u, _type b) ynothrow
{
	static_assert(std::is_unsigned<_type>::value, "Invalid type found.");

	if(b < u)
		u = b;
}

/*!
\brief 约束无符号整数 u 在左闭右开区间 [0, b) 中。
\pre 断言： <tt>b != FetchZero<_type>()</tt> 。
\post <tt>!(u < FetchZero<_type>()) && u < b</tt> 。
\since build 319
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
\since build 319
*/
template<typename _type>
inline void
RestrictLessEqual(_type& a, _type& b) ynothrow
{
	if(b < a)
		std::swap(a, b);
}


/*!
\since build 557
\throw LoggedEvent 范围检查失败。
*/
//@{
//! \brief 检查标量数值在指定类型的范围内。
template<typename _tDst, typename _type>
inline _tDst
CheckScalar(_type val, const std::string& name = "",
	LoggedEvent::LevelType lv = Err)
{
	using common_t = ystdex::common_type_t<_tDst, _type>;

	if(YB_UNLIKELY(common_t(val) > common_t(std::numeric_limits<_tDst>::max())))
		throw LoggedEvent(name + " value out of range.", lv);
	return _tDst(val);
}

//! \brief 检查非负标量数值在指定类型的范围内。
template<typename _tDst, typename _type>
inline _tDst
CheckNonnegativeScalar(_type val, const std::string& name = "",
	LoggedEvent::LevelType lv = Err)
{
	if(val < 0)
		// XXX: Use more specified exception type.
		throw LoggedEvent("Failed getting nonnegative " + name + " value.", lv);
	return CheckScalar<_tDst>(val, name, lv);
}

//! \brief 检查正标量数值在指定类型的范围内。
template<typename _tDst, typename _type>
inline _tDst
CheckPositiveScalar(_type val, const std::string& name = "",
	LoggedEvent::LevelType lv = Err)
{
	if(!(0 < val))
		// XXX: Use more specified exception type.
		throw LoggedEvent("Failed getting positive " + name + " value.", lv);
	return CheckScalar<_tDst>(val, name, lv);
}
//@}


/*!
\brief 清除指定的连续对象。
\pre 设类型 T 为 <tt>ystdex::remove_reference_t<decltype(*dst)></tt>， 则应满足
	<tt>std::is_pod<T> || (std::is_nothrow_default_constructible<T>::value
		&& std::is_nothrow_assignable<T, T>::value)</tt> 。
\note 忽略空指针和零长度。
\since build 322
*/
template<typename _tOut>
inline void
ClearSequence(_tOut dst, size_t n) ynothrow
{
	using _type = ystdex::remove_reference_t<decltype(*dst)>;
	static_assert(std::is_pod<_type>::value
		|| (std::is_nothrow_default_constructible<_type>::value
		&& std::is_nothrow_assignable<_type, _type>::value),
		"Invalid type found.");

	if(YB_LIKELY(dst && n))
		std::fill_n(dst, n, _type());
}


/*!
\brief delete 仿函数。
\since build 174
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
\since build 281
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
\since build 174
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
\since build 281
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
\since build 154
*/
struct safe_delete_obj
{
	/*!
	\brief 删除指针指向的对象，并置指针为空值。
	\note 使用 ADL \c reset 。
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
\since build 240
*/
template<typename _type>
yconstfn auto
CloneNonpolymorphic(const _type& p) -> decltype(&*p)
{
	return new typename ystdex::remove_reference_t<decltype(*p)>(*p);
}

/*!
\brief 使用 clone 成员函数复制指定指针指向的多态类类型对象。
\pre 断言： std::is_polymorphic<decltype(*p)>::value 。
\since build 240
*/
template<class _type>
auto
ClonePolymorphic(const _type& p) -> decltype(&*p)
{
	static_assert(std::is_polymorphic<ystdex::remove_reference_t<decltype(*p)>>
		::value, "Non-polymorphic class type found.");

	return p->clone();
}


/*!
\brief 查询第二参数指定名称的环境变量写入第一参数。
\note 若不存在则不修改第一参数。
\return 是否修改第一参数。
\since build 539
*/
YF_API bool
FetchEnvironmentVariable(string&, const string&);

} // namespace YSLib;

#endif

