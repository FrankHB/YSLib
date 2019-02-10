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
\version r2050
\author FrankHB <frankhb1989@gmail.com>
\since build 304
\par 创建时间:
	2012-08-03 19:55:41 +0800
\par 修改时间:
	2019-02-10 14:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SContext
*/


#ifndef NPL_INC_SContext_h_
#define NPL_INC_SContext_h_ 1

#include "YModules.h"
#include YFM_NPL_Lexical
#include YFM_YSLib_Core_ValueNode // for YSLib::NoContainer, YSLib::ValueObject,
//	YSLib::ValueNode, list, YSLib::LoggedEvent, YSLib::make_observer,
//	YSLib::make_pair, YSLib::observer_ptr, YSLib::pair, ystdex::totally_ordered,
//	YSLib::ListContainerTag, std::initializer_list, YSLib::AccessPtr,
//	std::mem_fn, ystdex::call_value_or, ystdex::addrof, ystdex::compose,
//	ystdex::forward_like, ystdex::invoke;
#include <ystdex/iterator.hpp> // for ystdex::transformed_iterator,
//	ystdex::reverse_iterator, ystdex::make_transform;

namespace NPL
{

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
*/
class YF_API TermNode
	: private ValueNode, private ystdex::totally_ordered<TermNode>,
	private ystdex::totally_ordered<TermNode, string>
{
private:
	struct ConstIteratorMap
	{
		template<typename _tIter>
		yconstfn const TermNode&
		operator()(const _tIter& i) const ynothrowv
		{
			return static_cast<const TermNode&>(*i);
		}
	};
	struct IteratorMap : ConstIteratorMap
	{
		template<typename _tIter>
		yconstfn TermNode&
		operator()(const _tIter& i) const ynothrowv
		{
			return static_cast<TermNode&>(*i);
		}
	};
	// TODO: Deduplicate within %ValueNode?
	template<typename... _tParams>
	using enable_value_constructible_t = ystdex::enable_if_t<
		std::is_constructible<ValueObject, _tParams...>::value>;

public:
	//! \sa ystdex::mapped_set
	struct YF_API MappedSetTraits : ValueNode::MappedSetTraits
	{
		template<typename _tKey, class _tCon>
		YB_ATTR_nodiscard YB_PURE static inline auto
		extend_key(_tKey&& k, _tCon& con) ynothrow -> decltype(
			TermNode(std::allocator_arg, con.get_allocator(), NoContainer,
			yforward(k)))
		{
			return {std::allocator_arg, con.get_allocator(), NoContainer,
				yforward(k)};
		}

		//! \note 这些重载和构造函数中可能由参数确定键的值的情形匹配。
		//@{
		using ValueNode::MappedSetTraits::get_value_key;
		YB_ATTR_nodiscard YB_PURE static PDefH(const string&, get_value_key,
			const TermNode& nd, allocator_type = {}) ynothrow
			ImplRet(nd.GetName())
		YB_ATTR_nodiscard YB_PURE static PDefH(string&&, get_value_key,
			TermNode&& nd, allocator_type = {}) ynothrow
			ImplRet(ValueNode::MappedSetTraits::get_value_key(
				static_cast<ValueNode&&>(nd)))
		//@}

		//! \sa ystdex::restore_key
		static PDefH(void, restore_key, TermNode& node, TermNode&& ek)
			ImplExpr(
				ValueNode::MappedSetTraits::restore_key(node, std::move(ek)))

		//! \sa ystdex::set_value_move
		static PDefH(TermNode, set_value_move, TermNode& node)
			ImplRet({std::move(node.GetContainerRef()),
				node.GetName(), std::move(node.Value)})
	};
	using ValueNode::Container;
	using ValueNode::allocator_type;
	using iterator
		= ystdex::transformed_iterator<ValueNode::iterator, IteratorMap>;
	using const_iterator = ystdex::transformed_iterator<
		ValueNode::const_iterator, ConstIteratorMap>;
	using reverse_iterator = ystdex::reverse_iterator<iterator>;
	using const_reverse_iterator = ystdex::reverse_iterator<const_iterator>;

	using ValueNode::Value;

	DefDeCtor(TermNode)
	// XXX: Not all constructors like %ValueNode need to be supported here.
	//! \note 部分构造函数和 ValueNode 类似。
	//@{
	explicit
	TermNode(allocator_type a)
		: ValueNode(a)
	{}
	TermNode(const Container& con)
		: ValueNode(con)
	{}
	TermNode(Container&& con)
		: ValueNode(std::move(con))
	{}
	template<typename _tString, typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	TermNode(const Container& con, _tString&& str, _tParams&&... args)
		: ValueNode(con, yforward(str), yforward(args)...)
	{}
	template<typename _tString, typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	TermNode(Container&& con, _tString&& str, _tParams&&... args)
		: ValueNode(std::move(con), yforward(str), yforward(args)...)
	{}
	template<typename _tString, typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	inline
	TermNode(std::allocator_arg_t, allocator_type a, NoContainerTag,
		_tString&& str, _tParams&&... args)
		: ValueNode(std::allocator_arg, a, NoContainer, yforward(str),
		yforward(args)...)
	{}
	template<typename _tString, typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	inline
	TermNode(std::allocator_arg_t, allocator_type a, const Container& con,
		_tString&& str, _tParams&&... args)
		: ValueNode(std::allocator_arg, a, con, yforward(str),
		yforward(args)...)
	{}
	template<typename _tString, typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	inline
	TermNode(std::allocator_arg_t, allocator_type a, Container&& con,
		_tString&& str, _tParams&&... args)
		: ValueNode(std::allocator_arg, a, std::move(con), yforward(str),
		yforward(args)...)
	{}
	TermNode(const ValueNode& nd, allocator_type a)
		: ValueNode(nd, a)
	{}
	TermNode(ValueNode&& nd, allocator_type a)
		: ValueNode(std::move(nd), a)
	{}
	//@}
	// XXX: This needs tag to avoid clash with other constructors.
	TermNode(YSLib::ListContainerTag, std::initializer_list<TermNode> il)
		: ValueNode(MapInitializerList(il))
	{}
	TermNode(std::initializer_list<TermNode> il, allocator_type a)
		: ValueNode(MapInitializerList(il, a))
	{}
	template<typename _tString, typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	inline
	TermNode(std::allocator_arg_t, allocator_type a,
		std::initializer_list<TermNode> il,
		_tString&& str, _tParams&&... args)
		: TermNode(std::allocator_arg, a, Container(MapInitializerList(il, a)),
		yforward(str), yforward(args)...)
	{}
	explicit
	TermNode(const ValueNode& nd)
		: ValueNode(nd)
	{}
	explicit
	TermNode(ValueNode&& nd)
		: ValueNode(std::move(nd))
	{}
	TermNode(const TermNode& tm, allocator_type a)
		: ValueNode(tm, a)
	{}
	TermNode(TermNode&& tm, allocator_type a)
		: ValueNode(std::move(tm), a)
	{}
	DefDeCopyMoveCtorAssignment(TermNode)

	using ValueNode::operator!;

	using ValueNode::operator[];

	friend PDefHOp(bool, ==, const TermNode& x, const TermNode& y) ynothrow
		ImplRet(x.GetName() == y.GetName())
	friend PDefHOp(bool, ==, const TermNode& x, const string& str) ynothrow
		ImplRet(x.GetName() == str)
	template<typename _tKey>
	YB_ATTR_nodiscard YB_PURE friend bool
	operator==(const TermNode& x, const _tKey& k) ynothrow
	{
		return x.GetName() == k;
	}

	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, <, const TermNode& x, const TermNode& y) ynothrow
		ImplRet(x.GetName() < y.GetName())
	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, <, const TermNode& x, const string& str) ynothrow
		ImplRet(x.GetName() < str)
	template<typename _tKey>
	YB_ATTR_nodiscard YB_PURE friend bool
	operator<(const TermNode& x, const _tKey& k) ynothrow
	{
		return x.GetName() < k;
	}
	template<typename _tKey>
	YB_ATTR_nodiscard YB_PURE friend bool
	operator<(const _tKey& k, const TermNode& y) ynothrow
	{
		return k < y.GetName();
	}
	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, >, const TermNode& x, const string& str) ynothrow
		ImplRet(x.GetName() > str)
	template<typename _tKey>
	YB_ATTR_nodiscard YB_PURE friend bool
	operator>(const TermNode& x, const _tKey& k) ynothrow
	{
		return x.GetName() > k;
	}
	template<typename _tKey>
	YB_ATTR_nodiscard YB_PURE friend bool
	operator>(const _tKey& k, const TermNode& y) ynothrow
	{
		return k > y.GetName();
	}

	using ValueNode::operator bool;

	using ValueNode::GetContainer;
	using ValueNode::GetContainerRef;
	using ValueNode::GetName;

	using ValueNode::SetContent;
	PDefH(void, SetContent, const TermNode& term)
		ImplExpr(ValueNode::SetContent(static_cast<const ValueNode&>(term)))
	PDefH(void, SetContent, TermNode&& term)
		ImplExpr(ValueNode::SetContent(static_cast<ValueNode&&>(term)))

	PDefH(bool, Add, const TermNode& term)
		ImplRet(insert(static_cast<const ValueNode&>(term)).second)
	PDefH(bool, Add, TermNode&& term)
		ImplRet(insert(static_cast<ValueNode&&>(term)).second)

	template<typename _tKey>
	yimpl(ystdex::enable_if_inconvertible_t)<_tKey&&, TermNode::const_iterator>
	AddChildToTail(_tKey&& k, const TermNode& tm)
	{
		ystdex::try_emplace(GetContainerRef(), yforward(k), tm.GetContainer(),
			yforward(k), tm.Value);
	}
	template<typename _tKey>
	yimpl(ystdex::enable_if_inconvertible_t)<_tKey&&, TermNode::const_iterator>
	AddChildToTail(_tKey&& k, TermNode&& tm)
	{
		ystdex::try_emplace(GetContainerRef(), yforward(k),
			std::move(tm.GetContainerRef()), yforward(k), std::move(tm.Value));
	}

	using ValueNode::AddValueTo;

	using ValueNode::Clear;

	using ValueNode::ClearContainer;

	using ValueNode::ClearTo;

	using ValueNode::CreateRecursively;

	template<typename _fCallable>
	Container
	CreateWith(_fCallable f) &
	{
		return CreateRecursively(GetContainerRef(), f);
	}
	template<typename _fCallable>
	Container
	CreateWith(_fCallable f) const&
	{
		return CreateRecursively(GetContainer(), f);
	}
	template<typename _fCallable>
	Container
	CreateWith(_fCallable f) &&
	{
		return CreateRecursively(std::move(GetContainerRef()), f);
	}
	template<typename _fCallable>
	Container
	CreateWith(_fCallable f) const&&
	{
		return CreateRecursively(std::move(GetContainer()), f);
	}

