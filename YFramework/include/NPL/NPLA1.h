/*
	© 2014-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.h
\ingroup NPL
\brief NPLA1 公共接口。
\version r2831
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 17:58:24 +0800
\par 修改时间:
	2017-03-27 11:05 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#ifndef NPL_INC_NPLA1_h_
#define NPL_INC_NPLA1_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA // for NPLATag, ValueNode, TermNode, LoggedEvent,
//	YSLib::Access, ystdex::equality_comparable, ystdex::exclude_self_t,
//	YSLib::AreEqualHeld, YSLib::GHEvent, ystdex::make_function_type_t,
//	ystdex::make_parameter_tuple_t, ystdex::make_expanded,
//	ystdex::invoke_nonvoid, ystdex::make_transform, std::accumulate,
//	ystdex::bind1, std::placeholders::_2, ystdex::examiners::equal_examiner;

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

/*!
\brief 取值记号的字符串表示。
\return 表示对应记号值的字符串，保证不相等的输入对应不相等的结果。
\throw std::invalid_argument 输入的值不是合法的值记号。
\relates ValueToken
\since build 768
*/
YF_API string
to_string(ValueToken);


//! \since build 674
//@{
//! \brief 插入 NPLA1 子节点。
//@{
/*!
\note 插入后按名称排序顺序。

第一参数指定的变换结果插入第二参数指定的容器。
若映射操作返回节点名称为空则根据当前容器内子节点数量加前缀 $ 命名以避免重复。
*/
YF_API void
InsertChild(TermNode&&, TermNode::Container&);

/*!
\note 保持顺序。

直接插入 NPLA1 子节点到序列容器末尾。
*/
YF_API void
InsertSequenceChild(TermNode&&, NodeSequence&);
//@}

/*!
\brief 变换 NPLA1 节点 S 表达式抽象语法树为 NPLA1 语义结构。
\exception std::bad_function_call 第三至五参数为空。
\return 变换后的新节点（及子节点）。

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
	NodeToString = ParseNPLANodeString, NodeInserter = InsertChild);

/*!
\brief 变换 NPLA1 节点 S 表达式抽象语法树为 NPLA1 序列语义结构。
\exception std::bad_function_call 第三至五参数为空。
\return 变换后的新节点（及子节点）。
\sa TransformNode

和 TransformNode 变换规则相同，
但插入的子节点以 NodeSequence 的形式作为变换节点的值而不是子节点，可保持顺序。
*/
YF_API ValueNode
TransformNodeSequence(const TermNode&, NodeMapper = {},
	NodeMapper = MapNPLALeafNode, NodeToString = ParseNPLANodeString,
	NodeSequenceInserter = InsertSequenceChild);
//@}


/*!
\brief 加载 NPLA1 翻译单元。
\throw LoggedEvent 警告：被加载配置中的实体转换失败。
*/
//@{
template<typename _type, typename... _tParams>
ValueNode
LoadNode(_type&& tree, _tParams&&... args)
{
	TryRet(A1::TransformNode(std::forward<TermNode&&>(tree),
		yforward(args)...))
	CatchThrow(ystdex::bad_any_cast& e, LoggedEvent(YSLib::sfmt(
		"Bad NPLA1 tree found: cast failed from [%s] to [%s] .", e.from(),
		e.to()), YSLib::Warning))
}

template<typename _type, typename... _tParams>
ValueNode
LoadNodeSequence(_type&& tree, _tParams&&... args)
{
	TryRet(A1::TransformNodeSequence(std::forward<TermNode&&>(tree),
		yforward(args)...))
	CatchThrow(ystdex::bad_any_cast& e, LoggedEvent(YSLib::sfmt(
		"Bad NPLA1 tree found: cast failed from [%s] to [%s] .", e.from(),
		e.to()), YSLib::Warning))
}
//@}


//! \since build 685
//@{
//! \brief 访问守护遍。
YF_API GuardPasses&
AccessGuardPassesRef(ContextNode&);

//! \brief 访问叶遍。
YF_API EvaluationPasses&
AccessLeafPassesRef(ContextNode&);

//! \brief 访问列表遍。
YF_API EvaluationPasses&
AccessListPassesRef(ContextNode&);
//@}

/*!
\brief 访问字面量遍。
\since build 738
*/
YF_API LiteralPasses&
AccessLiteralPassesRef(ContextNode&);

//! \sa InvokePasses
//@{
/*!
\brief 调用守护遍。
\sa GuardPasses
\since build 726
*/
YF_API Guard
InvokeGuard(TermNode& term, ContextNode&);

/*!
\sa EvaluationPasses
\since build 730
*/
//@{
//! \brief 调用叶遍。
YF_API ReductionStatus
InvokeLeaf(TermNode& term, ContextNode&);

