/*
	© 2014-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.h
\ingroup NPL
\brief NPLA1 公共接口。
\version r10344
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 17:58:24 +0800
\par 修改时间:
	2022-10-28 18:51 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#ifndef NPL_INC_NPLA1_h_
#define NPL_INC_NPLA1_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA // for NPLATag, TermNode, ContextNode,
//	ystdex::equality_comparable, std::declval, ystdex::exclude_self_t,
//	trivial_swap_t, trivial_swap, ystdex::ref_eq, string_view,
//	CombineReductionResult, SourceName, make_observer, YSLib::allocate_shared,
//	TNIter, LiftOtherValue, NPL::Deref, NPL::AsTermNode,
//	std::make_move_iterator, std::next, ystdex::retry_on_cond, std::find_if,
//	YSLib::AreEqualHeld, ystdex::or_, std::is_constructible, ystdex::decay_t,
//	ystdex::expanded_caller, ystdex::false_, ystdex::make_parameter_list_t,
//	ystdex::make_function_type_t, ystdex::true_, ystdex::nor_, tuple,
//	ystdex::enable_if_t, std::is_same, ystdex::is_same_param, ystdex::size_t_,
//	AssertCombiningTerm, IsList, ThrowListTypeErrorForNonList,
//	ThrowInsufficientTermsError, CountPrefix, ArityMismatch, TermReference,
//	TermTags, RegularizeTerm, ystdex::guard, ystdex::invoke, AssignParent,
//	shared_ptr, Environment, TokenValue, function, type_info, type_id,
//	SourceInformation, std::bind, std::placeholders::_1, ParseResultOf,
//	ByteParser, SourcedByteParser, std::integral_constant, pmr::memory_resource,
//	ystdex::well_formed_t, ystdex::ref, ReaderState,
//	ystdex::is_bitwise_swappable;
#include YFM_YSLib_Core_YEvent // for YSLib::GHEvent, YSLib::GCombinerInvoker,
//	YSLib::GDefaultLastValueInvoker;
#include <ystdex/algorithm.hpp> // for ystdex::fast_any_of, ystdex::split;
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;
#include <ystdex/type_op.hpp> // for ystdex::exclude_self_params_t,
//	ystdex::is_instance_of;
#include <ystdex/integer_sequence.hpp> // for ystdex::index_sequence,
//	ystdex::vseq::_a;
#include <ystdex/apply.hpp> // for ystdex::apply;
#include <ystdex/optional.h> // for ystdex::optional;
#include <iosfwd> // for std::ostream, std::streambuf;
#include <istream> // for std::istream;
#include <ystdex/string.hpp> // for ystdex::sfmt;

namespace NPL
{

//! \since build 665
namespace A1
{

/*!
\brief NPLA1 元标签。
\note NPLA1 是 NPLA 的具体实现。
\since build 597
*/
struct YF_API NPLA1Tag : NPLATag
{};


//! \ingroup ThunkType
//@{
//! \since build 674
//@{
//! \brief 上下文处理器类型。
using ContextHandler = YSLib::GHEvent<ReductionStatus(TermNode&, ContextNode&)>;
//! \brief 字面量处理器类型。
using LiteralHandler = YSLib::GHEvent<ReductionStatus(const ContextNode&)>;
//@}


/*!
\ingroup ThunkType
\brief 值记号：节点中的值的占位符。
*/
enum class ValueToken
{
	Null,
	/*!
	\brief 未定义值。
	\since build 732
	*/
	Undefined,
	/*!
	\brief 未指定值。
	\since build 732
	*/
	Unspecified,
	/*!
	\brief 指示匹配忽略值。
	\since build 929
	*/
	Ignore,
	GroupingAnchor,
	OrderedAnchor
};
//@}

/*!
\brief 取值记号的字符串表示。
\return 表示对应记号值的字符串，保证不相等的输入对应不相等的结果。
\throw std::invalid_argument 输入的值不是合法的值记号。
\relates ValueToken
\since build 768
*/
YB_ATTR_nodiscard YF_API YB_PURE string
to_string(ValueToken);


/*!
\brief NPLA1 一等续延。
\warning 非虚析构。
\since build 841
\todo 支持一等续延捕获。

利用 ContextState 接口，表示续延的一个帧的对象。
*/
class YF_API Continuation : private ystdex::equality_comparable<Continuation>
{
public:
	//! \since build 877
	using allocator_type
		= decltype(std::declval<const ContextNode&>().get_allocator());

	ContextHandler Handler;

	//! \since build 877
	//@{
	template<typename _func, yimpl(typename
		= ystdex::exclude_self_t<Continuation, _func>)>
	inline
	Continuation(_func&& handler, allocator_type a)
		: Handler(ystdex::make_obj_using_allocator<ContextHandler>(a,
		yforward(handler)))
	{}
	//! \since build 926
	template<typename _func, yimpl(typename
		= ystdex::exclude_self_t<Continuation, _func>)>
	inline
	Continuation(trivial_swap_t, _func&& handler, allocator_type a)
		: Handler(ystdex::make_obj_using_allocator<ContextHandler>(a,
		trivial_swap, yforward(handler)))
	{}
	template<typename _func, yimpl(typename
		= ystdex::exclude_self_t<Continuation, _func>)>
	inline
	Continuation(_func&& handler, const ContextNode& ctx)
		: Continuation(yforward(handler), ctx.get_allocator())
	{}
	//! \since build 926
	template<typename _func, yimpl(typename
		= ystdex::exclude_self_t<Continuation, _func>)>
	inline
	Continuation(trivial_swap_t, _func&& handler, const ContextNode& ctx)
		: Continuation(trivial_swap, yforward(handler), ctx.get_allocator())
	{}
	Continuation(const Continuation& cont, allocator_type a)
		: Handler(ystdex::make_obj_using_allocator<ContextHandler>(a,
		cont.Handler))
	{}
	Continuation(Continuation&& cont, allocator_type a)
		: Handler(ystdex::make_obj_using_allocator<ContextHandler>(a,
		std::move(cont.Handler)))
	{}
	//@}
	DefDeCopyMoveCtorAssignment(Continuation)

	YB_ATTR_nodiscard YB_STATELESS friend
		PDefHOp(bool, ==, const Continuation& x, const Continuation& y) ynothrow
		ImplRet(ystdex::ref_eq<>()(x, y))

	//! \since build 877
	ReductionStatus
	operator()(ContextNode&) const;

	friend DefSwap(ynothrow, Continuation, swap(_x.Handler, _y.Handler))
};


//! \since build 859
//@{
/*!
\brief 抛出参数指定消息的语法错误异常。
\throw InvalidSyntax 语法错误：参数指定的标识符是不被支持的字面量。
*/
//@{
//! \pre 间接断言：第一参数非空。
YB_NORETURN YF_API YB_NONNULL(1) void
ThrowInvalidSyntaxError(const char*);
//! \pre 间接断言：第一参数的数据指针非空。
YB_NORETURN YF_API void
ThrowInvalidSyntaxError(string_view);
//@}

/*!
\brief 抛出被赋值操作数不可修改的异常。
\throw TypeError 被赋值操作数不可修改错误。
*/
YB_NORETURN YF_API void
ThrowNonmodifiableErrorForAssignee();
//@}

/*!
\brief 抛出参数指定值的不支持的字面量语法错误异常。
\throw InvalidSyntax 语法错误。
\since build 896
*/
//@{
//! \pre 间接断言：第一参数非空。
YB_NORETURN YF_API YB_NONNULL(1) void
ThrowUnsupportedLiteralError(const char*);
YB_NORETURN inline PDefH(void, ThrowUnsupportedLiteralError, string_view sv)
	ImplExpr(ThrowUnsupportedLiteralError(sv.data()))
//@}

/*!
\brief 抛出不符合预期值类别的异常。
\throw ValueCategory 值类别错误。
\since build 902
*/
YB_NORETURN YF_API void
ThrowValueCategoryError(const TermNode&);


//! \since build 676
//@{
/*!
\ingroup functors
\brief 遍合并器：逐次调用序列中的遍直至成功。
\note 合并遍结果用于表示及早判断是否应继续规约，可在循环中实现再次规约一个项。
\note 忽略部分规约。不支持异步规约。
*/
struct PassesCombiner
{
	/*!
	\note 对遍调用异常中立。
	\since build 764
	*/
	template<typename _tIn>
	YB_ATTR_nodiscard YB_PURE ReductionStatus
	operator()(_tIn first, _tIn last) const
	{
		auto res(ReductionStatus::Neutral);

		return ystdex::fast_any_of(first, last, [&](ReductionStatus r) ynothrow{
			res = CombineReductionResult(res, r);
			// XXX: Currently %CheckReducible is not used. This should be safe
			//	because only %ReductionStatus::Partial is the exception to be
			//	set, which is not supported here.
			return r == ReductionStatus::Retrying;
		}) ? ReductionStatus::Retrying : res;
	}
};


/*!
\note 结果表示判断是否应继续规约。
\sa PassesCombiner
*/
//@{
//! \brief 一般合并遍。
template<typename... _tParams>
using GPasses = YSLib::GEvent<ReductionStatus(_tParams...),
	YSLib::GCombinerInvoker<ReductionStatus, PassesCombiner>>;
//! \brief 项合并遍。
using TermPasses = GPasses<TermNode&>;
//! \brief 求值合并遍。
using EvaluationPasses = GPasses<TermNode&, ContextNode&>;
/*!
\brief 字面量合并遍。
\pre 字符串参数的数据指针非空。
\since build 738
*/
using LiteralPasses = GPasses<TermNode&, ContextNode&, string_view>;
//@}


//! \brief 作用域守卫类型。
using Guard = any;
/*!
\brief 作用域守卫遍：用于需在规约例程的入口和出口关联执行的操作。
\todo 支持迭代使用旧值。
*/
using GuardPasses = YSLib::GEvent<Guard(TermNode&, ContextNode&),
	YSLib::GDefaultLastValueInvoker<Guard>>;
//@}


//! \since build 955
class GlobalState;

/*!
\brief NPLA1 上下文状态。
\note 扩展 ContextNode 。
\sa ContextNode::Derivation
\since build 842

NPLA1 上下文状态扩展 NPLA 上下文。
*/
class YF_API ContextState : public ContextNode
{
public:
	lref<const GlobalState> Global;
	//! \brief 守卫遍。
	GuardPasses Guard{};
	/*!
	\brief NPLA1 表达式节点一次规约续延。
	\pre 规约函数第二参数引用的对象是 NPLA1 上下文状态或 public 继承的派生类。
	\pre 若修改规约函数实现，应和 DefaultReduceOnce 具有相同的求值语义。
	\pre 若可能在实现中调用续延，调用时应确保下一求值项被正确设置。
	\sa SetNextTermRef
	\since build 877
	*/
	Continuation ReduceOnce{DefaultReduceOnce, *this};

private:
	/*!
	\brief 下一求值项指针。
	\note 可被续延访问。
	*/
	observer_ptr<TermNode> next_term_ptr{};
	/*!
	\brief 规约合并项指针。
	\since build 895
	*/
	observer_ptr<TermNode> combining_term_ptr{};

public:
	/*!
	\brief 规约合并项操作符名称。
	\since build 948
	*/
	mutable ValueObject OperatorName{};
	/*!
	\brief 当前源代码名称。
	\since build 955
	*/
	SourceName CurrentSource{};