private:	
	YB_ATTR_nodiscard static
		PDefH(const ValueNode&, DerefToBase, const TermNode* p_node) ynothrowv
		ImplRet(Deref(p_node))

	YB_ATTR_nodiscard YB_PURE static PDefH(Container, MapInitializerList,
		std::initializer_list<TermNode> il)
		ImplRet({MapIteratorForInitializerList(il.begin()),
			MapIteratorForInitializerList(il.begin() + il.size())})
	YB_ATTR_nodiscard YB_PURE static PDefH(Container, MapInitializerList,
		std::initializer_list<TermNode> il, allocator_type a)
		ImplRet(Container({MapIteratorForInitializerList(il.begin()),
			MapIteratorForInitializerList(il.begin() + il.size())}, a))

	YB_ATTR_nodiscard YB_PURE static
		PDefH(iterator, MapIterator, ValueNode::iterator i) ynothrow
		ImplRet(ystdex::make_transform(i, IteratorMap()))
	YB_ATTR_nodiscard YB_PURE static
		PDefH(const_iterator, MapIterator, ValueNode::const_iterator i) ynothrow
		ImplRet(ystdex::make_transform(i, ConstIteratorMap()))
	YB_ATTR_nodiscard YB_PURE static PDefH(reverse_iterator, MapIterator,
		ValueNode::reverse_iterator i) ynothrow
		ImplRet(reverse_iterator(MapIterator(i.base())))
	YB_ATTR_nodiscard YB_PURE static PDefH(const_reverse_iterator, MapIterator,
		ValueNode::const_reverse_iterator i) ynothrow
		ImplRet(const_reverse_iterator(MapIterator(i.base())))
	YB_ATTR_nodiscard YB_PURE static PDefH(pair<iterator YPP_Comma bool>,
		MapIterator, pair<ValueNode::iterator YPP_Comma bool> pr) ynothrow
		ImplRet(NPL::make_pair(MapIterator(pr.first), pr.second))

	YB_ATTR_nodiscard YB_PURE static PDefH(auto, MapIteratorForInitializerList,
		const TermNode* p) -> decltype(ystdex::make_transform(p, DerefToBase))
		ImplRet(ystdex::make_transform(p, DerefToBase))

