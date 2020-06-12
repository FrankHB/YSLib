/*
	© 2014-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.h
\ingroup NPL
\brief NPLA1 公共接口。
\version r7925
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 17:58:24 +0800
\par 修改时间:
	2020-06-12 20:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#ifndef NPL_INC_NPLA1_h_
#define NPL_INC_NPLA1_h_ 1

#include "YModules.h"
#include YFM_NPL_NPLA // for NPLATag, TermNode, ContextNode,
//	CombineReductionResult, ystdex::exclude_self_t, ystdex::ref_eq,
//	pmr::memory_resource, TNIter, LiftOther, ValueNode, LoggedEvent,
//	YSLib::Warning, std::declval, NPL::make_observer, NPL::Deref,
//	NPL::AsTermNode, std::make_move_iterator, IsBranch, std::next,
//	ystdex::retry_on_cond, std::find_if, ystdex::equality_comparable,
//	YSLib::AreEqualHeld, ystdex::make_parameter_list_t,
//	ystdex::make_function_type_t, ystdex::decay_t, ystdex::expanded_caller,
//	std::is_constructible, ystdex::or_, string_view, TermTags, TokenValue,
//	Environment, ParseResultOf, ByteParser, SourcedByteParser,
//	SourceInformation, std::integral_constant, SourceName;
#include YFM_YSLib_Core_YEvent // for YSLib::GHEvent, ystdex::fast_any_of,
//	YSLib::GEvent, YSLib::GCombinerInvoker, YSLib::GDefaultLastValueInvoker;
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;
#include <ystdex/algorithm.hpp> // for ystdex::split;
#include <ystdex/scope_guard.hpp> // for ystdex::guard;

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


/*!
\brief 续延。
\warning 非虚析构。
\since build 841
\todo 支持一等续延捕获。
*/
class YF_API Continuation
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
	template<typename _func, yimpl(typename
		= ystdex::exclude_self_t<Continuation, _func>)>
	inline
	Continuation(_func&& handler, const ContextNode& ctx)
		: Continuation(yforward(handler), ctx.get_allocator())
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

	friend PDefHOp(bool, ==, const Continuation& x, const Continuation& y)
		ynothrow
		ImplRet(ystdex::ref_eq<>()(x, y))

	//! \since build 877
	ReductionStatus
	operator()(ContextNode&) const;

	friend DefSwap(ynothrow, Continuation, swap(_x.Handler, _y.Handler))
};


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
	// XXX: Allocators are not used here for performance.
	//! \brief 叶遍。
	EvaluationPasses EvaluateLeaf{};
	//! \brief 列表遍。
	EvaluationPasses EvaluateList{};
	//! \brief 字面量遍。
	LiteralPasses EvaluateLiteral{};
	//! \brief 守卫遍。
	GuardPasses Guard{};
	/*!
	\brief NPLA1 表达式节点一次规约续延。
	\pre 规约函数第二参数引用的对象是 NPLA1 上下文状态或 public 继承的派生类。
	\pre 若修改规约函数实现，应和 DefaultReduceOnce 具有相同的求值语义。
	\since build 877
	*/
	Continuation ReduceOnce{DefaultReduceOnce, *this};

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
	\since build 883
	*/
	PDefH(void, SetNextTermRef, TermNode& term) ynothrow
		ImplExpr(next_term_ptr = NPL::make_observer(&term))

	//! \brief 清除下一项指针。
	PDefH(void, ClearNextTerm, ) ynothrow
		ImplExpr(next_term_ptr = {})

	/*!
	\brief NPLA1 表达式节点一次规约默认实现：调用求值例程规约子表达式。
	\pre 第二参数引用的对象是 NPLA1 上下文状态或 public 继承的派生类。
	\return 规约状态。
	\note 异常安全为调用遍的最低异常安全保证。
	\note 可能使参数中容器的迭代器失效。
	\note 默认不需要重规约。这可被求值遍改变。
	\note 可被求值遍调用以实现递归求值。
	\warning 若不满足上下文状态类型要求，行为未定义。
	\sa EvaluateLeaf
	\sa EvaluateList
	\sa ReduceAgain
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
	对枝节点调用 EvaluateList 求值；
	对空节点或值数据成员为 ValueToken 类型的值的叶节点不进行操作；
	对其它叶节点调用 EvaluateList 求值。
	支持重规约。异步重规约由 ContextNode 支持。
	此处约定的迭代中对节点的具体结构分类默认也适用于其它 NPLA1 实现 API ；
	例外情况应单独指定明确的顺序。
	例外情况包括输入节点不是表达式语义结构（而是 AST ）的 API ，如 TransformNode 。
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

	friend PDefH(void, swap, ContextState& x, ContextState& y) ynothrow
		ImplExpr(swap(static_cast<ContextNode&>(x),
			static_cast<ContextNode&>(y)), swap(x.next_term_ptr,
			y.next_term_ptr), swap(x.EvaluateLeaf, y.EvaluateLeaf),
			swap(x.EvaluateList, y.EvaluateList),
			swap(x.EvaluateLiteral, y.EvaluateLiteral), swap(x.Guard, y.Guard))
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

//! \return 若支持异步规约则为 ReductionStatus::Partial ，否则同 Reduce 。
//@{
/*!
\brief 再次规约。
\return ReductionStatus::Partial 。
\sa ReduceOnce
\sa RelayNext
\sa RelaySwitched
\since build 807

确保再次 ReduceOnce 调用并返回要求重规约的结果。
若使用异步实现，指定重规约，且可在尾上下文支持尾调用优化；否则同 ReduceOnce 。
*/
YF_API ReductionStatus
ReduceAgain(TermNode&, ContextNode&);

