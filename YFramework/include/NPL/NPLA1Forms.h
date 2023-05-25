/*
	© 2016-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1Forms.h
\ingroup NPL
\brief NPLA1 语法形式。
\version r10103
\author FrankHB <frankhb1989@gmail.com>
\since build 882
\par 创建时间:
	2020-02-15 11:19:21 +0800
\par 修改时间:
	2023-05-18 05:30 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1Forms
*/


#ifndef NPL_INC_NPLA1Forms_h_
#define NPL_INC_NPLA1Forms_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA1 // for ResolvedTermReferencePtr, NPL::IsMovable,
//	ReductionStatus, TermNode, FormContextHandler, ContextHandler, size_t,
//	TNIter, RetainList, CheckVariadicArity, RemoveHead, string, TokenValue,
//	shared_ptr, ystdex::expand_proxy, NPL::ResolveTerm, std::next,
//	AssertValueTags, Access, NPL::Deref, RetainN, std::declval, AccessRegular,
//	ystdex::make_expanded, std::ref, ystdex::invoke_nonvoid, AccessTypedValue,
//	ystdex::make_transform, std::accumulate, std::placeholders::_2,
//	ystdex::bind1, ContextNode, ystdex::equality_comparable,
//	ystdex::exclude_self_params_t, ystdex::examiners::equal_examiner,
//	ystdex::size_t_, trivial_swap_t, trivial_swap, ReferenceLeaf, IsAtom,
//	ystdex::ref_eq, ystdex::is_bitwise_swappable, ystdex::true_;

namespace NPL
{

//! \since build 665
namespace A1
{

//! \since build 974
//!@{
// XXX: This is more efficient, at least in code generation by x86_64-pc-linux
//	G++ 9.2 for %WrapN, as well as G++ 11.1 and 12.1.
template<typename _fCopy, typename _fMove>
YB_ATTR_nodiscard YB_FLATTEN auto
MakeValueOrMove(ResolvedTermReferencePtr p_ref, _fCopy cp, _fMove mv)
	-> decltype(NPL::IsMovable(p_ref) ? mv() : cp())
{
	return NPL::IsMovable(p_ref) ? mv() : cp();
}

inline
	PDefH(ReductionStatus, ReduceMakeForm, TermNode& term, FormContextHandler h)
	// XXX: Allocators are not used here on the %ContextHandler value for
	//	performance.
	ImplRet(term.Value = ContextHandler(std::allocator_arg,
		term.get_allocator(), std::move(h)), ReductionStatus::Clean)
inline PDefH(ReductionStatus, ReduceMakeForm, TermNode& term, ContextHandler h,
	size_t n)
	// XXX: Allocators are not used here on the %ContextHandler value for
	//	performance.
	ImplRet(term.Value = A1::MakeForm(term, std::move(h), n),
		ReductionStatus::Clean)

ReductionStatus
ReduceForwardHandler(TermNode&, ContextHandler&, ResolvedTermReferencePtr);


// XXX: Sizes are not compared here, as different count of subterms can consist
//	to equal representations when considering irregular representations of the
//	last element in the pairs.
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, TermUnequal, const TermNode& x, const TermNode& y)
	ImplRet(CountPrefix(x) != CountPrefix(y) || x.Value != y.Value)

YB_ATTR_nodiscard YB_PURE inline PDefH(bool, ExtractBool, const TermNode& term)
	ImplRet(ystdex::value_or(TryAccessReferencedTerm<bool>(term), true))


/*!
\brief 保留列表项，检查参数并移除非参数项。
\sa RetainList
\sa CheckVariadicArity
*/
inline PDefH(TNIter, RetainListArguments, TermNode& term)
	ImplRet(RetainList(term), A1::CheckVariadicArity(term, 0), RemoveHead(term),
		term.begin())
//!@}


/*!
\brief 判断字符串值是否可构成符号。
\note 不依赖具体字符集。
\note 不排除假阴性结果。
\since build 779
*/
YB_ATTR_nodiscard YF_API YB_PURE bool
IsSymbol(const string&) ynothrow;

/*!
\brief 创建等于指定字符串值的记号值。
\note 不检查值是否符合符号要求。若能构成符号，则名称为指定字符串。
*/
//!@{
//! \since build 786
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TokenValue, StringToSymbol, const string& str)
	// XXX: Allocators are not used here for performance in most cases.
	ImplRet(TokenValue(str))
