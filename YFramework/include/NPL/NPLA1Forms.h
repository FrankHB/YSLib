/*
	© 2016-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1Forms.h
\ingroup NPL
\brief NPLA1 语法形式。
\version r8880
\author FrankHB <frankhb1989@gmail.com>
\since build 882
\par 创建时间:
	2020-02-15 11:19:21 +0800
\par 修改时间:
	2022-09-12 02:53 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1Forms
*/


#ifndef NPL_INC_NPLA1Forms_h_
#define NPL_INC_NPLA1Forms_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA1 // for shared_ptr, TermNode, ReductionStatus, string,
//	TokenValue, AssertBranch, ystdex::exclude_self_t, ystdex::expand_proxy,
//	NPL::ResolveTerm, std::next, AssertValueTags, Access, NPL::Deref,
//	Forms::CallResolvedUnary, ResolvedTermReferencePtr, AccessRegular,
//	ystdex::make_expanded, std::ref, ystdex::invoke_nonvoid, TNIter,
//	AccessTypedValue, ystdex::make_transform, std::accumulate,
//	std::placeholders::_2, ystdex::bind1, ContextNode,
//	ystdex::equality_comparable, ystdex::exclude_self_params_t,
//	ystdex::examiners::equal_examiner, trivial_swap_t, trivial_swap,
//	Environment, ystdex::is_bitwise_swappable, ystdex::true_;

namespace NPL
{

//! \since build 665
namespace A1
{

/*!
\brief 续延帧检查。
\note 因对性能有影响，默认仅调试配置下启用。
\sa Continuation
\since build 943

若定义为 true ，则在续延调用时断言被捕获的帧在当前动作序列中存在且满足实现约束。
*/
#ifndef NPL_NPLA1Forms_CheckContinuationFrames
#	ifndef NDEBUG
#		define NPL_NPLA1Forms_CheckContinuationFrames true
#	else
#		define NPL_NPLA1Forms_CheckContinuationFrames false
#	endif
#endif


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
//@{
//! \since build 786
YB_ATTR_nodiscard YF_API YB_PURE TokenValue
StringToSymbol(const string&);
//! \since build 863
YB_ATTR_nodiscard YF_API YB_PURE TokenValue
StringToSymbol(string&&);
//@}

/*!
\brief 取符号对应的名称字符串。
\since build 786

参考调用文法：
<pre>symbol->string \<object></pre>
*/
YB_ATTR_nodiscard YF_API YB_STATELESS const string&
SymbolToString(const TokenValue&) ynothrow;


/*!
\warning 非虚析构。
\since build 924
*/
//@{
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

	DefGetter(const ynothrow, const EncapsulationBase&, , *this)
	DefGetter(ynothrow, EncapsulationBase&, Ref, *this)
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
//@}


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
\see %Documentation::NPL.
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
//@{
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
//@{
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
//@}
//@}

/*!
\note 确定项具有一个实际参数后展开调用参数指定的函数。
\sa NPL::EmplaceCallResultOrReturn
\since build 922

返回值的处理使用 NPL::EmplaceCallResultOrReturn 。
若需以和其它类型的值类似的方式被包装，在第一参数中构造 ValueObject 对象。
实现使用 ystdex::make_expanded 展开调用，但不复制或转移可调用对象，
	因此使用 std::ref 包装第一参数。注意当前无条件视第一参数为 const 左值。
*/
//@{
//! \brief 访问节点并调用一元函数。
//@{
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
//@}

//! \brief 访问节点并调用二元函数。
//@{
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
//@}

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
//@}


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
//@{
//! \since build 741
//@{
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
//@}


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
	A1::RegisterFormHandler(target, name,
		UnaryExpansion<_func>(std::move(f)), _vWrapping);
}
//! \since build 927
template<size_t _vWrapping = Strict, typename _func, class _tTarget>
inline void
RegisterUnary(_tTarget& target, string_view name, trivial_swap_t, _func f)
{
	A1::RegisterFormHandler(target, name, trivial_swap,
		UnaryExpansion<_func>(std::move(f)), _vWrapping);
}
template<size_t _vWrapping = Strict, typename _type, typename _func,
	class _tTarget>
inline void
RegisterUnary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterFormHandler(target, name,
		UnaryAsExpansion<_type, _func>(std::move(f)), _vWrapping);
}
//! \since build 927
template<size_t _vWrapping = Strict, typename _type, typename _func,
	class _tTarget>
