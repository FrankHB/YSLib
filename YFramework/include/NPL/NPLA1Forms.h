/*
	© 2016-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.h
\ingroup NPL
\brief NPLA1 语法形式。
\version r7621
\author FrankHB <frankhb1989@gmail.com>
\since build 882
\par 创建时间:
	2020-02-15 11:19:21 +0800
\par 修改时间:
	2020-05-20 17:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1Forms
*/


#ifndef NPL_INC_NPLA1Forms_h_
#define NPL_INC_NPLA1Forms_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA1 // for string, TokenValue, ReductionStatus, TermNode,
//	AssertBranch, ystdex::expand_proxy, NPL::Access, std::next, NPL::Deref,
//	Forms::CallResolvedUnary, ResolvedTermReferencePtr, NPL::AccessRegular,
//	ystdex::make_expanded, std::ref, ystdex::invoke_nonvoid, TNIter,
//	NPL::ResolveRegular, ystdex::make_transform, std::accumulate,
//	std::placeholders::_2, ystdex::bind1, ContextNode,
//	ystdex::equality_comparable, ystdex::examiners::equal_examiner, Environment,
//	shared_ptr;

namespace NPL
{

//! \since build 665
namespace A1
{

/*!
\brief NPLA1 语法形式对应的功能实现。
\pre 除非另行指定支持保存当前动作，若存在子项，关联的上下文中的尾动作为空。
\pre 设置为处理器调用的操作在进入调用前应确保设置尾上下文等内部状态。
\pre 作为操作数的项的子项不包含引用或非正规表示引入的对操作数内的子项的循环引用。
\since build 732

提供支持 NPLA1 对象语言文法的操作的接口。
提供的操作用于实现操作子或应用子底层的操作子。
除非另行指定，操作子的参数被通过直接转移项的形式转发。
*/
namespace Forms
{

/*!
\brief 判断字符串值是否可构成符号。
\since build 779

参考调用文法：
<pre>symbol-string? \<object></pre>
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsSymbol(const string&) ynothrow;

//! \since build 786
//@{
/*!
\brief 创建等于指定字符串值的记号值。
\note 不检查值是否符合符号要求。
*/
//@{
YB_ATTR_nodiscard YF_API YB_PURE TokenValue
StringToSymbol(const string&);
//! \since build 863
YB_ATTR_nodiscard YF_API YB_PURE TokenValue
StringToSymbol(string&&);
//@}

//! \brief 取符号对应的名称字符串。
YB_ATTR_nodiscard YF_API YB_STATELESS const string&
SymbolToString(const TokenValue&) ynothrow;
//@}


/*!
\pre 断言：项或容器对应枝节点。
\sa AssertBranch
\since build 733
*/
//@{
/*!
\note 保留求值留作保留用途，一般不需要被作为用户代码直接使用。
\note 只用于检查项的个数时，可忽略返回值。
\since build 765

可使用 RegisterForm 注册上下文处理器，参考文法：
$retain|$retainN \<expression>
*/
//@{
// XXX: G++ will warn with [-Wunused-value] if it is pure. See also $2019-01
//	@ %Documentation::Workflow.
#if defined(NDEBUG) && YB_IMPL_GNUCPP >= 100000
	YB_Diag_Push
	YB_Diag_Ignore(suggest-attribute=pure)
#endif
//! \brief 保留项：保留求值。
inline PDefH(ReductionStatus, Retain, const TermNode& term) ynothrowv
	ImplRet(AssertBranch(term), ReductionStatus::Retained)
#if defined(NDEBUG) && YB_IMPL_GNUCPP >= 100000
	YB_Diag_Pop
#endif

/*!
\brief 保留经检查确保具有指定个数参数的项：保留求值。
\return 项的参数个数。
\throw ArityMismatch 项的参数个数不等于第二参数。
\sa FetchArgumentN
*/
YF_API size_t
RetainN(const TermNode&, size_t = 1);
//@}


//! \since build 855
//@{
//! \brief 访问节点的子节点并调用一元函数。
template<typename _func, typename... _tParams>
inline auto
CallRawUnary(_func&& f, TermNode& term, _tParams&&... args)
	-> yimpl(decltype(ystdex::expand_proxy<void(TermNode&, _tParams&&...)>
	::call(f, NPL::Deref(std::next(term.begin())), yforward(args)...)))
{
	RetainN(term);
	return ystdex::expand_proxy<yimpl(void)(TermNode&, _tParams&&...)>::call(f,
		NPL::Deref(std::next(term.begin())), yforward(args)...);
}

//! \brief 解析节点的子节点并调用一元函数。
template<typename _func>
inline auto
CallResolvedUnary(_func&& f, TermNode& term)
	-> yimpl(decltype(NPL::ResolveTerm(yforward(f), term)))
{
	return Forms::CallRawUnary([&](TermNode& tm)
		-> yimpl(decltype(NPL::ResolveTerm(yforward(f), term))){
		return NPL::ResolveTerm(yforward(f), tm);
	}, term);
}

//! \exception bad_any_cast 异常中立：非列表项类型检查失败。
//@{
//! \since build 859
template<typename _type, typename _func, typename... _tParams>
inline auto
CallResolvedUnaryAs(_func&& f, TermNode& term, _tParams&&... args)
	-> yimpl(decltype(ystdex::expand_proxy<void(_type&, const
	ResolvedTermReferencePtr&, _tParams&&...)>::call(f, NPL::Access<_type>(
	term), ResolvedTermReferencePtr(), std::forward<_tParams>(args)...)))
{
	using handler_t
		= yimpl(void)(_type&, const ResolvedTermReferencePtr&, _tParams&&...);

	return Forms::CallResolvedUnary(
		[&](TermNode& nd, ResolvedTermReferencePtr p_ref)
		// TODO: Blocked. Use C++14 'decltype(auto)'.
		-> decltype(ystdex::expand_proxy<handler_t>::call(f,
		std::declval<_type&>(), p_ref)){
		// XXX: Blocked. 'yforward' cause G++ 7.1.0 failed silently.
		return ystdex::expand_proxy<handler_t>::call(f, NPL::Access<_type>(nd),
			p_ref, std::forward<_tParams>(args)...);
	}, term);
}

/*!
\note 访问节点的子节点，以正规值调用一元函数。
\sa NPL::AccessRegular
\exception ListTypeError 异常中立：项为列表项。
*/
template<typename _type, typename _func, typename... _tParams>
inline auto
CallRegularUnaryAs(_func&& f, TermNode& term, _tParams&&... args)
	-> yimpl(decltype(ystdex::expand_proxy<void(_type&, const
	ResolvedTermReferencePtr&, _tParams&&...)>::call(f, NPL::Access<_type>(
	term), ResolvedTermReferencePtr(), std::forward<_tParams>(args)...)))
{
	using handler_t
		= yimpl(void)(_type&, const ResolvedTermReferencePtr&, _tParams&&...);

	return Forms::CallResolvedUnary(
		// TODO: Blocked. Use C++14 'decltype(auto)'.
		[&](TermNode& nd, ResolvedTermReferencePtr p_ref)
		-> decltype(ystdex::expand_proxy<handler_t>::call(f,
		std::declval<_type&>(), p_ref)){
		// XXX: Blocked. 'yforward' cause G++ 7.1.0 failed silently.
		return ystdex::expand_proxy<handler_t>::call(f, NPL::AccessRegular<
			_type>(nd, p_ref), p_ref, std::forward<_tParams>(args)...);
	}, term);
}
//@}
//@}

/*!
\note 确定项具有一个实际参数后展开调用参数指定的函数。

若被调用的函数返回类型非 void ，返回值作为项的值被构造。
调用 YSLib::EmplaceCallResult 对 ValueObject 及引用值处理不同。
若需以和其它类型的值类似的方式被包装，在第一参数中构造 ValueObject 对象。
实现使用 ystdex::make_expanded 展开调用，但不复制或转移可调用对象，
	因此使用 std::ref 包装第一参数。注意当前无条件视第一参数为 const 左值。
考虑一般实现的性能不确定性，当前实现中，调用 YSLib::EmplaceCallResult 不使用分配器。
*/
//@{
/*!
\brief 访问节点并调用一元函数。
\sa YSLib::EmplaceCallResult
\since build 756
*/
//@{
template<typename _func, typename... _tParams>
void
CallUnary(_func&& f, TermNode& term, _tParams&&... args)
{
	Forms::CallRawUnary([&](TermNode& tm){
		YSLib::EmplaceCallResult(term.Value, ystdex::invoke_nonvoid(
			ystdex::make_expanded<void(TermNode&, _tParams&&...)>(std::ref(f)),
			tm, yforward(args)...), term.get_allocator());
	}, term);
}

template<typename _type, typename _func, typename... _tParams>
void
CallUnaryAs(_func&& f, TermNode& term, _tParams&&... args)
{
	Forms::CallUnary([&](TermNode& tm){
		// XXX: Blocked. 'yforward' cause G++ 5.3 crash: internal compiler
		//	error: Segmentation fault.
		return ystdex::make_expanded<void(_type&, _tParams&&...)>(std::ref(f))(
			NPL::ResolveRegular<_type>(tm), std::forward<_tParams>(args)...);
	}, term);
}
//@}

/*!
\brief 访问节点并调用二元函数。
\since build 760
*/
//@{
template<typename _func, typename... _tParams>
void
CallBinary(_func&& f, TermNode& term, _tParams&&... args)
{
	RetainN(term, 2);

	auto i(term.begin());
	auto& x(NPL::Deref(++i));

	YSLib::EmplaceCallResult(term.Value, ystdex::invoke_nonvoid(
		ystdex::make_expanded<void(TermNode&, TermNode&, _tParams&&...)>(
		std::ref(f)), x, NPL::Deref(++i), yforward(args)...),
		term.get_allocator());
}

//! \since build 835
template<typename _type, typename _type2, typename _func, typename... _tParams>
void
CallBinaryAs(_func&& f, TermNode& term, _tParams&&... args)
{
	RetainN(term, 2);

	auto i(term.begin());
	auto& x(NPL::ResolveRegular<_type>(NPL::Deref(++i)));

	YSLib::EmplaceCallResult(term.Value, ystdex::invoke_nonvoid(
		ystdex::make_expanded<void(_type&, _type2&, _tParams&&...)>(
		std::ref(f)), x, NPL::ResolveRegular<_type2>(NPL::Deref(++i)),
		yforward(args)...), term.get_allocator());
}
//@}
//@}
//@}

/*!
\brief 访问节点并以指定的初始值为基础逐项调用二元函数。
\note 为支持 std::bind 推断类型，和以上函数的情形不同，不支持省略参数。
\since build 758
*/
template<typename _type, typename _func, typename... _tParams>
void
CallBinaryFold(_func f, _type val, TermNode& term, _tParams&&... args)
{
	const auto n(FetchArgumentN(term));
	auto i(term.begin());
	const auto j(ystdex::make_transform(++i, [](TNIter it){
		return NPL::ResolveRegular<_type>(NPL::Deref(it));
	}));

	YSLib::EmplaceCallResult(term.Value, std::accumulate(j, std::next(j,
		typename std::iterator_traits<decltype(j)>::difference_type(n)), val,
		ystdex::bind1(f, std::placeholders::_2, yforward(args)...)),
		term.get_allocator());
}


/*!
\brief 保存函数展开调用的函数对象。
\todo 使用 C++14 lambda 表达式代替。

适配作为上下文处理器的除项以外可选参数的函数对象。
为适合作为上下文处理器，支持的参数列表类型实际存在限制：
参数列表以和元数相同数量的必须的 TermNode& 类型的参数起始；
之后是可选的 ContextNode& 可转换到的类型的参数。
*/
//@{
//! \since build 741
//@{
//! \sa Forms::CallUnary
template<typename _func>
struct UnaryExpansion
	: private ystdex::equality_comparable<UnaryExpansion<_func>>
{
	_func Function;

	//! \since build 787
	UnaryExpansion(_func f)
		: Function(std::move(f))
	{}

	/*!
	\brief 比较处理器相等。
	\since build 773
	*/
	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==, const UnaryExpansion& x,
		const UnaryExpansion& y)
		ImplRet(ystdex::examiners::equal_examiner::are_equal(x.Function,
			y.Function))