//! \since build 863
YB_ATTR_nodiscard YF_API YB_PURE inline
	PDefH(TokenValue, StringToSymbol, string&& str)
	// XXX: Ditto.
	ImplRet(TokenValue(std::move(str)))
//!@}

/*!
\brief 取符号对应的名称字符串。
\since build 786

参考调用文法：
<pre>symbol->string \<object></pre>
*/
YB_ATTR_nodiscard YF_API YB_STATELESS yconstfn const string&
SymbolToString(const TokenValue& s) ynothrow
	ImplRet(s)


/*!
\warning 非虚析构。
\since build 924
*/
//!@{
//! \brief 封装对象基类。
class YF_API EncapsulationBase
{
private:
	shared_ptr<void> p_type;

public:
	EncapsulationBase(shared_ptr<void> p) ynothrow
		: p_type(std::move(p))
	{}
	DefDeCopyMoveCtorAssignment(EncapsulationBase)

	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const EncapsulationBase& x, const EncapsulationBase& y) ynothrow
		ImplRet(x.p_type == y.p_type)

	YB_ATTR_nodiscard
		DefGetter(const ynothrow, const EncapsulationBase&, , *this)
	YB_ATTR_nodiscard DefGetter(ynothrow, EncapsulationBase&, Ref, *this)
	YB_ATTR_nodiscard
		DefGetter(const ynothrow, const shared_ptr<void>&, Type, p_type)
};


//! \brief 封装对象类。
class YF_API Encapsulation : private EncapsulationBase
{
public:
	mutable TermNode TermRef;

	Encapsulation(shared_ptr<void> p, TermNode term)
		: EncapsulationBase(std::move(p)), TermRef(std::move(term))
	{}
	DefDeCopyMoveCtorAssignment(Encapsulation)

	//! \note 使用同 Equal 的异步实现。
	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const Encapsulation& x, const Encapsulation& y) ynothrow
		ImplRet(x.Get() == y.Get()
			&& Equal(ReferenceTerm(x.TermRef), ReferenceTerm(y.TermRef)))

	using EncapsulationBase::Get;
	using EncapsulationBase::GetType;

	//! \note 使用不依赖上下文的跳板实现，以支持符合嵌套安全要求的递归比较。
	static bool
	Equal(const TermNode&, const TermNode&);
};


//! \brief 封装操作上下文处理器。
class YF_API Encapsulate : private EncapsulationBase
{
public:
	Encapsulate(shared_ptr<void> p)
		: EncapsulationBase(std::move(p))
	{}
	DefDeCopyMoveCtorAssignment(Encapsulate)

	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const Encapsulate& x, const Encapsulate& y) ynothrow
		ImplRet(x.Get() == y.Get())

	/*!
	\note 符合 Forms 中对规约函数的约定。
	\note 使用同步实现。
	*/
	ReductionStatus
	operator()(TermNode&) const;
};


//! \brief 封装值操作上下文处理器。
class EncapsulateValue : private EncapsulationBase
{
public:
	EncapsulateValue(shared_ptr<void> p)
		: EncapsulationBase(std::move(p))
	{}
	DefDeCopyMoveCtorAssignment(EncapsulateValue)

	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const EncapsulateValue& x, const EncapsulateValue& y) ynothrow
		ImplRet(x.Get() == y.Get())

	/*!
	\note 符合 Forms 中对规约函数的约定。
	\note 使用同步实现。
	*/
	ReductionStatus
	operator()(TermNode&) const;
};