//! \brief 调用列表遍。
YF_API ReductionStatus
InvokeList(TermNode& term, ContextNode&);
//@}

/*!
\brief 调用字面量遍。
\pre 断言：字符串参数的数据指针非空。
\sa LiteralPasses
\since build 738
*/
YF_API ReductionStatus
InvokeLiteral(TermNode&, ContextNode&, string_view);
//@}


/*!
\brief NPLA1 表达式节点规约：调用至少一次求值例程规约子表达式。
\return 规约状态。
\note 可能使参数中容器的迭代器失效。
\note 默认不需要重规约。这可被求值遍改变。
\note 可被求值遍调用以实现递归求值。
\note 异常安全取决于调用遍的最低异常安全保证。
\sa CheckReducible
\sa InvokeGuard
\sa InvokeLeaf
\sa InvokeList
\sa ValueToken
\since build 730
\todo 实现 ValueToken 保留处理。

规约顺序如下：
调用 InvokeGuard 进行必要的上下文重置；
迭代规约，直至不需要进行重规约。
对应不同的节点次级结构分类，一次迭代按以下顺序选择以下分支之一，按需规约子项：
对枝节点调用 InvokeList 求值；
对空节点或值为 ValueToken 的叶节点不进行操作；
对其它叶节点调用 InvokeLeaf 求值。
迭代结束调用 CheckReducible ，根据结果判断是否进行重规约。
此处约定的迭代中对节点的具体结构分类默认也适用于其它 NPLA1 实现 API ；
例外情况应单独指定明确的顺序。
例外情况包括输入节点不是表达式语义结构（而是 AST ）的 API ，如 TransformNode 。
当前实现返回的规约状态总是 ReductionStatus::Clean ，否则会循环迭代。
若需要保证无异常时仅在规约成功后终止，使用 ReduceChecked 代替。
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
//! \since build 685
inline PDefH(void, ReduceArguments, TermNode::Container& con, ContextNode& ctx)
	ImplRet(ReduceArguments(con.begin(), con.end(), ctx))
inline PDefH(void, ReduceArguments, TermNode& term, ContextNode& ctx)
	ImplRet(ReduceArguments(term.GetContainerRef(), ctx))
//@}

/*!
\since build 735
\note 失败视为重规约。
*/
//@{
/*!
\brief 规约并检查成功：调用 Reduce 并检查结果。
\sa CheckedReduceWith
\sa Reduce
*/
YF_API void
ReduceChecked(TermNode&, ContextNode&);

/*!
\brief 规约闭包。
\sa ReduceChecked

构造规约项，规约后替换到第一参数指定项。
规约项的内容由第四参数的闭包指定。第三参数指定是否通过转移构造而不保留原项。
规约后转移闭包规约的结果：子项以及引用的值的目标被转移到第一参数指定的项。
结果中子项和值之间被转移的相对顺序未指定。
*/
YF_API void
ReduceCheckedClosure(TermNode&, ContextNode&, bool, TermNode&);
//@}

/*!
\brief 规约子项。
\since build 697
*/
//@{
YF_API void
ReduceChildren(TNIter, TNIter, ContextNode&);
inline PDefH(void, ReduceChildren, TermNode::Container& con, ContextNode& ctx)
	ImplExpr(ReduceChildren(con.begin(), con.end(), ctx))
inline PDefH(void, ReduceChildren, TermNode& term, ContextNode& ctx)
	ImplExpr(ReduceChildren(term.GetContainerRef(), ctx))
//@}
//@}

/*!
\brief 有序规约子项。
\return 当存在子项时为最后一个子项的规约状态，否则为 ReductionStatus::Clean 。
\since build 773
*/
//@{
YF_API ReductionStatus
ReduceChildrenOrdered(TNIter, TNIter, ContextNode&);
inline PDefH(ReductionStatus, ReduceChildrenOrdered, TermNode::Container& con,
	ContextNode& ctx)
	ImplRet(ReduceChildrenOrdered(con.begin(), con.end(), ctx))
inline PDefH(ReductionStatus, ReduceChildrenOrdered, TermNode& term,
	ContextNode& ctx)
	ImplRet(ReduceChildrenOrdered(term.GetContainerRef(), ctx))
//@}

/*!
\brief 规约第一个子项。
\return 规约状态。
\sa Reduce
\see https://en.wikipedia.org/wiki/Fexpr 。
\since build 730

快速严格性分析：
无条件求值枝节点第一项以避免非确定性推断子表达式求值的附加复杂度。
*/
YF_API ReductionStatus
ReduceFirst(TermNode&, ContextNode&);

/*!
\brief 规约有序序列：顺序规约子项，结果为最后一个子项的规约结果。
\return 当存在子项时为最后一个子项的规约状态，否则为 ReductionStatus::Clean 。
\sa ReduceChildrenOrdered
\since build 764
*/
YF_API ReductionStatus
ReduceOrdered(TermNode&, ContextNode&);