	//! \since build 760
	template<typename... _tParams>
	inline void
	operator()(_tParams&&... args) const
	{
		Forms::CallUnary(Function, yforward(args)...);
	}
};


//! \sa Forms::CallUnaryAs
template<typename _type, typename _func>
struct UnaryAsExpansion
	: private ystdex::equality_comparable<UnaryAsExpansion<_type, _func>>
{
	_func Function;

	//! \since build 787
	UnaryAsExpansion(_func f)
		: Function(std::move(f))
	{}

	/*!
	\brief 比较处理器相等。
	\since build 773
	*/
	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const UnaryAsExpansion& x, const UnaryAsExpansion& y)
		ImplRet(ystdex::examiners::equal_examiner::are_equal(x.Function,
			y.Function))

	//! \since build 760
	template<typename... _tParams>
	inline void
	operator()(_tParams&&... args) const
	{
		Forms::CallUnaryAs<_type>(Function, yforward(args)...);
	}
};
//@}


//! \since build 760
//@{
//! \sa Forms::CallBinary
template<typename _func>
struct BinaryExpansion
	: private ystdex::equality_comparable<BinaryExpansion<_func>>
{
	_func Function;

	//! \since build 787
	BinaryExpansion(_func f)
		: Function(std::move(f))
	{}

	/*!
	\brief 比较处理器相等。
	\since build 773
	*/
	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const BinaryExpansion& x, const BinaryExpansion& y)
		ImplRet(ystdex::examiners::equal_examiner::are_equal(x.Function,
			y.Function))

