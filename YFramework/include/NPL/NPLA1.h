/*
	© 2014-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.h
\ingroup NPL
\brief NPLA1 公共接口。
\version r5523
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 17:58:24 +0800
\par 修改时间:
	2019-09-28 22:50 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#ifndef NPL_INC_NPLA1_h_
#define NPL_INC_NPLA1_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA // for NPLATag, TermNode, ValueNode, YSLib::GHEvent,
//	ystdex::ref_eq, LoggedEvent, shared_ptr, ystdex::equality_comparable,
//	ystdex::exclude_self_params_t, YSLib::AreEqualHeld,
//	ystdex::make_parameter_list_t, ystdex::make_function_type_t,
//	ystdex::decay_t, ystdex::expanded_caller, std::is_constructible,
//	ystdex::or_, ystdex::exclude_self_t, ystdex::enable_if_inconvertible_t, pmr,
//	NPL::Deref, ystdex::make_expanded, std::ref, NPL::Access, NPL::CheckRegular,
//	ResolvedTermReferencePtr, ystdex::invoke_nonvoid, NPL::ResolveRegular,
//	ystdex::make_transform, std::accumulate, ystdex::bind1,
//	std::placeholders::_2, ystdex::examiners::equal_examiner, Environment;
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;

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


//! \since build 676
//@{
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
	EvaluationPasses EvaluateLeaf{};
	EvaluationPasses EvaluateList{};
	LiteralPasses EvaluateLiteral{};
	GuardPasses Guard{};

private:
	/*!
	\brief 待求值的下一项的指针。
	\note 可被续延访问。
	*/
	observer_ptr<TermNode> next_term_ptr{};

public:
	/*!
	\brief 构造：使用指定的存储资源。
	\since build 845
	*/
	ContextState(pmr::memory_resource&);
	ContextState(const ContextState&);
	ContextState(ContextState&&);
	//! \brief 虚析构：类定义外默认实现。
	~ContextState() override;

	DefDeCopyMoveAssignment(ContextState)

	/*!
	\brief 访问实例。
	\pre 参数指定对象是 NPLA1 上下文状态或 public 继承的派生类。
	\warning 若不满足要求，行为未定义。
	*/
	//@{
	static PDefH(ContextState&, Access, ContextNode& ctx) ynothrowv
		ImplRet(ystdex::polymorphic_downcast<ContextState&>(ctx))
	static PDefH(const ContextState&, Access, const ContextNode& ctx) ynothrowv
		ImplRet(ystdex::polymorphic_downcast<const ContextState&>(ctx))
	//@}

	/*!
	\brief 取下一求值项的引用。
	\throw NPLException 下一求值项的指针为空。
	\sa next_term_ptr
	*/
	YB_ATTR_nodiscard YB_PURE TermNode&
	GetNextTermRef() const;

 	/*!
	\brief 设置下一求值项的引用。
	\throw NPLException 下一项指针为空。
	\sa next_term_ptr
	*/
	void
	SetNextTermRef(TermNode&);

	//! \brief 清除下一项指针。
	PDefH(void, ClearNextTerm, ) ynothrow
		ImplExpr(next_term_ptr = {})

	/*!
	\brief 构造作用域守卫并重写项。
	\sa Guard
	\sa ContextNode::Rewrite

	重写逻辑包括以下顺序的步骤：
	调用 Guard 进行必要的上下文重置；
	调用 ContextNode::Rewrite 。
	*/
	ReductionStatus
	RewriteGuarded(TermNode&, Reducer);

	friend PDefH(void, swap, ContextState& x, ContextState& y) ynothrow
		ImplExpr(swap(static_cast<ContextNode&>(x),
			static_cast<ContextNode&>(y)), swap(x.next_term_ptr,
			y.next_term_ptr), swap(x.EvaluateLeaf, y.EvaluateLeaf),
			swap(x.EvaluateList, y.EvaluateList),
			swap(x.EvaluateLiteral, y.EvaluateLiteral), swap(x.Guard, y.Guard))
};


/*!
\brief 续延。
\warning 非虚析构。
\since build 841
\todo 支持一等续延捕获。
*/
class YF_API Continuation
{
public:
	ContextHandler Handler;
	// \since build 842
	lref<ContextNode> Context;

	//! \since build 842
	template<typename _func>
	Continuation(_func&& handler, ContextNode& ctx)
		: Handler(ystdex::make_obj_using_allocator<ContextHandler>(
		ctx.get_allocator(), yforward(handler))), Context(ctx)
	{}

	DefDeCopyMoveCtorAssignment(Continuation)

	friend PDefHOp(bool, ==, const Continuation& x, const Continuation& y)
		ynothrow
		ImplRet(ystdex::ref_eq<>()(x, y))

	PDefHOp(ReductionStatus, (), ) const
		ImplRet(Handler(ContextState::Access(Context).GetNextTermRef(),
			Context))

	friend DefSwap(ynothrow, Continuation, swap(_x.Handler, _y.Handler),
		ystdex::swap_dependent(_x.Context, _y.Context))
};


/*!
\brief NPLA1 表达式节点规约：调用至少一次求值例程规约子表达式。
\return 规约状态。
\sa ContextState::RewriteGuarded
\sa ReduceOnce
\since build 730

以第一参数为项，以 ReduceOnce 为规约函数调用 ContextState::RewriteGuarded 。
*/
YF_API ReductionStatus
Reduce(TermNode&, ContextNode&);