//! \brief 封装类型判断谓词上下文处理器。
class YF_API Encapsulated : private EncapsulationBase
{
public:
	Encapsulated(shared_ptr<void> p)
		: EncapsulationBase(std::move(p))
	{}
	DefDeCopyMoveCtorAssignment(Encapsulated)

	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const Encapsulated& x, const Encapsulated& y) ynothrow
		ImplRet(x.Get() == y.Get())

	/*!
	\note 符合 Forms 中对规约函数的约定。
	\note 使用同步实现。
	*/
	ReductionStatus
	operator()(TermNode&) const;
};


//! \brief 解封装操作上下文处理器。
class Decapsulate : private EncapsulationBase
{
public:
	Decapsulate(shared_ptr<void> p)
		: EncapsulationBase(std::move(p))
	{}
	DefDeCopyMoveCtorAssignment(Decapsulate)

	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const Decapsulate& x, const Decapsulate& y) ynothrow
		ImplRet(x.Get() == y.Get())

	/*!
	\note 符合 Forms 中对规约函数的约定。
	\note 使用同步实现。
	*/
	ReductionStatus
	operator()(TermNode&) const;
};
//!@}


/*!
\brief NPLA1 语法形式对应的功能实现。
\pre 除非另行指定支持保存当前动作，若存在子项，关联的上下文中的尾动作为空。
\pre 设置为处理器调用的操作在进入调用前应确保设置尾上下文等内部状态。
\pre 作为操作数的项的子项不包含引用或非正规表示引入的对操作数内的子项的循环引用。
\pre 作为 NPLA1 规约函数的函数的参数符合规约函数实现约定。
\pre 若规约函数的参数指定被规约项，参数是规约合并项。
\pre 若规约函数的参数指定被规约项的容器非空，参数非空（满足规约合并项的要求）。
\pre 间接断言：作为规约函数第一参数指定的项是枝节点，以符合语法形式的实现要求。
\post 第一参数指定的被规约项在规约函数调用完成可表示一等对象。
\sa ContextState
\sa IsCombiningTerm
\see %Documentation::NPL 。
\since build 732

提供支持 NPLA1 对象语言文法的操作的接口。
提供的操作用于实现操作子或应用子底层的操作子。
除非另行指定，操作子的参数被通过直接转移项的形式转发。
对其中符合规约函数的 API ，满足各项前置和后置条件，且除非另行指定：
	可能使用同步（不依赖上下文）或异步（依赖上下文）实现；
	没有依赖上下文参数的规约函数使用同步实现；
	异步实现依赖的上下文是当前上下文；
	在异步实现中都要求下一项项和参数指定的项相同；
	对规约函数的上述约定（及各项前置和后置条件）可隐含使用间接的断言检查。
*/
namespace Forms
{

//! \since build 855
//!@{
//! \brief 访问节点的子节点并调用一元函数。
template<typename _func, typename... _tParams>
inline auto
CallRawUnary(_func&& f, TermNode& term, _tParams&&... args)
	-> yimpl(decltype(ystdex::expand_proxy<void(TermNode&, _tParams&&...)>
	::call(f, NPL::Deref(std::next(term.begin())), yforward(args)...)))
{
	RetainN(term);

	auto& x(NPL::Deref(std::next(term.begin())));

	AssertValueTags(x);
	return ystdex::expand_proxy<yimpl(void)(TermNode&, _tParams&&...)>::call(f,
		x, yforward(args)...);
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
//!@{
//! \since build 859
template<typename _type, typename _func, typename... _tParams>
inline auto
CallResolvedUnaryAs(_func&& f, TermNode& term, _tParams&&... args)
	-> yimpl(decltype(ystdex::expand_proxy<void(_type&, const
	ResolvedTermReferencePtr&, _tParams&&...)>::call(f, Access<_type>(
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
		return ystdex::expand_proxy<handler_t>::call(f, Access<_type>(nd),
			p_ref, std::forward<_tParams>(args)...);
	}, term);
}

/*!
\note 访问节点的子节点，以正规值调用一元函数。
\sa AccessRegular
\exception ListTypeError 异常中立：项为列表项。
*/
template<typename _type, typename _func, typename... _tParams>
inline auto
CallRegularUnaryAs(_func&& f, TermNode& term, _tParams&&... args)
	-> yimpl(decltype(ystdex::expand_proxy<void(_type&, const
	ResolvedTermReferencePtr&, _tParams&&...)>::call(f, Access<_type>(
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
		return ystdex::expand_proxy<handler_t>::call(f, AccessRegular<
			_type>(nd, p_ref), p_ref, std::forward<_tParams>(args)...);
	}, term);
}
//!@}
//!@}

/*!
\note 确定项具有一个实际参数后展开调用参数指定的函数。
\sa NPL::EmplaceCallResultOrReturn
\since build 922

返回值的处理使用 NPL::EmplaceCallResultOrReturn 。
若需以和其它类型的值类似的方式被包装，在第一参数中构造 ValueObject 对象。
实现使用 ystdex::make_expanded 展开调用，但不复制或转移可调用对象，
	因此使用 std::ref 包装第一参数。注意当前无条件视第一参数为 const 左值。
*/
//!@{
//! \brief 访问节点并调用一元函数。
//!@{
template<typename _func, typename... _tParams>
ReductionStatus
CallUnary(_func&& f, TermNode& term, _tParams&&... args)
{
	return Forms::CallRawUnary([&](TermNode& tm){
		return NPL::EmplaceCallResultOrReturn(term, ystdex::invoke_nonvoid(
			ystdex::make_expanded<void(TermNode&, _tParams&&...)>(std::ref(f)),
			tm, yforward(args)...));
	}, term);
}

template<typename _type, typename _func, typename... _tParams>
ReductionStatus
CallUnaryAs(_func&& f, TermNode& term, _tParams&&... args)
{
	return Forms::CallUnary([&](TermNode& tm){
		// XXX: Blocked. 'yforward' cause G++ 5.3 crash: internal compiler
		//	error: Segmentation fault.
		// XXX: This is a bit more efficient than directly use of
		//	%ystdex::expand_proxy for G++.
		return ystdex::make_expanded<void(decltype(
			AccessTypedValue<_type>(tm)), _tParams&&...)>(std::ref(f))(
			AccessTypedValue<_type>(tm), std::forward<_tParams>(args)...);
	}, term);
}
//!@}

//! \brief 访问节点并调用二元函数。
//!@{
template<typename _func, typename... _tParams>
ReductionStatus
CallBinary(_func&& f, TermNode& term, _tParams&&... args)
{
	RetainN(term, 2);

	auto i(term.begin());
	auto& x(NPL::Deref(++i));

	AssertValueTags(x);
	AssertValueTags(NPL::Deref(++i));
	return NPL::EmplaceCallResultOrReturn(term, ystdex::invoke_nonvoid(
		ystdex::make_expanded<void(TermNode&, TermNode&, _tParams&&...)>(
		std::ref(f)), x, *i, yforward(args)...));
}

template<typename _type, typename _type2, typename _func, typename... _tParams>
ReductionStatus
CallBinaryAs(_func&& f, TermNode& term, _tParams&&... args)
{
	RetainN(term, 2);

	auto i(term.begin());
	auto&& x(AccessTypedValue<_type>(NPL::Deref(++i)));

	return NPL::EmplaceCallResultOrReturn(term, ystdex::invoke_nonvoid(
		ystdex::make_expanded<void(decltype(x), decltype(
		AccessTypedValue<_type2>(*i)), _tParams&&...)>(std::ref(f)),
		yforward(x), AccessTypedValue<_type2>(NPL::Deref(++i)),
		yforward(args)...));
}
//!@}

/*!
\brief 访问节点并以指定的初始值为基础逐项调用二元函数。
\note 为支持 std::bind 推断类型，和以上函数的情形不同，不支持省略参数。
\since build 922
*/
template<typename _type, typename _func, typename... _tParams>
ReductionStatus
CallBinaryFold(_func f, _type val, TermNode& term, _tParams&&... args)
{
	const auto n(FetchArgumentN(term));
	auto i(term.begin());
	const auto j(ystdex::make_transform(++i, [](TNIter it){
		return AccessTypedValue<_type>(NPL::Deref(it));
	}));

	return NPL::EmplaceCallResultOrReturn(term, std::accumulate(j, std::next(
		j, typename std::iterator_traits<decltype(j)>::difference_type(n)), val,
		ystdex::bind1(f, std::placeholders::_2, yforward(args)...)));
}
//!@}


/*!
\brief 保存函数展开调用的函数对象。
\warning 非虚析构。

适配作为上下文处理器的除项以外可选参数的函数对象。
为适合作为上下文处理器，支持的参数树类型实际存在限制：
参数树作为参数列表，以和元数相同数量的必须的 TermNode& 类型的参数起始；
之后是可选的 ContextNode& 可转换到的类型的参数。
使用明确指定类型的 Forms::CallUnaryAs 等函数模板可以减少规约处理器的实现中的转换。
这类调用中，回调函数（数据成员 \c Function ）通常不需要返回规约结果，因为：
ReductionStatus::Clean 会被 NPL::EmplaceCallResultOrReturn 的调用隐式提供；
其它规约结果（如保留对象语言的列表时）通常需要处理分配器，按约定应从被规约项取得，
	但 Forms::CallUnaryAs 等函数模板接受的回调函数中，
	第一参数是转换类型后的值数据成员而不是项，包装函数展开调用的函数对象不再适用。
规约处理器实现中可直接调用 Forms::CallUnaryAs 等函数模板
	（其中第一参数作为回调函数，可捕获被规约项并在其中处理）代替这里的函数对象，
	并忽略返回值，在这个调用后显式地返回所需的规约状态，
	而无需通过回调函数的返回值指定。
少数情形下，回调函数内部可能需要分支，允许返回 ReductionStatus::Clean
	和其它不同的规约结果。此时，可使用回调函数直接返回这些规约结果，并通过
	Forms::CallUnaryAs 等蕴含的 NPL::EmplaceCallResultOrReturn 的调用返回。
类似 NPL::EmplaceCallResultOrReturn ，\c operator() 返回规约结果
	（而非 void 类型）不依赖 WrappedContextHandler ，对通常的 C++ 实现性能有利。
*/
//!@{
//! \since build 741
//!@{
//! \sa Forms::CallUnary
template<typename _func>
struct UnaryExpansion
	: private ystdex::equality_comparable<UnaryExpansion<_func>>
{
	_func Function;

	//! \since build 927
	template<typename... _tParams, yimpl(typename
		= ystdex::exclude_self_params_t<UnaryExpansion, _tParams...>)>
	UnaryExpansion(_tParams&&... args)
		: Function(yforward(args)...)
	{}

	/*!
	\brief 比较处理器相等。
	\since build 773
	*/
	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==, const UnaryExpansion& x,
		const UnaryExpansion& y)
		ImplRet(ystdex::examiners::equal_examiner::are_equal(x.Function,
			y.Function))

	//! \since build 922
	template<typename... _tParams>
	inline ReductionStatus
	operator()(_tParams&&... args) const
	{
		return Forms::CallUnary(Function, yforward(args)...);
	}
};


//! \sa Forms::CallUnaryAs
template<typename _type, typename _func>
struct UnaryAsExpansion
	: private ystdex::equality_comparable<UnaryAsExpansion<_type, _func>>
{
	_func Function;

	//! \since build 927
	template<typename... _tParams, yimpl(typename
		= ystdex::exclude_self_params_t<UnaryAsExpansion, _tParams...>)>
	UnaryAsExpansion(_tParams&&... args)
		: Function(yforward(args)...)
	{}

	/*!
	\brief 比较处理器相等。
	\since build 773
	*/
	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const UnaryAsExpansion& x, const UnaryAsExpansion& y)
		ImplRet(ystdex::examiners::equal_examiner::are_equal(x.Function,
			y.Function))

	//! \since build 922
	template<typename... _tParams>
	inline ReductionStatus
	operator()(_tParams&&... args) const
	{
		return Forms::CallUnaryAs<_type>(Function, yforward(args)...);
	}
};
//!@}


/*!
\sa Forms::CallBinary
\since build 760
*/
template<typename _func>
struct BinaryExpansion
	: private ystdex::equality_comparable<BinaryExpansion<_func>>
{
	_func Function;

	//! \since build 927
	template<typename... _tParams, yimpl(typename
		= ystdex::exclude_self_params_t<BinaryExpansion, _tParams...>)>
	BinaryExpansion(_tParams&&... args)
		: Function(yforward(args)...)
	{}

	/*!
	\brief 比较处理器相等。
	\since build 773
	*/
	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const BinaryExpansion& x, const BinaryExpansion& y)
		ImplRet(ystdex::examiners::equal_examiner::are_equal(x.Function,
			y.Function))

	//! \since build 922
	template<typename... _tParams>
	inline ReductionStatus
	operator()(_tParams&&... args) const
	{
		return Forms::CallBinary(Function, yforward(args)...);
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

	//! \since build 927
	template<typename... _tParams, yimpl(typename
		= ystdex::exclude_self_params_t<BinaryAsExpansion, _tParams...>)>
	BinaryAsExpansion(_tParams&&... args)
		: Function(yforward(args)...)
	{}

	/*!
	\brief 比较处理器相等。
	\since build 773
	*/
	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const BinaryAsExpansion& x, const BinaryAsExpansion& y)
		ImplRet(ystdex::examiners::equal_examiner::are_equal(x.Function,
			y.Function))

	//! \since build 922
	template<typename... _tParams>
	inline ReductionStatus
	operator()(_tParams&&... args) const
	{
		return Forms::CallBinaryAs<_type, _type2>(Function, yforward(args)...);
	}
};
//!@}


/*!
\pre 间接断言：第二参数的数据指针非空。
\since build 871
*/
//!@{
//! \brief 注册一元严格求值上下文处理器。
//!@{
template<size_t _vWrapping = Strict, typename _func, class _tTarget>
inline void
RegisterUnary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterFormHandler(target, name,
		UnaryExpansion<_func>(std::move(f)), ystdex::size_t_<_vWrapping>());
}
//! \since build 927
template<size_t _vWrapping = Strict, typename _func, class _tTarget>
inline void
RegisterUnary(_tTarget& target, string_view name, trivial_swap_t, _func f)
{
	A1::RegisterFormHandler(target, name, trivial_swap,
		UnaryExpansion<_func>(std::move(f)), ystdex::size_t_<_vWrapping>());
}
template<size_t _vWrapping = Strict, typename _type, typename _func,
	class _tTarget>