public:
	using ValueNode::MoveContent;
	PDefH(void, MoveContent, TermNode&& term)
		ImplExpr(MoveContent(static_cast<ValueNode&&>(term)))

	using ValueNode::Remove;
	PDefH(void, Remove, const_iterator i)
		ImplExpr(ValueNode::Remove(i.get()))

	PDefH(void, SwapContainer, TermNode& term) ynothrowv
		ImplExpr(term.ValueNode::SwapContainer(static_cast<TermNode&>(*this)))

	PDefH(void, SwapContent, TermNode& term)
		ImplRet(term.ValueNode::SwapContent(static_cast<TermNode&>(*this)))

	YB_ATTR_nodiscard YB_PURE PDefH(iterator, begin, ) ynothrow
		ImplRet(MapIterator(ValueNode::begin()))
	YB_ATTR_nodiscard YB_PURE PDefH(const_iterator, begin, ) const ynothrow
		ImplRet(MapIterator(ValueNode::begin()))

	DefFwdTmpl(, pair<iterator YPP_Comma bool>, emplace,
		MapIterator(ValueNode::emplace(yforward(args)...)))

	template<typename... _tParams>
	iterator
	emplace_hint(const_iterator position, _tParams&&... args)
	{
		return MapIterator(ValueNode::emplace_hint(position.get(),
			yforward(args)...));
	}

	using ValueNode::empty;

	PDefH(iterator, erase, const_iterator i)
		ImplRet(MapIterator(ValueNode::erase(i.get())))
	PDefH(iterator, erase, const_iterator first, const_iterator last)
		ImplRet(MapIterator(ValueNode::erase(first.get(), last.get())))

	YB_ATTR_nodiscard YB_PURE PDefH(iterator, end, ) ynothrow
		ImplRet(MapIterator(ValueNode::end()))
	YB_ATTR_nodiscard YB_PURE PDefH(const_iterator, end, ) const ynothrow
		ImplRet(MapIterator(ValueNode::end()))

	using ValueNode::get_allocator;

	// FIXME: Call of %insert with hint does not support correct type of hint
	//	parameter. Use %Add for workaround.
	DefFwdTmpl(-> decltype(MapIterator(ValueNode::insert(
		yforward(args)...))), auto, insert,
		MapIterator(ValueNode::insert(yforward(args)...)))

	DefFwdTmpl(-> decltype(MapIterator(ValueNode::insert_or_assign(
		yforward(args)...))), auto, insert_or_assign,
		MapIterator(ValueNode::insert_or_assign(yforward(args)...)))

	YB_ATTR_nodiscard YB_PURE PDefH(reverse_iterator, rbegin, ) ynothrow
		ImplRet(reverse_iterator(begin()))
	YB_ATTR_nodiscard YB_PURE
		PDefH(const_reverse_iterator, rbegin, ) const ynothrow
		ImplRet(const_reverse_iterator(begin()))

	YB_ATTR_nodiscard YB_PURE PDefH(reverse_iterator, rend, ) ynothrow
		ImplRet(reverse_iterator(end()))
	YB_ATTR_nodiscard YB_PURE
		PDefH(const_reverse_iterator, rend, ) const ynothrow
		ImplRet(const_reverse_iterator(end()))

	using ValueNode::size;

	friend DefSwap(ynothrow, TermNode, ystdex::swap_dependent(static_cast<
		ValueNode&>(_x), static_cast<ValueNode&>(_y)))
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