/*!
\brief 再次规约。
\return ReductionStatus::Partial 。
\sa ReduceOnce
\sa RelayNext
\sa RelaySwitchedUnchecked
\since build 807

确保再次 Reduce 调用并返回要求重规约的结果。
*/
YF_API ReductionStatus
ReduceAgain(TermNode&, ContextNode&);

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
inline PDefH(void, ReduceArguments, TermNode& term, ContextNode& ctx)
	ImplRet(ReduceArguments(term.begin(), term.end(), ctx))
//@}

//! \note 失败视为重规约。
//@{
/*!
\brief 规约并检查成功：等效调用 Reduce 并检查结果直至不需重规约。
\note 支持尾调用优化，不直接使用 CheckedReduceWith 和 Reduce 。
\return 若使用异步调用 ReductionStatus::Partial ，
	否则为 ReductionStatus::Regular 。
\sa CheckedReduceWith
\sa Reduce
\since build 817
*/
YF_API ReductionStatus
ReduceChecked(TermNode&, ContextNode&);
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
\return 当存在子项时为最后一个子项的规约状态，否则为 ReductionStatus::Clean 。
\sa ReduceChecked
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
\sa ReduceOnce
\see https://en.wikipedia.org/wiki/Fexpr 。
\since build 730

快速严格性分析：
当节点为分支列表的节点时，无条件求值第一项以避免非确定性推断子表达式求值的附加复杂度；
否则，返回 ReductionStatus::Retained 。
调用 ReduceOnce 规约子项。
*/
YF_API ReductionStatus
ReduceFirst(TermNode&, ContextNode&);

/*!
\brief NPLA1 表达式节点规约：调用求值例程规约子表达式。
\return 规约状态。
\note 异常安全为调用遍的最低异常安全保证。
\note 可能使参数中容器的迭代器失效。
\note 默认不需要重规约。这可被求值遍改变。
\note 可被求值遍调用以实现递归求值。
\sa ContextNode::EvaluateLeaf
\sa ContextNode::EvaluateList
\sa ValueToken
\sa ReduceAgain
\since build 806
\todo 实现 ValueToken 保留处理。

对应不同的节点次级结构分类，一次迭代按以下顺序选择以下分支之一，按需规约子项：
对枝节点调用 ContextNode::EvaluateList 求值；
对空节点或值为 ValueToken 的叶节点不进行操作；
对其它叶节点调用 ContextNode::EvaluateList 求值。
迭代结束调用 CheckReducible ，根据结果判断是否进行重规约。
此处约定的迭代中对节点的具体结构分类默认也适用于其它 NPLA1 实现 API ；
例外情况应单独指定明确的顺序。
例外情况包括输入节点不是表达式语义结构（而是 AST ）的 API ，如 TransformNode 。
当前实现返回的规约状态总是 ReductionStatus::Clean ，否则会循环迭代。
若需要保证无异常时仅在规约成功后终止，使用 ReduceChecked 代替。
*/
YF_API ReductionStatus
ReduceOnce(TermNode&, ContextNode&);

/*!
\brief 规约有序序列：顺序规约子项，结果为最后一个子项的规约结果。
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
\sa ReduceAgain
\since build 733
*/
YF_API ReductionStatus
ReduceTail(TermNode&, ContextNode&, TNIter);
//@}


/*!
\brief 设置跟踪深度节点：调用规约时显示深度和上下文等信息。
\note 主要用于调试。
\sa ContextState::Guard
\since build 842
*/
YF_API void
SetupTraceDepth(ContextState&, const string& = yimpl("$__depth"));


/*!
\brief 插入 NPLA1 子节点。
\note 插入后按名称排序顺序。
\since build 852

第一参数指定的变换结果插入第二参数指定的容器。
若映射操作返回节点名称为空则根据当前容器内子节点数量加前缀 $ 命名以避免重复。
*/
YF_API void
InsertChild(ValueNode&&, ValueNode::Container&);

/*!
\brief 变换 NPLA1 节点 S 表达式抽象语法树为 NPLA1 语义结构。
\exception std::bad_function_call 第三至五参数为空。
\return 变换后的新节点（及子节点）。
\since build 852

第一参数指定源节点，其余参数指定部分变换规则。
当被调用的第二至第四参数不修改传入的节点参数时变换不修改源节点。
过程如下：
若源节点为叶节点，直接返回使用第三参数创建映射的节点。
若源节点只有一个子节点，直接返回这个子节点的变换结果。
否则，使用第四参数从第一个子节点取作为变换结果名称的字符串。
	若名称非空则忽略第一个子节点，只变换剩余子节点。
		当剩余一个子节点（即源节点有两个子节点）时，直接递归变换这个节点并返回。
		若变换后的结果名称非空，则作为结果的值；否则，结果作为容器内单一的值。
	否则，新建节点容器，遍历并变换剩余的节点插入其中，返回以之构造的节点。
		第二参数指定此时的映射操作，若为空则默认使用递归 TransformNode 调用。
		调用第五参数插入映射的结果到容器。
*/
YF_API ValueNode
TransformNode(const TermNode&, NodeMapper = {}, NodeMapper = MapNPLALeafNode,
	TermNodeToString = ParseNPLATermString, NodeInserter = InsertChild);

