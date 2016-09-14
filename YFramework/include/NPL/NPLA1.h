/*
	© 2014-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.h
\ingroup NPL
\brief NPLA1 公共接口。
\version r1434
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 17:58:24 +0800
\par 修改时间:
	2016-09-13 12:42 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#ifndef NPL_INC_NPLA1_h_
#define NPL_INC_NPLA1_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA // for NPLATag, ValueNode, TermNode, LoggedEvent;

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


//! \brief 值记号：节点中的值的占位符。
enum class ValueToken
{
	Null,
	GroupingAnchor,
	OrderedAnchor
};


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
	否则，新建节点容器，遍历并变换剩余的节点插入这个容器，返回以这个容器构造的节点。
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

//! \brief 访问叶节点遍。
YF_API EvaluationPasses&
AccessLeafPassesRef(ContextNode&);

//! \brief 访问列表节点遍。
YF_API EvaluationPasses&
AccessListPassesRef(ContextNode&);

/*!
\sa InvokePasses
\since build 726
*/
//@{
/*!
\brief 调用守护遍。
\sa GuardPasses
*/
YF_API Guard
InvokeGuard(TermNode& term, ContextNode&);

//! \sa EvaluationPasses
//@{
//! \brief 调用叶节点遍。
YF_API bool
InvokeLeaf(TermNode& term, ContextNode&);

//! \brief 调用列表节点遍。
YF_API bool
InvokeList(TermNode& term, ContextNode&);
//@}
//@}


/*!
\brief NPLA1 表达式节点规约：调用至少一次求值例程规约子表达式。
\return 需要重规约。
\note 可能使参数中容器的迭代器失效。
\note 默认不需要重规约。这可被求值遍改变。
\note 可被求值遍调用以实现递归求值。
\sa DetectReducible
\sa InvokeGuard
\sa InvokeLeaf
\sa InvokeList
\sa ValueToken
\todo 实现 ValueToken 保留处理。

规约顺序如下：
调用 InvokeGuard 进行必要的上下文重置；
迭代规约，直至不需要进行重规约。
对应不同的节点次级结构分类，一次迭代按以下顺序判断选择以下分支之一，按需规约子项：
对非空列表节点调用 InvokeList 求值；
对空列表节点替换为 ValueToken::Null ；
对已替换为 ValueToken 的叶节点保留处理；
对其它叶节点调用 InvokeLeaf 求值。
单一求值的结果作为 DetectReducible 的第二参数，根据结果判断是否进行重规约。
此处约定的迭代中对节点的具体结构分类默认也适用于其它 NPLA1 实现 API ；
例外情况应单独指定明确的顺序。
例外情况包括输入节点不是表达式语义结构（而是抽象语法树）的 API ，如 TransformNode 。
*/
YF_API bool
Reduce(TermNode&, ContextNode&);
//@}

/*!
\note 按语言规范，子项规约顺序未指定。
\note 可能使参数中容器的迭代器失效。
\note 忽略子项重规约要求。
\sa Reduce
*/
//@{
/*!
\brief 规约子项。
\since build 697
*/
//@{
YF_API void
ReduceChildren(TermNode::iterator, TermNode::iterator, ContextNode&);
inline PDefH(void, ReduceChildren, TermNode::Container& con, ContextNode& ctx)
	ImplExpr(ReduceChildren(con.begin(), con.end(), ctx))
inline PDefH(void, ReduceChildren, TermNode& term, ContextNode& ctx)
	ImplExpr(ReduceChildren(term.GetContainerRef(), ctx))
//@}

/*!
\brief 对容器中的第二项开始逐项规约。
\throw InvalidSyntax 容器内的子项数不大于 1 。
\sa ReduceChildren
\since build 685
*/
YF_API void
ReduceArguments(TermNode::Container&, ContextNode&);
//@}

/*!
\brief 规约首项。
\return 需要重规约。
\note 快速严格性分析：无条件求值第一项以避免非确定性推断子表达式求值的附加复杂度。
\sa Reduce
\see https://en.wikipedia.org/wiki/Fexpr 。
\since build 686
*/
inline PDefH(bool, ReduceFirst, TermNode& term, ContextNode& ctx)
	ImplRet(!term.empty() ? Reduce(Deref(term.begin()), ctx) : false)


/*!
\brief 设置跟踪深度节点：调用规约时显示深度和上下文等信息。
\note 主要用于调试。
\sa InvokeGuard
\since build 685
*/
YF_API void
SetupTraceDepth(ContextNode& ctx, const string& name = yimpl("$__depth"));