YB_ATTR_nodiscard YB_PURE inline
	PDefH(const TermNode&, AsTermNode, const TermNode& node) ynothrow
	ImplRet(node)
template<typename _tString, typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline TermNode
AsTermNode(_tString&& str, _tParams&&... args)
{
	return {NoContainer, yforward(str), yforward(args)...};
}
template<typename _tString, typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline TermNode
AsTermNode(TermNode::allocator_type a, _tString&& str, _tParams&&... args)
{
	return
		{std::allocator_arg, a, NoContainer, yforward(str), yforward(args)...};
}


//! \since build 599
using YSLib::MakeIndex;

//! \since build 852
//@{
inline
	PDefH(void, AddChildToTail, size_t idx, TermNode& term, const TermNode& tm)
	ImplExpr(term.AddChildToTail(MakeIndex(idx), tm))
inline PDefH(void, AddChildToTail, size_t idx, TermNode& term, TermNode&& tm)
	ImplExpr(term.AddChildToTail(MakeIndex(idx), std::move(tm)))
inline PDefH(void, AddChildToTail, TermNode& term, const TermNode& tm)
	ImplExpr(NPL::AddChildToTail(term.size(), term, tm))
inline PDefH(void, AddChildToTail, TermNode& term, TermNode&& tm)
	ImplExpr(NPL::AddChildToTail(term.size(), term, std::move(tm)))
//@}

template<typename... _tParams>
YB_ATTR_nodiscard inline YB_PURE TermNode
AsIndexTermNode(size_t idx, _tParams&&... args)
{
	return NPL::AsTermNode(NPL::MakeIndex(idx), yforward(args)...);
}
template<typename... _tParams>
YB_ATTR_nodiscard inline YB_PURE TermNode
AsIndexTermNode(TermNode::allocator_type a, size_t idx, _tParams&&... args)
{
	return NPL::AsTermNode(a, NPL::MakeIndex(idx), yforward(args)...);
}


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
	using YSLib::AccessPtr;

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