/*!
\brief 移除容器第一个子项到指定迭代器的项后规约。
\note 按语言规范，子项规约顺序未指定。
\since build 733
*/
YF_API ReductionStatus
ReduceTail(TermNode&, ContextNode&, TNIter);
//@}


/*!
\brief 设置跟踪深度节点：调用规约时显示深度和上下文等信息。
\note 主要用于调试。
\sa InvokeGuard
\since build 685
*/
YF_API void
SetupTraceDepth(ContextNode& ctx, const string& name = yimpl("$__depth"));


//! \note ValueObject 参数分别指定替换添加的前缀和被替换的分隔符的值。
//@{
/*!
\brief 变换分隔符中缀表达式为前缀表达式。
\sa AsIndexNode
\since build 753

移除子项中值和指定分隔符指定的项，并以 AsIndexNode 添加指定前缀值作为子项。
最后一个参数指定返回值的名称。
*/
//@{
//! \note 非递归变换。
YF_API TermNode
TransformForSeparator(const TermNode&, const ValueObject&, const ValueObject&,
	const TokenValue& = {});

//! \note 递归变换。
YF_API TermNode
TransformForSeparatorRecursive(const TermNode&, const ValueObject&,
	const ValueObject&, const TokenValue& = {});
//@}

/*!
\brief 查找项中的指定分隔符，若找到则替换项为去除分隔符并添加替换前缀的形式。
\return 是否找到并替换了项。
\sa EvaluationPasses
\sa TransformForSeparator
\since build 730
*/
YF_API ReductionStatus
ReplaceSeparatedChildren(TermNode&, const ValueObject&, const ValueObject&);
//@}


//! \since build 751
//@{
/*!
\brief 包装上下文处理器。
\note 忽略被包装的上下文处理器可能存在的返回值，自适应默认返回规约结果。
*/
template<typename _func>
struct WrappedContextHandler
	: private ystdex::equality_comparable<WrappedContextHandler<_func>>
{
	_func Handler;

	//! \since build 757
	//@{
	template<typename _tParam, yimpl(typename
		= ystdex::exclude_self_t<WrappedContextHandler, _tParam>)>
	WrappedContextHandler(_tParam&& arg)
		: Handler(yforward(arg))
	{}
	template<typename _tParam1, typename _tParam2, typename... _tParams>
	WrappedContextHandler(_tParam1&& arg1, _tParam2&& arg2, _tParams&&... args)
		: Handler(yforward(arg1), yforward(arg2), yforward(args)...)
	{}

	DefDeCopyMoveCtorAssignment(WrappedContextHandler)
	//@}

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
	friend PDefHOp(bool, ==, const WrappedContextHandler& x,
		const WrappedContextHandler& y)
		ImplRet(YSLib::AreEqualHeld(x.Handler, y.Handler))
};

template<class _tDst, typename _func>
inline _tDst
WrapContextHandler(_func&& h, ystdex::false_)
{
	return WrappedContextHandler<YSLib::GHEvent<ystdex::make_function_type_t<
		void, ystdex::make_parameter_tuple_t<typename _tDst::BaseType>>>>(
		yforward(h));
}
template<class, typename _func>
inline _func
WrapContextHandler(_func&& h, ystdex::true_)
{
	return yforward(h);
}
template<class _tDst, typename _func>
inline _tDst
WrapContextHandler(_func&& h)
{
	using BaseType = typename _tDst::BaseType;

	// XXX: It is a hack to adjust the convertible result for the expanded
	//	caller here. It should have been implemented in %GHEvent, however types
	//	those cannot convert to expanded caller cannot be SFINAE'd out,
	//	otherwise it would cause G++ 5.4 crash with internal compiler error:
	//	"error reporting routines re-entered".
	return A1::WrapContextHandler<_tDst>(yforward(h), ystdex::or_<
		std::is_constructible<BaseType, _func&&>,
		std::is_constructible<BaseType, ystdex::expanded_caller<
		typename _tDst::FuncType, ystdex::decay_t<_func>>>>());
}
//@}