/*!
\brief 加载 NPLA1 翻译单元。
\throw LoggedEvent 警告：被加载配置中的实体转换失败。
\since build 665
*/
template<typename _type, typename... _tParams>
YB_ATTR_nodiscard ValueNode
LoadNode(_type&& tree, _tParams&&... args)
{
	TryRet(A1::TransformNode(std::forward<TermNode>(tree), yforward(args)...))
	CatchThrow(bad_any_cast& e, LoggedEvent(YSLib::sfmt(
		"Bad NPLA1 tree found: cast failed from [%s] to [%s] .", e.from(),
		e.to()), YSLib::Warning))
}


/*!
\note ValueObject 参数分别指定替换添加的前缀和被替换的分隔符的值。
\since build 852
*/
//@{
/*!
\brief 变换分隔符中缀表达式为前缀表达式。

移除子项中值和指定分隔符指定的项，并添加指定前缀值作为子项。
被添加的子项若是只有一个子项的列表项，该项被提升直接加入转换后的项作为子项。
最后一个参数指定返回值的名称。
*/
//@{
//! \note 非递归变换。
//@{
YB_ATTR_nodiscard YF_API YB_PURE TermNode
TransformForSeparator(const TermNode&, const ValueObject&, const TokenValue&);
YB_ATTR_nodiscard YF_API YB_PURE TermNode
TransformForSeparator(TermNode&&, const ValueObject&, const TokenValue&);
//@}

//! \note 递归变换。
//@{
YB_ATTR_nodiscard YF_API YB_PURE TermNode
TransformForSeparatorRecursive(const TermNode&, const ValueObject&,
	const TokenValue&);
//! \since build 824
YB_ATTR_nodiscard YF_API YB_PURE TermNode
TransformForSeparatorRecursive(TermNode&&, const ValueObject&,
	const TokenValue&);
//@}
//@}

/*!
\brief 查找项中的指定分隔符，若找到则替换项为去除分隔符并添加替换前缀的形式。
\return 是否找到并替换了项。
\note 子项的内容在替换时被转移。
\sa EvaluationPasses
\sa TransformForSeparator
*/
YF_API ReductionStatus
ReplaceSeparatedChildren(TermNode&, const ValueObject&, const TokenValue&);
//@}


//! \since build 751
//@{
/*!
\brief 包装上下文处理器。
\note 忽略被包装的上下文处理器可能存在的返回值，自适应默认返回规约结果。
\warning 非虚析构。
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
	YB_ATTR_nodiscard friend YB_PURE PDefHOp(bool, ==,
		const WrappedContextHandler& x, const WrappedContextHandler& y)
		ImplRet(YSLib::AreEqualHeld(x.Handler, y.Handler))

	//! \since build 834
	friend
		DefSwap(ynothrow, WrappedContextHandler, swap(_x.Handler, _y.Handler))
};

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
	using BaseType = typename _tDst::BaseType;

	// XXX: It is a hack to adjust the convertible result for the expanded
	//	caller here. It should have been implemented in %GHEvent, however types
	//	those cannot convert to expanded caller cannot be SFINAE'd out,
	//	otherwise it would cause G++ 5.4 crash with internal compiler error:
	//	"error reporting routines re-entered".
	return A1::WrapContextHandler<_tDst>(yforward(h), ystdex::or_<
		std::is_constructible<BaseType, _func>,
		std::is_constructible<BaseType, ystdex::expanded_caller<
		typename _tDst::FuncType, ystdex::decay_t<_func>>>>());
}
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
public:
	ContextHandler Handler;
	/*!
	\brief 项检查例程：验证被包装的处理器的调用符合前置条件。
	\since build 851
	*/
	function<bool(const TermNode&)> Check{IsBranchedList};
	//! \since build 859
	//@{
	/*!
	\brief 包装数。
	\note 决定调用前需要对操作数进行求值的次数。
	*/
	size_t Wrapping;

	//! \since build 697
	template<typename _func,
		yimpl(typename = ystdex::exclude_self_t<FormContextHandler, _func>)>
	FormContextHandler(_func&& f, size_t n = 0)
		: Handler(A1::WrapContextHandler<ContextHandler>(yforward(f))),
		Wrapping(n)
	{}
	template<typename _func, typename _fCheck,
		yimpl(typename = ystdex::enable_if_inconvertible_t<_fCheck, size_t>)>
	FormContextHandler(_func&& f, _fCheck c, size_t n = 0)
		: Handler(A1::WrapContextHandler<ContextHandler>(yforward(f))),
		Check(c), Wrapping(n)
	{}
	//@}
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
	\return Handler 调用的返回值，或 ReductionStatus::Clean 。
	\throw std::invalid_argument 项检查未通过。
	\warning 要求异步实现中对 Handler 调用时保证此对象生存期，否则行为未定义。
	\sa ReduceArguments
	\sa Wrapping
	\since build 751

	对每一个子项求值，重复 Wrapping 次；
	然后检查项，对可调用的项调用 Hanlder ，否则抛出异常。
	项检查不存在或在检查通过后，变换无参数规约，然后对节点调用 Hanlder ，
		否则抛出异常。
	无参数时第一参数具有两个子项且第二项为空节点。无参数变换删除空节点。
	*/
	PDefHOp(ReductionStatus, (), TermNode& term, ContextNode& ctx) const
		ImplRet(CallN(Wrapping, term, ctx))

private:
	//! \since build 859
	ReductionStatus
	CallN(size_t, TermNode&, ContextNode&) const;

	//! \since build 859
	YB_ATTR_nodiscard YB_PURE bool
	Equals(const FormContextHandler&) const;

public:
	//! \since build 834
	friend DefSwap(ynothrow, FormContextHandler, (swap(_x.Handler, _y.Handler),
		std::swap(_x.Wrapping, _y.Wrapping)))
};