	/*!
	\brief 构造：使用指定的全局状态引用。
	\since build 955
	*/
	ContextState(const GlobalState&);
	//! \brief 复制构造：复制下一项的指针以外的子对象。
	ContextState(const ContextState&);
	//! \brief 转移构造：转移基类子对象和下一项的指针，并复制其余子对象。
	ContextState(ContextState&&);
	//! \brief 虚析构：类定义外默认实现。
	~ContextState() override;

	DefDeCopyMoveAssignment(ContextState)

	/*!
	\brief 访问实例。
	\pre 参数引用的对象是 NPLA1 上下文状态或 public 继承的派生类。
	\warning 若不满足上下文状态类型要求，行为未定义。
	*/
	//@{
	YB_ATTR_nodiscard YB_PURE static
		PDefH(ContextState&, Access, ContextNode& ctx) ynothrowv
		ImplRet(ystdex::polymorphic_downcast<ContextState&>(ctx))
	YB_ATTR_nodiscard YB_PURE static
		PDefH(const ContextState&, Access, const ContextNode& ctx) ynothrowv
		ImplRet(ystdex::polymorphic_downcast<const ContextState&>(ctx))
	//@}

	/*!
	\brief 取规约合并项指针。
	\sa combining_term_ptr
	\since build 895
	*/
	DefGetter(const ynothrow, observer_ptr<TermNode>, CombiningTermPtr,
		combining_term_ptr)
	/*!
	\brief 取下一求值项引用。
	\throw NPLException 下一求值项的指针为空。
	\sa next_term_ptr
	*/
	YB_ATTR_nodiscard YB_PURE TermNode&
	GetNextTermRef() const;

	/*!
	\brief 设置规约合并项引用。
	\sa combining_term_ptr
	\since build 895
	*/
	PDefH(void, SetCombiningTermRef, TermNode& term) ynothrow
		ImplExpr(combining_term_ptr = make_observer(&term))
	/*!
	\brief 设置下一求值项引用。
	\sa next_term_ptr
	\since build 883
	*/
	PDefH(void, SetNextTermRef, TermNode& term) ynothrow
		ImplExpr(next_term_ptr = make_observer(&term))

	/*!
	\brief 清除规约合并项指针。
	\sa combining_term_ptr
	\since build 895
	*/
	PDefH(void, ClearCombiningTerm, ) ynothrow
		ImplExpr(combining_term_ptr = {})

	/*!
	\brief 清除下一求值项指针。
	\sa next_term_ptr
	*/
	PDefH(void, ClearNextTerm, ) ynothrow
		ImplExpr(next_term_ptr = {})

	/*!
	\brief NPLA1 表达式节点一次规约默认实现：调用求值例程规约子表达式。
	\pre 间接断言：第一参数的标签可表示一等对象的值。
	\pre 第二参数引用的对象是 NPLA1 上下文状态或 public 继承的派生类。
	\return 规约状态。
	\note 异常安全为调用遍的最低异常安全保证。
	\note 可能使参数中容器的迭代器失效。
	\note 默认不需要重规约。这可被求值遍改变。
	\note 可被求值遍调用以实现递归求值。
	\warning 若不满足上下文状态类型要求，行为未定义。
	\sa GlobalState::EvaluateLeaf
	\sa GlobalState::EvaluateList
	\sa ReduceOnce
	\sa ValueToken
	\since build 878

	对同步实现，调用求值遍对项规约；
	否则，设置上下文使用求值遍作为异步实现的规约动作，
		依赖外部跳板继续表示完成项的规约迭代。
	进入参数指定的项的异步规约调用遍前，清除上下文的最后一次规约状态为中立规约，
		不能使用之前的状态指定重规约。
	在已进入异步规约时，则在上下文最后一次规约状态设置为
		ReductionStatus::Retrying 时，不继续设置异步求值遍而跳过剩余的求值遍。
	对应不同的节点次级结构分类，一次迭代按以下顺序选择以下分支之一，按需规约子项：
	对枝节点调用 Global 的 EvaluateList 求值；
	对空节点或值数据成员为 ValueToken 类型的值的叶节点不进行操作；
	对其它叶节点调用 Global 的 EvaluateList 求值。
	支持重规约。异步重规约由 ContextNode 支持。
	此处约定的迭代中对节点的具体结构分类默认也适用于其它 NPLA1 实现 API ；
	例外情况应单独指定明确的顺序。
	例外情况包括输入节点不是表达式语义结构（而是 AST ）的 API 。
	规约结果由以下规则确定：
	对异步规约，返回 Reduction::Partial ；
	否则，对枝节点，返回被调用的规约遍的求值结果；
	否则，对非枝节点，返回的规约状态总是 ReductionStatus::Retained 。
	*/
	static ReductionStatus
	DefaultReduceOnce(TermNode&, ContextNode&);

	/*!
	\brief 构造作用域守卫并重写项。
	\sa Guard
	\sa Rewrite

	重写逻辑包括以下顺序的步骤：
	调用 Guard 进行必要的上下文重置守卫；
	调用 UnwindCurrent 回滚当前动作的守卫；
	调用 Rewrite 。
	*/
	ReductionStatus
	RewriteGuarded(TermNode&, Reducer);

	/*!
	\pre 间接断言：参数的标签可表示一等对象的值。
	\sa ReduceOnce
	\since build 892
	*/
	//@{
	/*!
	\brief 重写项：设置下一项为参数并使用 ReduceOnce 重写项。
	\sa Rewrite
	*/
	ReductionStatus
	RewriteTerm(TermNode&);

	/*!
	\brief 守卫重写项：设置下一项为参数，构造作用域守卫并使用 ReduceOnce 重写项。
	\note 通过调用 RewriteGuarded 构造作用域守卫。
	\sa RewriteGuarded
	*/
	ReductionStatus
	RewriteTermGuarded(TermNode&);
	//@}

	/*!
	\brief 设置当前源代码名称为参数指定初始化的新分配的共享名称。
	\since build 955
	*/
	template<typename... _tParams>
	void
	ShareCurrentSource(_tParams&&... args)
	{
		CurrentSource = YSLib::allocate_shared<string>(get_allocator(),
			yforward(args)...);
	}

	//! \since build 948
	//@{
	/*!
	\brief 尝试取当前尾动作规约的操作符名称。
	\return 若调用成功，操作符的名称符号；否则为空指针。

	对参数和已知的规约合并项比较，若不相同则调用失败。
	否则，尝试以 TokenValue 访问保存的操作符名称对象。若类型不匹配则调用不成功。
	*/
	YB_ATTR_nodiscard PDefH(observer_ptr<TokenValue>, TryGetTailOperatorName,
		TermNode& term) const ynothrow
		ImplRet(combining_term_ptr.get() == &term
			? TryAccessValue<TokenValue>(OperatorName) : nullptr)

	/*!
	\brief 尝试设置当前尾动作规约的操作符名称。
	\return 是否成功。

	检查第一参数指定的项是否同第二参数保存的规约合并项的第一项，若成功视为操作符项，
		并转移操作符项的值数据成员到规约合并项。
	*/
	bool
	TrySetTailOperatorName(TermNode&) const ynothrow;
	//@}

	friend PDefH(void, swap, ContextState& x, ContextState& y) ynothrow
		ImplExpr(swap(static_cast<ContextNode&>(x), static_cast<ContextNode&>(
			y)), swap(x.combining_term_ptr, y.combining_term_ptr),
			swap(x.next_term_ptr, y.next_term_ptr), swap(x.Guard, y.Guard))
};


inline PDefH(ReductionStatus, Continuation::operator(), ContextNode& ctx) const
	ImplRet(Handler(ContextState::Access(ctx).GetNextTermRef(), ctx))


/*!
\pre ContextNode& 类型的参数引用的对象是 NPLA1 上下文状态或 public 继承的派生类。
\note 第一参数为 TermNode& 类型时，引用的对象为被规约项。
\warning 若不满足上下文状态类型要求，行为未定义。
*/
//@{
/*!
\brief NPLA1 表达式节点规约：调用至少一次求值例程规约子表达式。
\pre 间接断言：第一参数的标签可表示一等对象的值。
\return 规约状态。
\sa ContextState::RewriteGuarded
\sa ReduceOnce
\since build 730

以 ReduceOnce 为规约动作调用 ContextState::RewriteGuarded 作为跳板进行重写。
这里的跳板对异步实现是必要的。
同时，在调用时对必要的异步状态（当前上下文的动作）进行保护，
	允许同步和异步的规约的本机实现混合调用。
*/
YF_API ReductionStatus
Reduce(TermNode&, ContextNode&);

/*!
\note 可能使参数中容器的迭代器失效。
\sa Reduce
*/
//@{
/*!
\note 按语言规范，子项规约顺序未指定。
\note 忽略子项重规约要求。
*/
//@{
/*!
\brief 对范围内的第二项开始逐项规约。
\throw InvalidSyntax 容器为空。
\sa ReduceChildren
\since build 773
*/
//@{
YF_API void
ReduceArguments(TNIter, TNIter, ContextNode&);
//! \warning 不检查列表表示，假定参数指定的项是真列表。
inline PDefH(void, ReduceArguments, TermNode& term, ContextNode& ctx)
	ImplRet(ReduceArguments(term.begin(), term.end(), ctx))
//@}

/*!
\brief 规约子项。
\since build 697
*/
//@{
YF_API void
ReduceChildren(TNIter, TNIter, ContextNode&);
inline PDefH(void, ReduceChildren, TermNode& term, ContextNode& ctx)
	ImplExpr(ReduceChildren(term.begin(), term.end(), ctx))
//@}
//@}

/*!
\brief 有序规约子项。
\pre 断言：参数指定的范围不存在子项或参数指定的上下文中的尾动作为空。
\return 当存在子项时为最后一个子项的规约状态，否则为 ReductionStatus::Neutral 。
\sa ReduceOnce
\since build 773
*/
//@{
YF_API ReductionStatus
ReduceChildrenOrdered(TNIter, TNIter, ContextNode&);
inline PDefH(ReductionStatus, ReduceChildrenOrdered, TermNode& term,
	ContextNode& ctx)
	ImplRet(ReduceChildrenOrdered(term.begin(), term.end(), ctx))
//@}

/*!
\brief 规约第一个子项。
\return 规约状态。
\sa IsCombiningTerm
\sa ReduceOnce
\see https://en.wikipedia.org/wiki/Fexpr 。
\since build 730

快速严格性分析：
当节点求值合并项项的节点时，无条件求值第一个子项；
否则，返回 ReductionStatus::Regular 。
这可避免非确定性推断子表达式求值的附加复杂度。
其中，判断求值合并项调用 IsCombiningTerm ，被求值的第一个子项能作为一等对象的表示；
规约子项调用 ReduceOnce 。
*/
YF_API ReductionStatus
ReduceFirst(TermNode&, ContextNode&);