	template<typename... _tParams>
	inline void
	operator()(_tParams&&... args) const
	{
		Forms::CallBinary(Function, yforward(args)...);
	}
};


/*!
\sa Forms::CallBinaryAs
\since build 835
*/
template<typename _type, typename _type2, typename _func>
struct BinaryAsExpansion : private
	ystdex::equality_comparable<BinaryAsExpansion<_type, _type2, _func>>
{
	_func Function;

	//! \since build 787
	BinaryAsExpansion(_func f)
		: Function(std::move(f))
	{}

	/*!
	\brief 比较处理器相等。
	\since build 773
	*/
	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const BinaryAsExpansion& x, const BinaryAsExpansion& y)
		ImplRet(ystdex::examiners::equal_examiner::are_equal(x.Function,
			y.Function))

	template<typename... _tParams>
	inline void
	operator()(_tParams&&... args) const
	{
		Forms::CallBinaryAs<_type, _type2>(Function, yforward(args)...);
	}
};
//@}
//@}


/*!
\pre 间接断言：第二参数的数据指针非空。
\since build 871
*/
//@{
//! \brief 注册一元严格求值上下文处理器。
//@{
template<size_t _vWrapping = Strict, typename _func, class _tTarget>
inline void
RegisterUnary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterHandler<_vWrapping>(target, name,
		UnaryExpansion<_func>(std::move(f)));
}
template<size_t _vWrapping = Strict, typename _type, typename _func,
	class _tTarget>