inline void
RegisterUnary(_tTarget& target, string_view name, trivial_swap_t, _func f)
{
	A1::RegisterFormHandler(target, name, trivial_swap,
		UnaryAsExpansion<_type, _func>(std::move(f)), _vWrapping);
}
//@}

//! \brief 注册二元严格求值上下文处理器。
//@{
template<size_t _vWrapping = Strict, typename _func, class _tTarget>
inline void
RegisterBinary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterFormHandler(target, name,
		BinaryExpansion<_func>(std::move(f)), _vWrapping);
}
//! \since build 927
template<size_t _vWrapping = Strict, typename _func, class _tTarget>
inline void
RegisterBinary(_tTarget& target, string_view name, trivial_swap_t, _func f)
{
	A1::RegisterFormHandler(target, name, trivial_swap,
		BinaryExpansion<_func>(std::move(f)), _vWrapping);
}
template<size_t _vWrapping = Strict, typename _type, typename _type2,
	typename _func, class _tTarget>
inline void
RegisterBinary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterFormHandler(target, name,
		BinaryAsExpansion<_type, _type2, _func>(std::move(f)), _vWrapping);
}
//! \since build 927
template<size_t _vWrapping = Strict, typename _type, typename _type2,
	typename _func, class _tTarget>
inline void
RegisterBinary(_tTarget& target, string_view name, trivial_swap_t, _func f)
{
	A1::RegisterFormHandler(target, name, trivial_swap,
		BinaryAsExpansion<_type, _type2, _func>(std::move(f)), _vWrapping);
}
//@}
//@}


/*!
\sa YSLib::ValueObject
\since build 904
*/
//@{
/*!
\brief 比较两个子项表示的值引用相同的对象。
\sa YSLib::HoldSame

参考调用文法：
<pre>eq? \<object1> \<object2></pre>
*/
YF_API void
Eq(TermNode&);

/*!
\brief 比较两个子项的值相等。

参考调用文法：
<pre>eql? \<object1> \<object2></pre>
*/
YF_API void
EqLeaf(TermNode&);

/*!
\brief 比较两个子项的值引用相同的对象。
\sa YSLib::HoldSame

参考调用文法：
<pre>eqr? \<object1> \<object2></pre>
*/
YF_API void
EqReference(TermNode&);

/*!
\brief 比较两个子项表示的值数据成员相等。
\note 依赖目标对象的相等，其中可能有不依赖上下文的异步实现。

参考调用文法：
<pre>eqv? \<object1> \<object2></pre>
*/
YF_API void
EqValue(TermNode&);

/*!
\brief 比较两个子项表示的值相等。

参考调用文法：
<pre>equal? \<object1> \<object2></pre>
*/
YF_API ReductionStatus
EqualTermValue(TermNode&, ContextNode&);
//@}


/*!
\note 测试条件成立，当且仅当 \<test> 非 #f 。
\exception ListReductionFailure 合并子的参数不是列表。
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
\since build 924

操作数直接由函数参数指定。
当项求值为 true 时返回 false ，否则返回 true 。

参考调用文法：
<pre>not? \<test></pre>
*/
YF_API YB_PURE bool
Not(const TermNode&);

/*!
\note 支持保存当前动作。
\exception ListReductionFailure 合并子的参数不是列表。
\sa ReduceOnce
\since build 754
*/
//@{
/*!
\brief 支持短路求值的逻辑与。

非严格求值若干个子项，返回求值结果的逻辑与：
除第一个子项，没有其它子项时，返回 true ；否则从左到右逐个求值子项。
当子项全求值为 true 时返回最后一个子项的值，否则返回 false 。

参考调用文法：
<pre>$and \<test>...</pre>
*/
YF_API ReductionStatus
And(TermNode&, ContextNode&);

/*!
\brief 支持短路求值的逻辑或。

非严格求值若干个子项，返回求值结果的逻辑或：
除第一个子项，没有其它子项时，返回 false ；否则从左到右逐个求值子项。
当子项全求值为 false 时返回 false，否则返回第一个不是 false 的子项的值。

参考调用文法：
<pre>$or \<test>...</pre>
*/
YF_API ReductionStatus
Or(TermNode&, ContextNode&);
//@}