/*!
\brief 再次规约提升后的项。
\pre 间接断言：第一和第三参数指定不相同的项。
\sa LiftOther
\sa ReduceAgain
\since build 869

调用 LiftOther 提升项，再调用 ReduceAgain 规约。
*/
inline PDefH(ReductionStatus, ReduceAgainLifted, TermNode& term,
	ContextNode& ctx, TermNode& tm)
	ImplRet(LiftOther(term, tm), ReduceAgain(term, ctx))
//@}

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
\sa ReduceOnce
\see https://en.wikipedia.org/wiki/Fexpr 。
\since build 730

快速严格性分析：
当节点为分支列表的节点时，无条件求值第一项以避免非确定性推断子表达式求值的附加复杂度；
否则，返回 ReductionStatus::Regular 。
调用 ReduceOnce 规约子项。
*/
YF_API ReductionStatus
ReduceFirst(TermNode&, ContextNode&);

/*!
\brief NPLA1 表达式节点一次规约：转发调用到 NPLA1 表达式节点一次规约续延并调用。
\pre 第二参数引用的对象是 NPLA1 上下文状态或 public 继承的派生类。
\return 规约状态。
\note 默认实现由 ContextState::DefaultReduceOnce 提供。
\note 异常安全和参数的要求同默认实现。
\warning 若不满足上下文状态类型要求，行为未定义。
\sa ContextState::DefaultReduceOnce
\sa ContextState::ReduceOnce
\since build 806

转换第二参数为 NPLA1 上下文状态引用，访问其中的 NPLA1 表达式节点一次规约续延并调用。
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
//@}


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
		using it_t = decltype(std::make_move_iterator(term.begin()));

		return AddRange([&](TermNode& res, it_t b, it_t e){
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
		}, yforward(term), pfx, filter);
	}

	template<typename _func, class _tTerm, class _fPred>
	YB_ATTR_nodiscard TermNode
	AddRange(_func add_range, _tTerm&& term, const ValueObject& pfx,
		_fPred filter) const
	{
		using it_t = decltype(std::make_move_iterator(term.begin()));
		const auto a(term.get_allocator());
		auto res(NPL::AsTermNode(a, yforward(term).Value));

		if(IsBranch(term))
		{
			res.Add(NPL::AsTermNode(a, pfx));
			ystdex::split(std::make_move_iterator(term.begin()),
				std::make_move_iterator(term.end()), filter,
				[&](it_t b, it_t e){
				add_range(res, b, e);
			});
		}
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
	using BaseType = typename _tDst::BaseType;

	// XXX: Ditto
	return A1::WrapContextHandler<_tDst>(yforward(h), a, ystdex::or_<
		std::is_constructible<BaseType, _func>,
		std::is_constructible<BaseType, ystdex::expanded_caller<
		typename _tDst::FuncType, ystdex::decay_t<_func>>>>());
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
public:
	ContextHandler Handler;
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
	//! \since build 886
	template<typename _func, class _tAlloc>
	FormContextHandler(std::allocator_arg_t, const _tAlloc& a, _func&& f,
		size_t n = 0)
		: Handler(std::allocator_arg, a, A1::WrapContextHandler<ContextHandler>(
		yforward(f), a)), Wrapping(n)
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
	\note 断言调用 Handler 前的项符合 IsBranchedList 。
	\warning 要求异步实现中对 Handler 调用时保证此对象生存期，否则行为未定义。
	\sa ReduceArguments
	\sa Wrapping
	\since build 751

	对每一个子项求值，重复 Wrapping 次；
	然后断言检查项，对可调用的项调用 Hanlder 。
	项检查不存在或在检查通过后，变换无参数规约，然后对节点调用 Hanlder ，
		否则抛出异常。
	无参数时第一参数应具有两个子项且第二项为空节点。
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


//! \since build 871
//@{
/*!
\brief 包装数种类枚举。
\note 用于指定创建上下文处理器的种类。
*/
enum WrappingKind : decltype(FormContextHandler::Wrapping)
{
	//! \brief 一般形式：不对参数求值。
	Form = 0,
	//! \brief 严格求值形式：对参数求值 1 次。
	Strict = 1
};