inline void
RegisterUnary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterFormHandler(target, name, UnaryAsExpansion<_type, _func>(
		std::move(f)), ystdex::size_t_<_vWrapping>());
}
//! \since build 927
template<size_t _vWrapping = Strict, typename _type, typename _func,
	class _tTarget>
inline void
RegisterUnary(_tTarget& target, string_view name, trivial_swap_t, _func f)
{
	A1::RegisterFormHandler(target, name, trivial_swap, UnaryAsExpansion<_type,
		_func>(std::move(f)), ystdex::size_t_<_vWrapping>());
}
//!@}

//! \brief 注册二元严格求值上下文处理器。
//!@{
template<size_t _vWrapping = Strict, typename _func, class _tTarget>
inline void
RegisterBinary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterFormHandler(target, name,
		BinaryExpansion<_func>(std::move(f)), ystdex::size_t_<_vWrapping>());
}
//! \since build 927
template<size_t _vWrapping = Strict, typename _func, class _tTarget>
inline void
RegisterBinary(_tTarget& target, string_view name, trivial_swap_t, _func f)
{
	A1::RegisterFormHandler(target, name, trivial_swap,
		BinaryExpansion<_func>(std::move(f)), ystdex::size_t_<_vWrapping>());
}
template<size_t _vWrapping = Strict, typename _type, typename _type2,
	typename _func, class _tTarget>