/*!
\brief 接受两个参数，返回以第一参数作为第一个元素插入第二参数创建的新的有序对。
\return ReductionStatus::Retained 。
*/
//@{
/*!
\sa LiftToReturn
\since build 779

按值传递返回值。构造的对象中的元素转换为右值。

参考调用文法：
<pre>cons \<object> \<pair></pre>
*/
YF_API ReductionStatus
Cons(TermNode&);

/*!
\since build 822

在返回时不提升项，允许返回引用。

参考调用文法：
<pre>cons% \<object> \<pair></pre>
*/
YF_API ReductionStatus
ConsRef(TermNode&);
//@}

//! \throw ListTypeError 参数不是有序对。
//@{
/*!
\brief 取有序对的第一个元素并转发给指定的应用子。
\since build 911

取第三参数指定的有序对的第一个元素作为参数，调用第二参数指定的应用子。
有序对参数在对象语言中按引用传递。

参考调用文法：
<pre>forward-first% \<applicative> \<pair></pre>
*/
YF_API ReductionStatus
ForwardFirst(TermNode&, ContextNode&);

/*!
\brief 取参数指定的有序对的第一个元素的值。
\since build 859
*/
//@{
/*!
转发参数的元素和函数值。

参考调用文法：
<pre>first \<pair></pre>
*/
YF_API ReductionStatus
First(TermNode&);

//! \throw ValueCategoryMismatch 参数不是引用值。
//@{
/*!
\since build 873

结果是有序对的第一个元素的引用值。保留结果中未折叠的引用值。

参考调用文法：
<pre>first@ \<pair></pre>
*/
YF_API ReductionStatus
FirstAt(TermNode&);

/*!
\since build 913

结果是有序对的第一个元素经过转发的引用值。保留结果中的引用值。

参考调用文法：
<pre>first% \<pair></pre>
*/
YF_API ReductionStatus
FirstFwd(TermNode&);

/*!
结果是有序对的第一个元素引用的对象的引用值。保留结果中的引用值。

参考调用文法：
<pre>first& \<pair></pre>
*/
YF_API ReductionStatus
FirstRef(TermNode&);
//@}

/*!
结果是有序对的第一个元素经过返回值转换的值。不保留结果中的引用值。

参考调用文法：
<pre>firstv \<pair></pre>
*/
YF_API ReductionStatus
FirstVal(TermNode&);
//@}

//! \brief 取有序对的第一个元素以外的元素值构成的有序对。
//@{
/*!
\since build 913

结果是有序对的第一个元素以外的元素值经过转发的值构成的有序对。
保留结果中的引用值。

参考调用文法：
<pre>rest% \<pair></pre>
*/
YF_API ReductionStatus
RestFwd(TermNode&);

/*!
\since build 953

结果是有序对第一个元素以外的元素值的引用值构成的有序对。
保留结果中的引用值。

参考调用文法：
<pre>rest& \<pair></pre>
*/
YF_API ReductionStatus
RestRef(TermNode&);

/*!
\since build 910

结果是有序对的第一个元素以外的元素经过返回值转换的值构成的有序对。
不保留结果中的引用值。

参考调用文法：
<pre>restv \<pair></pre>
*/
YF_API ReductionStatus
RestVal(TermNode&);
//@}
//@}

/*!
\throw ListTypeError 第一参数不是有序对。
\throw ValueCategoryMismatch 第一参数不是引用值。
\since build 834
*/
//@{
//! \brief 修改第一参数指定的有序对以第二参数作为第一个元素。
//@{
/*!
第二参数转换为右值。
替代引用值直接通过插入第一个元素引用的值实现。

参考调用文法：
<pre>set-first! \<pair> \<object></pre>
*/
YF_API void
SetFirst(TermNode&);

//! \warning 除自赋值外，不检查循环引用。
//@{
/*!
\since build 873

保留第二参数未折叠的引用值。

参考调用文法：
<pre>set-first@! \<pair> \<object></pre>
*/
YF_API void
SetFirstAt(TermNode&);

