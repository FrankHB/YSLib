/*
	© 2012-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SContext.h
\ingroup NPL
\brief S 表达式上下文。
\version r2402
\author FrankHB <frankhb1989@gmail.com>
\since build 304
\par 创建时间:
	2012-08-03 19:55:41 +0800
\par 修改时间:
	2019-02-15 00:26 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SContext
*/


#ifndef NPL_INC_SContext_h_
#define NPL_INC_SContext_h_ 1

#include "YModules.h"
#include YFM_NPL_Lexical
#include YFM_YSLib_Core_ValueNode // for YSLib::Deref, YSLib::MakeIndex,
//	YSLib::NoContainer, YSLib::NoContainerTag, YSLib::ValueObject,
//	YSLib::ValueNode, list, YSLib::LoggedEvent, YSLib::make_observer,
//	YSLib::make_pair, YSLib::observer_ptr, YSLib::pair, YSLib::ListContainerTag,
//	std::initializer_list, ystdex::forward_like, ystdex::invoke,
//	YSLib::AccessPtr, ystdex::false_, std::is_convertible, ystdex::decay_t,
//	ystdex::bool_, ystdex::cond_or_t, ystdex::not_, ystdex::enable_if_t,
//	ystdex::call_value_or, ystdex::addrof, ystdex::compose;

namespace NPL
{

//! \since build 853
using YSLib::Deref;
//! \since build 599
using YSLib::MakeIndex;
//! \since build 852
using YSLib::NoContainer;
//! \since build 852
using YSLib::NoContainerTag;
//! \since build 675
using YSLib::ValueObject;
//! \since build 335
using YSLib::ValueNode;
using TokenList = list<string>;
using TLIter = TokenList::iterator;
using TLCIter = TokenList::const_iterator;
//! \since build 674
//@{
using YSLib::LoggedEvent;
//! \since build 852
using YSLib::make_observer;
//! \since build 852
using YSLib::make_pair;
using YSLib::observer_ptr;
//! \since build 598
using YSLib::pair;


/*!
\brief 项节点：存储语法分析结果的值类型节点。
\since build 852

类似 ValueNode 的节点类型，但没有名称数据成员、按键比较和按键访问，
	且使用 list 而不是 ystdex::mapped_set 作为容器。
*/
class YF_API TermNode
{
private:
	// TODO: Deduplicate within %ValueNode?
	template<typename... _tParams>
	using enable_value_constructible_t = ystdex::enable_if_t<
		std::is_constructible<ValueObject, _tParams...>::value>;

public:
	using Container = list<TermNode>;
	using allocator_type = Container::allocator_type;
	using iterator = Container::iterator;
	using const_iterator = Container::const_iterator;
	using reverse_iterator = Container::reverse_iterator;
	using const_reverse_iterator = Container::const_reverse_iterator;

private:
	//! \since build 853
	//@{
	Container container{};

public:
	ValueObject Value{};
	//@}