inline void
RegisterBinary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterFormHandler(target, name, BinaryAsExpansion<_type, _type2,
		_func>(std::move(f)), ystdex::size_t_<_vWrapping>());
}
//! \since build 927
template<size_t _vWrapping = Strict, typename _type, typename _type2,
	typename _func, class _tTarget>
inline void
RegisterBinary(_tTarget& target, string_view name, trivial_swap_t, _func f)
{
	A1::RegisterFormHandler(target, name, trivial_swap, BinaryAsExpansion<_type,
		_type2, _func>(std::move(f)), ystdex::size_t_<_vWrapping>());
}
//!@}
//!@}


//! \since build 974
//!@{
template<typename _fComp, typename _func>
auto
EqTerm(TermNode& term, _fComp f, _func g) -> decltype(f(
	std::declval<_func&>()(std::declval<const TermNode&>()),
	std::declval<_func&>()(std::declval<const TermNode&>())))
{
	RetainN(term, 2);

	auto i(term.begin());
	const auto& x(*++i);

	return f(g(x), g(ystdex::as_const(*++i)));
}

template<typename _fComp, typename _func>
void
EqTermRet(TermNode& term, _fComp f, _func g)
{
	using type = decltype(g(term));

	EqTerm(term, [&, f](const type& x, const type& y){
		term.Value = f(x, y);
	}, g);
}