/*!
\pre 间接断言：第二参数的数据指针非空。
\since build 838
*/
//@{
//! \brief 注册一般形式上下文处理器。
template<class _tTarget, typename... _tParams>
inline void
RegisterForm(_tTarget& target, string_view name, _tParams&&... args)
{
	NPL::EmplaceLeaf<ContextHandler>(target, name,
		FormContextHandler(yforward(args)...));
}

//! \brief 注册严格上下文处理器。
template<class _tTarget, typename... _tParams>
inline void
RegisterStrict(_tTarget& target, string_view name, _tParams&&... args)
{
	NPL::EmplaceLeaf<ContextHandler>(target, name,
		FormContextHandler(yforward(args)..., size_t(1)));
}
//@}

/*!
\brief 注册分隔符转换变换和处理例程。
\sa NPL::RegisterContextHandler
\sa ReduceChildren
\sa ReduceOrdered
\sa ReplaceSeparatedChildren
\since build 847

变换带有中缀形式的分隔符记号的表达式为指定前缀对象并去除分隔符。
最后一个参数指定是否有序，选择语法形式为 ReduceOrdered 或 ReduceChildren 之一。
前缀名称不需要是记号支持的标识符。
*/
YF_API void
RegisterSequenceContextTransformer(EvaluationPasses&, const TokenValue&,
	bool = {});


/*!
\brief 取项的参数个数：子项数减 1 。
\pre 间接断言：参数指定的项是枝节点。
\return 项的参数个数。
\since build 733
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(size_t, FetchArgumentN, const TermNode& term) ynothrowv
	ImplRet(AssertBranch(term), term.size() - 1)


//! \note 第一参数指定输入的项，其 Value 指定输出的值。
//@{
/*!
\exception BadIdentifier 未在环境中找到指定标识符的绑定。
\note 默认结果为 ReductionStatus::Clean 以保证强规范化性质。
*/
//@{
/*!
\brief 求值标识符。
\pre 间接断言：第三参数的数据指针非空。
\note 不验证标识符是否为字面量；仅以字面量处理时可能需要重规约。
\sa LiteralHandler
\sa ReferenceTerm
\sa ResolveName
\sa TermReference
\since build 745

依次进行以下求值操作：
调用 ResolveName 根据指定名称查找值，若失败抛出未声明异常；
初始化值，进行引用折叠后重新引用为左值；
以 LiteralHandler 访问字面量处理器，若成功调用并返回字面量处理器的处理结果。
若未返回，根据节点表示的值进一步处理：
	对表示 TokenValue 值的叶节点，返回 ReductionStatus::Clean ；
	对枝节点视为列表，返回 ReductionStatus::Retained 。
初始化值的目标是第一参数的 Value 数据成员，包括以下操作：
	调用 ReferenceTerm 确保进一步操作解析得到的（保存在环境中的）项对象不是引用；
	若上一步得到的值是空列表或 TokenValue ，直接复制；
	否则，目标初始化为引用上一步得到的值的新创建 TermReference 值。
引用折叠通过调用 ReferenceTerm 再初始化 TermReference 实现，确保不构造引用的引用。
*/
YF_API ReductionStatus
EvaluateIdentifier(TermNode&, const ContextNode&, string_view);

/*!
\brief 求值叶节点记号。
\pre 断言：第三参数的数据指针非空。
\sa CategorizeLexeme
\sa ContextNode::EvaluateLiteral
\sa DeliteralizeUnchecked
\sa EvaluateIdentifier
\since build 736

处理非空字符串表示的节点记号。
依次进行以下求值操作。
对代码字面量，去除字面量边界分隔符后进一步求值。
对数据字面量，去除字面量边界分隔符作为字符串值。
对其它字面量，通过调用字面量遍处理。
最后求值非字面量的标识符。
*/
YF_API ReductionStatus
EvaluateLeafToken(TermNode&, ContextNode&, string_view);
//@}

/*!
\brief 规约合并项：检查项的第一个子项尝试作为操作符进行函数应用，并规范化。
\pre 断言：若第一个子项表示子对象引用，则符合子对象引用的非正规表示约定。
\return 规约状态。
\exception bad_any_cast 异常中立：子对象引用持有的值不是 ContextHandler 类型。
\throw ListReductionFailure 规约失败：枝节点的第一个子项不表示上下文处理器。
\sa ContextHandler
\sa Reduce
\since build 766

对枝节点尝试以第一个子项的 Value 数据成员为上下文处理器并调用，且当规约终止时规范化；
否则视为规约成功，没有其它作用。
若发生 ContextHandler 调用，调用前先转移处理器保证生存期，
	以允许处理器内部移除或修改之前占用的第一个子项（包括其中的 Value 数据成员）。
*/
YF_API ReductionStatus
ReduceCombined(TermNode&, ContextNode&);

/*!
\brief 规约提取名称的叶节点记号。
\exception BadIdentifier 标识符未声明。
\note 忽略名称查找失败，默认结果为 ReductionStatus::Clean 以保证强规范化性质。
\sa EvaluateLeafToken
\sa ReduceAgain
\sa TermToNamePtr
*/
YF_API ReductionStatus
ReduceLeafToken(TermNode&, ContextNode&);
//@}