	DefDeCtor(TermNode)
	// XXX: Not all constructors like %ValueNode need to be supported here.
	//! \note 部分构造函数和 ValueNode 类似。
	//@{
	explicit
	TermNode(allocator_type a)
		: container(a)
	{}
	TermNode(const Container& con)
		: container(con)
	{}
	TermNode(Container&& con)
		: container(std::move(con))
	{}
	//! \since build 853
	//@{
	template<typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	inline
	TermNode(NoContainerTag, _tParams&&... args)
		: Value(yforward(args)...)
	{}
	template<typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	TermNode(const Container& con, _tParams&&... args)
		: container(con), Value(yforward(args)...)
	{}
	template<typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	TermNode(Container&& con, _tParams&&... args)
		: container(std::move(con)), Value(yforward(args)...)
	{}
	template<typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	inline
	TermNode(std::allocator_arg_t, allocator_type a, NoContainerTag,
		_tParams&&... args)
		: container(a), Value(yforward(args)...)
	{}
	template<typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	inline
	TermNode(std::allocator_arg_t, allocator_type a, const Container& con,
		_tParams&&... args)
		: container(con, a), Value(yforward(args)...)
	{}
	template<typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	inline
	TermNode(std::allocator_arg_t, allocator_type a, Container&& con,
		_tParams&&... args)
		: container(std::move(con), a), Value(yforward(args)...)
	{}
	//@}
	//@}
	TermNode(const ValueNode& nd, allocator_type a)
		: container(ConCons(nd.GetContainer(), a)), Value(nd.Value)
	{}
	TermNode(ValueNode&& nd, allocator_type a)
		: container(ConCons(std::move(nd.GetContainerRef()), a)),
		Value(std::move(nd.Value))
	{}
	//@}
	// XXX: This needs tag to avoid clash with other constructors.
	TermNode(YSLib::ListContainerTag, std::initializer_list<TermNode> il)
		: container(il)
	{}
	TermNode(std::initializer_list<TermNode> il, allocator_type a)
		: container(il, a)
	{}
	//! \since build 853
	template<typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	inline
	TermNode(std::allocator_arg_t, allocator_type a,
		std::initializer_list<TermNode> il, _tParams&&... args)
		: TermNode(std::allocator_arg, a, Container(il, a), yforward(args)...)
	{}
	explicit
	TermNode(const ValueNode& nd)
		: container(ConCons(nd.GetContainer())), Value(nd.Value)
	{}
	explicit
	TermNode(ValueNode&& nd)
		: container(ConCons(std::move(nd.GetContainer()))),
		Value(std::move(nd.Value))
	{}
	TermNode(const TermNode& tm, allocator_type a)
		: container(tm.container, a), Value(tm.Value)
	{}
	TermNode(TermNode&& tm, allocator_type a)
		: container(std::move(tm.container), a), Value(std::move(tm.Value))
	{}
	DefDeCopyMoveCtorAssignment(TermNode)

	//! \since build 853
	DefBoolNeg(explicit, bool(Value) || !empty())

	//! \since build 853
	//@{
	DefGetter(const ynothrow, const Container&, Container, container)
	DefGetter(ynothrow, Container&, ContainerRef, container)

	template<class _tCon, class _type>
	yimpl(ystdex::enable_if_t)<
		ystdex::and_<std::is_assignable<Container, _tCon&&>,
		std::is_assignable<ValueObject, _type&&>>::value>
	SetContent(_tCon&& con, _type&& val) ynoexcept(ystdex::and_<
		std::is_nothrow_assignable<Container, _tCon&&>,
		std::is_nothrow_assignable<ValueObject, _type&&>>())
	{
		container = yforward(con);
		Value = yforward(val);
	}
	PDefH(void, SetContent, const TermNode& term)
		ImplExpr(SetContent(term.container, term.Value))
	PDefH(void, SetContent, TermNode&& term)
		ImplExpr(SetContent(std::move(term.container), std::move(term.Value)))
	//@}

	//! \since build 853
	PDefH(void, Add, const TermNode& term)
		ImplExpr(container.push_back(term))
	//! \since build 853
	PDefH(void, Add, TermNode&& term)
		ImplExpr(container.push_back(std::move(term)))

	//! \since build 853
	//@{
	template<typename... _tParams>
	static inline void
	AddValueTo(Container& con, _tParams&&... args)
	{
		con.emplace_back(NoContainer, yforward(args)...);
	}
	template<typename... _tParams>
	static inline void
	AddValueTo(const_iterator position, Container& con, _tParams&&... args)
	{
		con.emplace(position, NoContainer, yforward(args)...);
	}

	PDefH(void, Clear, ) ynothrow
		ImplExpr(Value.Clear(), ClearContainer())

	PDefH(void, ClearContainer, ) ynothrow
		ImplExpr(container.clear())

