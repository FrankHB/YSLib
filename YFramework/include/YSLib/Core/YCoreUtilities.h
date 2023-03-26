/*
	© 2010-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YCoreUtilities.h
\ingroup Core
\brief 核心实用模块。
\version r2719
\author FrankHB <frankhb1989@gmail.com>
\since build 539
\par 创建时间:
	2010-05-23 06:10:59 +0800
\par 修改时间:
	2023-03-26 02:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YCoreUtilities
*/


#ifndef YSL_INC_Core_YCoreUtilities_h_
#define YSL_INC_Core_YCoreUtilities_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YException // for LoggedEvent, string, string_view,
//	std::string, std::exception, size_t, ExtractException, FatalError,
//	to_std_string, make_string_view, linked_map;
#include <ystdex/algorithm.hpp> // for ystdex::clamp;

namespace YSLib
{

/*!
\note 对确定的构建版本，结果是确定不变的。
\since build 970
*/
//!@{
/*!
\brief 取库构建时确定的构建版本号。
\return 表示构建版本的整数。
\see Documentation::Projects @2.2.1.2 。
\see Documentation::Projects @2.2.4.4 。
\see Documentation::Projects @2.2.4.5 。

若构建时宏 \c YSL_BuildNumber 被定义，宏替换的值应是一个整数表达式，
	这个表达式的值是一个表示构建版本号的正整数。
构建版本号应表示主分支版本。
若构建时无法通过宏确定构建版本号，则默认指定最近的发布版本对应的构建版本号。
*/
YB_ATTR_nodiscard YF_API YB_STATELESS size_t
FetchBuildNumber() ynothrow;

/*!
\brief 取库构建时确定的版本字符串。
\return 具有静态存储期的字符串指针。

若构建时宏 \c YSL_VCS_Revision 被定义，宏替换的值应满足：
	求值为一个具有存储期的 \c char 字符串字面量的左值或 <tt>const char*</tt> 指针。
	求值时无异常抛出。
此时，结果等于这个宏指定的值。
否则，结果是空串。
*/
YB_ATTR_nodiscard YF_API YB_ATTR_returns_nonnull YB_STATELESS const char*
FetchVCSRevisionString() ynothrow;
//!@}


/*!
\brief 符号函数。
\note 若 <tt>a < b</tt> 则返回 -1 ，否则若 <tt>a = b</tt> 则返回 0 ，否则返回 1 。
\since build 633
*/
template<typename _type>
YB_STATELESS yconstfn std::int_fast8_t
FetchSign(_type a, _type b = _type(0)) ynothrow
{
	return a < b ? -1 : !(a == b);
}

/*!
\brief 判断 \c d 和以 \c a 和 \b 构成的闭区间的关系。
\return <tt>< 0</tt> ：\c d 在区间外；
\return <tt>= 0</tt> ：\c d 在区间端点上；
\return <tt>> 0</tt> ：\c d 在区间内。
\note 无精度修正。
\note 使用 ADL FetchSign 。
\since build 633
*/
template<typename _type>
YB_STATELESS yconstfn std::int_fast8_t
FetchSignFromInterval(_type d, _type a, _type b) ynothrow
{
	return FetchSign(a, d) * FetchSign(d, b);
}

/*!
\brief 计算指定类型的差值的一半。
\since build 554
*/
template<typename _type>
YB_STATELESS yconstfn _type
HalfDifference(_type x, _type y)
{
	return (x - y) / 2;
}

/*!
\brief 判断 i 是否在左闭右开区间 [_type(0), b) 中。
\pre 断言：<tt>_type(0) < b</tt> 。
\since build 319
*/
template<typename _type>
YB_STATELESS inline bool
IsInInterval(_type i, _type b) ynothrow
{
	YAssert(_type(0) < b,
		"Zero element as lower bound is not less than upper bound.");
	return !(i < _type(0)) && i < b;
}
/*!
\brief 判断 i 是否在左闭右开区间 [a, b) 中。
\pre 断言：<tt>a < b</tt> 。
\since build 319
*/
template<typename _type>
YB_STATELESS inline bool
IsInInterval(_type i, _type a, _type b) ynothrow
{
	YAssert(a < b, "Lower bound is not less than upper bound.");
	return !(i < a) && i < b;
}

/*!
\brief 判断 i 是否在闭区间 [_type(0), b] 中。
\pre 断言：<tt>_type(0) < b</tt> 。
\since build 470
*/
template<typename _type>
YB_STATELESS inline bool
IsInClosedInterval(_type i, _type b) ynothrow
{
	YAssert(_type(0) < b,
		"Zero element as lower bound is not less than upper bound.");
	return !(i < _type(0) || b < i);
}
/*!
\brief 判断 i 是否在闭区间 [a, b] 中。
\pre 断言：<tt>a < b</tt> 。
\since build 470
*/
template<typename _type>
YB_STATELESS inline bool
IsInClosedInterval(_type i, _type a, _type b) ynothrow
{
	YAssert(a < b, "Lower bound is not less than upper bound.");
	return !(i < a || b < i);
}

/*!
\brief 判断 i 是否在开区间 (_type(0), b) 内。
\pre 断言：<tt>_type(0) < b</tt> 。
\since build 319
*/
template<typename _type>
YB_STATELESS inline bool
IsInOpenInterval(_type i, _type b) ynothrow
{
	YAssert(_type(0) < b,
		"Zero element as lower bound is not less than upper bound.");
	return _type(0) < i && i < b;
}
/*!
\brief 判断 i 是否在开区间 (a, b) 内。
\pre 断言：<tt>a < b</tt> 。
\since build 319
*/
template<typename _type>
YB_STATELESS inline bool
IsInOpenInterval(_type i, _type a, _type b) ynothrow
{
	YAssert(a < b, "Lower bound is not less than upper bound.");
	return a < i && i < b;
}

/*!
\pre 断言：<tt>n != 0</tt> 。
\pre 间接断言：<tt>a</tt> 。
\pre 断言：<tt>!(v < *a)</tt> 。
\since build 319
*/
//!@{
/*!
\brief 计算满足指定的值 v 在区间 [\c a[i], <tt>a[i + 1]</tt>) 内最小的 i 。
*/
template<typename _type>
YB_ATTR_nodiscard YB_NONNULL(2) YB_PURE size_t
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
YB_ATTR_nodiscard YB_NONNULL(2) YB_PURE size_t
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
//!@}

/*!
\brief 约束 v 在闭区间 [a, b] 中。
\pre 间接断言：<tt>!(b < a)</tt> 。
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
\pre 断言：<tt>a < b</tt> 。
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
\pre 断言：<tt>b != _type(0)</tt> 。
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
\since build 833
\sa ystdex::narrow
*/
//!@{
//! \brief 检查算术类型数值不小于指定类型的下界。
template<typename _tDst, typename _type>
inline _tDst
CheckLowerBound(_type val, const string& name = {}, RecordLevel lv = Err)
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
CheckUpperBound(_type val, const string& name = {}, RecordLevel lv = Err)
{
	using namespace ystdex;
	// XXX: See WG21 N3387.
	// TODO: Add and use safe %common_arithmetic_type interface instead?
	// TODO: Add direct integer rank comparison?
	using common_t = typename ystdex::common_int_type<_tDst, _type>::type;

	if((std::is_signed<common_t>() && std::is_unsigned<_tDst>()
		&& ystdex::integer_width<common_t>() <= ystdex::integer_width<_tDst>())
		|| !(common_t(std::numeric_limits<common_t>::max()) < common_t(val)))
		return _tDst(val);
	throw LoggedEvent("Value of '" + name + "' is greater than upper boundary.",
		lv);
}

//! \brief 检查算术类型数值在指定类型的范围内。
template<typename _tDst, typename _type>
inline _tDst
CheckArithmetic(_type val, const string& name = {}, RecordLevel lv = Err)
{
	return
		CheckUpperBound<_tDst>(CheckLowerBound<_tDst>(val, name, lv), name, lv);
}

//! \brief 检查非负算术类型数值在指定类型的范围内。
template<typename _tDst, typename _type>
inline _tDst
CheckNonnegative(_type val, const string& name = {}, RecordLevel lv = Err)
{
	if(val < 0)
		// XXX: Use more specified exception type.
		throw LoggedEvent("Failed getting nonnegative " + name + " value.", lv);
	return CheckUpperBound<_tDst>(val, name, lv);
}

//! \brief 检查正算术类型数值在指定类型的范围内。
template<typename _tDst, typename _type>
inline _tDst
CheckPositive(_type val, const string& name = {}, RecordLevel lv = Err)
{
	if(!(0 < val))
		// XXX: Use more specified exception type.
		throw LoggedEvent("Failed getting positive " + name + " value.", lv);
	return CheckUpperBound<_tDst>(val, name, lv);
}
//!@}


/*!
\brief 清除指定的连续对象。
\pre 设类型 \c T 为 ystdex::decay_t<decltype(*dst)>，则应满足
	<tt>std::is_trivial\<T>() || (std::is_nothrow_default_constructible\<T>()
		&& std::is_nothrow_assignable\<T, T>())</tt> 。
\since build 624
*/
template<typename _tOut>
inline void
ClearSequence(_tOut dst, size_t n) ynothrowv
{
	using _type = ystdex::decay_t<decltype(*dst)>;
	static_assert(std::is_trivial<_type>()
		|| (std::is_nothrow_default_constructible<_type>()
		&& std::is_nothrow_assignable<_type, _type>()), "Invalid type found.");

	std::fill_n(dst, n, _type());
}


/*!
\brief 执行关键动作。
\throw FatalError 调用失败。
\note 第二参数表示调用签名；后两个参数用于抛出异常。
\since build 955
*/
template<typename _func>
void
PerformKeyAction(_func f, const char* sig, const char* t, string_view sv)
{
	std::string res;

	try
	{
		f();
		return;
	}
	CatchExpr(std::exception& e, ExtractException(
		[&](const std::string & str, size_t level){
		res += std::string(level, ' ') + "ERROR: " + str + '\n';
	}, e))
	CatchExpr(..., res += std::string("Unknown exception @ ") + sig + ".\n")
	throw FatalError(t, make_string_view(to_std_string(sv) + res));
}


/*!
\brief 替换第一参数中和第二参数相等的字符为第三参数指定的随机字符。
\pre 断言：第三参数是非空字符串。
\return 从第一参数转移的替换的字符后的结果。
\since build 970
*/
YF_API string
RandomizeTemplateString(string, char, string_view);


/*!
\brief 参数字符串向量。
\since build 797

用于存储命令行参数等字符串向量。
*/
class YF_API ArgumentsVector final
{
public:
	CommandArguments::VectorType Arguments{};