/*!
\brief 设置默认解释：解释使用的公共处理遍。
\note 非强异常安全：加入遍可能提前设置状态而不在失败时回滚。
\sa ReduceCombined
\sa ReduceFirst
\sa ReduceHeadEmptyList
\sa ReduceLeafToken
\since build 842
*/
YF_API void
SetupDefaultInterpretation(ContextState&, EvaluationPasses);


/*
\brief REPL 上下文。
\warning 非虚析构。
\since build 740

REPL 表示读取-求值-输出循环。
每个循环包括一次翻译。
这只是一个基本的可扩展实现。功能通过操作数据成员控制。
*/
class YF_API REPLContext
{
public:
	/*!
	\brief 节点分配器。
	\since build 845
	*/
	TermNode::allocator_type Allocator;
	/*!
	\brief 上下文根节点。
	\since build 842
	*/
	ContextState Root;
	//! \brief 预处理节点：每次翻译时预先处理调用的公共例程。
	TermPasses Preprocess{Allocator};
	//! \brief 列表项处理例程：每次翻译中规约回调处理调用的公共例程。
	EvaluationPasses ListTermPreprocess{Allocator};

	/*!
	\brief 构造：使用默认的解释和指定的存储资源。
	\note 参数指定是否启用对规约深度进行跟踪。
	\sa ListTermPreprocess
	\sa SetupDefaultInterpretation
	\sa SetupTraceDepth
	\since build 845
	*/
	REPLContext(bool = {}, pmr::memory_resource&
		= NPL::Deref(pmr::new_delete_resource()));

	/*!
	\brief 加载：从指定参数指定的来源读取并处理源代码。
	\exception std::invalid_argument 异常中立：由 ReadFrom 抛出。
	\sa ReadFrom
	\sa ReduceAndFilter
	\since build 802
	*/
	//@{
	template<class _type>
	void
	LoadFrom(_type& input)
	{
		LoadFrom(input, Root);
	}
	template<class _type>
	void
	LoadFrom(_type& input, ContextNode& ctx) const
	{
		auto term(ReadFrom(input));

		ReduceAndFilter(term, ctx);
	}
	//@}

	/*!
	\brief 执行循环：对非空输入进行翻译。
	\pre 断言：字符串的数据指针非空。
	\throw LoggedEvent 输入为空串。
	\sa Process
	*/
	//@{
	PDefH(TermNode, Perform, string_view unit)
		ImplRet(Perform(unit, Root))
	TermNode
	Perform(string_view, ContextNode&);
	//@}

	/*!
	\brief 准备规约项：分析输入并标记记号节点和预处理。
	\sa Preprocess
	\sa TokenizeTerm
	\since build 802
	*/
	//@{
	void
	Prepare(TermNode&) const;
	/*!
	\return 从参数输入读取的准备的项。
	\sa SContext::Analyze
	*/
	//@{
	YB_ATTR_nodiscard TermNode
	Prepare(const TokenList&) const;
	YB_ATTR_nodiscard TermNode
	Prepare(const Session&) const;
	//@}
	//@}

	/*!
	\brief 处理：准备规约项并进行规约。
	\sa Prepare
	\sa ReduceAndFilter
	\since build 742
	*/
	//@{
	PDefH(void, Process, TermNode& term)
		ImplExpr(Process(term, Root))
	//! \since build 802
	//@{
	void
	Process(TermNode&, ContextNode&) const;
	template<class _type>
	YB_ATTR_nodiscard TermNode
	Process(const _type& input)
	{
		return Process(input, Root);
	}
	template<class _type>
	YB_ATTR_nodiscard TermNode
	Process(const _type& input, ContextNode& ctx) const
	{
		auto term(Prepare(input));

		ReduceAndFilter(term, ctx);
		return term;
	}
	//@}
	//@}

	/*!
	\brief 读取：从指定参数指定的来源输入源代码并准备规约项。
	\return 从参数输入读取的准备的项。
	\sa Prepare
	\since build 802
	*/
	//@{
	//! \throw std::invalid_argument 流状态错误或缓冲区不存在。
	YB_ATTR_nodiscard TermNode
	ReadFrom(std::istream&) const;
	YB_ATTR_nodiscard TermNode
	ReadFrom(std::streambuf&) const;
	//@}

	/*!
	\brief 规约入口：规约并过滤异常。
	\exception NPLException 异常中立。
	\throw LoggedEvent 警告：类型不匹配，由 Reduce 抛出的 bad_any_cast 转换。
	\throw LoggedEvent 错误：由 Reduce 抛出的 bad_any_cast 外的
		std::exception 转换。
	\sa Reduce
	\since build 854
	*/
	static ReductionStatus
	ReduceAndFilter(TermNode&, ContextNode&);
};

/*!
\brief 尝试加载源代码。
\exception NPLException 嵌套异常：加载失败。
\note 第二参数表示来源，仅用于诊断消息。
\relates REPLContext
\since build 838
*/
template<typename... _tParams>
YB_NONNULL(2) void
TryLoadSource(REPLContext& context, const char* name, _tParams&&... args)
{
	TryExpr(context.LoadFrom(yforward(args)...))
	CatchExpr(..., std::throw_with_nested(NPLException(
		ystdex::sfmt("Failed loading external unit '%s'.", name))));
}