inline void
RegisterUnary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterHandler<_vWrapping>(target, name,
		UnaryAsExpansion<_type, _func>(std::move(f)));
}
//@}

//! \brief 注册二元严格求值上下文处理器。
//@{
template<size_t _vWrapping = Strict, typename _func, class _tTarget>
inline void
RegisterBinary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterHandler<_vWrapping>(target, name,
		BinaryExpansion<_func>(std::move(f)));
}
template<size_t _vWrapping = Strict, typename _type, typename _type2,
	typename _func, class _tTarget>
inline void
RegisterBinary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterHandler<_vWrapping>(target, name,
		BinaryAsExpansion<_type, _type2, _func>(std::move(f)));
}
//@}
//@}


/*!
\brief 比较两个子项表示的值引用相同的对象。
\sa YSLib::HoldSame
\since build 804

参考调用文法：
<pre>eq? \<object1> \<object2></pre>
*/
YF_API void
Equal(TermNode&);

/*!
\brief 比较两个子项的值相等。
\sa YSLib::ValueObject
\since build 804

参考调用文法：
<pre>eql? \<object1> \<object2></pre>
*/
YF_API void
EqualLeaf(TermNode&);

//! \since build 748
//@{
/*!
\brief 比较两个子项的值引用相同的对象。
\sa YSLib::HoldSame

参考调用文法：
<pre>eqr? \<object1> \<object2></pre>
*/
YF_API void
EqualReference(TermNode&);

/*!
\brief 比较两个子项表示的值相等。
\sa YSLib::ValueObject

参考调用文法：
<pre>eqv? \<object1> \<object2></pre>
*/
YF_API void
EqualValue(TermNode&);
//@}


/*!
\note 测试条件成立，当且仅当 \<test> 非 #f 。
\sa ReduceOnce
*/
//@{
/*!
\brief 分支判断：根据求值的条件选取表达式求值。
\since build 750

求值第一参数作为测试条件，成立时取第二参数子项，否则当第三参数子项时取第三参数子项。
和 Kernel 不同而和 Scheme 类似，求值结果非 #f 的条件都成立，且支持省略第三操作数。

参考调用文法：
<pre>$if \<test> \<consequent> \<alternate>
$if \<test> \<consequent></pre>
*/
YF_API ReductionStatus
If(TermNode&, ContextNode&);

/*!
\brief 条件分支选择：根据条件列表顺序选取第一个符合其中条件对应的表达式求值。
\since build 860

参考调用文法：
<pre>$cond \<clauses>...</pre>
*/
YF_API ReductionStatus
Cond(TermNode&, ContextNode&);

/*!
\brief 条件顺序求值：预期条件成立。
\since build 868

求值第一参数子项作为测试条件，成立时顺序求值之后的子项。

参考调用文法：
<pre>$when \<test> \<expression-sequence></pre>
*/
YF_API ReductionStatus
When(TermNode&, ContextNode&);

/*!
\brief 条件顺序求值：预期条件不成立。
\since build 868

求值第一参数子项作为测试条件，不成立时顺序求值之后的子项。

参考调用文法：
<pre>$unless \<test> \<expression-sequence></pre>
*/
YF_API ReductionStatus
Unless(TermNode&, ContextNode&);
//@}

/*!
\brief 逻辑非。
\since build 861

当子项求值为 true 时返回 false ，否则返回子项。

参考调用文法：
<pre>not? \<test></pre>
*/
YF_API YB_PURE bool
Not(TermNode&);

/*!
\note 支持保存当前动作。
\sa ReduceOnce
\since build 754
*/
//@{
/*!
\brief 逻辑与。

非严格求值若干个子项，返回求值结果的逻辑与：
除第一个子项，没有其它子项时，返回 true ；否则从左到右逐个求值子项。
当子项全求值为 true 时返回最后一个子项的值，否则返回 false 。

参考调用文法：
<pre>$and? \<test>...</pre>
*/
YF_API ReductionStatus
And(TermNode&, ContextNode&);

/*!
\brief 逻辑或。

非严格求值若干个子项，返回求值结果的逻辑或：
除第一个子项，没有其它子项时，返回 false ；否则从左到右逐个求值子项。
当子项全求值为 false 时返回 false，否则返回第一个不是 false 的子项的值。

参考调用文法：
<pre>$or? \<test>...</pre>
*/
YF_API ReductionStatus
Or(TermNode&, ContextNode&);
//@}