/*!
\note ValueObject 参数分别指定替换添加的前缀和被替换的分隔符的值。
\since build 697
*/
//@{
/*!
\note 移除子项中值和指定分隔符指定的项，并以 AsIndexNode 添加指定前缀值作为子项。
\note 最后一个参数指定返回值的名称。
\sa AsIndexNode
*/
//@{
//! \brief 变换分隔符中缀表达式为前缀表达式。
YF_API TermNode
TransformForSeparator(const TermNode&, const ValueObject&, const ValueObject&,
	const string& = {});

//! \brief 递归变换分隔符中缀表达式为前缀表达式。
YF_API TermNode
TransformForSeparatorRecursive(const TermNode&, const ValueObject&,
	const ValueObject&, const string& = {});
//@}

/*!
\brief 查找项中的指定分隔符，若找到则替换项为去除分隔符并添加替换前缀的形式。
\return 是否找到并替换了项。
\sa EvaluationPasses
\sa TransformForSeparator
*/
YF_API bool
ReplaceSeparatedChildren(TermNode&, const ValueObject&, const ValueObject&);
//@}


/*!
\brief 形式上下文处理器。
\since build 674
*/
class YF_API FormContextHandler
{
public:
	ContextHandler Handler;

	//! \since build 697
	template<typename _func,
		yimpl(typename = ystdex::exclude_self_t<FormContextHandler, _func>)>
	FormContextHandler(_func&& f)
		: Handler(yforward(f))
	{}

	/*!
	\brief 处理一般形式。
	\exception NPLException 异常中立。
	\throw LoggedEvent 警告：类型不匹配，
		由 Handler 抛出的 ystdex::bad_any_cast 转换。
	\throw LoggedEvent 错误：由 Handler 抛出的 ystdex::bad_any_cast 外的
		std::exception 转换。
	\throw std::invalid_argument 项为空。

	对非空项调用 Hanlder ，否则抛出异常。
	*/
	void
	operator()(TermNode&, ContextNode&) const;
};


/*!
\brief 函数上下文处理器。
\since build 696
*/
struct YF_API FunctionContextHandler
{
public:
	FormContextHandler Handler;

	//! \since build 697
	template<typename _func,
		yimpl(typename = ystdex::exclude_self_t<FunctionContextHandler, _func>)>
	FunctionContextHandler(_func&& f)
		: Handler(yforward(f))
	{}

	/*!
	\brief 处理函数。
	\throw ListReductionFailure 列表子项不大于一项。
	\sa ReduceArguments

	对每一个子项求值；然后检查项数，对可调用的项调用 Hanlder ，否则抛出异常。
	*/
	void
	operator()(TermNode&, ContextNode&) const;
};


//! \since build 726
template<typename _func>
inline void
RegisterFormContextHandler(ContextNode& node, const string& name, _func&& f)
{
	NPL::RegisterContextHandler(node, name, FormContextHandler(yforward(f)));
}

//! \since build 697
//@{
//! \brief 转换上下文处理器。
template<typename _func>
inline ContextHandler
ToContextHandler(_func&& f)
{
	return FunctionContextHandler(yforward(f));
}

/*!
\brief 注册函数上下文处理器。
\note 使用 ADL ToContextHandler 。
*/
template<typename _func>
inline void
RegisterFunction(ContextNode& node, const string& name, _func&& f)
{
	NPL::RegisterContextHandler(node, name, ToContextHandler(yforward(f)));
}
//@}

/*!
\brief 注册分隔符转换变换和处理例程。
\sa NPL::RegisterContextHandler
\sa ReplaceSeparatedChildren
\since build 726
*/
YF_API void
RegisterSequenceContextTransformer(EvaluationPasses&, ContextNode&,
	const string&, const ValueObject&);


//! \return \c false 表示总是不需要重规约。
//@{
/*!
\brief 检查非空项的首项并尝试按上下文列表求值。
\throw ListReductionFailure 规约失败：找不到可规约项。
\sa ContextHandler
\sa Reduce
\since build 697
*/
YF_API bool
EvaluateContextFirst(TermNode&, ContextNode&);

/*!
\brief 规约标识符：项作为标识符取对应的值并替换，并根据替换的值尝试以字面量处理。
\throw BadIdentifier 标识符未声明。
\note 不验证标识符是否为字面量；仅以字面量处理时可能需要重规约。
\sa FetchValue
\sa LiteralHandler
\since build 726
*/
YF_API bool
EvaluateIdentifier(TermNode&, ContextNode&, const string&);
//@}

} // namesapce A1;

} // namespace NPL;

#endif