/*!
\brief 形式上下文处理器。
\since build 674

处理列表项的操作符。
*/
class YF_API FormContextHandler
	: private ystdex::equality_comparable<FormContextHandler>
{
public:
	ContextHandler Handler;
	/*!
	\brief 项检查例程：验证被包装的处理器的调用符合前置条件。
	\since build 733
	*/
	std::function<bool(const TermNode&)> Check{IsBranch};

	//! \since build 697
	template<typename _func,
		yimpl(typename = ystdex::exclude_self_t<FormContextHandler, _func>)>
	FormContextHandler(_func&& f)
		: Handler(A1::WrapContextHandler<ContextHandler>(yforward(f)))
	{}
	//! \since build 733
	template<typename _func, typename _fCheck>
	FormContextHandler(_func&& f, _fCheck c)
		: Handler(A1::WrapContextHandler<ContextHandler>(yforward(f))), Check(c)
	{}
	//! \since build 757
	DefDeCopyMoveCtorAssignment(FormContextHandler)

	/*!
	\brief 比较上下文处理器相等。
	\note 忽略检查例程的等价性。
	\since build 748
	*/
	friend PDefHOp(bool, ==, const FormContextHandler& x,
		const FormContextHandler& y)
		ImplRet(x.Handler == y.Handler)

	/*!
	\brief 处理一般形式。
	\return Handler 调用的返回值，或 ReductionStatus::Clean 。
	\exception NPLException 异常中立。
	\throw LoggedEvent 警告：类型不匹配，
		由 Handler 抛出的 ystdex::bad_any_cast 转换。
	\throw LoggedEvent 错误：由 Handler 抛出的 ystdex::bad_any_cast 外的
		std::exception 转换。
	\throw std::invalid_argument 项检查未通过。
	\since build 751

	项检查不存在或在检查通过后，变换无参数规约，然后对节点调用 Hanlder ，否则抛出异常。
	无参数时第一参数具有两个子项且第二项为空节点。无参数变换删除空节点。
	*/
	ReductionStatus
	operator()(TermNode&, ContextNode&) const;
};


/*!
\brief 严格上下文处理器。
\since build 754

对参数先进行求值的处理列表项的操作符。
*/
class YF_API StrictContextHandler
	: private ystdex::equality_comparable<StrictContextHandler>
{
public:
	//! \since build 696
	FormContextHandler Handler;

	//! \since build 697
	template<typename _func,
		yimpl(typename = ystdex::exclude_self_t<StrictContextHandler, _func>)>
	StrictContextHandler(_func&& f)
		: Handler(yforward(f))
	{}
	//! \since build 733
	template<typename _func, typename _fCheck>
	StrictContextHandler(_func&& f, _fCheck c)
		: Handler(yforward(f), c)
	{}
	//! \since build 757
	DefDeCopyMoveCtorAssignment(StrictContextHandler)

	//! \brief 比较上下文处理器相等。
	friend PDefHOp(bool, ==, const StrictContextHandler& x,
		const StrictContextHandler& y)
		ImplRet(x.Handler == y.Handler)

	/*!
	\brief 处理函数。
	\return Handler 调用的返回值。
	\throw ListReductionFailure 列表子项不大于一项。
	\sa ReduceArguments
	\since build 751

	对每一个子项求值；然后检查项数，对可调用的项调用 Hanlder ，否则抛出异常。
	*/
	ReductionStatus
	operator()(TermNode&, ContextNode&) const;
};


//! \since build 733
//@{
template<typename... _tParams>
inline void
RegisterForm(ContextNode& node, const string& name,
	_tParams&&... args)
{
	NPL::RegisterContextHandler(node, name,
		FormContextHandler(yforward(args)...));
}

//! \brief 转换上下文处理器。
template<typename... _tParams>
inline ContextHandler
ToContextHandler(_tParams&&... args)
{
	return StrictContextHandler(yforward(args)...);
}

/*!
\brief 注册严格上下文处理器。
\note 使用 ADL ToContextHandler 。
\since build 754
*/
template<typename... _tParams>
inline void
RegisterStrict(ContextNode& node, const string& name, _tParams&&... args)
{
	NPL::RegisterContextHandler(node, name,
		ToContextHandler(yforward(args)...));
}
//@}

/*!
\brief 注册分隔符转换变换和处理例程。
\sa NPL::RegisterContextHandler
\sa ReduceChildren
\sa ReduceOrdered
\sa ReplaceSeparatedChildren
\since build 753

变换带有中缀形式的分隔符记号的表达式为指定名称的前缀表达式并去除分隔符，
然后注册前缀语法形式。
最后一个参数指定是否有序，选择语法形式为 ReduceOrdered 或 ReduceChildren 之一。
前缀名称不需要是记号支持的标识符。
*/
YF_API void
RegisterSequenceContextTransformer(EvaluationPasses&, ContextNode&,
	const TokenValue&, const ValueObject&, bool = {});


/*!
\brief 断言枝节点。
\pre 断言：参数指定的项是枝节点。
\since build 761
*/
inline PDefH(void, AssertBranch, const TermNode& term,
	const char* msg = "Invalid term found.") ynothrowv
	ImplExpr(yunused(msg), YAssert(IsBranch(term), msg))

/*!
\brief 取项的参数个数：子项数减 1 。
\pre 间接断言：参数指定的项是枝节点。
\return 项的参数个数。
\since build 733
*/
inline PDefH(size_t, FetchArgumentN, const TermNode& term) ynothrowv
	ImplRet(AssertBranch(term), term.size() - 1)