/*!
\brief 接受两个参数，返回以第一参数作为第一个元素插入第二参数创建的新的列表。
\return ReductionStatus::Retained 。
\throw ListTypeError 第二参数不是列表。
\note NPLA 无 cons 对，所以要求创建的总是列表。
*/
//@{
/*!
\sa LiftSubtermsToReturn
\since build 779

按值传递返回值。构造的对象中的元素转换为右值。
替代引用值通过 NPL::LiftSubtermsToReturn(@6.6.2) 提升求值后的项的每个子项实现。

参考调用文法：
<pre>cons \<object> \<list></pre>
*/
YF_API ReductionStatus
Cons(TermNode&);

/*!
\since build 822

在返回时不提升项，允许返回引用。

参考调用文法：
<pre>cons% \<object> \<list></pre>
*/
YF_API ReductionStatus
ConsRef(TermNode&);
//@}

//! \throw ListTypeError 参数不是非空列表。
//@{
/*!
\brief 取列表的第一元素并转发给指定的应用子。
\since build 875

使用第一参数指定的应用子调用第三参数指定的列表，取结果匹配的第一元素作为参数，
	调用第二参数指定的应用子。
列表参数在对象语言中按引用传递。

参考调用文法：
<pre>forward-list-first% \<applicative1> \<applicative2> \<list></pre>
*/
YF_API ReductionStatus
ForwardListFirst(TermNode&, ContextNode&);

/*!
\brief 取参数指定的列表中的第一元素的引用值。
\since build 859
*/
//@{
/*!
转发参数的元素和函数值。

参考调用文法：
<pre>first \<list></pre>
*/
YF_API ReductionStatus
First(TermNode&);

//! \throw ValueCategoryMismatch 参数不是引用值。
//@{
/*!
\since build 873

结果是列表的第一个元素的引用值。保留结果中未折叠的引用值。

参考调用文法：
<pre>first@ \<list></pre>
*/
YF_API ReductionStatus
FirstAt(TermNode&);

/*!
结果是列表的第一个元素引用的对象的引用值。保留结果中的引用值。

参考调用文法：
<pre>first& \<list></pre>
*/
YF_API ReductionStatus
FirstRef(TermNode&);

/*!
结果是列表的第一个元素经过返回值转换的值。不保留结果中的引用值。

参考调用文法：
<pre>firstv \<list></pre>
*/
YF_API ReductionStatus
FirstVal(TermNode&);
//@}
//@}
//@}

/*!
\throw ListTypeError 第一参数不是非空列表。
\throw ValueCategoryMismatch 第一参数不是引用值。
\since build 834
*/
//@{
//! \brief 修改第一参数指定的列表以第二参数作为第一个元素。
//@{
/*!
第二参数转换为右值。
替代引用值直接通过插入第一个元素引用的值实现。

参考调用文法：
<pre>set-first! \<list> \<object></pre>
*/
YF_API void
SetFirst(TermNode&);

//! \warning 除自赋值外，不检查循环引用。
//@{
/*!
\since build 873

保留第二参数未折叠的引用值。

参考调用文法：
<pre>set-first@! \<list> \<object></pre>
*/
YF_API void
SetFirstAt(TermNode&);

/*!
保留第二参数引用值。

参考调用文法：
<pre>set-first%! \<list> \<object></pre>
*/
YF_API void
SetFirstRef(TermNode&);
//@}
//@}

/*!
\brief 修改第一参数指定的列表以第二参数作为第一个元素外的列表。
\throw ListTypeError 第二参数不是列表。
*/
//@{
/*!
\sa LiftSubtermsToReturn

第二参数的元素转换为右值。
替代引用值通过 LiftSubtermsToReturn 插入第一个元素前的每个子项实现。

参考调用文法：
<pre>set-rest! \<list> \<object></pre>
*/
YF_API void
SetRest(TermNode&);

/*!
\warning 不检查循环引用。

保留第二参数元素中的引用值。

参考调用文法：
<pre>set-rest%! \<list> \<object></pre>
*/
YF_API void
SetRestRef(TermNode&);
//@}
//@}


/*!
\brief 对指定项以指定的环境求值。
\note 支持保存当前动作。
\sa ResolveEnvironment

以表达式 \c \<expression> 和环境 \c \<environment> 为指定的参数进行求值。
环境以 ContextNode 的引用表示。
*/
//@{
/*!
\since build 787

按值传递返回值：提升求值后的项。

参考调用文法：
<pre>eval \<expression> \<environment></pre>
*/
YF_API ReductionStatus
Eval(TermNode&, ContextNode&);

/*!
\since build 822

不提升求值后的项，允许返回引用值。

参考调用文法：
<pre>eval% \<expression> \<environment></pre>
*/
YF_API ReductionStatus
EvalRef(TermNode&, ContextNode&);
//@}

/*!
\since build 835
\return ReductionStatus::Retained 。
*/
//@{
/*!
\brief 在参数指定的环境中求值作为外部表示的字符串。
\note 没有 REPL 中的预处理过程。
\sa Eval

参考调用文法：
<pre>eval-string \<string> \<environment></pre>
*/
YF_API ReductionStatus
EvalString(TermNode&, ContextNode&);