/*!
\brief NPLA1 表达式节点一次规约：转发调用到 NPLA1 表达式节点一次规约续延并调用。
\pre 间接断言：第一参数的标签可表示一等对象的值。
\pre 第二参数引用的对象是 NPLA1 上下文状态或 public 继承的派生类。
\return 规约状态。
\note 默认实现由 ContextState::DefaultReduceOnce 提供。
\note 异常安全和参数的要求同默认实现。
\warning 若不满足上下文状态类型要求，行为未定义。
\sa ContextState::DefaultReduceOnce
\sa ContextState::ReduceOnce
\sa ContextState::SetNextTermRef
\since build 806

转换第二参数为 NPLA1 上下文状态引用，访问其中的 NPLA1 表达式节点一次规约续延并调用。
若使用异步实现，首先设置下一求值项为第一参数指定的项。
对使用 TCO 的异步实现，在尾上下文支持尾调用优化。
*/
YF_API ReductionStatus
ReduceOnce(TermNode&, ContextNode&);

/*!
\brief 再次规约提升后的项。
\pre 间接断言：第一和第三参数指定不相同的项。
\return 同 ReduceOnce 。
\sa LiftOtherValue
\sa ReduceOnce
\since build 869

调用 LiftOtherValue 提升项，再调用 ReduceOnce 规约。
*/
inline PDefH(ReductionStatus, ReduceOnceLifted, TermNode& term,
	ContextNode& ctx, TermNode& tm)
	ImplRet(LiftOtherValue(term, tm), ReduceOnce(term, ctx))

/*!
\brief 规约有序序列：顺序规约子项，结果为最后一个子项的规约结果。
\note 忽略项的值数据成员。
\sa ReduceChildrenOrdered
\since build 764
*/
YF_API ReductionStatus
ReduceOrdered(TermNode&, ContextNode&);

/*!
\brief 移除容器第一个子项到指定迭代器的项后规约。
\pre 断言：参数指定的上下文中的尾动作为空。
\return ReductionStatus::Partial 。
\note 按语言规范，子项规约顺序未指定。
\sa ReduceOnce
\since build 733
*/
YF_API ReductionStatus
ReduceTail(TermNode&, ContextNode&, TNIter);
//@}
//@}

/*!
\note 不访问项的值数据成员。若需返回值正确地反映规约状态，需确保为空。
\return ReductionStatus::Retained

检查最后一个参数指定的项表示的对象语言中的列表或列表的引用值，
	处理其中的元素构造列表到第一参数中的子项。
结果中的列表元素是对应源列表的元素。
最后一个参数指定的源列表可能被第一参数所有。
*/
//@{
/*!
\brief 规约到引用列表。
\since build 913

若源列表为左值，则结果中的列表元素是对应的左值引用值；
否则，结果中的列表元素从源列表中的元素复制初始化（按标签可发生转移）。
*/
YF_API ReductionStatus
ReduceToReferenceList(TermNode&, ContextNode&, TermNode&);

/*!
\brief 规约到可带有唯一引用标签的引用列表。
\since build 915

若源列表为左值，则结果中的列表元素是对应的左值引用值；
否则，结果中的列表元素是源列表中的元素的唯一引用。
*/
YF_API ReductionStatus
ReduceToReferenceUList(TermNode&, TermNode&);
//@}


/*!
\brief 规约至确定的未指定值。
\sa ValueToken::Unspecified
\since build 896
*/
inline PDefH(ReductionStatus, ReduceReturnUnspecified, TermNode& term) ynothrow
	ImplRet(term.Value = ValueToken::Unspecified, ReductionStatus::Clean)


/*!
\brief 设置跟踪深度节点：调用规约时显示深度和上下文等信息。
\note 主要用于调试。
\sa ContextState::Guard
\since build 842

添加设置跟踪深度的例程到 ContextState::Guard 。
*/
YF_API void
SetupTraceDepth(ContextState&, const string& = yimpl("$__depth"));


/*!
\ingroup functors
\brief 中缀分隔符变换器。
\since build 881

变换分隔符中缀表达式为对应的前缀表达式形式。
移除子项中值和指定分隔符指定的项，并添加指定前缀值作为子项。
被添加的子项若是只有一个子项的列表项，该项被提升直接加入转换后的项作为子项。
最后一个参数指定返回值的名称。
*/
struct SeparatorTransformer
{
	//! 最后两个参数分别指定替换添加的前缀和判断被替换的项的过滤条件。
	//@{
	template<typename _func, class _tTerm, class _fPred>
	YB_ATTR_nodiscard TermNode
	operator()(_func trans, _tTerm&& term, const ValueObject& pfx,
		_fPred filter) const
	{
		const auto a(term.get_allocator());
		auto res(NPL::AsTermNode(a, yforward(term).Value));

		using it_t = decltype(std::make_move_iterator(term.begin()));

		res.Add(NPL::AsTermNode(a, pfx));
		ystdex::split(std::make_move_iterator(term.begin()),
			std::make_move_iterator(term.end()), filter,
			[&](it_t b, it_t e){
			const auto add([&](TermNode& node, it_t i){
				node.Add(trans(NPL::Deref(i)));
			});

			if(b != e)
			{
				// XXX: This guarantees a single element is converted with
				//	no redundant parentheses according to NPLA1 syntax,
				//	consistent to the trivial reduction for term with one
				//	subnode in %ContextState::DefaultReduceOnce.
				if(std::next(b) == e)
					add(res, b);
				else
				{
					auto child(NPL::AsTermNode(res.get_allocator()));

					ystdex::retry_on_cond([&]() ynothrow{
						return b != e;
					}, [&]{
						add(child, b++);
					});
					res.Add(std::move(child));
				}
			}
		});
		return res;
	}

	template<class _tTerm, class _fPred>
	YB_ATTR_nodiscard static TermNode
	Process(_tTerm&& term, const ValueObject& pfx, _fPred filter)
	{
		return SeparatorTransformer()([&](_tTerm&& tm) ynothrow{
			return yforward(tm);
		}, yforward(term), pfx, filter);
	}

	// XXX: This is only one pass and used at current in the preprocessing pass.
	//	See $2020-02 @ %Documentation::Workflow.
	/*!
	\brief 找到子项符合过滤条件的并替换项为添加替换前缀的形式。
	\return 是否找到并替换了项。
	\note 子项的内容在替换时被转移。
	*/
	template<class _fPred>
	static void
	ReplaceChildren(TermNode& term, const ValueObject& pfx,
		_fPred filter)
	{
		if(std::find_if(term.begin(), term.end(), filter) != term.end())
			term = Process(std::move(term), pfx, filter);
	}
	//@}
};


/*!
\pre 间接断言：字符串参数的数据指针非空。
\pre 间接断言：字符串参数非空。
\return 分析结果。
\since build 926
*/
//@{
/*!
\brief 分析参数指定的叶节点词素。

依次进行以下转换操作确定值数据成员后返回：
对代码字面量，去除字面量边界分隔符后进一步求值；
对数据字面量，去除字面量边界分隔符作为字符串值；
对其它字面量，转换为符号；
结果保留到第一参数。
使用第一参数指定的分配器。
*/
YF_API void
ParseLeaf(TermNode&, string_view);

/*!
\brief 分析参数指定的带有源代码信息和叶节点词素。
\pre 间接断言：字符串参数的数据指针非空。

同 ParseLeaf ，但同时在分析结果的记号中包含源代码信息。
第二参数表示源代码来源。
第三参数表示记号在源代码中的位置。
*/
YF_API void
ParseLeafWithSourceInformation(TermNode&, string_view, const SourceName&,
	const SourceLocation&);
//@}


/*!
\brief 包装上下文处理器。
\note 忽略被包装的上下文处理器可能存在的返回值，自适应默认返回规约结果。
\warning 非虚析构。
\since build 751

使用 WrappedContextHandler 也可代替 NPL::EmplaceCallResultOrReturn ，
	直接支持隐式返回 ReductionStatus::Clean 状态的处理器，
	但返回类型为 void 而不是 ReductionStatus 。
理论上通过完全的内联，使用 WrappedContextHandler 不影响性能；通常的 C++ 实现中，
	依赖 WrappedContextHandler 可能较小地影响性能。
*/
template<typename _func>
class WrappedContextHandler
	: private ystdex::equality_comparable<WrappedContextHandler<_func>>
{
public:
	_func Handler;

	//! \since build 849
	template<typename... _tParams, yimpl(typename
		= ystdex::exclude_self_params_t<WrappedContextHandler, _tParams...>)>
	WrappedContextHandler(_tParams&&... args)
		: Handler(yforward(args)...)
	{}
	//! \since build 757
	DefDeCopyMoveCtorAssignment(WrappedContextHandler)

	template<typename... _tParams>
	ReductionStatus
	operator()(_tParams&&... args) const
	{
		Handler(yforward(args)...);
		return ReductionStatus::Clean;
	}

	/*!
	\brief 比较上下文处理器相等。
	\note 使用 YSLib::AreEqualHeld 。
	\since build 756
	*/
	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const WrappedContextHandler& x, const WrappedContextHandler& y)
		ImplRet(YSLib::AreEqualHeld(x.Handler, y.Handler))

	//! \since build 834
	friend
		DefSwap(ynothrow, WrappedContextHandler, swap(_x.Handler, _y.Handler))
};

//! \relates WrappedContextHandler
//@{
/*!
\ingroup metafunction
\since build 927
*/
template<typename _func, typename _tDst>
// XXX: It is a hack to adjust the convertible result for the expanded caller
//	here. It should have been implemented in %GHEvent, however types those
//	cannot convert to expanded caller cannot be SFINAE'd out, otherwise it would
//	cause G++ 5.4 crash with internal compiler error:
//	"error reporting routines re-entered".
using WrapContextHandlerTarget = ystdex::or_<std::is_constructible<typename
	_tDst::BaseType, _func>, std::is_constructible<typename _tDst::BaseType,
	ystdex::expanded_caller<typename _tDst::FuncType, ystdex::decay_t<_func>>>>;