template<typename _func>
void
EqTermValue(TermNode& term, _func f)
{
	EqTermRet(term, f, [](const TermNode& x) -> const ValueObject&{
		return ReferenceLeaf(x).Value;
	});
}

template<typename _func>
void
EqTermReference(TermNode& term, _func f)
{
	EqTermRet(term,
		[f] YB_LAMBDA_ANNOTATE((const TermNode& x, const TermNode& y), , pure){
		return IsAtom(x) && IsAtom(y) ? f(x.Value, y.Value)
			: ystdex::ref_eq<>()(x, y);
	}, static_cast<const TermNode&(&)(const TermNode&)>(ReferenceTerm));
}
//!@}

} // namespace Forms;

} // namesapce A1;

} // namespace NPL;

//! \since build 927
namespace ystdex
{

//! \relates NPL::A1::EncapsulationBase
template<>
struct is_bitwise_swappable<NPL::A1::EncapsulationBase> : true_
{};

//! \relates NPL::A1::Encapsulation
template<>
struct is_bitwise_swappable<NPL::A1::Encapsulation> : true_
{};

//! \relates NPL::A1::Encapsulate
template<>
struct is_bitwise_swappable<NPL::A1::Encapsulate> : true_
{};

//! \relates NPL::A1::EncapsulateValue
template<>
struct is_bitwise_swappable<NPL::A1::EncapsulateValue> : true_
{};

//! \relates NPL::A1::Encapsulated
template<>
struct is_bitwise_swappable<NPL::A1::Encapsulated> : true_
{};

//! \relates NPL::A1::Decapsulate
template<>
struct is_bitwise_swappable<NPL::A1::Decapsulate> : true_
{};

//! \relates NPL::A1::UnaryExpansion
template<typename _func>
struct is_bitwise_swappable<NPL::A1::Forms::UnaryExpansion<_func>>
	: is_bitwise_swappable<_func>
{};

//! \relates NPL::A1::UnaryAsExpansion
template<typename _type, typename _func>
struct is_bitwise_swappable<NPL::A1::Forms::UnaryAsExpansion<_type, _func>>
	: is_bitwise_swappable<_func>
{};

//! \relates NPL::A1::BinaryExpansion
template<typename _func>
struct is_bitwise_swappable<NPL::A1::Forms::BinaryExpansion<_func>>
	: is_bitwise_swappable<_func>
{};

//! \relates NPL::A1::BinaryAsExpansion
template<typename _type, typename _type2, typename _func>
struct is_bitwise_swappable<
	NPL::A1::Forms::BinaryAsExpansion<_type, _type2, _func>>
	: is_bitwise_swappable<_func>
{};

} // namespace ystdex;

#endif