/*!
\brief 在参数指定的环境中求值作为外部表示的字符串。
\note 没有 REPL 中的预处理过程。
\sa EvalRef

不提升求值后的项，允许返回引用值。

参考调用文法：
<pre>eval-string% \<string> \<environment></pre>
*/
YF_API ReductionStatus
EvalStringRef(TermNode&, ContextNode&);

/*!
\brief 在参数指定的 REPL 环境中规约字符串表示的翻译单元以求值。
\exception LoggedEvent 翻译单元为空串。
\sa Reduce
\since build 835

提供创建 REPL 新实例并求值的便利接口。

参考调用文法：
<pre>eval-unit \<string> \<object></pre>
*/
YF_API ReductionStatus
EvalUnit(TermNode&);
//@}


//! \pre 间接断言：第一参数指定的项是枝节点。
//@{
/*!
\brief 创建以参数指定的环境列表作为父环境的新环境。
\exception NPLException 异常中立：由 Environment 的构造函数抛出。
\sa Environment::CheckParent
\sa EnvironmentList
\since build 798
\todo 使用专用的异常类型。

取以指定的参数初始化新创建的父环境。

参考调用文法：
<pre>make-environment \<environment>...</pre>
*/
YF_API void
MakeEnvironment(TermNode&);

/*!
\brief 取当前环境的引用。
\since build 785

取得的宿主值类型为 weak_ptr<Environment> 。

参考调用文法：
<pre>() get-current-environment</pre>
*/
YF_API void
GetCurrentEnvironment(TermNode&, ContextNode&);

/*!
\brief 锁定当前环境的引用。
\since build 837

取得的宿主值类型为 shared_ptr<Environment> 。

参考调用文法：
<pre>() lock-current-environment</pre>
*/
YF_API void
LockCurrentEnvironment(TermNode&, ContextNode&);

/*!
\brief 求值标识符得到指称的实体。
\sa EvaluateIdentifier
\since build 757

在对象语言中实现函数接受一个 string 类型的参数项，返回值为指定的实体。
当名称查找失败时，返回的值为 ValueToken::Null 。

参考调用文法：
<pre>value-of \<object></pre>
*/
YF_API ReductionStatus
ValueOf(TermNode&, const ContextNode&);


/*!
\exception ParameterMismatch 绑定匹配失败。
\throw InvalidSyntax 绑定的源为空。
\sa BindParameter
\sa Vau
\since build 840

实现修改环境的特殊形式。
使用 \<definiend> 指定绑定目标，和 Vau 的 \<formals> 格式相同。
剩余表达式 \<expressions> 指定绑定的源。
返回未指定值。
限定第三参数后可使用 RegisterForm 注册上下文处理器。
*/
//@{
/*!
\brief 以惰性求值的表达式定义值。
\note 不对剩余表达式进一步求值。

剩余表达式视为求值结果，直接绑定到 \c \<definiend> 。

参考调用文法：
<pre>$deflazy! \<definiend> \<expressions></pre>
*/
YF_API ReductionStatus
DefineLazy(TermNode&, ContextNode&);

/*!
\brief 不带递归匹配的定义。

剩余表达式视为一个表达式在上下文决定的当前环境进行求值后绑定到 \c \<definiend> 。

参考调用文法：
<pre>$def! \<definiend> \<expressions></pre>
*/
YF_API ReductionStatus
DefineWithNoRecursion(TermNode&, ContextNode&);

/*!
\brief 带递归匹配的定义。
\note 支持直接递归和互相递归绑定。
\sa InvalidReference

分阶段解析可能递归绑定的名称。
剩余表达式视为一个表达式，在上下文决定的当前环境进行求值后绑定到 \c \<definiend> 。
常规绑定前后遍历被绑定的操作数树以支持强递归绑定。不存在的操作数被绑定为中间值。
循环引用以此引入的名称可能抛出 InvalidReference 异常。

参考调用文法：
<pre>$defrec! \<definiend> \<expressions></pre>
*/
YF_API ReductionStatus
DefineWithRecursion(TermNode&, ContextNode&);
//@}

//! \since build 868
//@{
/*!
\brief 修改指定环境的绑定，不带递归匹配。

同 DefineWithNoRecursion ，但由规约第一参数子项的结果显式地确定被修改的环境。

参考调用文法：
<pre>$set! \<environment> \<formals> \<expressions></pre>
*/
YF_API ReductionStatus
SetWithNoRecursion(TermNode&, ContextNode&);

/*!
\brief 修改指定环境的绑定，带递归匹配。

同 DefineWithRecursion ，但由规约第一参数子项的结果显式地确定被修改的环境。

参考调用文法：
<pre>$setrec! \<environment> \<formals> \<expressions></pre>
*/
YF_API ReductionStatus
SetWithRecursion(TermNode&, ContextNode&);
//@}