//! \since build 751
//@{
template<class _tDst, typename _func>
YB_ATTR_nodiscard YB_PURE inline _tDst
WrapContextHandler(_func&& h, ystdex::false_)
{
	return WrappedContextHandler<YSLib::GHEvent<ystdex::make_function_type_t<
		void, ystdex::make_parameter_list_t<typename _tDst::BaseType>>>>(
		yforward(h));
}
template<class, typename _func>
YB_ATTR_nodiscard YB_PURE inline _func
WrapContextHandler(_func&& h, ystdex::true_)
{
	return yforward(h);
}
template<class _tDst, typename _func>
YB_ATTR_nodiscard YB_PURE inline _tDst
WrapContextHandler(_func&& h)
{
	return A1::WrapContextHandler<_tDst>(yforward(h),
		WrapContextHandlerTarget<_func, _tDst>());
}
//@}
//! \since build 886
//@{
template<class _tDst, typename _func, class _tAlloc>
YB_ATTR_nodiscard YB_PURE inline _tDst
WrapContextHandler(_func&& h, const _tAlloc& a, ystdex::false_)
{
	return WrappedContextHandler<YSLib::GHEvent<ystdex::make_function_type_t<
		void, ystdex::make_parameter_list_t<typename _tDst::BaseType>>>>(
		std::allocator_arg, a, yforward(h));
}
template<class, typename _func, class _tAlloc>
YB_ATTR_nodiscard YB_PURE inline _func
WrapContextHandler(_func&& h, const _tAlloc&, ystdex::true_)
{
	return yforward(h);
}
template<class _tDst, typename _func, class _tAlloc>
YB_ATTR_nodiscard YB_PURE inline _tDst
WrapContextHandler(_func&& h, const _tAlloc& a)
{
	return A1::WrapContextHandler<_tDst>(yforward(h), a,
		WrapContextHandlerTarget<_func, _tDst>());
}
//@}
//! \since build 927
//@{
template<class _tDst, typename _func>
YB_ATTR_nodiscard YB_PURE inline _tDst
WrapContextHandler(trivial_swap_t, _func&& h, ystdex::false_)
{
	return WrappedContextHandler<YSLib::GHEvent<ystdex::make_function_type_t<
		void, ystdex::make_parameter_list_t<typename _tDst::BaseType>>>>(
		trivial_swap, yforward(h));
}
template<class, typename _func>
YB_ATTR_nodiscard YB_PURE inline _func
WrapContextHandler(trivial_swap_t, _func&& h, ystdex::true_)
{
	return yforward(h);
}
template<class _tDst, typename _func>
YB_ATTR_nodiscard YB_PURE inline _tDst
WrapContextHandler(trivial_swap_t, _func&& h)
{
	return A1::WrapContextHandler<_tDst>(trivial_swap, yforward(h),
		WrapContextHandlerTarget<_func, _tDst>());
}
template<class _tDst, typename _func, class _tAlloc>
YB_ATTR_nodiscard YB_PURE inline _tDst
WrapContextHandler(trivial_swap_t, _func&& h, const _tAlloc& a, ystdex::false_)
{
	return WrappedContextHandler<YSLib::GHEvent<ystdex::make_function_type_t<
		void, ystdex::make_parameter_list_t<typename _tDst::BaseType>>>>(
		std::allocator_arg, a, trivial_swap, yforward(h));
}
template<class, typename _func, class _tAlloc>
YB_ATTR_nodiscard YB_PURE inline _func
WrapContextHandler(trivial_swap_t, _func&& h, const _tAlloc&, ystdex::true_)
{
	return yforward(h);
}
template<class _tDst, typename _func, class _tAlloc>
YB_ATTR_nodiscard YB_PURE inline _tDst
WrapContextHandler(trivial_swap_t, _func&& h, const _tAlloc& a)
{
	return A1::WrapContextHandler<_tDst>(trivial_swap, yforward(h), a,
		WrapContextHandlerTarget<_func, _tDst>());
}
//@}
//@}


/*!
\brief 形式上下文处理器。
\since build 674
\warning 非虚析构。

处理列表项的操作符。
*/
class YF_API FormContextHandler
	: private ystdex::equality_comparable<FormContextHandler>
{
private:
	//! \since build 958
	//@{
	// XXX: This is needed because the following %NotTag does not exclude
	//	%ystdex::index_sequence instances not intended to be the function type.
	struct PTag final
	{};
	template<typename _type>
	using NotTag = ystdex::nor_<ystdex::is_instance_of<_type,
		ystdex::vseq::_a<tuple>>, std::is_same<_type, PTag>,
		std::is_same<_type, std::allocator_arg_t>,
		std::is_same<_type, trivial_swap_t>>;
	template<typename _tParam>
	using MaybeFunc = ystdex::enable_if_t<NotTag<_tParam>::value
		&& !ystdex::is_same_param<FormContextHandler, _tParam>::value>;
	using Caller = ReductionStatus(*)(const FormContextHandler&, TermNode&,
		ContextNode&);
	//@}

public:
	ContextHandler Handler;

private:
	/*!
	\brief 包装数。
	\note 决定调用前需要对操作数进行求值的次数。
	\since build 958
	*/
	size_t wrapping;
	/*!
	\invariant \c call_n 。
	\invariant <tt>InitCall(wrapping) == call_n</tt> 。
	\since build 958
	*/
	mutable Caller call_n = DoCallN;

public:
	//! \since build 958
	//@{
	template<typename _func, typename... _tParams,
		yimpl(typename = MaybeFunc<_func>)>
	inline
	FormContextHandler(_func&& f, _tParams&&... args)
		: FormContextHandler(
		NPL::forward_as_tuple(yforward(f)), yforward(args)...)
	{}
	template<typename _func, typename... _tParams,
		yimpl(typename = MaybeFunc<_func>)>
	inline
	FormContextHandler(trivial_swap_t, _func&& f, _tParams&&... args)
		: FormContextHandler(NPL::forward_as_tuple(trivial_swap, yforward(f)),
		yforward(args)...)
	{}
	template<typename _func, class _tAlloc, typename... _tParams>
	inline
	FormContextHandler(std::allocator_arg_t, const _tAlloc& a, _func&& f,
		_tParams&&... args)
		: FormContextHandler(NPL::forward_as_tuple(std::allocator_arg, a,
		yforward(f)), yforward(args)...)
	{}
	template<typename _func, class _tAlloc, typename... _tParams>
	inline
	FormContextHandler(std::allocator_arg_t, const _tAlloc& a,
		trivial_swap_t, _func&& f, _tParams&&... args)
		: FormContextHandler(NPL::forward_as_tuple(std::allocator_arg, a,
		trivial_swap, yforward(f)), yforward(args)...)
	{}

private:
	template<typename... _tFuncParams>
	inline
	FormContextHandler(tuple<_tFuncParams...> func_args)
		: Handler(InitWrap(func_args)), wrapping(0), call_n(DoCall0)
	{}
	template<typename... _tFuncParams>
	inline
	FormContextHandler(tuple<_tFuncParams...> func_args, ystdex::size_t_<0>)
		: Handler(InitWrap(func_args)), wrapping(0), call_n(DoCall0)
	{}
	template<typename... _tFuncParams>
	inline
	FormContextHandler(tuple<_tFuncParams...> func_args, ystdex::size_t_<1>)
		: Handler(InitWrap(func_args)), wrapping(1), call_n(DoCall1)
	{}
	template<typename... _tFuncParams, size_t _vN,
		yimpl(typename = ystdex::enable_if_t<(_vN > 1)>)>
	inline
	FormContextHandler(tuple<_tFuncParams...> func_args, ystdex::size_t_<_vN>)
		: Handler(InitWrap(func_args)), wrapping(_vN), call_n(DoCallN)
	{}
	template<typename... _tFuncParams>
	inline
	FormContextHandler(tuple<_tFuncParams...> func_args, size_t n)
		: Handler(InitWrap(func_args)), wrapping(n), call_n(InitCall(n))
	{}
	//@}

public:
	//! \since build 757
	DefDeCopyMoveCtorAssignment(FormContextHandler)

	/*!
	\brief 比较上下文处理器相等。
	\note 忽略检查例程的等价性。
	\since build 748
	*/
	YB_ATTR_nodiscard YB_PURE friend PDefHOp(bool, ==,
		const FormContextHandler& x, const FormContextHandler& y)
		ImplRet(x.Equals(y))

	/*!
	\brief 处理一般形式。
	\pre 断言：若使用异步实现，参数指定的项和下一求值项相同。
	\return Handler 调用的返回值，或 ReductionStatus::Clean 。
	\note 断言调用 Handler 前的项符合 IsBranchedList 。
	\warning 要求异步实现中对 Handler 调用时保证此对象生存期，否则行为未定义。
	\sa ReduceArguments
	\sa wrapping
	\since build 751

	求值每一个参数子项，整体重复 wrapping 次；
	然后断言检查项，对可调用的项调用 Hanlder 。
	项检查不存在或在检查通过后，变换无参数规约，然后对节点调用 Hanlder ，
		否则抛出异常。
	无参数时第一参数应具有两个子项且第二项为空节点。
	*/
	PDefHOp(ReductionStatus, (), TermNode& term, ContextNode& ctx) const
		ImplRet(CheckArguments(wrapping, term), call_n(*this, term, ctx))

	//! \since build 958
	DefPred(const ynothrow, Operative, wrapping == 0)
	/*!
	\brief 判断合并子的实现是否在初始化时按非常量值的包装数确定调用例程。
	\since build 959
	*/
#if __OPTIMIZE_SIZE__
	// XXX: Avoid to mention %DoCallN to allow it opted-out by static linking if
	//	not used.
	DefPred(const ynothrow, DynamicWrapper,
		!(call_n == DoCall0 || call_n == DoCall1))
#else
	DefPred(const ynothrow, DynamicWrapper, call_n == DoCallN)
#endif

	//! \since build 958
	DefGetter(const ynothrow, size_t, WrappingCount, wrapping)

	//! \pre 断言：若使用异步实现，参数指定的项和下一求值项相同。
	//@{
	/*!
	\brief 调用上下文处理器。
	\since build 943
	*/
	ReductionStatus
	CallHandler(TermNode&, ContextNode&) const;

private:
	/*!
	\sa CallHandler
	\since build 859
	*/
	ReductionStatus
	CallN(size_t, TermNode&, ContextNode&) const;
	//@}

public:
	/*!
	\brief 检查参数是否符合应用子要求。
	\pre 参数指定的项是规约合并项。
	\exception ListTypeError 第一参数不等于 0 且第二参数不表示列表。
	\note 因为对象语言中的参数求值规则不影响参数项的结构，所以调用前只需要检查一次。
	\sa AssertCombiningTerm
	\since build 950
	*/
	static void
	CheckArguments(size_t, const TermNode&);

private:
	//! \since build 958
	//@{
	//! \pre 断言：若使用异步实现，参数指定的项和下一求值项相同。
	//@{
	//! \pre 断言：第一参数的包装数等于 0 。
	static ReductionStatus
	DoCall0(const FormContextHandler&, TermNode&, ContextNode&);

	//! \pre 断言：第一参数的包装数等于 1 。
	static ReductionStatus
	DoCall1(const FormContextHandler&, TermNode&, ContextNode&);

	//! \pre 断言：第一参数的包装数大于 1 。
	static ReductionStatus
	DoCallN(const FormContextHandler&, TermNode&, ContextNode&);
	//@}

	//! \since build 859
	YB_ATTR_nodiscard YB_PURE bool
	Equals(const FormContextHandler&) const;

	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_PURE yconstfn static
		PDefH(Caller, InitCall, size_t n) ynothrow
		ImplRet(n == 0 ? DoCall0 : (n == 1 ? DoCall1 : DoCallN))

	template<typename... _tParams>
	YB_ATTR_nodiscard static inline auto
	InitHandler(_tParams&&... args)
		-> decltype(A1::WrapContextHandler<ContextHandler>(yforward(args)...))
	{
		return A1::WrapContextHandler<ContextHandler>(yforward(args)...);
	}

	template<typename... _tFuncParams>
	YB_ATTR_nodiscard static inline auto
	InitWrap(tuple<_tFuncParams...>& func_args) -> decltype(ystdex::apply(
		InitHandler<_tFuncParams...>, std::move(func_args)))
	{
		return
			ystdex::apply(InitHandler<_tFuncParams...>, std::move(func_args));
	}

public:
	//! \pre 断言：包装数不等于 0 。
	PDefH(void, Unwrap, ) ynothrowv
		ImplExpr(YAssert(wrapping != 0, "An operative cannot be unwrapped."),
			call_n = InitCall(--wrapping))
	//@}

	//! \since build 834
	friend DefSwap(ynothrow, FormContextHandler, (swap(_x.Handler, _y.Handler),
		std::swap(_x.wrapping, _y.wrapping)))
};