/*!
\brief 注册一般形式上下文处理器。
\pre 间接断言：第二参数的数据指针非空。
*/
//! \brief 注册一般形式上下文处理器。
template<size_t _vWrapping = Strict, class _tTarget, typename... _tParams>
inline void
RegisterHandler(_tTarget& target, string_view name, _tParams&&... args)
{
	NPL::EmplaceLeaf<ContextHandler>(target, name,
		FormContextHandler(yforward(args)..., _vWrapping));
}
//@}

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
	A1::RegisterHandler<Form>(target, name, yforward(args)...);
}

//! \brief 注册严格上下文处理器。
template<class _tTarget, typename... _tParams>
inline void
RegisterStrict(_tTarget& target, string_view name, _tParams&&... args)
{
	A1::RegisterHandler<>(target, name, yforward(args)...);
}
//@}


/*!
\brief 取项的参数个数：子项数减 1 。
\pre 间接断言：参数指定的项是枝节点。
\return 项作为列表操作数被匹配的最大实际参数的个数。
\since build 733
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(size_t, FetchArgumentN, const TermNode& term) ynothrowv
	ImplRet(AssertBranch(term), term.size() - 1)


//! \pre 间接断言：字符串参数的数据指针非空。
//@{
/*!
\brief 分析参数指定的叶节点词素。
\since build 880

依次进行以下求值操作。
对代码字面量，去除字面量边界分隔符后进一步求值。
对数据字面量，去除字面量边界分隔符作为字符串值。
对其它字面量，规约为符号。
*/
YF_API TermNode
ParseLeaf(string_view, TermNode::allocator_type);

/*!
\brief 分析参数指定的带有源代码信息和叶节点词素。
\pre 间接断言：字符串参数的数据指针非空。
\since build 891

同 ParseLeaf ，但同时在分析结果的记号中包含源代码信息。
第二参数表示源代码来源。
第三参数表示记号在源代码中的位置。
*/
YF_API TermNode
ParseLeafWithSourceInformation(string_view, const shared_ptr<string>&,
	const SourceLocation&, TermNode::allocator_type);
//@}

/*!
\pre 间接断言：字符串参数的数据指针非空。
\note 第一参数指定输入的项，其 Value 指定输出的值。
*/
//@{
/*!
\brief 字面量和数值的叶节点求值默认实现。
\return 是否成功求值得到字面量或数值。
\throw InvalidSyntax 语法错误：第二参数是不被支持的数值。
\sa ContextState::EvaluateLiteral
\since build 880

判断第二参数是否可被解析和为被 NPLA1 对象语言语法支持的字面量或数值。
返回 ReductionStatus::Clean 表示成功求值。
返回 ReductionStatus::Retrying 表示不成功求值，需进一步被作为符号处理。
可实现字面量遍。
*/
YF_API ReductionStatus
DefaultEvaluateLeaf(TermNode&, string_view);