/*!
保留第二参数引用值。

参考调用文法：
<pre>set-first%! \<pair> \<object></pre>
*/
YF_API void
SetFirstRef(TermNode&);
//@}
//@}

//! \brief 修改第一参数指定的有序对以第二参数作为第一个元素外的有序对。
//@{
/*!
第二参数被转换为右值。
但第二参数的元素不被转换。

参考调用文法：
<pre>set-rest! \<pair> \<object></pre>
*/
YF_API void
SetRest(TermNode&);

/*!
\warning 不检查循环引用。

保留第二参数元素中的引用值。

参考调用文法：
<pre>set-rest%! \<pair> \<object></pre>
*/
YF_API void
SetRestRef(TermNode&);
//@}
//@}


/*!
\brief 对指定项以指定的环境求值。
\sa ResolveEnvironment

以视为表达式的对象 \c \<object> 和环境 \c \<environment> 为指定的参数进行求值。
*/
//@{
/*!
\since build 787

按值传递返回值：提升求值后的项。

参考调用文法：
<pre>eval \<object> \<environment></pre>
*/
YF_API ReductionStatus
Eval(TermNode&, ContextNode&);

/*!
\since build 822

不提升求值后的项，允许返回引用值。

参考调用文法：
<pre>eval% \<object> \<environment></pre>
*/
YF_API ReductionStatus
EvalRef(TermNode&, ContextNode&);
//@}

//! \since build 835
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
\brief 在当前环境中规约字符串表示的翻译单元以求值。
\exception LoggedEvent 翻译单元为空串。
\return ReductionStatus::Retained 。
\sa Reduce

参考调用文法：
<pre>eval-unit \<string></pre>
*/
YF_API ReductionStatus
EvalUnit(TermNode&, ContextNode&);
//@}

/*!
\brief 在动态环境求值第二参数得到的环境中求值第一参数。
\sa ResolveEnvironment
\since build 923

以表达式 \c \<expression> 和环境 \c \<environment> 为指定的参数进行求值。
*/
//@{
/*!
按值传递返回值：提升求值后的项。

参考调用文法：
<pre>$remote-eval \<expression> \<environment></pre>
*/
YF_API ReductionStatus
RemoteEval(TermNode&, ContextNode&);

/*!
不提升求值后的项，允许返回引用值。

参考调用文法：
<pre>$remote-eval% \<expression> \<environment></pre>
*/
YF_API ReductionStatus
RemoteEvalRef(TermNode&, ContextNode&);
//@}


/*!
\brief 创建空环境。
\exception ListReductionFailure 合并子的参数不是列表。
\exception NPLException 异常中立：由 Environment 的构造函数抛出。
\sa Environment::CheckParent
\sa EnvironmentList
\since build 798
\todo 使用专用的异常类型替代 NPLException 。

取以参数指定父环境的空环境。
可选地通过参数指定的一个或多个环境作为父环境。
当且仅当参数指定超过一个环境时，使用环境列表。

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
<pre>$deflazy! \<definiend> \<body></pre>
*/
YF_API ReductionStatus
DefineLazy(TermNode&, ContextNode&);

/*!
\brief 不带递归匹配的定义。

剩余表达式视为一个表达式在上下文决定的当前环境进行求值后绑定到 \c \<definiend> 。

参考调用文法：
<pre>$def! \<definiend> \<body></pre>
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
<pre>$defrec! \<definiend> \<body></pre>
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
<pre>$set! \<environment> \<formals> \<body></pre>
*/
YF_API ReductionStatus
SetWithNoRecursion(TermNode&, ContextNode&);

/*!
\brief 修改指定环境的绑定，带递归匹配。

同 DefineWithRecursion ，但由规约第一参数子项的结果显式地确定被修改的环境。

参考调用文法：
<pre>$setrec! \<environment> \<formals> \<body></pre>
*/
YF_API ReductionStatus
SetWithRecursion(TermNode&, ContextNode&);
//@}

/*!
\throw InvalidSyntax 标识符不是符号。
\throw TypeError 当前环境被冻结。
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

使用 ExtractParameters 检查参数列表并捕获和绑定变量，
然后设置节点的值为表示函数抽象的上下文处理器。
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
\brief 带环境的 λ 抽象：求值为一个捕获当前上下文的严格求值的函数。
\since build 918

捕获的静态环境由环境参数 \<parent> 求值后指定。
*/
//@{
/*!
按值传递返回值：提升项。

参考调用文法：
<pre>$lambda/e \<parent> \<formals> \<body></pre>
*/
YF_API ReductionStatus
LambdaWithEnvironment(TermNode&, ContextNode&);