/*!
\relates FormContextHandler
\since build 921
*/
//@{
template<typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline ContextHandler
MakeForm(TermNode::allocator_type a, _tParams&&... args)
{
	return ContextHandler(std::allocator_arg, a,
		FormContextHandler(yforward(args)...));
}
template<typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline ContextHandler
MakeForm(const TermNode& term, _tParams&&... args)
{
	return A1::MakeForm(term.get_allocator(), yforward(args)...);
}

template<typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline TermNode
AsForm(TermNode::allocator_type a, _tParams&&... args)
{
	// XXX: Allocators are not used as the parameter of the constructor of
	//	%FormContextHandler for performance in most cases.
#if true
	return NPL::AsTermNode(a, std::allocator_arg, a,
		in_place_type<ContextHandler>, std::allocator_arg, a,
		FormContextHandler(yforward(args)...));
#elif true
	return NPL::AsTermNode(a, in_place_type<ContextHandler>, std::allocator_arg,
		a, FormContextHandler(yforward(args)...));
#else
	return NPL::AsTermNode(a, MakeForm(a, yforward(args)...));
#endif
}
//@}


/*!
\brief 注册一般形式上下文处理器。
\pre 间接断言：第二参数的数据指针非空。
\since build 942
*/
//@{
template<class _tTarget>
YB_ATTR_always_inline inline void
RegisterFormHandler(_tTarget& target, string_view name, FormContextHandler fm)
{
	// XXX: %ContextHandler is specialized enough without %trivial_swap.
	NPL::EmplaceLeaf<ContextHandler>(target, name,
		std::allocator_arg, ToBindingsAllocator(target), std::move(fm));
}
//! \note 使用 ADL ToBindingsAllocator 。
template<class _tTarget, typename... _tParams, yimpl(typename
	= ystdex::exclude_self_params_t<FormContextHandler, _tParams...>)>
YB_ATTR_always_inline inline void
RegisterFormHandler(_tTarget& target, string_view name, _tParams&&... args)
{
	// XXX: %FormContextHandler is specialized enough without %trivial_swap.
#if true
	NPL::EmplaceLeaf<ContextHandler>(target, name,
		std::allocator_arg, ToBindingsAllocator(target),
		FormContextHandler(yforward(args)...));
#else
	// NOTE: Any optimized implemenations shall be equivalent to this.
	A1::RegisterFormHandler(target, name,
		FormContextHandler(yforward(args)...));
#endif
}
//@}

/*!
\brief 包装数种类枚举。
\note 用于指定创建上下文处理器的种类。
\since build 871
*/
enum WrappingKind
	: decltype(std::declval<FormContextHandler>().GetWrappingCount())
{
	//! \brief 一般形式：不对参数求值。
	Form = 0,
	//! \brief 严格求值形式：对参数求值 1 次。
	Strict = 1
};

/*!
\pre 间接断言：第二参数的数据指针非空。
\since build 838
*/
//@{
//! \brief 注册一般形式上下文处理器。
template<class _tTarget, typename... _tParams>
YB_ATTR_always_inline inline void
RegisterForm(_tTarget& target, string_view name, _tParams&&... args)
{
	A1::RegisterFormHandler(target, name, yforward(args)..., Form);
}

//! \brief 注册严格上下文处理器。
template<class _tTarget, typename... _tParams>
YB_ATTR_always_inline inline void
RegisterStrict(_tTarget& target, string_view name, _tParams&&... args)
{
	A1::RegisterFormHandler(target, name, yforward(args)..., Strict);
}
//@}


/*!
\brief 检查参数是否是列表。
\pre 参数指定的项是规约合并项。
\throw ListTypeError 参数不表示列表。
\sa AssertCombiningTerm
\since build 951

检查合并子的参数是列表。若检查失败，抛出异常。
抛出的异常同 FormContextHandler::CheckArguments ，但通常直接用于底层合并子实现。
*/
inline PDefH(void, CheckArgumentList, const TermNode& term)
	ImplExpr(AssertCombiningTerm(term), IsList(term) ? void()
		// NOTE: The combiner position is skipped.
		: ThrowListTypeErrorForNonList(term, {}, 1))

/*!
\brief 检查可变参数数量。
\pre 间接断言：参数指定的项是分支列表节点。
\exception ParameterMismatch 缺少项的错误。
\sa ThrowInsufficientTermsError
\since build 952

检查第一参数的子项数大于第二参数指定的参数个数。
若具有不大于第二参数指定的参数个数，则抛出缺少项的异常。
抛出缺少项的异常时，异常消息的项移除第一个子项。
*/
inline PDefH(void, CheckVariadicArity, const TermNode& term, size_t n)
	ImplExpr(AssertCombiningTerm(term), CountPrefix(term) - 1 > n ? void()
		: ThrowInsufficientTermsError(term, {}, 1))

/*!
\pre 间接断言：参数指定的项是分支列表节点或项的容器非空（对应枝节点）。
\sa AssertBranchedList
*/
//@{
/*!
\brief 取项的参数个数：子项数减 1 。
\pre 间接断言：参数指定的项是分支列表节点。
\return 项作为列表操作数被匹配的最大实际参数的个数。
\exception ListReductionFailure 异常中立：由 CheckArgumentList 抛出。
\sa CountPrefix
\since build 951
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(size_t, FetchArgumentN, const TermNode& term)
	ImplRet(CheckArgumentList(term), term.size() - 1)

/*!
\note 保留求值留作保留用途，一般不需要被作为用户代码直接使用。
\note 只用于检查项的个数时，可忽略返回值。
\since build 765
\sa AssertCombiningTerm

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
	ImplRet(AssertCombiningTerm(term), ReductionStatus::Regular)
#if defined(NDEBUG) && YB_IMPL_GNUCPP >= 100000
	YB_Diag_Pop
#endif

/*!
\brief 保留列表项：保留求值且检查参数是列表。
\exception ListReductionFailure 异常中立：由 CheckArgumentList 抛出。
\since build 951
*/
inline PDefH(ReductionStatus, RetainList, const TermNode& term)
	ImplRet(CheckArgumentList(term), ReductionStatus::Regular)

/*!
\brief 保留经检查确保具有指定个数参数的项：保留求值。
\pre 间接断言：参数指定的项是分支列表节点。
\return 项的参数个数。
\exception ListReductionFailure 异常中立：由 FetchArgumentN 抛出。
\throw ArityMismatch 项的参数个数不等于第二参数。
\sa FetchArgumentN
*/
inline size_t
RetainN(const TermNode& term, size_t m = 1)
	ImplRet([&](size_t n){
		if(n == m)
			return n;
		throw ArityMismatch(m, n);
	}(FetchArgumentN(term)))
//@}
//@}


/*!
\pre 间接断言：字符串参数的数据指针非空。
\note 第一参数指定输入的项，其 Value 指定输出的值。
*/
//@{
/*!
\brief 字面量和数值的叶节点求值默认实现。
\pre 字符串参数非空。
\return 是否成功求值得到字面量或数值。
\throw InvalidSyntax 语法错误：第二参数是不被支持的数值。
\sa ContextState::EvaluateLiteral
\since build 880

判断第二参数是否可被解析和为被 NPLA1 对象语言语法支持的字面量或数值。
返回 ReductionStatus::Clean 表示成功求值。
返回 ReductionStatus::Retrying 表示不成功求值，需进一步被作为符号处理。
支持解析基于 NPLAMath 的 NPLA1 数值字面量。
可实现字面量遍。
*/
YF_API ReductionStatus
DefaultEvaluateLeaf(TermNode&, string_view);

/*!
\exception BadIdentifier 未在环境中找到指定标识符的绑定。
\warning 若不满足上下文状态类型要求，行为未定义。
*/
//@{
/*!
\brief 求值标识符。
\note 不验证标识符是否为字面量；仅以字面量处理时可能需要重规约。
\sa ContextState::TrySetTailOperatorName
\sa LiteralHandler
\sa ReferenceTerm
\sa ResolveIdentifier
\sa TermReference
\since build 745

根据第三参数指定的标识符求值，保存结果到第一参数指定的目标项中。
标识符通常来自对象语言中的名称表达式。求值标识符即对先前保存此标识符的项求值：
解析名称并初始化目标项的值；然后检查字面量处理器，若存在则调用。
具体依次进行以下求值操作：
调用 ResolveIdentifier 解析指定的标识符，若失败则抛出异常；
以第二参数作为 ContextState ，调用 TrySetTailOperatorName 设置上下文中的操作符，
	若失败则清空上下文中的操作符；
解析的结果进行引用折叠后重新引用为左值，并赋值到第一参数指定的项的值数据成员；
以 LiteralHandler 访问字面量处理器，若成功调用并返回字面量处理器的处理结果；
否则，返回 ReductionStatus::Neutral 。
其中 ResolveIdentifier 包含的引用折叠确保不引入引用的引用；
只有解析结果自身是未折叠的引用值时，求值标识符的结果才可能是未折叠的引用值。
确保左值引用的操作总是去除解析结果的标签中的 TermTags::Unique ，
	以保证求值结果不是消亡值；
这个过程不移除 TermTags::Temporary ，之后可区分指称的被绑定的对象是否为临时对象。
关于保留标签的用法，参见 TermReference::IsReferencedLValue ；
一般地，因为无法保留消亡值，不使用 EvaluateIdentifier 而用基于调用
	ResolveIdentifier 的（不排除 TermTags::Uniuqe ）的方式与之配合取得结果。
*/
YF_API ReductionStatus
EvaluateIdentifier(TermNode&, const ContextNode&, string_view);

/*!
\brief 求值叶节点记号。
\pre 第二参数引用的对象是 NPLA1 上下文状态或 public 继承的派生类。
\return 求值标识符的结果或 ReductionStatus::Retained 。
\sa CategorizeLexeme
\sa GlobalState::EvaluateLiteral
\sa DeliteralizeUnchecked
\sa EvaluateIdentifier
\since build 736

处理字符串表示的节点记号。
被处理的记号视为符号。
若字面量遍非空，通过调用字面量遍处理记号。
若字面量遍为空或要求重规约，求值非字面量的标识符。
*/
YF_API ReductionStatus
EvaluateLeafToken(TermNode&, ContextNode&, string_view);
//@}

/*!
\brief 若第三参数指定的项表示字面量处理器则调用并以结果其正规化第一参数指定的项。
\sa LiteralHandler
\sa RegularizeTerm
\since build 883
*/
inline PDefH(void, EvaluateLiteralHandler, TermNode& term,
	const ContextNode& ctx, const TermNode& tm)
	// NOTE: This is optional, as it is not guaranteed to be saved as
	//	%ContextHandler in %ReduceCombined.
	ImplExpr([&]{
		if(const auto p_handler = TryAccessTerm<const LiteralHandler>(tm))
			RegularizeTerm(term, (*p_handler)(ctx));
	}())