//! \exception BadIdentifier 未在环境中找到指定标识符的绑定。
//@{
/*!
\brief 求值标识符。
\note 不验证标识符是否为字面量；仅以字面量处理时可能需要重规约。
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
\throw InvalidSyntax 语法错误：作为符号的记号是不被支持的扩展字面量。
\warning 若不满足上下文状态类型要求，行为未定义。
\sa CategorizeLexeme
\sa ContextNode::EvaluateLiteral
\sa DeliteralizeUnchecked
\sa EvaluateIdentifier
\since build 736

处理非空字符串表示的节点记号。
被处理的记号视为符号。
若字面量遍非空，通过调用字面量遍处理记号。
若字面量遍为空或要求重规约，且记号不是不被支持的扩展字面量，求值非字面量的标识符。
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
		if(const auto p_handler = NPL::TryAccessTerm<const LiteralHandler>(tm))
			RegularizeTerm(term, (*p_handler)(ctx));
	}())

/*!
\brief 规约合并项：检查项的第一个子项尝试作为操作符进行函数应用，并规范化。
\pre 断言：若第一个子项表示子对象引用，则符合子对象引用的非正规表示约定。
\return 规约状态。
\exception bad_any_cast 异常中立：子对象引用持有的值不是 ContextHandler 类型。
\throw ListReductionFailure 规约失败：枝节点的第一个子项不表示上下文处理器。
\sa ContextHandler
\sa Reduce
\sa TermTags::Temporary
\since build 766

对枝节点尝试以第一个子项的 Value 数据成员为上下文处理器并调用，且当规约终止时规范化；
否则视为规约成功，没有其它作用。
若发生 ContextHandler 调用，调用前先转移处理器保证生存期，
	以允许处理器内部移除或修改之前占用的第一个子项（包括其中的 Value 数据成员）。
在被规约的项没有取得范式时，标记临时对象标签以允许转移作为操作符的函数右值。
*/
//@{
YF_API ReductionStatus
ReduceCombined(TermNode&, ContextNode&);

/*!
\brief 规约列表合并项：同 ReduceCombined ，但只适用于枝节点。
\pre 断言：项满足 IsBranchedList 。
\since build 882
*/
YF_API ReductionStatus
ReduceCombinedBranch(TermNode&, ContextNode&);

/*!
\brief 规约列表合并项：同 ReduceCombinedBranch ，但先设置 NPLA1 上下文的下一项。
\pre 断言：项满足 IsBranchedList 。
\since build 884
*/
YB_FLATTEN inline PDefH(ReductionStatus, ReduceNextCombinedBranch,
	TermNode& term, ContextState& cs)
	ImplRet(cs.SetNextTermRef(term), ReduceCombinedBranch(term, cs))

/*!
\brief 规约列表合并项：同 ReduceCombined ，但使用第三参数指定的值。
\note 若第三参数不表示上下文处理器的宿主值，抛出的异常消息指定其为引用项。
\since build 883
*/
YF_API ReductionStatus
ReduceCombinedReferent(TermNode&, ContextNode&, const TermNode&);
//@}

/*!
\brief 规约提取名称的叶节点记号。
\exception BadIdentifier 标识符未声明。
\note 忽略名称查找失败，默认结果为 ReductionStatus::Retained 。
\sa EvaluateLeafToken
\sa ReduceAgain
\sa TermToNamePtr
*/
YF_API ReductionStatus
ReduceLeafToken(TermNode&, ContextNode&);
//@}


//! \since build 876
//@{
/*!
\brief 求值环境守卫。

作用域退出时调用环境切换器恢复保存的环境的作用域守卫。
*/
using EnvironmentGuard = ystdex::guard<EnvironmentSwitcher>;


/*!
\pre ContextNode& 类型的参数引用的对象是 NPLA1 上下文状态或 public 继承的派生类。
\pre 对 TCO 实现，存在 TCOAction 当前动作。
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
\brief 函数调用规约（ β-规约）。
\pre 不存在临时函数或已通过 ReduceCombined 的内部实现调用被适当保存。
\since build 876
*/
YF_API ReductionStatus
RelayForCall(ContextNode&, TermNode&, EnvironmentGuard&&, bool);
//@}
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
\brief 匹配参数。
\exception std::bad_function_call 异常中立：参数指定的处理器为空。
\sa TermTags
\since build 882

进行匹配的算法递归搜索形式参数及其子项。
若匹配成功，调用参数指定的匹配处理器。
参数指定形式参数、实际参数、两个处理器、绑定选项和引用值关联的环境。
其中，形式参数被视为作为形式参数树的右值。
绑定选项以 TermTags 编码，但含义和作用在项上时不完全相同：
Unique 表示唯一引用项（在此即消亡值值）；
Nonmodifying 表示需要复制；
Temporary 表示不被共享的项（在此即纯右值或没有匹配列表的引用值）。
当需要复制时，递归处理的所有对实际参数的绑定以复制代替转移；
可能被共享的项在发现表达数树中存在需被匹配的列表后失效，对之后的子项进行递归处理。
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
	TokenValue&, TermTags, const EnvironmentReference&)>,
	function<void(const TokenValue&, TermNode&, TermTags,
	const EnvironmentReference&)>, TermTags, const EnvironmentReference&);

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
//@}
//@}