/*!
\brief NPLA1 语法形式对应的功能实现。
\pre 除非另行指定支持保存当前动作，若存在子项，关联的上下文中的尾动作为空。
\note 提供的操作用于实现操作子或应用子底层的操作子。
\since build 732
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
YB_ATTR_nodiscard YF_API YB_PURE const string&
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
//! \brief 保留项：保留求值。
inline PDefH(ReductionStatus, Retain, const TermNode& term) ynothrowv
	ImplRet(AssertBranch(term), ReductionStatus::Retained)

/*!
\brief 保留经检查确保具有指定个数参数的项：保留求值。
\return 项的参数个数。
\throw ArityMismatch 项的参数个数不等于第二参数。
\sa FetchArgumentN
*/
YF_API size_t
RetainN(const TermNode&, size_t = 1);
//@}


//! \throw ParameterMismatch 匹配失败。
//@{
/*!
\pre 断言：字符串参数的数据指针非空。
\since build 794
*/
//@{
//! \brief 检查记号值是符合匹配条件的符号。
template<typename _func>
auto
CheckSymbol(string_view n, _func f) -> decltype(f())
{
	if(IsNPLASymbol(n))
		return f();
	throw ParameterMismatch(ystdex::sfmt(
		"Invalid token '%s' found for symbol parameter.", n.data()));
}

//! \brief 检查记号值是符合匹配条件的参数符号。
template<typename _func>
auto
CheckParameterLeafToken(string_view n, _func f) -> decltype(f())
{
	if(n != "#ignore")
		CheckSymbol(n, f);
}
//@}

/*!
\note 不具有强异常安全保证。匹配失败时，其它的绑定状态未指定。

递归遍历参数和操作数树进行结构化匹配。
若匹配失败，则抛出异常。
*/
//@{
/*!
\brief 使用操作数结构化匹配并绑定参数。
\throw ArityMismatch 子项数匹配失败。
\throw InvalidReference 非法的 @ 引用标记字符绑定。
\note 第一参数指定绑定所在的环境。
\sa MatchParameter
\sa TermReference
\since build 868

形式参数和操作数为项指定的表达式树。
第二参数指定形式参数，第三参数指定操作数。
进行匹配的算法递归搜索形式参数及其子项，要求参见 MatchParameter 。
若匹配成功，在第一参数指定的环境内绑定未被忽略的匹配的项。
对结尾序列总是匹配前缀为 . 的符号为目标按以下规则忽略或绑定：
子项为 . 时，对应操作数的结尾序列被忽略；
否则，绑定项的目标为移除前缀 . 和后续可选前缀 & 后的符号。
非列表项的绑定使用以下规则：
若匹配成功，在第一参数指定的环境内绑定未被忽略的匹配的非列表项。
匹配要求如下：
若项是 #ignore ，则忽略操作数对应的项；
若项的值是符号，则操作数的对应的项应为非列表项。
若被绑定的目标有引用标记字符，则以按引用传递的方式绑定；否则以按值传递的方式绑定。
当绑定的引用标记字符为 @ 且不是列表项时抛出异常。
按引用传递绑定直接转移该项的内容。
*/
YF_API void
BindParameter(Environment&, const TermNode&, TermNode&);

/*!
\brief 匹配参数。
\exception std::bad_function_call 异常中立：参数指定的处理器为空。
\sa TermTags
\since build 858

进行匹配的算法递归搜索形式参数及其子项。
若匹配成功，调用参数指定的匹配处理器。
参数指定形式参数、实际参数、两个处理器、绑定选项和引用值对应的锚对象指针。
其中，形式参数被视为作为形式参数树的右值。
绑定选项以 TermTags 编码，但含义和作用在项上时不完全相同：
仅使用其中的 Unique 位和 Nonmodifying 位；
Unique 表示不被共享的项（在此即右值）；
Nonmodifying 表示需要复制。
当需要复制时，递归处理的所有对实际参数的绑定以复制代替转移；
可能被共享的项在发现表达数树中存在列表后失效，且对之后的子项进行递归处理。
以上处理的操作数的子项仅在确定参数对应位置是列表时进行。
处理器为参数列表结尾的结尾序列处理器和值处理器，分别匹配以 . 起始的项和非列表项。
处理器参数列表中的记号值为匹配的名称；
处理器最后的参数指定按值传递时的复制（而非转移）；
其余参数指定被匹配的操作数子项。
若操作数的某一个需匹配的列表子项是 TermReference 或复制标识为 true ，
	序列处理器中需要进行复制。
结尾序列处理器传入的字符串参数表示需绑定的表示结尾序列的列表标识符。
匹配要求如下：
若项是非空列表，则操作数的对应的项应为满足确定子项数的列表：
	若最后的子项为 . 起始的符号，则匹配操作数中结尾的任意个数的项作为结尾序列：
	其它子项一一匹配操作数的子项；
若项是空列表，则操作数的对应的项应为空列表；
若项是 TermReference ，则以其引用的项作为子项继续匹配；
否则，匹配非列表项。
*/
YF_API void
MatchParameter(const TermNode&, TermNode&, function<void(TNIter, TNIter, const
	TokenValue&, TermTags, const AnchorPtr&)>, function<void(const TokenValue&,
	TermNode&, TermTags, const AnchorPtr&)>, TermTags, const AnchorPtr&);
//@}
//@}
//@}


//! \since build 855
//@{
//! \brief 访问节点的子节点并调用一元函数。
template<typename _func, typename... _tParams>
inline auto
CallRawUnary(_func&& f, TermNode& term, _tParams&&... args)
	-> yimpl(decltype(ystdex::make_expanded<void(TermNode&, _tParams&&...)>(
	std::ref(f))(NPL::Deref(std::next(term.begin())), yforward(args)...)))
{
	RetainN(term);
	return ystdex::make_expanded<void(TermNode&, _tParams&&...)>(std::ref(f))(
		NPL::Deref(std::next(term.begin())), yforward(args)...);
}