/*!
\brief 规约合并项：检查项的第一个子项尝试作为操作符进行函数应用，并规范化。
\pre 断言：若第一个子项表示子对象引用，则符合子对象引用的非正规表示约定。
\pre ContextNode& 类型的参数引用的对象是 NPLA1 上下文状态或 public 继承的派生类。
\return 规约状态。
\warning 若不满足上下文状态类型要求，行为未定义。
\exception bad_any_cast 异常中立：子对象引用持有的值不是 ContextHandler 类型。
\throw ListReductionFailure 规约失败：枝节点的第一个子项不表示上下文处理器。
\sa ContextHandler
\sa IsCombiningTerm
\sa Reduce
\sa TermTags::Temporary
\since build 766

对枝节点尝试以第一个子项的 Value 数据成员为上下文处理器并调用，且当规约终止时规范化；
否则视为规约成功，没有其它作用。
若发生 ContextHandler 调用，调用前先转移处理器保证生存期，
	以允许处理器内部移除或修改之前占用的第一个子项（包括其中的 Value 数据成员）。
调用处理器时：
	对异步实现，首先创建用于异步调用的对象并以 \c std::ref(Handler) 初始化，
		并调用创建的对象。
	否则，直接调用 Handler 数据成员。
在被规约的项没有取得范式时，标记临时对象标签以允许转移作为操作符的函数右值。
成功调用后，第一个子项指定的合并子被转移到第一参数以外的位置保存。
这允许简化之后的项的处理，如可假定合并子对象可能所有的项不是第一参数的子项，
	可使用 TermNode::SetContent 代替 LiftOther 提升项。
*/
//@{
//! \note 对非规约合并项直接返回 ReductionStatus::Regular 。
YF_API ReductionStatus
ReduceCombined(TermNode&, ContextNode&);

//! \pre 断言：第一参数是规约合并项。
//@{
/*!
\brief 规约列表合并项：同 ReduceCombined ，但只适用于枝节点。
\since build 882
*/
YF_API ReductionStatus
ReduceCombinedBranch(TermNode&, ContextNode&);

//! \note 若第三参数不表示上下文处理器的宿主值，抛出的异常消息指定其为引用项。
//@{
/*!
\brief 规约列表合并项：同 ReduceCombined ，但使用第三参数指定的值。
\pre 第二参数引用的对象是 NPLA1 上下文状态或 public 继承的派生类。
\sa ContextState::OperatorName 
\since build 883
*/
YF_API ReductionStatus
ReduceCombinedReferent(TermNode&, ContextNode&, const TermNode&);

/*!
\brief 规约列表合并项：同 ReduceCombinedReferent ，但使用第四参数指定操作符名称。
\note 若抛出异常，则先设置上下文中的规约合并项操作符名称和规约合并项引用。
\sa ContextState::OperatorName
\sa ContextState::SetCombiningTermRef
\since build 957
*/
YF_API ReductionStatus
ReduceCombinedReferentWithOperator(TermNode&, ContextNode&, const TermNode&,
	ValueObject&);
//@}
//@}
//@}

/*!
\brief 规约提取名称的叶节点记号。
\exception BadIdentifier 标识符未声明。
\note 忽略名称查找失败，默认结果为 ReductionStatus::Retained 。
\note 当抛出 BadIdentifier 异常时，无条件查询源代码信息，若存在则设置到异常对像。
\sa EvaluateLeafToken
\sa QuerySourceInformation
\sa ReduceOnce
\sa TermToNamePtr
*/
YF_API ReductionStatus
ReduceLeafToken(TermNode&, ContextNode&);
//@}


//! \since build 876
//@{
/*!
\ingroup guards
\brief 求值环境守卫。

作用域退出时调用环境切换器恢复保存的环境的作用域守卫。
*/
using EnvironmentGuard = ystdex::guard<EnvironmentSwitcher>;


/*!
\brief 创建和切换新环境并以结果初始化求值环境守卫。
\since build 945

以第一参数作为上下文，切换到参数指定的新创建的环境，并创建被切换的环境的守卫。
其余参数被作为初始化新创建环境的参数。
其余参数指定的父环境被 Environment 的构造函数检查。若不需检查（如已在之前调用
	Environment::CheckParent 或 Environment::EnsureValid ），可不使用其余参数，
	而在创建环境后设置当前环境的父环境对象。
*/
template<typename... _tParams>
YB_ATTR_nodiscard inline EnvironmentGuard
GuardFreshEnvironment(ContextNode& ctx, _tParams&&... args)
{
	return EnvironmentGuard(ctx,
		NPL::SwitchToFreshEnvironment(ctx, yforward(args)...));
}

/*!
\brief 加载代码调用。
\since build 955
*/
template<typename _fCallable, typename... _tParams>
auto
InvokeIn(ContextNode& ctx, _fCallable&& f, _tParams&&... args)
	-> decltype(ystdex::invoke(yforward(f), yforward(args)...))
{
	ValueObject parent(ctx.WeakenRecord());
	auto gd(GuardFreshEnvironment(ctx));

	AssignParent(ctx, std::move(parent));
	return ystdex::invoke(yforward(f), yforward(args)...);
}

//! \since build 942
//@{
/*!
\brief 加载代码作为模块。
\return 作为环境模块的环境对象强引用。
\post 返回值非空，指定冻结的环境。
*/
template<typename _fCallable, typename... _tParams>
shared_ptr<Environment>
GetModuleFor(ContextNode& ctx, _fCallable&& f, _tParams&&... args)
{
	ValueObject parent(ctx.WeakenRecord());
	auto gd(GuardFreshEnvironment(ctx));

	AssignParent(ctx, std::move(parent));
	ystdex::invoke(yforward(f), yforward(args)...);
	ctx.GetRecordRef().Frozen = true;
	return ctx.ShareRecord();
}

/*!
\pre 间接断言：模块名称字符串的数据指针非空。
\sa A1::GetModuleFor
*/
//@{
//! \brief 加载模块为变量，若已存在则忽略。
template<typename _fCallable, typename... _tParams>
inline void
LoadModule(ContextNode& ctx, string_view module_name, _fCallable&& f,
	_tParams&&... args)
{
	ctx.GetRecordRef().Define(module_name,
		A1::GetModuleFor(ctx, yforward(f), yforward(args)...));
}

/*!
\brief 加载模块为变量，若已存在抛出异常。
\exception BadIdentifier 变量绑定已存在。
*/
template<typename _fCallable, typename... _tParams>
inline void
LoadModuleChecked(ContextNode& ctx, string_view module_name, _fCallable&& f,
	_tParams&&... args)
{
	ctx.GetRecordRef().DefineChecked(module_name,
		A1::GetModuleFor(ctx, yforward(f), yforward(args)...));
}
//@}
//@}


/*!
\pre ContextNode& 类型的参数引用的对象是 NPLA1 上下文状态或 public 继承的派生类。
\pre TCO 实现：当前动作是 TCO 动作，且其中的当前项和被规约的项相同。
\note 参数分别表示规约上下文、被规约的项和待被保存的当前求值环境守卫。
\note 第四参数指定避免规约后提升结果。
\note 对 TCO 实现利用 TCOAction 以尾上下文进行规约。
\warning 若不满足上下文状态类型要求，行为未定义。
\since build 876
*/
//@{
/*!
\brief 设置当前项并直接求值规约。
\note 第五参数指定下一个动作的续延。
*/
YF_API ReductionStatus
RelayForEval(ContextNode&, TermNode&, EnvironmentGuard&&, bool, Continuation);

/*!
\brief 函数调用规约（ β-规约）中的函数体规约。
\pre 不存在临时函数或已通过 ReduceCombined 的内部实现调用被适当保存。
\since build 876
*/
YF_API ReductionStatus
RelayForCall(ContextNode&, TermNode&, EnvironmentGuard&&, bool);
//@}
//@}


/*!
\brief 判断项是否表示 #ignore 。
\since build 929
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsIgnore, const TermNode& nd) ynothrow
	ImplRet(HasValue(nd, ValueToken::Ignore))

/*!
\brief 修饰引用字符。
\since build 920

确保非空的没有 & 前缀的符号以一个 & 引用标记字符。
若参数具有 % 引用字符前缀，替换 % 为 & 。
*/
YB_ATTR_nodiscard YF_API YB_PURE TokenValue
Ensigil(TokenValue);

/*!
\throw InvalidSyntax 嵌套异常：项不符合语法要求。
\note 异常条件视为语法错误而非直接的类型错误。
*/
//@{
/*!
\brief 检查形式参数树。
\since build 917

递归遍历项及其子项，检查其中的子项符合对象语言 \<ptree> 形式的语法要求。
一般适用提前检查对象语言操作的 \<formals> 而不是 \<definiend> 描述的参数。
*/
YF_API void
CheckParameterTree(const TermNode&);

/*!
\brief 从项中提取环境形式参数。
\return 项表示 \c #ignore 时为空值，否则为和环境形式参数相等的字符串。
\since build 959
*/
YB_ATTR_nodiscard YF_API YB_PURE ystdex::optional<string>
ExtractEnvironmentFormal(TermNode&);
//@}

/*!
\throw InvalidSyntax 嵌套异常 ArityMismatch ：参数数匹配失败。
\throw InvalidSyntax 嵌套异常 ParameterMismatch ：参数匹配失败。
\note 不具有强异常安全保证。匹配失败时，其它的绑定状态未指定。

递归遍历参数和操作数树进行结构化匹配。
若匹配失败，则抛出异常。
*/
//@{
/*!
\brief 匹配参数。
\exception std::bad_function_call 异常中立：参数指定的处理器为空。
\throw InvalidSyntax 嵌套异常 ParameterMismatch ：形式参数项不符合语法要求。
\sa FindStickySubterm
\sa TermTags
\since build 951

进行匹配的算法递归搜索形式参数及其子项。
若匹配成功，调用参数指定的匹配处理器。
参数指定形式参数、实际参数、两个处理器、绑定选项和引用值关联的环境。
其中，形式参数被视为作为形式参数树的右值。
绑定选项以 TermTags 编码，但含义和作用在项上时不完全相同：
Unique 表示唯一引用项（在此即消亡值）；
Nonmodifying 表示需要复制；
Temporary 表示不被共享的项（在此即纯右值或没有匹配列表的引用值）。
当需要复制时，递归处理的所有对实际参数的绑定以复制代替转移；
可能被共享的项在发现表达数树中存在需被匹配的列表后失效，对之后的子项进行递归处理。
以上处理的操作数的子项仅在确定参数对应位置是列表时进行。
处理器为参数列表结尾的结尾序列处理器和值处理器：
	结尾序列处理器匹配以 . 起始的真列表的最后一项，或匹配非真列表的最后一项。
	值处理器匹配其它项。
处理器类型的参数列表中：
	TermNode& 参数指定被匹配的操作数所在的操作数项：
		结尾序列处理器中，同时使用 TNIter 参数指定操作数子项的起始位置：
			操作数项的子项的范围是起始迭代器到项的终止迭代器。
		值处理器中，所在的操作数项即为被匹配的操作数项。
	const TokenValue& 或 sting_view 参数指定匹配的名称。
	TermTags 参数指定匹配使用的标签，如决定按值传递时是否转移。
	const EnvironmentReference& 参数指定初始化被绑定的引用值时使用的环境。
值处理器中不匹配形式参数树中子项前缀的终止迭代器（即此项作为参数调用
	FindStickySubterm 的结果）之后的子项；
	序列处理器负责匹配剩余子项（若存在）和值数据成员表示的形式参数和对应操作数。
若操作数的某一个需匹配的子项通过 TermTags 参数或者 TermReference 的标签决定不可转移，
	则需要进行复制。
匹配规则如下：
	若项是非空列表，则操作数的对应的项应为满足确定子项数的列表：
		若最后的子项为 . 起始的记号，则匹配操作数中结尾的任意个数的项作为结尾序列。
		其它子项一一匹配操作数的子项。
	若项是空列表，则操作数的对应的项应为空列表。
	若项是引用值，则以表示其被引用对象的项作为子项，继续匹配一次。
	若项是 #ignore ，则忽略操作数对应的项。
	若项的值不是符号，则匹配出错。
	否则，匹配非列表项。
*/
YF_API void
MatchParameter(const TermNode&, TermNode&, function<void(TermNode&, TNIter,
	string_view, TermTags, const EnvironmentReference&)>, function<
	void(const TokenValue&, TermNode&, TermTags, const EnvironmentReference&)>,
	TermTags, const EnvironmentReference&);