/*!
\brief 设置默认解释：解释使用的公共处理遍。
\note 非强异常安全：加入遍可能提前设置状态而不在失败时回滚。
\sa ReduceCombined
\sa ReduceFirst
\sa ReduceHeadEmptyList
\sa ReduceLeafToken
\since build 842

设置默认解释的求值遍到第一参数指定的上下文的列表求值遍和叶求值遍中。
列表求值遍等效第二参数后依次添加 ReduceFirst 、ReduceHeadEmptyList 和
	ReduceCombined ，但不保证分别作为单独的遍添加。
叶求值遍设置为 ReduceLeafToken 。
*/
YF_API void
SetupDefaultInterpretation(ContextState&, EvaluationPasses);


/*!
\ingroup metafunctions
\since build 891
*/
//@{
//! \brief 解析结果元素类型。
template<typename _fParse>
using GParsedValue = typename ParseResultOf<_fParse>::value_type;

//! \brief 泛型标记器：分析解析结果元素转换为可能包含记号的节点。
template<typename _fParse>
using GTokenizer = function<TermNode(const GParsedValue<_fParse>&)>;
//@}

/*!
\brief 标记器：分析词素转换为可能包含记号的节点。
\since build 880
*/
using Tokenizer = GTokenizer<ByteParser>;

//! \brief 标记器：分析带有源代码位置信息的词素转换为可能包含记号的节点。
using SourcedTokenizer = GTokenizer<SourcedByteParser>;


/*!
\brief 取对象中的源代码信息。
\since build 891
*/
YB_ATTR_nodiscard YB_PURE YF_API observer_ptr<const SourceInformation>
QuerySourceInformation(const ValueObject&);

/*!
\brief 取需要对象中的名称信息。
\return 若存在名称则为内部指定来源的名称字符字符串，否则是数据指针为空的结果。
\since build 892
*/
YB_ATTR_nodiscard YB_PURE YF_API string_view
QueryContinuationName(const Continuation&);


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

private:
	struct LeafConverter final
	{
		const REPLContext& Context;

		PDefHOp(TermNode, (), const GParsedValue<ByteParser>& val) const
			ImplRet(Context.ConvertLeaf(val))
		PDefHOp(TermNode, (), const GParsedValue<SourcedByteParser>& val) const
			ImplRet(Context.ConvertLeafSourced(val))
	};

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
	/*!
	\brief 预处理例程：每次翻译时预先处理调用的公共例程。
	\sa Prepare
	\since build 888

	在遍历节点时调用的预处理例程。
	只保留一个遍的处理器，以减小一般情形（包括 NPLA1 默认情形）的开销。
	若需要多个遍，可使用 TermPasses 作为目标。
	*/
	TermPasses::HandlerType Preprocess{std::allocator_arg, Allocator};
	//! \brief 列表项处理例程：每次翻译中规约回调处理调用的公共例程。
	EvaluationPasses ListTermPreprocess{Allocator};
	//! \since build 891
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
	//! \brief 当前源代码名称。
	SourceName CurrentSource{};
	/*!
	\brief 默认启用源代码位置。
	\sa Perform
	*/
	bool UseSourceLocation = {};
	//@}

	/*!
	\sa ListTermPreprocess
	\sa SetupDefaultInterpretation
	*/
	//@{
	/*!
	\brief 构造：使用默认的解释、指定的存储资源和默认的叶节点词素转换器。
	\sa ParseLeaf
	\since build 879
	*/
	REPLContext(pmr::memory_resource& = NPL::Deref(pmr::new_delete_resource()));
	/*!
	\brief 构造：使用默认的解释、指定的存储资源和叶节点词素转换器。
	\since build 891
	*/
	REPLContext(Tokenizer, SourcedTokenizer,
		pmr::memory_resource& = NPL::Deref(pmr::new_delete_resource()));
	//@}

