/*
	© 2010-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YCoreUtilities.h
\ingroup Core
\brief 核心实用模块。
\version r2506
\author FrankHB <frankhb1989@gmail.com>
\since build 539
\par 创建时间:
	2010-05-23 06:10:59 +0800
\par 修改时间:
	2017-06-19 03:08 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YCoreUtilities
*/


#ifndef YSL_INC_Core_YCoreUtilities_h_
#define YSL_INC_Core_YCoreUtilities_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YException // for std::string, LoggedEvent, string;
#include <ystdex/algorithm.hpp> // for ystdex::clamp;

namespace YSLib
{

/*!
\brief 符号函数。
\note 若 <tt>a < b</tt> 则返回 -1 ，否则若 <tt>a = b</tt> 则返回 0 ，否则返回 1 。
\since build 633
*/
template<typename _type>
yconstfn YB_STATELESS std::int_fast8_t
FetchSign(_type a, _type b = _type(0)) ynothrow
{
	return a < b ? -1 : !(a == b);
}

/*!
\brief 判断 d 和以 [a, b](a ≤ b) 或 [b, a](a > b) 区间的关系。
\return < 0 ：d 在区间外；
\return = 0 ：d 在区间端点上；
\return > 0 ：d 在区间内。
\note 无精度修正。
\note 使用 ADL FetchSign 。
\since build 633
*/
template<typename _type>
yconstfn YB_STATELESS std::int_fast8_t
FetchSignFromInterval(_type d, _type a, _type b) ynothrow
{
	return FetchSign(a, d) * FetchSign(d, b);
}

/*!
\brief 计算指定类型的差值的一半。
\since build 554
*/
template<typename _type>
yconstfn YB_STATELESS _type
HalfDifference(_type x, _type y)
{
	return (x - y) / 2;
}

/*!
\brief 判断 i 是否在左闭右开区间 [_type(0), b) 中。
\pre 断言： <tt>_type(0) < b</tt> 。
\since build 319
*/
template<typename _type>
inline YB_STATELESS bool
IsInInterval(_type i, _type b) ynothrow
{
	YAssert(_type(0) < b,
		"Zero element as lower bound is not less than upper bound.");
	return !(i < _type(0)) && i < b;
}
/*!
\brief 判断 i 是否在左闭右开区间 [a, b) 中。
\pre 断言： <tt>a < b</tt> 。
\since build 319
*/
template<typename _type>
inline YB_STATELESS bool
IsInInterval(_type i, _type a, _type b) ynothrow
{
	YAssert(a < b, "Lower bound is not less than upper bound.");
	return !(i < a) && i < b;
}

/*!
\brief 判断 i 是否在闭区间 [_type(0), b] 中。
\pre 断言： <tt>_type(0) < b</tt> 。
\since build 470
*/
template<typename _type>
inline YB_STATELESS bool
IsInClosedInterval(_type i, _type b) ynothrow
{
	YAssert(_type(0) < b,
		"Zero element as lower bound is not less than upper bound.");
	return !(i < _type(0) || b < i);
}
/*!
\brief 判断 i 是否在闭区间 [a, b] 中。
\pre 断言： <tt>a < b</tt> 。
\since build 470
*/
template<typename _type>
inline YB_STATELESS bool
IsInClosedInterval(_type i, _type a, _type b) ynothrow
{
	YAssert(a < b, "Lower bound is not less than upper bound.");
	return !(i < a || b < i);
}

/*!
\brief 判断 i 是否在开区间 (_type(0), b) 内。
\pre 断言： <tt>_type(0) < b</tt> 。
\since build 319
*/
template<typename _type>
inline YB_STATELESS bool
IsInOpenInterval(_type i, _type b) ynothrow
{
	YAssert(_type(0) < b,
		"Zero element as lower bound is not less than upper bound.");
	return _type(0) < i && i < b;
}
/*!
\brief 判断 i 是否在开区间 (a, b) 内。
\pre 断言： <tt>a < b</tt> 。
\since build 319
*/
template<typename _type>
inline YB_STATELESS bool
IsInOpenInterval(_type i, _type a, _type b) ynothrow
{
	YAssert(a < b, "Lower bound is not less than upper bound.");
	return a < i && i < b;
}

/*!
\pre 断言： <tt>n != 0</tt> 。
\pre 间接断言： <tt>a</tt> 。
\pre 断言： <tt>!(v < *a)</tt> 。
\since build 319
*/
//@{
/*!
\brief 计算满足指定的值 v 在区间 [\c a[i], <tt>a[i + 1]</tt>) 内最小的 i 。
*/
template<typename _type>
YB_PURE YB_NONNULL(2) size_t
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
\brief 计算满足指定的值 v 在区间 [s(i), s(i + 1)) 内的最小的 i ；
	其中 s(i) 是 \c a[i] 前 i 项的和。
*/
template<typename _type>
YB_PURE YB_NONNULL(2) size_t
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
//@}

/*!
\brief 约束 v 在闭区间 [a, b] 中。
\pre 间接断言： <tt>!(b < a)</tt> 。
\post <tt>!(i < a || b < i)</tt> 。
\since build 448
*/
template<typename _type>
void
RestrictInClosedInterval(_type& v, const _type& a, const _type& b) ynothrow
{
	v = ystdex::clamp(v, a, b);
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
	static_assert(std::is_unsigned<_type>(), "Invalid type found.");

	if(b < u)
		u = b;
}

/*!
\brief 约束无符号整数 u 在左闭右开区间 [0, b) 中。
\pre 断言： <tt>b != _type(0)</tt> 。
\post <tt>!(u < _type(0)) && u < b</tt> 。
\since build 319
*/
template<typename _type>
void
RestrictUnsigned(_type& u, unsigned b) ynothrow
{
	YAssert(b != _type(0), "Zero upper bound found.");
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
\throw LoggedEvent 范围检查失败。
\note 对运行时由外部引入数值的检查，失败抛出运行时异常。
\note 不依赖运行时引入数值的检查可使用 ystdex::narrow 代替。
\since build 624
\sa ystdex::narrow
*/
//@{
//! \since build 703
//@{
//! \brief 检查算术类型数值不小于指定类型的下界。
template<typename _tDst, typename _type>
inline _tDst
CheckLowerBound(_type val, const std::string& name = {}, RecordLevel lv = Err)
{
	using namespace ystdex;
	// XXX: See WG21 N3387.
	// TODO: Add and use safe %common_arithmetic_type interface instead?
	using common_t = typename ystdex::common_int_type<_tDst, _type>::type;

	if(!(common_t(val) < common_t(std::numeric_limits<_tDst>::min())))
		return _tDst(val);
	throw
		LoggedEvent("Value of '" + name + "' is less than lower boundary.", lv);
}

//! \brief 检查算术类型数值不大于指定类型的上界。
template<typename _tDst, typename _type>
inline _tDst
CheckUpperBound(_type val, const std::string& name = {}, RecordLevel lv = Err)
{
	using namespace ystdex;
	// XXX: See WG21 N3387.
	// TODO: Add and use safe %common_arithmetic_type interface instead?
	using common_t = typename ystdex::common_int_type<_tDst, _type>::type;

	if(!(common_t(std::numeric_limits<_tDst>::max()) < common_t(val)))
		return _tDst(val);
	throw LoggedEvent("Value of '" + name + "' is greater than upper boundary.",
		lv);
}

//! \brief 检查算术类型数值在指定类型的范围内。
template<typename _tDst, typename _type>
inline _tDst
CheckArithmetic(_type val, const std::string& name = {}, RecordLevel lv = Err)
{
	return
		CheckUpperBound<_tDst>(CheckLowerBound<_tDst>(val, name, lv), name, lv);
}
//@}

//! \brief 检查非负算术类型数值在指定类型的范围内。
template<typename _tDst, typename _type>
inline _tDst
CheckNonnegative(_type val, const std::string& name = {}, RecordLevel lv = Err)
{
	if(val < 0)
		// XXX: Use more specified exception type.
		throw LoggedEvent("Failed getting nonnegative " + name + " value.", lv);
	return CheckUpperBound<_tDst>(val, name, lv);
}

//! \brief 检查正算术类型数值在指定类型的范围内。
template<typename _tDst, typename _type>
inline _tDst
CheckPositive(_type val, const std::string& name = {}, RecordLevel lv = Err)
{
	if(!(0 < val))
		// XXX: Use more specified exception type.
		throw LoggedEvent("Failed getting positive " + name + " value.", lv);
	return CheckUpperBound<_tDst>(val, name, lv);
}
//@}


/*!
\brief 清除指定的连续对象。
\pre 设类型 T 为 <tt>ystdex::decay_t<decltype(*dst)></tt>， 则应满足
	<tt>std::is_pod<T>() || (std::is_nothrow_default_constructible<T>()
		&& std::is_nothrow_assignable<T, T>())</tt> 。
\since build 624
*/
template<typename _tOut>
inline void
ClearSequence(_tOut dst, size_t n) ynothrowv
{
	using _type = ystdex::decay_t<decltype(*dst)>;
	static_assert(std::is_pod<_type>()
		|| (std::is_nothrow_default_constructible<_type>()
		&& std::is_nothrow_assignable<_type, _type>()),
		"Invalid type found.");

	std::fill_n(dst, n, _type());
}


/*!
\brief 参数字符串向量。
\since build 797

用于存储命令行参数等字符串向量。
*/
class YF_API ArgumentsVector final
{
public:
	vector<string> Arguments{};

	/*!
	\brief 设置值。
	\pre 间接断言： 第二参数的元素都是非空指针。
	\exception LoggedEvent 输入的参数数小于 0 。
	\sa CheckNonnegative
	\since build 797

	设置参数向量的值。
	参数分别指定参数向量和表示 NTMBS 的指针数组。
	*/
	void
	Reset(int, char*[]);
};


/*!
\brief 查询第二参数指定名称的环境变量写入第一参数。
\pre 断言：参数非空。
\note 若不存在则不修改第一参数。
\return 是否修改第一参数。
\since build 659
*/
YF_API YB_NONNULL(2) bool
FetchEnvironmentVariable(string&, const char*);

} // namespace YSLib;

#endif