//! \pre 间接断言：第三参数的标签可表示一等对象的值。
//@{
/*!
\brief 使用操作数结构化匹配并绑定参数。
\pre 间接断言：第一参数非空。
\throw ArityMismatch 子项数匹配失败。
\throw InvalidReference 非法的 @ 引用标记字符绑定。
\throw InvalidSyntax 嵌套异常 ParameterMismatch ：形式参数项不符合语法要求。
\note 第一参数指定绑定所在的环境。
\sa MatchParameter
\sa TermReference
\since build 894

形式参数和操作数为项指定的表达式树。
第二参数指定形式参数，第三参数指定操作数。
进行匹配的算法递归搜索形式参数及其子项，要求参见 MatchParameter 。
若匹配成功，在第一参数指定的环境内绑定未被忽略的匹配的非列表项。
对结尾序列总是匹配前缀为 . 的符号为目标按以下规则忽略或绑定：
子项为 . 时，对应操作数的结尾序列被忽略；
否则，绑定项的目标为移除前缀 . 和后续可选前缀 & 后的符号。
非列表项绑定规则如下：
	若被绑定的目标有引用标记字符，则以按引用传递的方式绑定，否则以按值传递的方式绑定。
	当绑定的引用标记字符为 @ 且不是列表项时抛出异常。
	按引用传递绑定直接转移该项的内容。
*/
YF_API void
BindParameter(const shared_ptr<Environment>&, const TermNode&, TermNode&);

/*!
\brief 使用操作数结构化匹配并绑定参数到合式的形式参数树。
\pre 参数指定的形式参数树确保通过检查没有语法错误。
\throw ArityMismatch 参数数匹配失败。
\throw ParameterMismatch 参数匹配失败。
\sa CheckParameterTree
\since build 917

同 BindParameter ，但假定形式参数树确保通过检查没有语法错误，否则行为未定义。
实现假定不存在形式参数树具有引起语法错误的错误条件。
通过先前在同一形式参数树上的 CheckParameterTree 调用可保证符合前置条件。
若确保绑定不具有引起对象语言中可观察行为的副作用，先前的 BindParameter
	或 BindParameterWellFormed 也可确保满足前置条件。
*/
YF_API void
BindParameterWellFormed(const shared_ptr<Environment>&, const TermNode&,
	TermNode&);
//@}
//@}

/*!
\brief 使用操作数结构化匹配并绑定符号。
\since build 920

同 BindParameter ，但形式参数指定为符号，不进行递归绑定或结尾序列匹配。
*/
YF_API void
BindSymbol(const shared_ptr<Environment>&, const TokenValue&, TermNode&);


/*!
\brief 设置参数指定的上下文为尾上下文。
\pre 断言：第二参数是规约合并项。
\note 在不支持 TCO 的实现忽略设置上下文。
\sa IsCombiningTerm
\since build 895
*/
YF_API void
SetupTailContext(ContextNode&, TermNode&);


//! \ingroup NPLDiagnostics
//@{
/*!
\pre 断言：参数的数据指针非空。
\note 不对名称参数指向的数据具有所有权。一般需要使用字符串字面量。
\since build 896
*/
//@{
/*!
\brief 添加全局类型名称表项。
\return 是否添加成功。
*/
YB_ATTR_nodiscard YF_API bool
AddTypeNameTableEntry(const type_info&, string_view);

/*!
\brief 初始化全局类型名称表项。
\pre 类型在全局类型名称表中非重复。
*/
template<typename _type>
inline void
InitializeTypeNameTableEntry(string_view desc)
{
	YAssertNonnull(desc.data());

	static struct Init final
	{
		Init(string_view sv)
		{
			const auto res(AddTypeNameTableEntry(type_id<_type>(), sv));

			yunused(res);
			YAssert(res, "Duplicated name found.");
		}
	} init(desc);
}

//! \brief 初始化作为扩展调用者的全局类型名称表项并转发参数。
template<class _tTarget, typename _func, typename _fCallable>
YB_ATTR_nodiscard YB_PURE inline _fCallable
NameExpandedHandler(_fCallable&& x, string_view desc)
{
	static_assert(std::is_constructible<_tTarget, _fCallable>(),
		"Invalid callable type found.");
	using expanded_t
		= ystdex::expanded_caller<_func, ystdex::remove_cvref_t<_fCallable>>;

	A1::InitializeTypeNameTableEntry<ystdex::cond_t<ystdex::and_<
		ystdex::not_<std::is_constructible<function<_func>, _fCallable>>,
		std::is_constructible<expanded_t, _fCallable>>,
		expanded_t, ystdex::remove_cvref_t<_fCallable>>>(desc);
	return yforward(x);
}

//! \brief 初始化作为上下文处理器的全局类型名称表项并转发参数。
template<typename _fCallable>
YB_ATTR_nodiscard YB_PURE inline _fCallable
NameTypedContextHandler(_fCallable&& x, string_view desc)
{
	return A1::NameExpandedHandler<ContextHandler,
		ContextHandler::FuncType>(yforward(x), desc);
}

//! \brief 初始化全局类型名称表项并转发参数。
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline _type
NameTypedObject(_type&& x, string_view desc)
{
	InitializeTypeNameTableEntry<ystdex::remove_cvref_t<_type>>(desc);
	return yforward(x);
}

//! \brief 初始化作为规约器处理器的全局类型名称表项并转发参数。
template<typename _fCallable>
YB_ATTR_nodiscard YB_PURE inline _fCallable
NameTypedReducerHandler(_fCallable&& x, string_view desc)
{
	return A1::NameExpandedHandler<Reducer, ReducerFunctionType>(yforward(x),
		desc);
}
//@}

/*!
\brief 查询续延中的名称信息。
\return 若存在名称则为内部指定来源的名称字符串，否则是数据指针为空的结果。
\sa InitializeTypeNameTableEntry
\sa QueryTypeName
\since build 893

取参数指定的动作对应的续延中的名称信息。
支持的目标类型包括：
特定目标的 Continuation ；
TCO 动作；
特定的其它实现续延的内部类型。
若非特定目标类型，使用全局类型名称表查询。
实现和用户操作全局类型名称表直接添加的类型信息不冲突。
*/
YB_ATTR_nodiscard YB_PURE YF_API string_view
QueryContinuationName(const Reducer&);

/*!
\brief 查询对象中的源代码信息。
\since build 891
*/
YB_ATTR_nodiscard YB_PURE YF_API observer_ptr<const SourceInformation>
QuerySourceInformation(const ValueObject&);

/*!
\brief 查询全局类型名称表。
\since build 896
*/
YB_ATTR_nodiscard YB_PURE YF_API string_view
QueryTypeName(const type_info&);

/*!
\brief 追踪记录 NPL 续延。
\sa QueryContinuationName
\sa QuerySourceInformation
\sa QueryTailOperatorName
\sa YSLib::FilterException
\see $2021-08 @ %Documentation::Workflow.
\since build 925

追踪第一参数指定的动作序列记录的 NPL 续延。
追踪每一个动作以未指定的格式使用第二参数指定的 Logger 对象打印特定的文本。
若处理任意动作时抛出异常，则追踪中止，并确保最终无异常抛出。
当前使用 YSLib::FilterException 处理追踪中止后的异常。
调用本函数以外，一般应确保之后的动作序列被清除，以确保语言规则要求的临时对象清理。
清除动作序列也确保剩余的续延中引用的子项等对象在生存期结束前被及时移除。
为符合语言规则要求，不论是否使用本函数的实现，以下约定的作用外的顺序未指定：
追踪任意动作和清除任意动作之间非决定性有序；
追踪动作先序清楚同一个动作；
追踪的动作之间的顺序同 ContextNode::ReducerSequence::clear 清除其元素的顺序。
调用本函数实现上述动作时，不修改第一参数，因此一般需要在之后显式清除动作序列。
*/
YF_API void
TraceBacktrace(const ContextNode::ReducerSequence&, YSLib::Logger&) ynothrow;
//@}


//! \since build 942
//@{
//! \brief 保持环境守卫。
template<class _tGuard>
inline ReductionStatus
KeepGuard(_tGuard&, ContextNode& ctx) ynothrow
{
	return ctx.LastStatus;
}

//! \brief 环境守卫类型。
template<class _tGuard>
using GKeptGuardAction = decltype(std::bind(KeepGuard<_tGuard>,
	std::declval<_tGuard&&>(), std::placeholders::_1));

/*!
\brief 转移保持环境守卫。
\since build 943
*/
//@{
template<class _tGuard>
YB_ATTR_nodiscard inline GKeptGuardAction<_tGuard>
MoveKeptGuard(_tGuard& gd)
{
	return A1::NameTypedReducerHandler(std::bind(KeepGuard<_tGuard>,
		std::move(gd), std::placeholders::_1), "eval-guard");
}
template<class _tGuard>
YB_ATTR_nodiscard inline GKeptGuardAction<_tGuard>
MoveKeptGuard(_tGuard&& gd)
{
	return A1::MoveKeptGuard(gd);
}
//@}
//@}


/*!
\brief 设置默认解释：解释使用的公共处理遍。
\note 非强异常安全：加入遍可能提前设置状态而不在失败时回滚。
\sa ReduceCombined
\sa ReduceFirst
\sa ReduceHeadEmptyList
\sa ReduceLeafToken
\since build 955

设置默认解释的求值遍到第一参数指定的上下文的列表求值遍和叶求值遍中。
列表求值遍等效第二参数后依次添加 ReduceFirst 、ReduceHeadEmptyList 和
	ReduceCombined ，但不保证分别作为单独的遍添加。
叶求值遍设置为 ReduceLeafToken 。
*/
YF_API void
SetupDefaultInterpretation(GlobalState&, EvaluationPasses);


/*!
\ingroup metafunctions
\since build 891
*/
//@{
//! \brief 解析结果元素类型。
template<typename _fParse>
using GParsedValue = typename ParseResultOf<_fParse>::value_type;

// XXX: Use %function instead of %ystdex::unchecked_function is no less
//	efficient.
//! \brief 泛型标记器：分析解析结果元素转换为可能包含记号的节点。
template<typename _fParse, typename... _tParams>
using GTokenizer
	= function<TermNode(const GParsedValue<_fParse>&, _tParams...)>;