//! \brief 解析节点的子节点并调用一元函数。
template<typename _func>
inline auto
CallResolvedUnary(_func&& f, TermNode& term)
	-> yimpl(decltype(NPL::ResolveTerm(yforward(f), term)))
{
	return Forms::CallRawUnary([&](TermNode& node)
		-> yimpl(decltype(NPL::ResolveTerm(yforward(f), term))){
		return NPL::ResolveTerm(yforward(f), node);
	}, term);
}

//! \exception bad_any_cast 异常中立：非列表项类型检查失败。
//@{
//! \since build 859
template<typename _type, typename _func, typename... _tParams>
auto
CallResolvedUnaryAs(_func&& f, TermNode& term, _tParams&&... args)
	-> yimpl(decltype(ystdex::make_expanded<void(_type&,
	const ResolvedTermReferencePtr&, _tParams&&...)>(std::ref(f))(
	NPL::Access<_type>(term), ResolvedTermReferencePtr(),
	std::forward<_tParams>(args)...)))
{
	return Forms::CallResolvedUnary(
		[&](TermNode& nd, ResolvedTermReferencePtr p_ref)
		// TODO: Blocked. Use C++14 'decltype(auto)'.
		-> decltype(ystdex::make_expanded<void(_type&,
		const ResolvedTermReferencePtr&, _tParams&&...)>(std::ref(f))(
		NPL::Access<_type>(term), ResolvedTermReferencePtr())){
		// XXX: Blocked. 'yforward' cause G++ 7.1.0 failed silently.
		return ystdex::make_expanded<void(_type&,
			const ResolvedTermReferencePtr&, _tParams&&...)>(std::ref(f))(
			NPL::Access<_type>(nd), p_ref, std::forward<_tParams>(args)...);
	}, term);
}

/*!
\note 访问节点的子节点，以正规值调用一元函数。
\sa CheckRegular
\exception ListTypeError 异常中立：项为列表项。
*/
template<typename _type, typename _func, typename... _tParams>
auto
CallRegularUnaryAs(_func&& f, TermNode& term, _tParams&&... args)
	-> yimpl(decltype(ystdex::make_expanded<void(_type&,
	const ResolvedTermReferencePtr&, _tParams&&...)>(std::ref(f))(
	NPL::Access<_type>(term), ResolvedTermReferencePtr(),
	std::forward<_tParams>(args)...)))
{
	return Forms::CallResolvedUnary(
		// TODO: Blocked. Use C++14 'decltype(auto)'.
		[&](TermNode& nd, ResolvedTermReferencePtr p_ref)
		-> decltype(ystdex::make_expanded<void(_type&,
		const ResolvedTermReferencePtr&, _tParams&&...)>(std::ref(f))(
		NPL::Access<_type>(term), ResolvedTermReferencePtr())){
		NPL::CheckRegular<_type>(nd, p_ref);
		// XXX: Blocked. 'yforward' cause G++ 7.1.0 failed silently.
		return ystdex::make_expanded<void(_type&,
			const ResolvedTermReferencePtr&, _tParams&&...)>(std::ref(f))(
			NPL::Access<_type>(nd), p_ref, std::forward<_tParams>(args)...);
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
	Forms::CallRawUnary([&](TermNode& node){
		YSLib::EmplaceCallResult(term.Value, ystdex::invoke_nonvoid(
			ystdex::make_expanded<void(TermNode&, _tParams&&...)>(std::ref(f)),
			node, yforward(args)...));
	}, term);
}

template<typename _type, typename _func, typename... _tParams>
void
CallUnaryAs(_func&& f, TermNode& term, _tParams&&... args)
{
	Forms::CallUnary([&](TermNode& node){
		// XXX: Blocked. 'yforward' cause G++ 5.3 crash: internal compiler
		//	error: Segmentation fault.
		return ystdex::make_expanded<void(_type&, _tParams&&...)>(std::ref(f))(
			NPL::ResolveRegular<_type>(node), std::forward<_tParams>(args)...);
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
		std::ref(f)), x, NPL::Deref(++i), yforward(args)...));
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
		yforward(args)...));
}
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
		ystdex::bind1(f, std::placeholders::_2, yforward(args)...)));
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
		: Function(f)
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
		: Function(f)
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
		: Function(f)
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
		: Function(f)
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
\since build 838
*/
//@{
//! \brief 注册一元严格求值上下文处理器。
//@{
template<typename _func, class _tTarget>
void
RegisterStrictUnary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterStrict(target, name, UnaryExpansion<_func>(f));
}
template<typename _type, typename _func, class _tTarget>
void
RegisterStrictUnary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterStrict(target, name, UnaryAsExpansion<_type, _func>(f));
}
//@}