	PDefH(void, ClearTo, const ValueObject& vo) ynothrow
		ImplExpr(ClearContainer(), Value = vo)
	PDefH(void, ClearTo, ValueObject&& vo) ynothrow
		ImplExpr(ClearContainer(), Value = std::move(vo))

private:
	static TermNode::Container
	ConCons(const ValueNode::Container&);
	static TermNode::Container
	ConCons(ValueNode::Container&&);
	static TermNode::Container
	ConCons(const ValueNode::Container&, allocator_type);
	static TermNode::Container
	ConCons(ValueNode::Container&&, allocator_type);

public:
	template<class _tCon, typename _fCallable,
		yimpl(typename = ystdex::enable_if_t<
		std::is_same<Container&, ystdex::remove_cvref_t<_tCon>&>::value>)>
	static Container
	CreateRecursively(_tCon&& con, _fCallable f)
	{
		Container res(con.get_allocator());

		for(auto&& tm : con)
			res.emplace_back(CreateRecursively(
				ystdex::forward_like<_tCon>(tm.container), f),
				ystdex::invoke(f, ystdex::forward_like<_tCon>(tm.Value)));
		return res;
	}
	//@}

	template<typename _fCallable>
	Container
	CreateWith(_fCallable f) &
	{
		return CreateRecursively(container, f);
	}
	template<typename _fCallable>
	Container
	CreateWith(_fCallable f) const&
	{
		return CreateRecursively(container, f);
	}
	template<typename _fCallable>
	Container
	CreateWith(_fCallable f) &&
	{
		return CreateRecursively(std::move(container), f);
	}
	template<typename _fCallable>
	Container
	CreateWith(_fCallable f) const&&
	{
		return CreateRecursively(std::move(container), f);
	}

	//! \since build 953
	void
	MoveContent(TermNode&&);

	PDefH(void, Remove, const_iterator i)
		ImplExpr(erase(i))

	PDefH(void, SwapContainer, TermNode& term) ynothrowv
		ImplExpr(YAssert(get_allocator() == term.get_allocator(),
			"Invalid allocator found."), container.swap(term.container))

	void
	SwapContent(TermNode&) ynothrowv;

	YB_ATTR_nodiscard YB_PURE PDefH(iterator, begin, ) ynothrow
		ImplRet(container.begin())
	YB_ATTR_nodiscard YB_PURE PDefH(const_iterator, begin, ) const ynothrow
		ImplRet(container.begin())

	//! \since build 853
	DefFwdTmpl(, iterator, emplace, (container.emplace_back(yforward(args)...),
		std::prev(container.end())))

	//! \since build 853
	YB_ATTR_nodiscard PDefH(bool, empty, ) const ynothrow
		ImplRet(container.empty())

	PDefH(iterator, erase, const_iterator i)
		ImplRet(container.erase(i))
	PDefH(iterator, erase, const_iterator first, const_iterator last)
		ImplRet(container.erase(first, last))

	YB_ATTR_nodiscard YB_PURE PDefH(iterator, end, ) ynothrow
		ImplRet(container.end())
	YB_ATTR_nodiscard YB_PURE PDefH(const_iterator, end, ) const ynothrow
		ImplRet(container.end())

	//! \since buid 853
	YB_ATTR_nodiscard YB_PURE
		PDefH(allocator_type, get_allocator, ) const ynothrow
		ImplRet(container.get_allocator())

	DefFwdTmpl(-> decltype(container.insert(yforward(args)...)), auto,
		insert, container.insert(yforward(args)...))

	YB_ATTR_nodiscard YB_PURE PDefH(reverse_iterator, rbegin, ) ynothrow
		ImplRet(container.rbegin())
	YB_ATTR_nodiscard YB_PURE
		PDefH(const_reverse_iterator, rbegin, ) const ynothrow
		ImplRet(container.rbegin())

	YB_ATTR_nodiscard YB_PURE PDefH(reverse_iterator, rend, ) ynothrow
		ImplRet(container.rend())
	YB_ATTR_nodiscard YB_PURE
		PDefH(const_reverse_iterator, rend, ) const ynothrow
		ImplRet(container.rend())

	//! \since build 853
	YB_ATTR_nodiscard YB_PURE PDefH(size_t, size, ) const ynothrow
		ImplRet(container.size())

	YF_API friend void
	swap(TermNode&, TermNode&) ynothrowv;
};

//! \relates TermNode
//@{
using TNIter = TermNode::iterator;
using TNCIter = TermNode::const_iterator;

/*!
\brief 项节点分类判断操作。
\since build 733
*/
//@{
YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsBranch, const TermNode& term)
	ynothrow
	ImplRet(!term.empty())

YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsEmpty, const TermNode& term)
	ynothrow
	ImplRet(!term)

YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsLeaf, const TermNode& term)
	ynothrow
	ImplRet(term.empty())

//! \since build 774
YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsList, const TermNode& term)
	ynothrow
	ImplRet(!term.empty() || !term.Value)
//@}

//! \since build 853
using YSLib::Access;
template<typename _type>
YB_ATTR_nodiscard inline _type&
Access(TermNode& term) ynothrow
{
	return term.Value.Access<_type>();
}
template<typename _type>
YB_ATTR_nodiscard inline const _type&
Access(const TermNode& term) ynothrow
{
	return term.Value.Access<_type>();
}

//! \since build 853
using YSLib::AccessPtr;
template<typename _type>
YB_ATTR_nodiscard inline observer_ptr<_type>
AccessPtr(TermNode& term) ynothrow
{
	return term.Value.AccessPtr<_type>();
}
template<typename _type>
YB_ATTR_nodiscard inline observer_ptr<const _type>
AccessPtr(const TermNode& term) ynothrow
{
	return term.Value.AccessPtr<_type>();
}

//! \since build 853
template<typename... _tParam, typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline
ystdex::enable_if_t<ystdex::not_<ystdex::cond_or_t<ystdex::bool_<
	(sizeof...(_tParams) >= 1)>, ystdex::false_, std::is_convertible,
	ystdex::decay_t<_tParams>..., TermNode::allocator_type>>::value, TermNode>
AsTermNode(_tParams&&... args)
{
	return {NoContainer, yforward(args)...};
}
//! \since build 853
template<typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline TermNode
AsTermNode(TermNode::allocator_type a, _tParams&&... args)
{
	return {std::allocator_arg, a, NoContainer, yforward(args)...};
}


//! \since build 853
inline PDefH(const string&, GetNodeNameOf, const ValueNode& node) ynothrow
	ImplRet(node.GetName())
//! \since build 853
inline PDefH(string, GetNodeNameOf, const TermNode&) ynothrow
	ImplRet(string("[TermNode]"))

// NOTE: Like %YSLib::GetValueOf.
YB_ATTR_nodiscard YB_PURE inline
	PDefH(ValueObject, GetValueOf, observer_ptr<const TermNode> p_term)
	ImplRet(ystdex::call_value_or(
		[](const TermNode& term) -> const ValueObject&{
		return term.Value;
	}, p_term))

// NOTE: Like %YSLib::GetValuePtrOf.
YB_ATTR_nodiscard YB_PURE inline PDefH(observer_ptr<const ValueObject>,
	GetValuePtrOf, observer_ptr<const TermNode> p_term)
	ImplRet(ystdex::call_value_or(ystdex::compose(make_observer<const
		ValueObject>, ystdex::addrof<>(),
		[](const TermNode& term) -> const ValueObject&{
		return term.Value;
	}), p_term))

//! \pre 断言：项节点容器非空。
inline PDefH(void, RemoveHead, TermNode& term) ynothrowv
	ImplExpr(YAssert(!term.empty(), "Empty term container found."),
		term.erase(term.begin()))

template<typename _fCallable, class _tNode>
void
SetContentWith(TermNode& dst, _tNode&& node, _fCallable f)
{
	// NOTE: Similar to %YSLib::SetContentWith.
	auto con(yforward(node).CreateWith(f));

	dst.GetContainerRef() = std::move(con);
	dst.Value = ystdex::invoke(f, yforward(node).Value);
}
//@}
//@}

/*!
\brief 遍历子节点。
\note 使用 ADL AccessPtr 。
\since build 803

遍历节点容器中的子节点。
直接作为节点容器访问。
*/
template<typename _fCallable, class _tNode>
void
TraverseSubnodes(_fCallable f, const _tNode& node)
{
	// TODO: Null coalescing or variant value?
	if(const auto p = AccessPtr<YSLib::NodeSequence>(node))
		for(const auto& nd : *p)
			ystdex::invoke(f, _tNode(nd));
	else
		for(const auto& nd : node)
			ystdex::invoke(f, nd);
}