//@}

/*!
\brief 标记器：分析词素转换为可能包含记号的节点。
\since build 880
*/
using Tokenizer = GTokenizer<ByteParser>;

//! \brief 标记器：分析带有源代码位置信息的词素转换为可能包含记号的节点。
using SourcedTokenizer = GTokenizer<SourcedByteParser, ContextState&>;


/*
\brief 全局状态。
\warning 非虚析构。
\warning 初始化后不检查成员使用的分配器相等性。
\since build 955

提供各个上下文共享的实现 REPL（读取-求值-输出循环）的数据和可变状态。
每个循环包括一次翻译。
这只是一个基本的可扩展实现。
部分功能可通过操作数据成员控制，但一般在初始化后不改变非可变状态，
	因此通常使用 const GlobalState& 访问，且不提供全局状态锁。
一般仅需锁定派生实现中的可能共享操可变状态。
*/
class YF_API GlobalState
{
public:
	//! \since build 891
	//@{
	//! \brief 代码加载选项。
	enum LoadOption
	{
		//! \brief 使用上下文中的状态决定是否使用源代码信息。
		Contextual,
		//! \brief 使用源代码位置。
		WithSourceLocation,
		//! \brief 不使用源代码信息。
		NoSourceInformation
	};
	//! \brief 代码加载选项标签类型。
	template<LoadOption _vOpt = Contextual>
	using LoadOptionTag = std::integral_constant<LoadOption, _vOpt>;
	//@}
	// XXX: Use %function instead of %ystdex::unchecked_function is a bit more
	//	efficient.
	/*!
	\brief 加载器。
	\since build 899

	接受上下文状态和指定名称，转换翻译单元内容为待求值节点的例程。
	*/
	using Loader = function<TermNode(ContextState&, string)>;

private:
	//! \since build 891
	struct LeafConverter final
	{
		//! \since build 955
		ContextState& Context;

		YB_ATTR_nodiscard
			PDefHOp(TermNode, (), const GParsedValue<ByteParser>& val) const
			ImplRet(Context.Global.get().ConvertLeaf(val))
		YB_ATTR_nodiscard PDefHOp(TermNode, (),
			const GParsedValue<SourcedByteParser>& val) const
			ImplRet(Context.Global.get().ConvertLeafSourced(val, Context))
	};

public:
	/*!
	\brief 节点分配器。
	\since build 845
	*/
	TermNode::allocator_type Allocator;
	//! \brief 叶遍。
	EvaluationPasses EvaluateLeaf{Allocator};
	//! \brief 列表遍。
	EvaluationPasses EvaluateList{Allocator};
	//! \brief 字面量遍。
	LiteralPasses EvaluateLiteral{Allocator};
	/*!
	\brief 预处理例程：每次翻译时预先处理调用的公共例程。
	\sa Prepare
	\since build 888

	在遍历节点时调用的预处理例程。
	只保留一个遍的处理器，以减小一般情形（包括 NPLA1 默认情形）的开销。
	若需要多个遍，可使用 TermPasses 作为目标。
	*/
	TermPasses::HandlerType Preprocess{std::allocator_arg, Allocator};
	//! \since build 891
	//@{
	//! \invariant 值被调用时若返回，能被视为是纯函数。
	//@{
	/*!
	\brief 叶节点词素转换器。
	\sa ParseLeaf
	*/
	Tokenizer ConvertLeaf;
	/*!
	\brief 带有源代码信息的叶节点词素转换器。
	\sa ParseLeafWithSourceInformation
	*/
	SourcedTokenizer ConvertLeafSourced;
	//@}
	/*!
	\brief 默认启用源代码位置。
	\sa Perform
	*/
	bool UseSourceLocation = {};
	//@}
	/*!
	\brief 加载例程。
	\since build 899

	指定字符串参数作为加载对象描述，结合自身状态加载输入为待求值的项。
	加载翻译单元时，加载例程被同步地调用。
	*/
	Loader Load{DefaultLoad};
	/*!
	\brief 默认输出流指针。
	\since build 901
	*/
	observer_ptr<std::ostream> OutputStreamPtr{};

	/*!
	\sa ListTermPreprocess
	\sa SetupDefaultInterpretation
	\since build 566
	*/
	//@{
	//! \sa ParseLeaf
	//@{
	//! \brief 构造：使用默认解释、指定的分配器和默认的叶节点词素转换器。
	GlobalState(TermNode::allocator_type a = {});
	//! \brief 构造：使用默认解释、指定的存储资源和默认的叶节点词素转换器。
	GlobalState(pmr::memory_resource& rsrc)
		: GlobalState(TermNode::allocator_type(&rsrc))
	{}
	//@}
	//! \brief 构造：使用默认解释、指定的分配器和叶节点词素转换器。
	GlobalState(Tokenizer, SourcedTokenizer, TermNode::allocator_type a = {});
	//! \brief 构造：使用默认解释、指定的存储资源和叶节点词素转换器。
	GlobalState(Tokenizer leaf_conv,
		SourcedTokenizer sourced_leaf_conv, pmr::memory_resource& rsrc)
		: GlobalState(std::move(leaf_conv), std::move(sourced_leaf_conv),
		TermNode::allocator_type(&rsrc))
	{}
	//@}

	/*!
	\brief 判断当前实现是否为异步实现。
	\since build 879

	判断当前实现是否支持在本机实现中使用异步调用。
	若不支持同步调用，则只应使用同步调用的本机实现。
	*/
	YB_STATELESS bool
	IsAsynchronous() const ynothrow;

	/*!
	\brief 取输出流引用。
	\throw ystdex::unsupported 不支持的输出流：流指针为空。
	\sa OutputStreamPtr
	\since build 901
	*/
	YB_ATTR_nodiscard YB_PURE std::ostream&
	GetOutputStreamRef() const;

	/*!
	\brief 默认加载。

	以参数作为文件名读取内容并转换。
	不处理文件名。
	若被平台支持，使用相对路径文件名指定的文件位置可能和当前工作目录相关。
	*/
	YB_ATTR_nodiscard static TermNode
	DefaultLoad(ContextState&, string);

	/*!
	\brief 执行：从指定参数指定的来源读取并翻译源代码，并返回处理结果。
	\note 不使用 YB_ATTR_nodiscard 。
	\exception std::invalid_argument 异常中立：由 ReadFrom 抛出。
	\sa ReadFrom
	\sa Reduce
	*/
	//@{
	/*!
	\note 使用默认预处理例程。
	\sa Preprocess
	*/
	template<typename... _tParams>
	inline TermNode
	Perform(ContextState& cs, _tParams&&... args) const
	{
		return Perform(ystdex::ref(Preprocess), cs, yforward(args)...);
	}
	/*!
	\note 使用第一参数指定的预处理例程。
	\since build 958
	*/
	template<typename _func, typename... _tParams>
	inline auto
	Perform(_func preprocess, ContextState& cs, _tParams&&... args) const
		-> yimpl(ystdex::well_formed_t)<TermNode,
		decltype(preprocess(std::declval<TermNode&>()))>
	{
		auto term(ReadFrom(yforward(args)..., cs));

		preprocess(term);
		Reduce(term, cs);
		return term;
	}
	//@}

	/*!
	\brief 准备规约项：分析输入并标记记号节点。
	\return 从参数输入读取的准备的项。
	\sa SContext::Analyze

	按需分析。
	词素的处理由分析完成，不需单独调用 TokenizeTerm 转换。
	之后可能需另行调用预处理例程。
	*/
	template<typename... _tParams>
	YB_ATTR_nodiscard TermNode
	Prepare(ContextState& cs, _tParams&&... args) const
	{
		return SContext::Analyze(std::allocator_arg, Allocator,
			LeafConverter{cs}, yforward(args)...);
	}

	/*!
	\brief 读取：从指定参数指定的来源输入源代码并准备规约项。
	\return 从参数输入读取的准备的项。
	\sa Prepare
	\since build 955
	*/
	//@{
	template<class _type>
	YB_ATTR_nodiscard inline TermNode
	ReadFrom(_type&& input, ContextState& cs) const
	{
		return ReadFrom(LoadOptionTag<>(), yforward(input), cs);
	}
	template<class _type>
	YB_ATTR_nodiscard inline TermNode
	ReadFrom(_type&& input, ReaderState& rs, ContextState& cs) const
	{
		return ReadFrom(LoadOptionTag<>(), yforward(input), rs, cs);
	}
	/*!
	\throw std::invalid_argument 流状态错误或缓冲区不存在。
	\since build 899
	*/
	template<LoadOption _vOpt, typename... _tParams>
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<_vOpt> opt, std::istream& is, _tParams&&... args)
		const
	{
		if(is)
		{
			if(const auto p = is.rdbuf())
				return ReadFrom(opt, *p, yforward(args)...);
			throw std::invalid_argument("Invalid stream buffer found.");
		}
		else
			throw std::invalid_argument("Invalid stream found.");
	}
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<>, std::streambuf&, ContextState&) const;
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<>, std::streambuf&, ReaderState&, ContextState&)
		const;
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<WithSourceLocation>, std::streambuf&, ContextState&)
		const;
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<WithSourceLocation>, std::streambuf&, ReaderState&,
		ContextState&) const;
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<NoSourceInformation>, std::streambuf&, ContextState&)
		const;
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<NoSourceInformation>, std::streambuf&, ReaderState&,
		ContextState&) const;
	//! \pre 断言：字符串的数据指针非空。
	//@{
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<>, string_view, ContextState&) const;
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<WithSourceLocation>, string_view, ContextState&)
		const;
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<NoSourceInformation>, string_view, ContextState&)
		const;
	//@}
	//@}
};

//! \since build 955
template<typename... _tParams>
// XXX: No %YB_ATTR_nodiscard.
inline TermNode
Perform(ContextState& cs, _tParams&&... args)
{
	return cs.Global.get().Perform(cs, yforward(args)...);
}

/*!
\brief 尝试加载源代码。
\exception NPLException 嵌套异常：加载失败。
\note 第二参数表示来源，仅用于诊断消息。
\note 不使用可能自定义的 GlobalState::Load 。
\relates GlobalState
\sa GlobalState::LoadFrom
\since build 955
*/
template<typename... _tParams>
YB_NONNULL(2) void
TryLoadSource(ContextState& cs, const char* name, _tParams&&... args)
{
	TryExpr(A1::Perform(cs, yforward(args)...))
	CatchExpr(..., std::throw_with_nested(NPLException(
		ystdex::sfmt("Failed loading external unit '%s'.", name))));
}

} // namesapce A1;

} // namespace NPL;

//! \since build 926
namespace ystdex
{

//! \relates NPL::A1::Continuation
template<>
struct is_bitwise_swappable<NPL::A1::Continuation> : true_
{};

//! \since build 927
//@{
//! \relates NPL::A1::WrappedContextHandler
template<typename _func>
struct is_bitwise_swappable<NPL::A1::WrappedContextHandler<_func>> : true_
{};

//! \relates NPL::A1::FormContextHandler
template<>
struct is_bitwise_swappable<NPL::A1::FormContextHandler> : true_
{};
//@}

} // namespace ystdex;

#endif