//! \brief 注册二元严格求值上下文处理器。
//@{
template<typename _func, class _tTarget>
void
RegisterStrictBinary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterStrict(target, name, BinaryExpansion<_func>(f));
}
//! \since build 835
template<typename _type, typename _type2, typename _func, class _tTarget>
void
RegisterStrictBinary(_tTarget& target, string_view name, _func f)
{
	A1::RegisterStrict(target, name,
		BinaryAsExpansion<_type, _type2, _func>(f));
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


//! \note 测试条件成立，当且仅当 \<test> 非 #f 。
//@{
/*!
\brief 分支判断：根据求值的条件选取表达式求值。
\sa ReduceChecked
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
\sa ReduceChecked
\since build 860

参考调用文法：
<pre>$cond \<clauses>...</pre>
*/
YF_API ReductionStatus
Cond(TermNode&, ContextNode&);

//! \since build 868
//@{
/*!
\brief 条件顺序求值：预期条件成立。

求值第一参数子项作为测试条件，成立时顺序求值之后的子项。

参考调用文法：
<pre>$when \<test> \<expression-sequence></pre>
*/
YF_API ReductionStatus
When(TermNode&, ContextNode&);

/*!
\brief 条件顺序求值：预期条件不成立。

求值第一参数子项作为测试条件，不成立时顺序求值之后的子项。

参考调用文法：
<pre>$unless \<test> \<expression-sequence></pre>
*/
YF_API ReductionStatus
Unless(TermNode&, ContextNode&);
//@}
//@}

/*!
\brief 逻辑非。
\since build 861

当子项求值为 true 时返回 false ，否则返回子项。

参考调用文法：
<pre>not? \<test></pre>
*/
YF_API bool
Not(TermNode&);

/*!
\note 支持保存当前动作。
\sa ReduceChecked
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

按值传递返回值：提升项以避免返回引用造成内存安全问题。

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

参考调用文法：
<pre>set-first! \<list> \<object></pre>
*/
YF_API void
SetFirst(TermNode&);

//! \warning 除自赋值外，不检查循环引用。
//@{
/*!
保留第二参数引用值。

参考调用文法：
<pre>set-first%! \<list> \<object></pre>
*/
YF_API void
SetFirstRef(TermNode&);

/*!
\since build 858

保留第二参数未折叠的引用值。

参考调用文法：
<pre>set-first@! \<list> \<object></pre>
*/
YF_API void
SetFirstRefAt(TermNode&);
//@}
//@}

/*!
\brief 修改第一参数指定的列表以第二参数作为第一个元素外的列表。
\throw ListTypeError 第二参数不是列表。
*/
//@{
/*!
第二参数的元素转换为右值。

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
\brief 取参数指定的列表中的第一元素的引用值。
\throw ListTypeError 参数不是非空列表。
\since build 859
*/
//@{
/*!
转发参数和函数值。

参考调用文法：
<pre>first \<list></pre>
*/
YF_API ReductionStatus
First(TermNode&);

//! \throw ValueCategoryMismatch 参数不是引用值。
//@{
/*!
结果是列表的第一个元素引用的对象的引用值。保留结果中的引用值。

参考调用文法：
<pre>first& \<list></pre>
*/
YF_API ReductionStatus
FirstRef(TermNode&);

/*!
结果是列表的第一个元素的引用值。保留结果中未折叠的引用值。

参考调用文法：
<pre>first@ \<list></pre>
*/
YF_API ReductionStatus
FirstRefAt(TermNode&);

/*!
结果是列表的第一个元素经过返回值转换的值。不保留结果中的引用值。

参考调用文法：
<pre>firstv \<list></pre>
*/
YF_API ReductionStatus
FirstVal(TermNode&);
//@}
//@}


/*!
\brief 对指定项按指定的环境求值。
\note 支持保存当前动作。
\sa ResolveEnvironment

以表达式 \c \<expression> 和环境 \c \<environment> 为指定的参数进行求值。
环境以 ContextNode 的引用表示。
*/
//@{
/*!
\since build 787
按值传递返回值：提升项以避免返回引用造成内存安全问题。

参考调用文法：
<pre>eval \<expression> \<environment></pre>
*/
YF_API ReductionStatus
Eval(TermNode&, ContextNode&);

/*!
\since build 822

在返回时不提升项，允许返回引用。

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

在返回时不提升项，允许返回引用。

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
\brief 定义。
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
\note 不对剩余表达式进一步求值。

剩余表达式视为求值结果，直接绑定到 \c \<definiend> 。

参考调用文法：
<pre>$deflazy! \<definiend> \<expressions></pre>
*/
YF_API ReductionStatus
DefineLazy(TermNode&, ContextNode&);

/*!
\note 不支持递归绑定。

剩余表达式视为一个表达式在上下文决定的当前环境进行求值后绑定到 \c \<definiend> 。

参考调用文法：
<pre>$def! \<definiend> \<expressions></pre>
*/
YF_API ReductionStatus
DefineWithNoRecursion(TermNode&, ContextNode&);

/*!
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
按值传递返回值：提升项以避免返回引用造成内存安全问题。

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
\throw InvalidSyntax \<eformal> 不符合要求。

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

捕获的静态环境由环境参数 \<env> 求值后指定。
根据环境参数的类型为 \c shared_ptr<Environment> 或 \c weak_ptr<Environment>
	决定是否保留所有权。
*/
//@{
/*!
按值传递返回值：提升项以避免返回引用造成内存安全问题。

参考调用文法：
<pre>$vau/e \<env> \<formals> \<eformal> \<body></pre>
*/
YF_API ReductionStatus
VauWithEnvironment(TermNode&, ContextNode&);

/*!
在返回时不提升项，允许返回引用。

参考调用文法：
<pre>$vau/e% \<env> \<formals> \<eformal> \<body></pre>
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
<pre>list* \<object>+ </pre>
*/
YF_API ReductionStatus
ListAsterisk(TermNode&);

/*!
结果是构造的列表的值。保留结果中的引用值。

参考调用文法：
<pre>list*% \<object>+ </pre>
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