//! \note 第一参数指定输入的项，其 Value 指定输出的值。
//@{
//! \sa LiftDelayed
//@{
/*!
\brief 求值以节点数据结构间接表示的项。
\since build 752

以 TermNode 按项访问值，若成功调用 LiftTermRef 替换值并返回要求重规约。
以项访问对规约以项转移的可能未求值的操作数是必要的。
*/
YF_API ReductionStatus
EvaluateDelayed(TermNode&);
/*!
\brief 求值指定的延迟求值项。
\return ReductionStatus::Retrying 。
\since build 761

提升指定的延迟求值项并规约。
*/
YF_API ReductionStatus
EvaluateDelayed(TermNode&, DelayedTerm&);
//@}

/*!
\pre 断言：第三参数的数据指针非空。
\exception BadIdentifier 标识符未声明。
\note 默认结果为 ReductionStatus::Clean 以保证强规范化性质。
*/
//@{
/*!
\brief 求值标识符。
\note 不验证标识符是否为字面量；仅以字面量处理时可能需要重规约。
\sa EvaluateDelayed
\sa LiftTermRef
\sa LiteralHandler
\sa ResolveName
\since build 745

依次进行以下求值操作：
调用 ResolveName 根据指定名称查找值，若失败抛出未声明异常；
调用 LiftTermRef 或 TermNode::SetContentIndirect 替换非列表或列表节点的值；
以 LiteralHandler 访问字面量处理器，若成功调用并返回字面量处理器的处理结果。
若未返回，根据节点表示的值进一步处理：
	对表示非 TokenValue 值的叶节点，调用 EvaluateDelayed 求值；
	对表示 TokenValue 值的叶节点，返回 ReductionStatus::Retrying 重规约；
	对枝节点视为列表，返回 ReductionStatus::Retained 避免进一步求值。
*/
YF_API ReductionStatus
EvaluateIdentifier(TermNode&, const ContextNode&, string_view);

/*!
\brief 求值叶节点记号。
\sa CategorizeLexeme
\sa DeliteralizeUnchecked
\sa EvaluateIdentifier
\sa InvokeLiteral
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
\return 规约状态。
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
\sa TermToNode
*/
YF_API ReductionStatus
ReduceLeafToken(TermNode&, ContextNode&);
//@}


/*!
\brief 解析名称：处理保留名称并查找名称。
\pre 断言：第二参数的数据指针非空。
\since build 777
*/
YF_API observer_ptr<const ValueNode>
ResolveName(const ContextNode&, string_view);


/*!
\brief 设置默认解释：解释使用的公共处理遍。
\note 非强异常安全：加入遍可能提前设置状态而不在失败时回滚。
\sa ReduceCombined
\sa ReduceFirst
\sa ReduceHeadEmptyList
\sa ReduceLeafToken
\since build 736
*/
YF_API void
SetupDefaultInterpretation(ContextNode&, EvaluationPasses);


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
	//! \brief 上下文根节点。
	ContextNode Root{};
	//! \brief 预处理节点：每次翻译时预先处理调用的公共例程。
	TermPasses Preprocess{};
	//! \brief 表项处理例程：每次翻译中规约回调处理调用的公共例程。
	EvaluationPasses ListTermPreprocess{};

	/*!
	\brief 构造：使用默认的解释。
	\note 参数指定是否启用对规约深度进行跟踪。
	\sa ListTermPreprocess
	\sa SetupDefaultInterpretation
	\sa SetupTraceDepth
	*/
	REPLContext(bool = {});

	/*!
	\brief 加载：从指定参数指定的来源读取并处理源代码。
	\sa Process
	\since build 758
	*/
	//@{
	//! \throw std::invalid_argument 流状态错误或缓冲区不存在。
	void
	LoadFrom(std::istream&);
	void
	LoadFrom(std::streambuf&);
	//@}

	/*!
	\brief 执行循环：对非空输入进行翻译。
	\pre 断言：字符串的数据指针非空。
	\throw LoggedEvent 输入为空串。
	\sa Process
	*/
	TermNode
	Perform(string_view);

	/*!
	\brief 处理：分析输入并标记记号节点，预处理后进行规约。
	\sa SContext::Analyze
	\sa Preprocess
	\sa Reduce
	\sa TokenizeTerm
	\since build 742
	*/
	//@{
	void
	Process(TermNode&);
	TermNode
	Process(const TokenList&);
	TermNode
	Process(const Session&);
	//@}
};