/*!
在返回时不提升项，允许返回引用。

参考调用文法：
<pre>$lambda/e% \<parent> \<formals> \<body></pre>
*/
YF_API ReductionStatus
LambdaWithEnvironmentRef(TermNode&, ContextNode&);
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

捕获的静态环境由环境参数 \<parent> 求值后指定。
*/
//@{
/*!
按值传递返回值：提升项以避免返回引用造成内存安全问题。

参考调用文法：
<pre>$vau/e \<parent> \<formals> \<eformal> \<body></pre>
*/
YF_API ReductionStatus
VauWithEnvironment(TermNode&, ContextNode&);

/*!
在返回时不提升项，允许返回引用。

参考调用文法：
<pre>$vau/e% \<parent> \<formals> \<eformal> \<body></pre>
*/
YF_API ReductionStatus
VauWithEnvironmentRef(TermNode&, ContextNode&);
//@}

/*!
\brief 包装的 vau 抽象：求值为一个捕获当前上下文的严格求值的函数。
\since build 921

捕获的静态环境由当前动态环境隐式确定。
不保留环境的所有权。
*/
//@{
/*!
按值传递返回值：提升项以避免返回引用造成内存安全问题。

参考调用文法：
<pre>$wvau \<formals> \<eformal> \<body></pre>
*/
YF_API ReductionStatus
WVau(TermNode&, ContextNode&);

/*!
在返回时不提升项，允许返回引用。

参考调用文法：
<pre>$wvau% \<formals> \<eformal> \<body></pre>
*/
YF_API ReductionStatus
WVauRef(TermNode&, ContextNode&);
//@}

/*!
\brief 带环境的包装的 vau 抽象：求值为一个捕获当前上下文的严格求值的函数。
\sa ResolveEnvironment

捕获的静态环境由环境参数 \<parent> 求值后指定。
*/
//@{
/*!
按值传递返回值：提升项以避免返回引用造成内存安全问题。

参考调用文法：
<pre>$wvau/e \<parent> \<formals> \<eformal> \<body></pre>
*/
YF_API ReductionStatus
WVauWithEnvironment(TermNode&, ContextNode&);

/*!
在返回时不提升项，允许返回引用。

参考调用文法：
<pre>$wvau/e% \<parent> \<formals> \<eformal> \<body></pre>
*/
YF_API ReductionStatus
WVauWithEnvironmentRef(TermNode&, ContextNode&);
//@}
//@}
//@}


//! \since build 913
//@{
/*!
\brief 包装合并子为应用子。
\exception NPLException 包装数溢出。

参考调用文法：
<pre>wrap \<combiner></pre>
*/
YF_API ReductionStatus
Wrap(TermNode&);

/*!
\brief 包装合并子为应用子。
\exception NPLException 包装数溢出。

允许返回引用。

参考调用文法：
<pre>wrap& \<combiner></pre>
*/
YF_API ReductionStatus
WrapRef(TermNode&);

//! \exception TypeError 应用子参数的类型不符合要求。
//@{
/*!
\brief 包装操作子为应用子。

参考调用文法：
<pre>wrap1 \<operative></pre>
*/
YF_API ReductionStatus
WrapOnce(TermNode&);

/*!
\brief 包装操作子为应用子。

允许返回引用。

参考调用文法：
<pre>wrap1& \<operative></pre>
*/
YF_API ReductionStatus
WrapOnceRef(TermNode&);

/*!
\brief 解包装应用子为合并子。

参考调用文法：
<pre>unwrap \<applicative></pre>
*/
YF_API ReductionStatus
Unwrap(TermNode&);
//@}
//@}


/*!
\return ReductionStatus::Regular 。
\note 对参数指定的项进行检查，接受对象语言的一个参数。
\since build 859
*/
//@{
/*!
\brief 检查参数指定的项表示环境。
\exception TypeError 检查失败：参数指定的项不表示环境。
\exception ListTypeError 检查失败：参数指定的项表示列表。
\sa ResolveEnvironment

若接受的对象语言参数不表示环境，抛出异常；否则，对象语言中返回为参数指定的值。

参考调用文法：
<pre>check-environment \<object></pre>
*/
YF_API ReductionStatus
CheckEnvironment(TermNode&);

