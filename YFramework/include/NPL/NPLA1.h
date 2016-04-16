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
\version r1079
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 17:58:24 +0800
\par 修改时间:
	2016-04-16 11:10 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#ifndef NPL_INC_NPLA1_h_
#define NPL_INC_NPLA1_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA // for NPLATag, ValueNode, TermNode, LoggedEvent;
#include YFM_YSLib_Core_YEvent // for YSLib::GHEvent, ystdex::fast_any_of,
//	ystdex::indirect, YSLib::GEvent, YSLib::GCombinerInvoker,
//	YSLib::GDefaultLastValueInvoker;
#include <ystdex/any.h> // for ystdex::any;

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


//! \since build 674
//@{
//! \brief 上下文节点类型。
using ContextNode = ValueNode;
//! \since build 685
using YSLib::AccessChildPtr;
//! \brief 上下文处理器类型。
using ContextHandler = YSLib::GHEvent<void(TermNode&, ContextNode&)>;
//! \brief 字面量处理器类型。
using LiteralHandler = YSLib::GHEvent<bool(const ContextNode&)>;

//! \brief 注册上下文处理器。
inline PDefH(void, RegisterContextHandler, ContextNode& node,
	const string& name, ContextHandler f)
	ImplExpr(node[name].Value = f)

//! \brief 注册字面量处理器。
inline PDefH(void, RegisterLiteralHandler, ContextNode& node,
	const string& name, LiteralHandler f)
	ImplExpr(node[name].Value = f)


//! \brief 形式上下文处理器。
class YF_API FormContextHandler
{
public:
	ContextHandler Handler;

	template<typename _func>
	FormContextHandler(_func f)
		: Handler(f)
	{}

	/*!
	\brief 处理函数。
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
//@}


//! \since build 675
//@{
//! \brief 从指定上下文取指定名称指称的值。
YF_API ValueObject
FetchValue(const ContextNode&, const string&);

//! \brief 从指定上下文查找名称对应的节点。
YF_API observer_ptr<const ValueNode>
LookupName(const ContextNode&, const string&) ynothrow;
//@}


//! \since build 676
//@{
/*!
\brief 移除节点的空子节点，然后判断是否可继续规约。
\return 可继续规约：第二参数为 true 且移除空子节点后的节点非空。
*/
YF_API bool
DetectReducible(TermNode&, bool);


/*!
\brief 遍合并器：逐次调用直至返回 true 。
\note 合并遍结果用于表示及早判断是否应继续规约，可在循环中实现再次规约一个项。
*/
struct PassesCombiner
{
	template<typename _tIn>
	bool
	operator()(_tIn first, _tIn last) const
	{
		return ystdex::fast_any_of(first, last, ystdex::indirect<>());
	}
};


//! \brief 一般合并遍。
template<typename... _tParams>
using GPasses = YSLib::GEvent<bool(_tParams...),
	YSLib::GCombinerInvoker<bool, PassesCombiner>>;
//! \brief 项合并遍。
using TermPasses = GPasses<TermNode&>;
//! \brief 求值合并遍。
using EvaluationPasses = GPasses<TermNode&, ContextNode&>;


//! \brief 作用域守护类型。
using Guard = ystdex::any;
/*!
\brief 作用域守护遍：用于需在规约例程的入口和出口关联执行的操作。
\todo 支持迭代使用旧值。
*/
using GuardPasses = YSLib::GEvent<Guard(TermNode&, ContextNode&),
	YSLib::GDefaultLastValueInvoker<Guard>>;


//! \brief 使用第二个参数指定的项的内容替换第一个项的内容。
inline PDefH(void, LiftTerm, TermNode& term, TermNode& tm)
	ImplExpr(TermNode(std::move(tm)).SwapContent(term))
//@}

/*!
\brief 使用首个子项替换项的内容。
\since build 685
*/
inline PDefH(void, LiftFirst, TermNode& term)
	ImplExpr(LiftTerm(term, Deref(term.begin())))


//! \since build 685
//@{
//! \brief 调用处理遍：从指定名称的节点中访问指定类型的遍并以指定上下文调用。
template<class _tPasses>
typename _tPasses::result_type
InvokePasses(TermNode& term, ContextNode& ctx, const string& name)
{
	return ystdex::call_value_or<typename _tPasses::result_type>(
		[&](_tPasses& passes){
		return passes(term, ctx);
	}, AccessChildPtr<_tPasses>(ctx, name));
}


//! \brief 访问守护遍。
YF_API GuardPasses&
AccessGuardPassesRef(ContextNode&);

//! \brief 访问叶节点遍。
YF_API EvaluationPasses&
AccessLeafPassesRef(ContextNode&);

//! \brief 访问列表节点遍。
YF_API EvaluationPasses&
AccessListPassesRef(ContextNode&);

//! \brief 求值守护。
YF_API Guard
EvaluateGuard(TermNode& term, ContextNode&);

//! \brief 求值叶节点遍。
YF_API bool
EvaluateLeafPasses(TermNode& term, ContextNode&);

//! \brief 求值列表节点遍。
YF_API bool
EvaluateListPasses(TermNode& term, ContextNode&);


/*!
\brief NPLA1 节点规约。
\note 可能使参数中容器的迭代器失效。
\return 确定是否需要重新规约。
\sa DetectReducible
\sa EvaluateGuard
\sa EvaluateLeafPasses
\sa EvaluateListPasses
\sa ValueToken
\todo 实现 ValueToken 保留处理。

规约表达式节点：调用至少一次求值例程递归规约子表达式。
调用 EvaluateGuard 进行必要的上下文重置。
对非空列表节点调用 EvaluateListPasses 求值。
对空表节点替换为 ValueToken::Null 。
对已替换为 ValueToken 的叶节点保留处理。 
对其它叶节点调用 EvaluateLeafPasses 求值。
单一求值的结果作为 DetectReducible 的第二参数，根据结果判断是否重新规约。
*/
YF_API bool
Reduce(TermNode&, ContextNode&);

/*!
\brief 对容器中的第二项开始逐项规约。
\throw LoggedEvent 错误：容器内的子表达式不大于一项。
\note 语言规范指定规约顺序不确定。
\note 可能使参数中容器的迭代器失效。
\sa Reduce
*/
YF_API void
ReduceArguments(TermNode::Container&, ContextNode&);


/*!
\brief 设置跟踪深度节点：调用规约时显示深度和上下文等信息。
\note 主要用于调试。
\sa EvaluateGuard
*/
YF_API void
SetupTraceDepth(ContextNode& ctx, const string& name = yimpl("$__depth"));
//@}

} // namesapce A1;

} // namespace NPL;

#endif