/*!
\brief NPLA1 语法形式对应的功能实现。
\since build 732
*/
namespace Forms
{

/*!
\pre 断言：项或容器对应枝节点。
\sa AssertBranch
\since build 733
*/
//@{
/*!
\note 保留求值留作保留用途，一般不需要被作为用户代码直接使用。
\since build 765

可使用 RegisterForm 注册上下文处理器，参考文法：
$retain|$retainN <expression>
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


/*!
\throw ParameterMismatch 匹配失败。
\note 不具有强异常安全保证。匹配失败时，其它的绑定状态未指定。
\since build 776

绑定前需要进行结构化匹配检查。
若匹配失败，则抛出异常；否则，在第一参数指定的环境内绑定未被忽略的匹配的项：
非空列表项的绑定为对应子项的绑定；
空列表项的绑定为空操作；
非列表项操作数直接绑定到名称为符号值的参数对应项。
绑定按深度优先的词法顺序进行。若已存在绑定则重新绑定。
*/
//@{
/*!
\brief 使用操作数结构化匹配并绑定参数。
\throw ArityMismatch 子项数匹配失败。
\sa BindParameterLeaf

形式参数和操作数为项指定的表达式树。
第二参数指定形式参数，第三参数指定操作数。
进行匹配的算法递归搜索形式参数及其子项，匹配要求如下：
若项是非空列表，则操作数的对应的项应为满足确定子项数的列表：
	若最后的子项为符号 ... ，则匹配操作数中结尾的任意个数的项；
	其它子项一一匹配操作数的子项；
若项是空列表，则操作数的对应的项应为空列表；
否则，调用 BindParameterLeaf 匹配非列表项。
*/
YF_API void
BindParameter(ContextNode&, const TermNode&, TermNode&);

/*!
\brief 使用操作数结构化匹配并绑定参数到非列表项。
\sa BindParameter

形式参数和操作数为项指定的表达式树。
第二参数指定名称，之后的参数指定值。
匹配要求如下：
若项是 #ignore ，则忽略操作数对应的项；
若项的值是符号，则操作数的对应的项应为非列表项。
*/
//@{
YF_API void
BindParameterLeaf(ContextNode&, const TokenValue&, TermNode::Container&&,
	ValueObject&&);
inline PDefH(void, BindParameterLeaf, ContextNode& e, const TokenValue& n,
	TermNode&& o)
	ImplExpr(BindParameterLeaf(e, n, std::move(o.GetContainerRef()),
		std::move(o.Value)))
//@}
//@}

/*!
\brief 检查项中是否存在为修饰符的第二个子项，若存在则移除。
\return 是否存在并移除了修饰符。
\since build 732

检查第一参数指定的容器或项是否存在第二参数指定的修饰符为项的第一参数。
若检查发现存在修饰符则移除。
*/
//@{
YF_API bool
ExtractModifier(TermNode::Container&, const ValueObject& = string("!"));
inline PDefH(bool, ExtractModifier, TermNode& term,
	const ValueObject& mod = string("!"))
	ImplRet(ExtractModifier(term.GetContainerRef(), mod))
//@}

//! \brief 规约可能带有修饰符的项。
template<typename _func>
void
ReduceWithModifier(TermNode& term, ContextNode& ctx, _func f)
{
	const bool mod(ExtractModifier(term));

	if(IsBranch(term))
		f(term, ctx, mod);
	else
		throw InvalidSyntax("Argument not found.");
}
//@}


/*!
\brief 访问节点并调用一元函数。
\sa YSLib::EmplaceCallResult
\since build 756

确定项具有一个实际参数后展开调用参数指定的函数。
若被调用的函数返回类型非 void ，返回值作为项的值被构造。
调用 YSLib::EmplaceCallResult 对 ValueObject 及引用值处理不同。
若需以和其它类型的值类似的方式被包装，在第一个参数中构造 ValueObject 对象。
*/
//@{
template<typename _func, typename... _tParams>
void
CallUnary(_func&& f, TermNode& term, _tParams&&... args)
{
	RetainN(term);
	YSLib::EmplaceCallResult(term.Value, ystdex::invoke_nonvoid(
		ystdex::make_expanded<void(TermNode&, _tParams&&...)>(yforward(f)),
		Deref(std::next(term.begin())), yforward(args)...));
}

template<typename _type, typename _func, typename... _tParams>
void
CallUnaryAs(_func&& f, TermNode& term, _tParams&&... args)
{
	Forms::CallUnary([&](TermNode& node){
		// XXX: Blocked. 'yforward' cause G++ 5.3 crash: internal compiler
		//	error: Segmentation fault.
		return ystdex::make_expanded<void(_type&, _tParams&&...)>(yforward(f))(
			YSLib::Access<_type>(node), std::forward<_tParams&&>(args)...);
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
	auto& x(Deref(++i));

	YSLib::EmplaceCallResult(term.Value, ystdex::invoke_nonvoid(
		ystdex::make_expanded<void(TermNode&, TermNode&, _tParams&&...)>(
		yforward(f)), x, Deref(++i), yforward(args)...));
}

template<typename _type, typename _func, typename... _tParams>
void
CallBinaryAs(_func&& f, TermNode& term, _tParams&&... args)
{
	RetainN(term, 2);

	auto i(term.begin());
	auto& x(YSLib::Access<_type>(Deref(++i)));

	YSLib::EmplaceCallResult(term.Value, ystdex::invoke_nonvoid(
		ystdex::make_expanded<void(_type&, _type&, _tParams&&...)>(yforward(f)),
		x, YSLib::Access<_type>(Deref(++i)), yforward(args)...));
}
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
		return YSLib::Access<_type>(Deref(it));
	}));

	YSLib::EmplaceCallResult(term.Value, std::accumulate(j, std::next(j,
		typename std::iterator_traits<decltype(j)>::difference_type(n)), val,
		ystdex::bind1(f, std::placeholders::_2, yforward(args)...)));
}