/*!
\throw InvalidSyntax 标识符不是符号。
\sa IsNPLASymbol
\sa RemoveIdentifier
\since build 867
*/
//@{
/*!
\brief 移除定义引入的绑定。

移除名称和关联的值，返回是否被移除。
移除不存在的名称时忽略。

参考调用文法：
<pre>$undef! \<symbol></pre>
*/
YF_API void
Undefine(TermNode&, ContextNode&);

/*!
\brief 检查并移除名称绑定。
\exception BadIdentifier 移除不存在的名称。

移除名称和关联的值。
移除不存在的名称抛出异常。

参考调用文法：
<pre>$undef-checked! \<symbol></pre>
*/
YF_API void
UndefineChecked(TermNode&, ContextNode&);
//@}


/*!
\exception InvalidSyntax 语法错误：缺少项。
\exception InvalidSyntax 语法错误：\<formals> 不是形式参数树。
\exception ParameterMismatch 异常中立：由 BindParameter 抛出。
\sa EvaluateIdentifier
\sa ExtractParameters
\sa MatchParameter
\warning 返回闭包调用引用变量超出绑定目标的生存期引起未定义行为。
\since build 840
\todo 优化捕获开销。

使用 ExtractParameters 检查参数列表并捕获和绑定变量，
然后设置节点的值为表示 λ 抽象的上下文处理器。
可使用 RegisterForm 注册上下文处理器。
和 Scheme 等不同，参数以项而不是位置的形式被转移，函数应用时可能有副作用。
按引用捕获上下文中的绑定。被捕获的上下文中的绑定依赖宿主语言的生存期规则。
*/
//@{
/*!
\brief λ 抽象：求值为一个捕获当前上下文的严格求值的函数。

捕获的静态环境由当前动态环境隐式确定。
不保留环境的所有权。
*/
//@{
/*!
按值传递返回值：提升项。

参考调用文法：
<pre>$lambda \<formals> \<body></pre>
*/
YF_API ReductionStatus
Lambda(TermNode&, ContextNode&);

/*!
在返回时不提升项，允许返回引用。

参考调用文法：
<pre>$lambda% \<formals> \<body></pre>
*/
YF_API ReductionStatus
LambdaRef(TermNode&, ContextNode&);
//@}

/*!
\note 动态环境的上下文参数被捕获为一个 lref<ContextNode> 对象。
\note 初始化的 \<eformal> 表示动态环境的上下文参数，应为一个符号或 #ignore 。
\note 引入的处理器的 operator() 支持保存当前动作。
\exception InvalidSyntax 语法错误：\<eformal> 不是符号。

上下文中环境以外的数据成员总是被复制而不被转移，
	以避免求值过程中继续访问这些成员引起未定义行为。
*/
//@{
/*!
\brief vau 抽象：求值为一个捕获当前上下文的非严格求值的函数。

捕获的静态环境由当前动态环境隐式确定。
不保留环境的所有权。
*/
//@{
/*!
按值传递返回值：提升项以避免返回引用造成内存安全问题。

参考调用文法：
<pre>$vau \<formals> \<eformal> \<body></pre>
*/
YF_API ReductionStatus
Vau(TermNode&, ContextNode&);

/*!
\since build 822

在返回时不提升项，允许返回引用。

参考调用文法：
<pre>$vau% \<formals> \<eformal> \<body></pre>
*/
YF_API ReductionStatus
VauRef(TermNode&, ContextNode&);
//@}

/*!
\brief 带环境的 vau 抽象：求值为一个捕获当前上下文的非严格求值的函数。
\sa ResolveEnvironment

捕获的静态环境由环境参数 \<environment> 求值后指定。
根据环境参数的类型为 \c shared_ptr<Environment> 或 \c weak_ptr<Environment>
	决定是否保留所有权。
*/
//@{
/*!
按值传递返回值：提升项以避免返回引用造成内存安全问题。

参考调用文法：
<pre>$vau/e \<environment> \<formals> \<eformal> \<body></pre>
*/
YF_API ReductionStatus
VauWithEnvironment(TermNode&, ContextNode&);

/*!
在返回时不提升项，允许返回引用。

参考调用文法：
<pre>$vau/e% \<environment> \<formals> \<eformal> \<body></pre>
*/
YF_API ReductionStatus
VauWithEnvironmentRef(TermNode&, ContextNode&);
//@}
//@}
//@}


//! \since build 859
//@{
/*!
\brief 包装合并子为应用子。
\exception NPLException 包装数溢出。

参考调用文法：
<pre>wrap \<combiner></pre>
*/
YF_API ReductionStatus
Wrap(TermNode&, ContextNode&);

/*!
\brief 包装合并子为应用子。
\exception NPLException 包装数溢出。

允许返回引用。

参考调用文法：
<pre>wrap& \<combiner></pre>
*/
YF_API ReductionStatus
WrapRef(TermNode&, ContextNode&);

//! \exception TypeError 应用子参数的类型不符合要求。
//@{
/*!
\brief 包装操作子为应用子。

参考调用文法：
<pre>wrap1 \<operative></pre>
*/
YF_API ReductionStatus
WrapOnce(TermNode&, ContextNode&);