private:
	//! \since build 891
	template<typename _tParam>
	YB_ATTR_nodiscard inline ContextNode&
	FetchContextParameter(_tParam&&)
	{
		return Root;
	}
	//! \since build 891
	template<typename... _tParams>
	YB_ATTR_nodiscard inline ContextNode&
	FetchContextParameter(std::tuple<_tParams..., ContextNode&> args)
	{
		return std::get<sizeof...(_tParams)>(args);
	}

public:
	/*!
	\brief 判断当前实现是否为异步实现。
	\since build 879

	判断当前实现是否支持在本机实现中使用异步调用。
	若不支持同步调用，则只应使用同步调用的本机实现。
	*/
	YB_STATELESS bool
	IsAsynchronous() const ynothrow;

	/*!
	\exception std::invalid_argument 异常中立：由 ReadFrom 抛出。
	\sa ReadFrom
	\sa Reduce
	\since build 891
	*/
	//@{
	//! \brief 加载：从指定参数指定的来源读取并翻译源代码。
	//@{
	template<typename... _tParams>
	inline void
	LoadFrom(_tParams&&... args)
	{
		auto term(ReadFrom(yforward(args)...));

		Reduce(term,
			FetchContextParameter(std::forward_as_tuple(yforward(args)...)));
	}
	//@}

	//! \brief 执行循环：从指定参数指定的来源读取并翻译源代码，并返回处理结果。
	template<typename... _tParams>
	// XXX: No %YB_ATTR_nodiscard.
	inline TermNode
	Perform(_tParams&&... args)
	{
		auto term(ReadFrom(yforward(args)...));

		Reduce(term,
			FetchContextParameter(std::forward_as_tuple(yforward(args)...)));
		return term;
	}
	//@}

	/*!
	\brief 准备规约项：分析输入并标记记号节点和预处理。
	\return 预处理后的项。
	\sa Preprocess
	\since build 890

	按需分析并调用预处理例程。
	词素的处理由分析完成，不需单独调用 TokenizeTerm 转换。
	*/
	//@{
	YB_ATTR_nodiscard PDefH(TermNode, Prepare, TermNode&& term) const
		ImplRet(Preprocess(term), std::move(term))
	/*!
	\return 从参数输入读取的准备的项。
	\sa SContext::Analyze
	*/
	template<typename... _tParams>
	YB_ATTR_nodiscard TermNode
	Prepare(_tParams&&... args) const
	{
		return Prepare(SContext::Analyze(std::allocator_arg, Allocator,
			LeafConverter{*this}, yforward(args)...));
	}
	//@}

	/*!
	\brief 读取：从指定参数指定的来源输入源代码并准备规约项。
	\return 从参数输入读取的准备的项。
	\sa Prepare
	\since build 891
	*/
	//@{
	template<class _type>
	YB_ATTR_nodiscard inline TermNode
	ReadFrom(_type&& input)
	{
		return ReadFrom(yforward(input), Root);
	}
	template<class _type>
	YB_ATTR_nodiscard inline TermNode
	ReadFrom(_type&& input, ContextNode& ctx) const
	{
		return ReadFrom(LoadOptionTag<>(), yforward(input), ctx);
	}
	template<LoadOption _vOpt, class _type>
	YB_ATTR_nodiscard inline TermNode
	ReadFrom(LoadOptionTag<_vOpt> opt, _type&& input)
	{
		return ReadFrom(opt, yforward(input), Root);
	}
	//! \throw std::invalid_argument 流状态错误或缓冲区不存在。
	template<LoadOption _vOpt>
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<_vOpt> opt, std::istream& is, ContextNode& ctx) const
	{
		if(is)
		{
			if(const auto p = is.rdbuf())
				return ReadFrom(opt, *p, ctx);
			throw std::invalid_argument("Invalid stream buffer found.");
		}
		else
			throw std::invalid_argument("Invalid stream found.");
	}
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<>, std::streambuf&, ContextNode&) const;
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<WithSourceLocation>, std::streambuf&, ContextNode&)
		const;
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<NoSourceInformation>, std::streambuf&, ContextNode&)
		const;
	//! \pre 断言：字符串的数据指针非空。
	//@{
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<>, string_view, ContextNode&) const;
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<WithSourceLocation>, string_view, ContextNode&)
		const;
	YB_ATTR_nodiscard TermNode
	ReadFrom(LoadOptionTag<NoSourceInformation>, string_view, ContextNode&)
		const;
	//@}
	//@}
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

} // namesapce A1;

} // namespace NPL;

#endif

