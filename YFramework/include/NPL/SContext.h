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
\version r2532
\author FrankHB <frankhb1989@gmail.com>
\since build 304
\par 创建时间:
	2012-08-03 19:55:41 +0800
\par 修改时间:
	2019-08-16 12:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SContext
*/


#ifndef NPL_INC_SContext_h_
#define NPL_INC_SContext_h_ 1

#include "YModules.h"
#include YFM_NPL_Lexical // for function, pmr;
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


//! \since build 857
//@{
//! \brief 项标签索引：指定项标签元数据掩码的位。
enum TermTagIndices : size_t
{
	UnqualifiedIndex = 0,
	UniqueIndex,
	NonmodifyingIndex,
	TemporaryIndex
};

/*!
\brief 标签：表示引用元数据的位。
\relates TermTagIndices

指定项引用具有的元数据。选项为位掩码值。
*/
enum class TermTags
{
	/*!
	\brief 非限定对象。

	指定默认情形的对象或对象引用。
	当前用于实现对象语言的左值引用。
	*/
	Unqualified = 1 << UnqualifiedIndex,
	/*!
	\brief 唯一引用。

	指定被引用的对象具有唯一引用。
	被引用的对象应未被别名，或可假定被未被别名。
	当前用于实现对象语言的右值引用。
	通常在派生实现绑定特定对象引入右值引用时使用。
	*/
	Unique = 1 << UniqueIndex,
	/*!
	\brief 不可修改。

	指定对象不可修改或被引用的对象不通过此引用。
	除非唯一，被引用的对象仍可能通过其它引用修改。
	当前用于实现对象语言的不可修改引用。
	通常在派生实现绑定特定的对象引入不可修改引用时使用。
	*/
	Nonmodifying = 1 << NonmodifyingIndex,
	/*!
	\brief 临时对象。

	指定被引用的对象是绑定在环境中的显式的临时对象。
	类似宿主语言中声明的右值引用，但实际作用在被引用的对象的项而不是引用值所在的项。
	和宿主语言不同，引用不需要被扩展生存期的临时对象具有所有权。
	通常在派生实现绑定特定引用且指定被引用的对象是右值时使用。
	*/
	Temporary = 1 << TemporaryIndex
};

//! \relates TermTags
//@{
DefBitmaskEnum(TermTags)

yconstfn PDefH(TermTags, GetLValueTagsOf, const TermTags& tags) ynothrow
	ImplRet(tags & ~TermTags::Temporary)
//@}
//@}


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
	//! \since build 857
	TermTags Tags = TermTags::Unqualified;

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

	/*!
	\brief 转移参数内容。
	\pre 断言：参数不是 *this 。
	\since build 853

	转移参数指定的节点的内容到对象。转移后的节点内容是转移前的参数内容。
	允许被转移的参数直接或间接地被容器引用。
	*/
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

//! \since build 858
YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsBranchedList,
	const TermNode& term) ynothrow
	ImplRet(!(term.empty() || term.Value))

YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsEmpty, const TermNode& term)
	ynothrow
	ImplRet(!term)

//! \since build 858
YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsExtendedList,
	const TermNode& term) ynothrow
	ImplRet(!(term.empty() && term.Value))

YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsLeaf, const TermNode& term)
	ynothrow
	ImplRet(term.empty())

//! \since build 774
YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsList, const TermNode& term)
	ynothrow
	ImplRet(!term.Value)
//@}

//! \since build 853
using YSLib::Access;
//! \since build 854
template<typename _type>
YB_ATTR_nodiscard inline _type&
Access(TermNode& term)
{
	return term.Value.Access<_type>();
}
//! \since build 854
template<typename _type>
YB_ATTR_nodiscard inline const _type&
Access(const TermNode& term)
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
	return TermNode(NoContainer, yforward(args)...);
}
//! \since build 853
template<typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline TermNode
AsTermNode(TermNode::allocator_type a, _tParams&&... args)
{
	return TermNode(std::allocator_arg, a, NoContainer, yforward(args)...);
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
	auto vo(ystdex::invoke(f, yforward(node).Value));

	dst.SetContent(std::move(con), std::move(vo));
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
	ImplExpr(yunused(term), yunused(msg), YAssert(IsBranch(term), msg))


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
	using CharParser = void(&)(LexicalAnalyzer&, char);

	//! \since build 592
	LexicalAnalyzer Lexer;

	//! \since build 618
	DefDeCtor(Session)
	//! \since build 861
	Session(pmr::memory_resource&);
	//! \since build 862
	//@{
	//! \throw LoggedEvent 关键失败：无法访问源内容。
	//@{
	template<typename _tIn, typename _tCharParser = CharParser>
	Session(_tIn first, _tIn last, _tCharParser parse = DefaultParseByte)
		: Lexer()
	{
		std::for_each(first, last,
			std::bind(parse, std::ref(Lexer), std::placeholders::_1));
	}
	template<typename _tIn, typename _tCharParser = CharParser>
	Session(pmr::memory_resource& rsrc, _tIn first, _tIn last,
		_tCharParser parse = DefaultParseByte)
		: Lexer(rsrc)
	{
		std::for_each(first, last,
			std::bind(parse, std::ref(Lexer), std::placeholders::_1));
	}
	//@}
	template<typename _tRange, typename _tCharParser = CharParser,
		yimpl(typename = ystdex::exclude_self_t<Session, _tRange>,
		typename = ystdex::exclude_self_t<pmr::memory_resource, _tRange>)>
	Session(const _tRange& c, _tCharParser parse = DefaultParseByte)
		: Session(ystdex::begin(c), ystdex::end(c), parse)
	{}
	template<typename _tRange, typename _tCharParser = CharParser,
		yimpl(typename = ystdex::exclude_self_t<Session, _tRange>)>
	Session(pmr::memory_resource& rsrc, const _tRange& c,
		_tCharParser parse = DefaultParseByte)
		: Session(rsrc, ystdex::begin(c), ystdex::end(c), parse)
	{}
	//@}
	DefDeCopyMoveCtorAssignment(Session)

	//! \since build 862
	DefGetterMem(const ynothrow, const SmallString&, Buffer, Lexer)
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