/*!
\brief 包装操作子为应用子。

允许返回引用。

参考调用文法：
<pre>wrap1& \<operative></pre>
*/
YF_API ReductionStatus
WrapOnceRef(TermNode&, ContextNode&);

/*!
\brief 解包装应用子为合并子。
\since build 858

参考调用文法：
<pre>unwrap \<applicative></pre>
*/
YF_API ReductionStatus
Unwrap(TermNode&, ContextNode&);
//@}
//@}


//! \since build 859
//@{
/*!
\brief 抛出缺少项的异常。
\throw ParameterMismatch 缺少项的错误。
*/
YF_API YB_NORETURN void
ThrowInsufficientTermsError();

/*!
\brief 抛出参数指定消息的语法错误异常。
\throw InvalidSyntax 语法错误。
*/
//@{
//! \pre 间接断言：第一参数非空。
YF_API YB_NORETURN YB_NONNULL(1) void
ThrowInvalidSyntaxError(const char*);
YF_API YB_NORETURN void
ThrowInvalidSyntaxError(string_view);
//@}

/*!
\brief 抛出被赋值操作数不可修改的异常。
\throw TypeError 被赋值操作数不可修改错误。
*/
YF_API YB_NORETURN void
ThrowNonmodifiableErrorForAssignee();

/*!
\brief 抛出第一参数不符合预期值类别的异常。
\throw ValueCategory 第一参数值类别错误。
*/
YF_API YB_NORETURN void
ThrowValueCategoryErrorForFirstArgument(const TermNode&);

//! \return ReductionStatus::Regular 。
//@{
/*!
\brief 检查参数指定的项表示环境。
\exception TypeError 检查失败：参数指定的项不表示环境。
\exception ListTypeError 检查失败：参数指定的项表示列表。

对参数指定的项进行检查。
接受对象语言的一个参数。若这个参数不表示环境，抛出异常；
	否则，对象语言中返回为参数指定的值。

参考调用文法：
<pre>check-environment \<object></pre>
*/
YF_API ReductionStatus
CheckEnvironment(TermNode&);

/*!
\brief 检查参数指定的项表示非空列表的引用。
\exception ListTypeError 检查失败：参数指定的项不表示非空列表。
\since build 857

对参数指定的项进行检查。
接受对象语言的一个参数。若这个参数不表示非空列表引用值，抛出异常；
	否则，对象语言中返回为参数指定的值。

参考调用文法：
<pre>check-list-reference \<object></pre>
*/
YF_API ReductionStatus
CheckListReference(TermNode&);
//@}
//@}


/*!
\brief 创建封装类型。
\return ReductionStatus::Retained 。
\since build 834

创建封装类型操作的应用子。基本语义同 Kernel 的 make-encapsulation-type 。
构造器传递值：转移右值参数，或转换左值参数到右值并复制值进行封装。
访问器取得的是引用值。
用户代码需自行确定封装对象的生存期以避免访问取得的引用值引起未定义行为。

参考调用文法：
<pre>() make-encapsulation-type</pre>
*/
YF_API ReductionStatus
MakeEncapsulationType(TermNode&);


/*!
\brief 序列有序参数规约：移除第一项后顺序规约子项，结果为最后一个子项的规约结果。
\return 子项被规约时为最后一个子项的规约状态，否则为 ReductionStatus::Clean 。
\note 可直接实现顺序求值。在对象语言中，若参数为空，返回未指定值。
\sa ReduceOrdered
\sa RemoveHead
\since build 823

参考调用文法：
<pre>$sequence \<expression-sequence>...</pre>
*/
YF_API ReductionStatus
Sequence(TermNode&, ContextNode&);

/*!
\brief 函数应用：应用参数指定的函数和作为函数参数的列表。
\since build 859

结果是对解包装的应用子应用参数的函数值。保留结果中的引用值。

参考调用文法：
<pre>apply \<applicative> \<object> \<environment>
apply \<applicative> \<object></pre>
*/
YF_API ReductionStatus
Apply(TermNode&, ContextNode&);

/*!
\brief 使用可选的参数指定的不定数量的元素和结尾列表构造新列表。
\exception ParameterMismatch 参数不是
\throw ListTypeError 参数超过一个，且最后参数不是列表。
\since build 860
*/
//@{
/*!
结果是构造的列表的值。不保留结果中的引用值。

参考调用文法：
<pre>list* \<object>+</pre>
*/
YF_API ReductionStatus
ListAsterisk(TermNode&);

/*!
结果是构造的列表的值。保留结果中的引用值。

参考调用文法：
<pre>list*% \<object>+</pre>
*/
YF_API ReductionStatus
ListAsteriskRef(TermNode&);
//@}


/*!
\brief 调用 UTF-8 字符串的系统命令，并保存 int 类型的结果到项的值中。
\sa usystem
\since build 741

参考调用文法：
<pre>system \<string></pre>
*/
YF_API void
CallSystem(TermNode&);
//@}

} // namespace Forms;

} // namesapce A1;

} // namespace NPL;

#endif