/*!
\brief 检查参数指定的项表示适合作为父环境的环境或环境列表。
\exception TypeError 检查失败：参数指定的项不表示环境。
\exception ListTypeError 检查失败：参数指定的项表示列表。
\since build 909

若接受的对象语言参数不表示适合作为父环境的环境或环境列表，抛出异常；
	否则，对象语言中返回为参数指定的值。

参考调用文法：
<pre>check-parent \<object></pre>
*/
YF_API ReductionStatus
CheckParent(TermNode&);

/*!
\brief 检查参数指定的项表示列表的引用。
\exception ListTypeError 检查失败：参数指定的项不表示列表的引用值。
\since build 857

若接受的对象语言参数不表示列表的引用值，抛出异常；
	否则，对象语言中返回为参数指定的值。

参考调用文法：
<pre>check-list-reference \<object></pre>
*/
YF_API ReductionStatus
CheckListReference(TermNode&);
//@}

/*!
\brief 检查参数指定的项表示有序对的引用。
\exception ListTypeError 检查失败：参数指定的项不表示有序对的引用值。
\since build 951

若接受的对象语言参数不表示有序对的引用值，抛出异常；
	否则，对象语言中返回为参数指定的值。

参考调用文法：
<pre>check-pair-reference \<object></pre>
*/
YF_API ReductionStatus
CheckPairReference(TermNode&);


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
\brief 函数应用：应用参数指定的函数和作为函数参数的对象。
\since build 859

结果是对解包装的应用子应用参数的函数值。保留结果中的引用值。

参考调用文法：
<pre>apply \<applicative> \<object> \<environment>
apply \<applicative> \<object></pre>
*/
YF_API ReductionStatus
Apply(TermNode&, ContextNode&);

/*!
\brief 函数应用：应用参数指定的函数和作为函数参数的列表。
\exception ListReductionFailure 合并子的参数不是列表。
\since build 951

结果是对解包装的应用子应用参数的函数值。保留结果中的引用值。
首先检查参数是列表。

参考调用文法：
<pre>apply-list \<applicative> \<list> \<environment>
apply-list \<applicative> \<list></pre>
*/
YF_API ReductionStatus
ApplyList(TermNode&, ContextNode&);

/*!
\brief 序列有序参数规约：移除第一项后顺序规约子项，结果为最后一个子项的规约结果。
\return 子项被规约时为最后一个子项的规约状态，否则为 ReductionStatus::Clean 。
\exception ListReductionFailure 合并子的参数不是列表。
\note 可直接实现顺序求值。在对象语言中，若参数为空，返回未指定值。
\sa RemoveHead
\since build 823

参考调用文法：
<pre>$sequence \<expression-sequence>...</pre>
*/
YF_API ReductionStatus
Sequence(TermNode&, ContextNode&);

/*!
\brief 使用可选的参数指定的不定数量的元素和结尾列表构造新列表。
\since build 860
*/
//@{
/*!
\sa LiftToReturn

结果是构造的列表的值。不保留结果中的引用值。
但若最后一个参数是有序对，其元素在结果中不被转换。

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
\since build 898

对 \<object1> 指定的抽象列表进行处理，取得部分和。
当谓词 \<predicate> 对列表应用结果不为假时，处理的结果为参数 \<object2> 指定的对象；
	否则，继续处理抽象列表中余下的元素。
处理抽象的列表的操作通过余下的应用子分别定义：
取列表头、取列表尾和部分和的二元合并操作。
*/
//@{
/*!
\brief 在抽象列表元素上应用左结合的二元操作。

参考调用文法：
<pre>accl \<object1> \<predicate> \<object2> \<applicative1></pre>
	<pre>\<applicative2> \<applicative3></pre>
*/
YF_API ReductionStatus
AccL(TermNode&, ContextNode&);