	//! \since build 919
	//!@{
	DefDeCtor(ArgumentsVector)
	ArgumentsVector(CommandArguments::VectorType::allocator_type a)
		: Arguments(a)
	{}
	DefDeCopyMoveCtorAssignment(ArgumentsVector)
	//!@}

	/*!
	\brief 设置值。

	设置参数向量的值。
	参数分别指定参数向量和表示 NTMBS 的指针数组。
	*/
	//!@{
	/*!
	\pre 间接断言：第二参数的元素都是非空指针。
	\exception LoggedEvent 输入的参数数小于 0 。
	\sa CheckNonnegative
	\since build 797
	*/
	void
	Reset(int, char*[]);
	//! \since build 919
	PDefH(void, Reset, const CommandArguments& cmd_args)
		ImplExpr(Arguments = cmd_args.ToVector())
	//! \since build 919
	PDefH(void, Reset, CommandArguments&& cmd_args)
		ImplExpr(Arguments = std::move(cmd_args).ToVector())
	//!@}
};

/*!
\brief 锁定默认命令行参数对象。
\return 静态对象的非空锁定指针。
\note 使用默认分配器而不支持指定分配器，以避免静态对象析构时超出分配器的生存期。
\relates ArgumentsVector
\since build 839
*/
YF_API locked_ptr<ArgumentsVector, recursive_mutex>
LockCommandArguments();


//! \since build 567
//!@{
//! \brief 默认命令缓冲区大小。
yconstexpr const size_t DefaultCommandBufferSize(yimpl(4096));

/*!
\brief 取命令在标准输出上的执行结果。
\pre 间接断言：第一参数非空。
\return 读取的二进制存储和关闭管道的返回值（可来自被调用的命令）。
\exception std::bad_alloc 缓冲区创建失败。
\throw std::system_error 管道打开失败。
\throw std::system_error 读取失败。
\throw std::invalid_argument 第二参数的值等于 \c 0 。
\note 第一参数指定命令；第二参数指定每次读取的缓冲区大小上限，先于执行命令进行检查。
\since build 791
*/
YF_API YB_NONNULL(1) pair<string, int>
FetchCommandOutput(const char*, size_t = DefaultCommandBufferSize);


// XXX: The results can be potentionally large. Use %linked_map is more
//	efficient than %value_map in such case.
//! \brief 命令和命令执行结果的缓冲区类型。
using CommandCache = linked_map<string, string>;

/*!
\brief 锁定命令执行缓冲区。
\return 静态对象的非空锁定指针。
*/
YF_API locked_ptr<CommandCache>
LockCommandCache();
//!@}

/*!
\brief 取缓冲的命令执行结果。
\pre 断言：第一参数的数据指针非空。
\since build 839
*/
YF_API const string&
FetchCachedCommandResult(string_view, size_t = DefaultCommandBufferSize);

} // namespace YSLib;

#endif