/*!
\brief 断言枝节点。
\pre 断言：参数指定的项是枝节点。
\since build 761
*/
inline PDefH(void, AssertBranch, const TermNode& term,
	const char* msg = "Invalid term found.") ynothrowv
	ImplExpr(yunused(msg), YAssert(IsBranch(term), msg))


/*!
\brief 检查项节点是否具有指定的值。
\since build 753
*/
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline bool
HasValue(const TermNode& term, const _type& x)
{
	return term.Value == x;
}


/*!
\brief 会话：分析指定 NPL 代码。
\since build 304
*/
class YF_API Session
{
public:
	//! \since build 546
	//@{
	using CharParser = function<void(LexicalAnalyzer&, char)>;

	//! \since build 592
	LexicalAnalyzer Lexer;

	//! \since build 618
	DefDeCtor(Session)
	//! \throw LoggedEvent 关键失败：无法访问源内容。
	template<typename _tIn>
	Session(_tIn first, _tIn last, CharParser parse = DefaultParseByte)
		: Lexer()
	{
		std::for_each(first, last,
			std::bind(parse, std::ref(Lexer), std::placeholders::_1));
	}
	template<typename _tRange,
		yimpl(typename = ystdex::exclude_self_t<Session, _tRange>)>
	Session(const _tRange& c, CharParser parse = DefaultParseByte)
		: Session(ystdex::begin(c), ystdex::end(c), parse)
	{}
	DefDeCopyMoveCtorAssignment(Session)

	DefGetterMem(const ynothrow, const string&, Buffer, Lexer)
	//@}
	DefGetter(const, TokenList, TokenList, Tokenize(Lexer.Literalize()))

	/*!
	\brief 默认字符解析实现：直接使用 LexicalAnalyzer::ParseByte 。
	\since build 546
	*/
	static void
	DefaultParseByte(LexicalAnalyzer&, char);

	/*!
	\brief 默认字符解析实现：直接使用 LexicalAnalyzer::ParseQuoted 。
	\since build 546
	*/
	static void
	DefaultParseQuoted(LexicalAnalyzer&, char);
};


/*!
\brief S 表达式上下文：处理 S 表达式。
\since build 330
*/
namespace SContext
{

/*!
\brief 遍历记号列表，验证基本合法性：圆括号是否对应。
\param b 起始迭代器。
\param e 终止迭代器。
\pre 迭代器是同一个记号列表的迭代器，其中 b 必须可解引用，且在 e 之前。
\return e 或指向冗余的 ')' 的迭代器。
\throw LoggedEvent 警报：找到冗余的 '(' 。
\since build 335
*/
YB_ATTR_nodiscard YF_API TLCIter
Validate(TLCIter b, TLCIter e);

//! \since build 674
//@{
/*!
\brief 遍历规约记号列表，取抽象语法树储存至指定值类型节点。
\param term 项节点。
\param b 起始迭代器。
\param e 终止迭代器。
\pre 迭代器是同一个记号列表的迭代器，其中 b 必须可解引用，且在 e 之前。
\return e 或指向冗余的 ')' 的迭代器。
\throw LoggedEvent 警报：找到冗余的 '(' 。
*/
YB_ATTR_nodiscard YF_API TLCIter
Reduce(TermNode& term, TLCIter b, TLCIter e);


/*!
\brief 分析指定源，取抽象语法树储存至指定值类型节点。
\throw LoggedEvent 警报：找到冗余的 ')' 。
*/
//@{
YF_API void
Analyze(TermNode&, const TokenList&);
YF_API void
Analyze(TermNode&, const Session&);
//@}
/*!
\note 调用 ADL Analyze 分析节点。
\since build 844
*/
template<typename _type>
YB_ATTR_nodiscard TermNode
Analyze(const _type& arg, TermNode::allocator_type a = {})
{
	TermNode root(a);

	Analyze(root, arg);
	return root;
}
//@}

} // namespace SContext;

} // namespace NPL;

#endif