/*!
\brief 保存函数展开调用的函数对象。
\todo 使用 C++1y lambda 表达式代替。

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
{
	_func Function;

	/*!
	\brief 比较处理器相等。
	\since build 773
	*/
	friend PDefHOp(bool, ==, const UnaryExpansion& x, const UnaryExpansion& y)
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
{
	_func Function;

	/*!
	\brief 比较处理器相等。
	\since build 773
	*/
	friend PDefHOp(bool, ==, const UnaryAsExpansion& x,
		const UnaryAsExpansion& y)
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
{
	_func Function;

	/*!
	\brief 比较处理器相等。
	\since build 773
	*/
	friend PDefHOp(bool, ==, const BinaryExpansion& x, const BinaryExpansion& y)
		ImplRet(ystdex::examiners::equal_examiner::are_equal(x.Function,
			y.Function))

	template<typename... _tParams>
	inline void
	operator()(_tParams&&... args) const
	{
		Forms::CallBinary(Function, yforward(args)...);
	}
};


//! \sa Forms::CallBinaryAs
template<typename _type, typename _func>
struct BinaryAsExpansion
{
	_func Function;

	/*!
	\brief 比较处理器相等。
	\since build 773
	*/
	friend PDefHOp(bool, ==, const BinaryAsExpansion& x,
		const BinaryAsExpansion& y)
		ImplRet(ystdex::examiners::equal_examiner::are_equal(x.Function,
			y.Function))

	template<typename... _tParams>
	inline void
	operator()(_tParams&&... args) const
	{
		Forms::CallBinaryAs<_type>(Function, yforward(args)...);
	}
};
//@}
//@}


/*!
\brief 注册一元严格求值上下文处理器。
\since build 754
*/
//@{
template<typename _func>
void
RegisterStrictUnary(ContextNode& node, const string& name, _func f)
{
	RegisterStrict(node, name, UnaryExpansion<_func>{f});
}
template<typename _type, typename _func>
void
RegisterStrictUnary(ContextNode& node, const string& name, _func f)
{
	RegisterStrict(node, name, UnaryAsExpansion<_type, _func>{f});
}
//@}

/*!
\brief 注册二元严格求值上下文处理器。
\since build 760
*/
//@{
template<typename _func>
void
RegisterStrictBinary(ContextNode& node, const string& name, _func f)
{
	RegisterStrict(node, name, BinaryExpansion<_func>{f});
}
template<typename _type, typename _func>
void
RegisterStrictBinary(ContextNode& node, const string& name, _func f)
{
	RegisterStrict(node, name, BinaryAsExpansion<_type, _func>{f});
}
//@}


/*!
\note 在节点后的 bool 参数指定使用定义而不是设置（重定义）。
\note 支持修饰符。
\sa ReduceWithModifier
\since build 735
*/
//@{
/*!
\brief 定义或设置项。
\throw InvalidSyntax 节点分类非法，或由 DefineOrSetFor 抛出的异常。
\sa DefineOrSetFor
\sa Lambda

实现修改环境的特殊形式。
值以项的形式被转移，在替换前进一步求值。返回未指定值。
限定第三参数后可使用 RegisterForm 注册上下文处理器。
特殊形式参考文法：
$define|$set [!] <variable> <expression>
$define|$set [!] (<variable> <formals>) <body>
*/
YF_API void
DefineOrSet(TermNode&, ContextNode&, bool);

/*!
\brief 定义或设置标识符的值为指定的项。
\pre 断言：字符串参数的数据指针非空。
\throw InvalidSyntax 标识符是字面量。
\note 参数分别为标识符、被规约的项、上下文、使用定义以及是否存在修饰符。
\sa DefineValue
\sa EvaluateIdentifier
\sa IsNPLASymbol
\sa RedefineValue
\since build 772

定义或设置参数指定的值：首先检查标识符不是字面量，然后替换。
排除可选项外，若第二子项是列表，
则定义或设置以此列表第一子项为名称、剩余项为参数的 λ 抽象。
否则，直接定义值。
*/
YF_API void
DefineOrSetFor(string_view, TermNode&, ContextNode&, bool, bool);
//@}

/*
\pre 间接断言：第一参数指定的项是枝节点。
\note 实现特殊形式。
\throw InvalidSyntax 语法错误。
*/
//@{
/*!
\brief 条件判断：根据求值的条件取表达式。
\sa ReduceChecked
\since build 750

求值第一子项作为测试条件，成立时取第二子项，否则当第三子项时取第三子项。
特殊形式参考文法：
$if <test> <consequent> <alternate>
$if <test> <consequent>
*/
YF_API ReductionStatus
If(TermNode&, ContextNode&);

/*!
\exception InvalidSyntax 异常中立：由 ExtractParameters 抛出。
\sa EvaluateIdentifier
\sa ExtractParameters
\warning 返回闭包调用引用变量超出绑定目标的生存期引起未定义行为。
\todo 优化捕获开销。

使用 ExtractParameters 检查参数列表并捕获和绑定变量，
然后设置节点的值为表示 λ 抽象的上下文处理器。
可使用 RegisterForm 注册上下文处理器。
和 Scheme 等不同参数以项而不是位置的形式被转移，在函数应用时可能进一步求值。
按引用捕获上下文中的绑定。被捕获的上下文中的绑定依赖宿主语言的生存期规则。
*/
//@{
/*!
\brief λ 抽象：求值为一个捕获当前上下文的严格求值的函数。
\since build 735

特殊形式参考文法：
$lambda <formals> <body>
*/
YF_API void
Lambda(TermNode&, ContextNode&);

/*!
\brief vau 抽象：求值为一个捕获当前上下文的非严格求值的函数。
\note 动态环境的上下文参数被捕获为一个 ystdex::ref<ContextNode> 对象。
\throw InvalidSynta <eformal> 不符合要求。
\since build 767

初始化的 <eformal> 表示动态环境的上下文参数，应为一个符号或 #ignore 。
特殊形式参考文法：
$vau <formals> <eformal> <body>
*/
YF_API void
Vau(TermNode&, ContextNode&);
//@}
//@}


/*!
\sa ReduceChecked
\since build 754
*/
//@{
/*!
\brief 逻辑与。

非严格求值若干个子项，返回求值结果的逻辑与：
除第一个子项，没有其它子项时，返回 true ；否则从左到右逐个求值子项。
当子项全求值为 true 时返回最后一个子项的值，否则返回 false 。
特殊形式参考文法：
$and <test1>...
*/
YF_API ReductionStatus
And(TermNode&, ContextNode&);

/*!
\brief 逻辑或。

非严格求值若干个子项，返回求值结果的逻辑或：
除第一个子项，没有其它子项时，返回 false ；否则从左到右逐个求值子项。
当子项全求值为 false 时返回 false，否则返回第一个不是 false 的子项的值。
特殊形式参考文法：
$or <test1>...
*/
YF_API ReductionStatus
Or(TermNode&, ContextNode&);
//@}


/*!
\brief 调用 UTF-8 字符串的系统命令，并保存 int 类型的结果到项的值中。
\sa usystem
\since build 741

参考文法：
system <string>
*/
YF_API void
CallSystem(TermNode&);

//! \since build 748
//@{
/*!
\brief 比较两个子项的值相等。
\sa YSLib::HoldSame

参考文法：
eq? <object1> <object2>
*/
YF_API void
EqualReference(TermNode&);

/*!
\brief 比较两个子项的值相等。
\sa YSLib::ValueObject

参考文法：
eqv? <object1> <object2>
*/
YF_API void
EqualValue(TermNode&);
//@}

//! \since build 772
//@{
/*!
\brief 对指定项按指定的环境求值。

以表达式 <expression> 和环境 <environment> 为指定的参数进行求值。
环境以 ContextNode 的引用表示。
参考文法：
eval <expression> <environment>
*/
YF_API ReductionStatus
Eval(TermNode&);

//! \brief 创建参数指定的 REPL 的副本并在其中对翻译单元规约以求值。
YF_API void
EvaluateUnit(TermNode&, const REPLContext&);
//@}

/*!
\brief 求值标识符得到指称的实体。
\sa EvaluateIdentifier
\since build 757

在对象语言中实现函数接受一个 string 类型的参数项，返回值为指定的实体。
当名称查找失败时，返回的值为 ValueToken::Null 。
*/
YF_API ReductionStatus
ValueOf(TermNode&, const ContextNode&);

} // namespace Forms;

} // namesapce A1;

} // namespace NPL;

#endif