/*!
\brief 在抽象列表元素上应用右结合的二元操作。

参考调用文法：
<pre>accr \<object1> \<predicate> \<object2> \<applicative1></pre>
	<pre>\<applicative2> \<applicative3></pre>
*/
YF_API ReductionStatus
AccR(TermNode&, ContextNode&);
//@}

/*!
\brief 在列表元素上应用右结合的二元操作。
\since build 899

对 \<list> 指定的列表进行处理，取得部分和。
当列表非空时，处理的结果为参数 <object> 指定的对象；
	否则，继续处理列表中余下的元素。
参数 \<applicative> 定义部分和的二元合并操作，应为列表构造器。
名称中的 1 指 \<list> 参数的个数。

参考调用文法：
<pre>foldr1 \<applicative> \<object> \<list></pre>
*/
YF_API ReductionStatus
FoldR1(TermNode&, ContextNode&);

/*!
\brief 单列表映射操作。
\since build 899

使用指定应用子对列表中每个参数进行调用，结果为应用子调用结果作为元素的列表。

参考调用文法：
<pre>map1 \<applicative> \<list></pre>
*/
YF_API ReductionStatus
Map1(TermNode&, ContextNode&);

//! \since build 917
//@{
/*!
\brief 顺序连接两个列表。

参考调用文法：
<pre>list-concat \<list> \<object></pre>
*/
YF_API ReductionStatus
ListConcat(TermNode&);

/*!
\brief 顺序连接零个或多个列表。

参考调用文法：
<pre>append \<list>...</pre>
*/
YF_API ReductionStatus
Append(TermNode&);

/*!
\brief 以 First 在指定列表中选取并合并内容为新的列表。
\sa First

参考调用文法：
<pre>list-extract-first \<list></pre>
*/
YF_API ReductionStatus
ListExtractFirst(TermNode&);

/*!
\brief 以 RestFwd 在指定列表中选取并合并内容为新的列表。
\sa RestFwd

参考调用文法：
<pre>list-extract-rest% \<list></pre>
*/
YF_API ReductionStatus
ListExtractRestFwd(TermNode&);
//@}

/*!
\brief 在列表前插入元素。
\since build 921

参考调用文法：
<pre>list-push-front! \<list> \<object></pre>
*/
YF_API ReductionStatus
ListPushFront(TermNode&);

//! \since build 914
//@{
//! \brief 局部绑定求值。
//@{
/*
不保留结果中的引用值。

参考调用文法：
<pre>$let \<bindings> \<body></pre>
*/
YF_API ReductionStatus
Let(TermNode&, ContextNode&);

/*
保留结果中的引用值。

参考调用文法：
<pre>$let% \<bindings> \<body></pre>
*/
YF_API ReductionStatus
LetRef(TermNode&, ContextNode&);
//@}

//! \brief 指定静态环境并局部绑定求值。
//@{
/*
不保留结果中的引用值。

参考调用文法：
<pre>$let/e \<parent> \<bindings> \<body></pre>
*/
YF_API ReductionStatus
LetWithEnvironment(TermNode&, ContextNode&);

/*
保留结果中的引用值。

参考调用文法：
<pre>$let/e% \<parent> \<bindings> \<body></pre>
*/
YF_API ReductionStatus
LetWithEnvironmentRef(TermNode&, ContextNode&);
//@}
//@}

//! \since build 917
//@{
//! \brief 顺序局部绑定求值。
//@{
/*
不保留结果中的引用值。

参考调用文法：
<pre>$let* \<bindings> \<body></pre>
*/
YF_API ReductionStatus
LetAsterisk(TermNode&, ContextNode&);

/*
保留结果中的引用值。

参考调用文法：
<pre>$let*% \<bindings> \<body></pre>
*/
YF_API ReductionStatus
LetAsteriskRef(TermNode&, ContextNode&);
//@}

//! \brief 允许递归引用绑定的顺序局部绑定求值
//@{
/*
不保留结果中的引用值。

参考调用文法：
<pre>$letrec \<bindings> \<body></pre>
*/
YF_API ReductionStatus
LetRec(TermNode&, ContextNode&);

/*
保留结果中的引用值。

参考调用文法：
<pre>$letrec% \<bindings> \<body></pre>
*/
YF_API ReductionStatus
LetRecRef(TermNode&, ContextNode&);
//@}
//@}

//! \since build 918
//@{
/*!
\brief 求值表达式以构造环境。

参考调用文法：
<pre>$as-environment \<body></pre>
*/
YF_API ReductionStatus
AsEnvironment(TermNode&, ContextNode&);

/*!
\brief 转换绑定列表为以指定的环境列表中的环境为父环境的具有这些绑定的环境。

参考调用文法：
<pre>$bindings/p->environment (\<environment>...) \<binding>...</pre>
*/
YF_API ReductionStatus
BindingsWithParentToEnvironment(TermNode&, ContextNode&);

/*!
\brief 转换绑定列表为没有父环境的具有这些绑定的环境。

参考调用文法：
<pre>$bindings->environment \<binding>...</pre>
*/
YF_API ReductionStatus
BindingsToEnvironment(TermNode&, ContextNode&);

/*!
\brief 转换符号列表为未求值的适合初始化符号导入列表的初值符列表。

参考调用文法：
<pre>symbols->imports! \<symbol>...</pre>
*/
YF_API ReductionStatus
SymbolsToImports(TermNode&);
//@}

/*!
\brief 指定局部绑定后在当前环境中提供绑定。
\since build 919

参考调用文法：
<pre>$provide/let! \<symbols> \<bindings> \<body></pre>
*/
YF_API ReductionStatus
ProvideLet(TermNode&, ContextNode&);

/*!
\brief 在当前环境中提供绑定。
\since build 919

参考调用文法：
<pre>$provide! \<symbols> \<body></pre>
*/
YF_API ReductionStatus
Provide(TermNode&, ContextNode&);

/*!
\brief 从指定的环境导入指定的符号。
\since build 920

参考调用文法：
<pre>$import! \<environment> \<symbols></pre>
*/
YF_API ReductionStatus
Import(TermNode&, ContextNode&);

/*!
\brief 从指定的环境以引用绑定导入指定的符号。
\since build 920

参考调用文法：
<pre>$import&! \<environment> \<symbols></pre>
*/
YF_API ReductionStatus
ImportRef(TermNode&, ContextNode&);

/*!
\since build 943

若不存在相等的元素，结果为空列表右值；否则是同 first% 访问得到的等价的列表的值。
*/
//@{
/*!
\brief 取关联列表中和参数的引用相同的元素。
\since build 943

以 eq? 依次判断第二参数指定的列表中的第一个元素是否和第一参数指定的元素等价。

参考调用文法：
<pre>assq \<object> \<list></pre>
*/
YF_API ReductionStatus
Assq(TermNode&);

/*!
\brief 取关联列表中和参数的值相等的元素。

以 eqv? 依次判断第二参数指定的列表中的第一个元素是否和第一参数指定的元素等价。

参考调用文法：
<pre>assv \<object> \<list></pre>
*/
YF_API ReductionStatus
Assv(TermNode&);
//@}


//! \since build 943
//@{
/*!
\brief 捕获一次续延，具现为一等续延作为参数调用合并子。
\warning 应确保实现选项以避免未定义行为。

参考调用文法：
<pre>call/1cc \<combiner></pre>

对捕获的续延，若被合并子调用，则可移除特定的对象语言活动记录。
调用捕获的合并子应确保启用实现选项 NPL_Impl_NPLA1_Enable_Thunked ，
	以确保随活动记录时同时在宿主语言中移除对相应资源访问；
否则，除非被抛出异常，之后在宿主语言（主调函数）中访问这些活动记录，
	总是存在未定义行为。
*/
YF_API ReductionStatus
Call1CC(TermNode&, ContextNode&);

/*!
\brief 捕获一次续延，具现为一等续延作为参数调用合并子。

参考调用文法：
<pre>continuation->applicative \<continuation></pre>
*/
YF_API ReductionStatus
ContinuationToApplicative(TermNode&);
//@}

/*!
\brief 应用续延。
\since build 952

参考调用文法：
<pre>apply-continuation \<continuation> \<object></pre>
*/
YF_API ReductionStatus
ApplyContinuation(TermNode&, ContextNode&);


/*!
\brief 调用 UTF-8 字符串的系统命令，并保存 int 类型的结果到项的值中。
\sa usystem
\since build 741

参考调用文法：
<pre>system \<string></pre>
*/
YF_API void
CallSystem(TermNode&);

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

