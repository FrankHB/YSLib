/*
	© 2012-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SContext.h
\ingroup NPL
\brief S 表达式上下文。
\version r3587
\author FrankHB <frankhb1989@gmail.com>
\since build 304
\par 创建时间:
	2012-08-03 19:55:41 +0800
\par 修改时间:
	2020-05-29 19:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SContext
*/


#ifndef NPL_INC_SContext_h_
#define NPL_INC_SContext_h_ 1

#include "YModules.h"
#include YFM_NPL_Lexical // for function, pmr, ByteParser, ystdex::expand_proxy,
//	ystdex::unref, ystdex::as_const, LexemeList;
#include YFM_YSLib_Core_ValueNode // for YSLib::Deref, YSLib::LoggedEvent,
//	YSLib::MakeIndex, YSLib::NoContainer, YSLib::NoContainerTag,
//	YSLib::ValueNode, YSLib::ValueObject, YSLib::make_observer,
//	YSLib::make_pair, YSLib::share_move, YSLib::make_shared, YSLib::make_weak,
//	YSLib::observer_ptr, YSLib::weak_ptr, list, YSLib::ListContainerTag,
//	std::initializer_list, ystdex::create_and_swap, ystdex::forward_like,
//	ystdex::invoke, YSLib::AccessPtr, ystdex::false_, std::is_convertible,
//	ystdex::decay_t, ystdex::bool_, ystdex::cond_or_t, ystdex::not_,
//	ystdex::enable_if_t, ystdex::call_value_or, ystdex::addrof, ystdex::compose,
//	YSLib::Alert, YSLib::stack;
#include <iterator> // for std::iterator_traits, std::input_iterator_tag,
//	std::random_access_iterator_tag;
#include <algorithm> // for std::for_each;
#include <ystdex/scope_guard.hpp> // for ystdex::make_guard;

namespace NPL
{

//! \since build 853
using YSLib::Deref;
//! \since build 674
using YSLib::LoggedEvent;
//! \since build 599
using YSLib::MakeIndex;
//! \since build 852
using YSLib::NoContainer;
//! \since build 852
using YSLib::NoContainerTag;
//! \since build 335
using YSLib::ValueNode;
//! \since build 675
using YSLib::ValueObject;
//! \since build 852
using YSLib::make_observer;
//! \since build 852
using YSLib::make_pair;
//! \since build 788
using YSLib::make_shared;
//! \since build 788
using YSLib::make_weak;
//! \since build 674
using YSLib::observer_ptr;
//! \since build 788
using YSLib::weak_ptr;


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
	当前用于实现对象语言的被绑定的对象或消亡值以外的表达式。
	*/
	Unqualified = 1 << UnqualifiedIndex,
	/*!
	\brief 唯一引用。

	指定被引用的对象具有唯一引用。
	被引用的对象应未被别名，或可假定未被别名。
	当前用于实现对象语言的消亡值。
	通常在派生实现绑定特定消亡值或显式标记引用值为消亡值时使用。
	*/
	Unique = 1 << UniqueIndex,
	/*!
	\brief 不可修改。

	指定对象不可修改或被引用的对象不可通过此引用被修改。
	除非唯一，被引用的对象仍可能通过其它引用修改。
	当前用于实现对象语言的不可修改引用。
	通常在派生实现绑定特定的对象引入不可修改引用时使用。
	*/
	Nonmodifying = 1 << NonmodifyingIndex,
	/*!
	\brief 临时对象。

	指定特定的项是临时对象。
	通常在派生实现绑定特定引用且指定被引用的对象是右值时使用。
	取得范式的项中，临时对象标签指示被绑定对象或其引用的绑定的是临时对象，
		类似宿主语言中声明的右值引用变量，但实际作用在被引用的对象的项，
		而不引入引用项。
	范式中来自绑定的临时对象外的标签在对象语言中完全通过表达式的值类别决定。
	排除临时对象标签的项，在特定上下文中被推断为左值，仍保留剩余的标签；
	在这些上下文中，临时对象标签使推断结果为右值。
	和宿主语言的 std::forward 不同，此处推断的右值除了是消亡值外，也可以是纯右值。
	不取得范式的项中，派生语言可指定指示被绑定对象以外的语义。
	*/
	Temporary = 1 << TemporaryIndex
};

//! \relates TermTags
//@{
DefBitmaskEnum(TermTags)

/*!
\brief 取推断为左值表达式时保留的标签。
\sa TermTags::Temporary

取推断为左值表达式的标签，即除临时对象以外的标签。
*/
YB_ATTR_nodiscard YB_STATELESS yconstfn
	PDefH(TermTags, GetLValueTagsOf, const TermTags& tags) ynothrow
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
	//! \brief 子项的容器。
	Container container{};

public:
	//! \brief 值数据成员。
	ValueObject Value{};
	//@}
	/*!
	\brief 项的标签。
	\since build 857
	*/
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
	/*!
	\brief 复制构造：使用参数和参数指定的分配器。
	\since build 879
	*/
	TermNode(const TermNode& tm)
		: TermNode(tm, tm.get_allocator())
	{}
	TermNode(const TermNode& tm, allocator_type a)
		: container(tm.container, a), Value(tm.Value), Tags(tm.Tags)
	{}
	DefDeMoveCtor(TermNode)
	TermNode(TermNode&& tm, allocator_type a)
		: container(std::move(tm.container), a), Value(std::move(tm.Value)),
		Tags(tm.Tags)
	{}

	/*!
	\brief 复制赋值：使用以参数的分配器构造的副本和交换操作。
	\since build 879
	*/
	PDefHOp(TermNode&, =, const TermNode& tm)
		ImplRet(ystdex::copy_and_swap(*this, tm))
	/*!
	\pre 被转移的参数不是被子节点容器直接或间接所有的其它节点。
	\warning 违反前置条件的转移可能引起循环引用。
	*/
	DefDeMoveAssignment(TermNode)

	//! \since build 853
	YB_PURE DefBoolNeg(YB_PURE explicit, bool(Value) || !empty())

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
	YB_ATTR_nodiscard YB_PURE PDefH(bool, empty, ) const ynothrow
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
//! \since build 674
using TNIter = TermNode::iterator;
//! \since build 674
using TNCIter = TermNode::const_iterator;

/*!
\brief 项节点分类判断操作。
\since build 733
*/
//@{
//! \brief 判断项是否为枝节点。
YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsBranch, const TermNode& term)
	ynothrow
	ImplRet(!term.empty())

/*!
\brief 判断项是否为分支列表节点。
\since build 858
*/
YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsBranchedList,
	const TermNode& term) ynothrow
	ImplRet(!(term.empty() || term.Value))

//! \brief 判断项是否为空节点。
YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsEmpty, const TermNode& term)
	ynothrow
	ImplRet(!term)

/*!
\brief 判断项是否为扩展列表节点。
\since build 858
*/
YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsExtendedList,
	const TermNode& term) ynothrow
	ImplRet(!(term.empty() && term.Value))

//! \brief 判断项是否为叶节点。
YB_ATTR_nodiscard YB_PURE inline PDefH(bool, IsLeaf, const TermNode& term)
	ynothrow
	ImplRet(term.empty())

/*!
\brief 判断项是否为列表节点。
\since build 774
*/
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
//! \since build 852
template<typename _type>
YB_ATTR_nodiscard inline observer_ptr<_type>
AccessPtr(TermNode& term) ynothrow
{
	return term.Value.AccessPtr<_type>();
}
//! \since build 852
template<typename _type>
YB_ATTR_nodiscard inline observer_ptr<const _type>
AccessPtr(const TermNode& term) ynothrow
{
	return term.Value.AccessPtr<_type>();
}

/*!
\brief 断言枝节点。
\pre 断言：参数指定的项是枝节点。
\since build 761
*/
inline PDefH(void, AssertBranch, const TermNode& term,
	const char* msg = "Invalid term found.") ynothrowv
	ImplExpr(yunused(term), yunused(msg), YAssert(IsBranch(term), msg))

//! \brief 创建项节点。
//@{
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
//! \since build 891
template<typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline TermNode
AsTermNode(TermNode::allocator_type a, _tParams&&... args)
{
	return TermNode(std::allocator_arg, a, NoContainer, yforward(args)...);
}
//@}

//! \since build 853
inline PDefH(const string&, GetNodeNameOf, const ValueNode& node) ynothrow
	ImplRet(node.GetName())
//! \since build 853
inline PDefH(string, GetNodeNameOf, const TermNode&) ynothrow
	ImplRet(string("[TermNode]"))

//! \since build 852
//@{
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

//! \since build 872
//@{
//! \pre 断言：参数指定的项是枝节点。
//@{
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TermNode&, AccessFirstSubterm, TermNode& term)
	ImplRet(AssertBranch(term), NPL::Deref(term.begin()))
YB_ATTR_nodiscard YB_PURE inline
	PDefH(const TermNode&, AccessFirstSubterm, const TermNode& term)
	ImplRet(AssertBranch(term), NPL::Deref(term.begin()))

YB_ATTR_nodiscard YB_PURE inline
	PDefH(TermNode&&, MoveFirstSubterm, TermNode& term)
	ImplRet(std::move(AccessFirstSubterm(term)))
//@}

YB_ATTR_nodiscard inline
	PDefH(shared_ptr<TermNode>, ShareMoveTerm, TermNode& term)
	ImplRet(YSLib::share_move(term.get_allocator(), term))
YB_ATTR_nodiscard inline
	PDefH(shared_ptr<TermNode>, ShareMoveTerm, TermNode&& term)
	ImplRet(YSLib::share_move(term.get_allocator(), term))
//@}

//! \pre 断言：项节点容器非空。
inline PDefH(void, RemoveHead, TermNode& term) ynothrowv
	ImplExpr(YAssert(!term.empty(), "Empty term container found."),
		term.erase(term.begin()))

/*!
\brief 根据节点和节点容器创建操作设置目标节点的值或子节点。
\note 可用于创建副本。
\warning 不检查嵌套深度，不支持嵌套调用安全。
*/
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
\brief 检查项节点是否具有指定的值。
\since build 753
*/
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline bool
HasValue(const TermNode& term, const _type& x)
{
	return term.Value == x;
}


//! \warning 非虚析构。
//@{
/*!
\brief 会话：分析组成 NPL 基本翻译单元的源代码。
\since build 304
*/
class YF_API Session
{
public:
	//! \since build 890
	using DefaultParser = ByteParser;
	//! \since build 592
	LexicalAnalyzer Lexer{};

private:
	//! \since build 890
	pmr::polymorphic_allocator<yimpl(byte)> allocator;

public:
	//! \since build 618
	DefDeCtor(Session)
	//! \since build 887
	Session(pmr::polymorphic_allocator<yimpl(byte)> a)
		: allocator(a)
	{}

	/*!
	\brief 取解析结果。
	\since build 891

	调用参数指定的解析器的对应函数取解析结果列表。
	*/
	template<typename _fParse>
	auto
	GetResult(const _fParse& parse) const -> decltype(ystdex::expand_proxy<
		GParserResult<_fParse>(const decltype(ystdex::as_const(
		ystdex::unref(std::declval<const _fParse&>())))&,
		const Session&)>::invoke(&ParserClassOf<_fParse>::GetResult,
		ystdex::as_const(ystdex::unref(std::declval<const _fParse&>())),
		std::declval<const Session&>()))
	{
		return ystdex::expand_proxy<GParserResult<_fParse>(const
			decltype(ystdex::as_const(ystdex::unref(
			std::declval<const _fParse&>())))&, const Session&)>::invoke(
			&ParserClassOf<_fParse>::GetResult,
			ystdex::as_const(ystdex::unref(parse)), *this);
	}

	/*!
	\brief 使用解析器处理输入范围。
	\exception LoggedEvent 关键失败：无法访问源内容。
	\sa DefaultParser
	\sa SContextParsers
	\since build 889

	前两个迭代器参数或第一个范围参数指定要处理的输入序列。
	之后可选的解析器参数指定使用的解析器。
	对没有使用解析器的参数，使用新创建的 DefaultParser 的引用。
	解析器参数应指定 SContext 语法解析器、其兼容的函数或这些类型对应的引用包装类型。
	*/
	//@{
	//! \since build 890
	template<typename _tIn>
	YB_ATTR_nodiscard inline DefaultParser
	Process(_tIn first, _tIn last)
	{
		return ProcessImpl(first, last,
			typename std::iterator_traits<_tIn>::iterator_category());
	}
	template<typename _tIn, typename _fParse>
	YB_ATTR_nodiscard inline _fParse
	Process(_tIn first, _tIn last, _fParse parse)
	{
		return ProcessImpl(first, last, parse,
			typename std::iterator_traits<_tIn>::iterator_category());
	}
	//! \since build 890
	template<typename _tRange>
	YB_ATTR_nodiscard inline DefaultParser
	Process(const _tRange& c)
	{
		return Process(ystdex::begin(c), ystdex::end(c));
	}
	template<typename _tRange, typename _fParse>
	YB_ATTR_nodiscard inline _fParse
	Process(const _tRange& c, _fParse parse)
	{
		return Process(ystdex::begin(c), ystdex::end(c), parse);
	}
	//@}

private:
	//! \since build 890
	//@{
	template<typename _tIn, class _tTag>
	YB_ATTR_nodiscard inline DefaultParser
	ProcessImpl(_tIn first, _tIn last, _tTag)
	{
		DefaultParser parse(Lexer, allocator);

		yunused(ProcessImpl(first, last, ystdex::ref(parse), _tTag()));
		// NOTE: Return the parser object with copy elimination.
		return parse;
	}
	template<typename _tIn, typename _fParse>
	YB_ATTR_nodiscard inline _fParse
	ProcessImpl(_tIn first, _tIn last, _fParse parse, std::input_iterator_tag)
	{
		std::for_each(first, last, parse);
		return parse;
	}
	template<typename _tRandom, typename _fParse>
	YB_ATTR_nodiscard inline _fParse
	ProcessImpl(_tRandom first, _tRandom last, _fParse parse,
		std::random_access_iterator_tag)
	{
		// XXX: This should be safe since there should be no more space out of
		//	the range of %size_t.
		ProcessReserve(parse, size_t(last - first));
		return ProcessImpl(first, last, parse, std::input_iterator_tag());
	}
	//@}

	//! \since build 890
	template<class _tParser>
	static auto
	ProcessReserve(_tParser&& parse, size_t n)
		-> decltype(ystdex::unref(parse).reserve(n))
	{
		ystdex::unref(parse).reserve(n);
	}
	//! \since build 890
	template<typename... _tParams>
	static void
	ProcessReserve(_tParams&&...)
	{}

public:
	/*!
	\brief 取使用的分配器。
	\since build 890
	*/
	YB_ATTR_nodiscard YB_PURE PDefH(pmr::polymorphic_allocator<yimpl(byte)>,
		get_allocator, ) const ynothrow
		ImplRet(allocator)
};
//@}


/*!
\brief S 表达式上下文：处理 S 表达式。
\since build 330
*/
namespace SContext
{

//! \since build 891
//@{
/*!
\brief 转换参数为词素。
\note 可配合其它不能直接转换为词素的参数类型的 ADL 重载使用。
*/
//@{
template<class _type, yimpl(typename = ystdex::enable_if_convertible_t<
	const _type&, const LexemeList::value_type&>)>
YB_ATTR_nodiscard YB_STATELESS const _type&
ToLexeme(const _type& val) ynothrow
{
	return val;
}
YB_ATTR_nodiscard YB_PURE inline PDefH(const string&, ToLexeme,
	const SourcedByteParser::ParseResult::value_type& val) ynothrow
	ImplRet(val.second)
//@}


/*!
\ingroup functors
\brief 转换记号为词素。
*/
struct LexemeTokenizer
{
	TermNode::allocator_type Allocator;

	LexemeTokenizer(TermNode::allocator_type a = {})
		: Allocator(a)
	{}

	//! \note 使用 ADL ToLexeme 转换访问的迭代器值为词素值。
	template<class _type>
	YB_ATTR_nodiscard YB_PURE TermNode
	operator()(const _type& val) const
	{
		return NPL::AsTermNode(Allocator, std::allocator_arg, Allocator,
			ToLexeme(val));
	}
};


/*!
\param first 起始迭代器。
\param last 终止迭代器。
\pre <tt>[first, last)</tt> 是可访问的记号值范围。
\return last 或对应冗余的 ')' 的迭代器。
\throw LoggedEvent 警报：找到冗余的 '(' 。
\note 使用 ADL ToLexeme 转换访问的迭代器值为词素值。
\sa ToLexeme
*/
//@{
//! \brief 遍历记号列表，验证基本合法性：圆括号是否对应。
template<typename _tIn>
YB_ATTR_nodiscard _tIn
Validate(_tIn first, _tIn last)
{
	size_t left(0);

	for(; first != last; ++first)
		if(ToLexeme(*first) == "(")
			++left;
		else if(ToLexeme(*first) == ")")
		{
			if(left != 0)
				--left;
			else
				return first;
		}
	if(left == 0)
		return first;
	throw LoggedEvent("Redundant '(' found.", YSLib::Alert);
}

/*!
\brief 遍历规约记号列表，取抽象语法树储存至指定值类型节点。
\param term 项节点。
*/
//@{
template<typename _tIn>
YB_ATTR_nodiscard inline _tIn
Reduce(TermNode& term, _tIn first, _tIn last)
{
	return Reduce(term, first, last, LexemeTokenizer{term.get_allocator()});
}
//! \param tokenize 标记器。
template<typename _tIn, typename _fTokenize>
YB_ATTR_nodiscard _tIn
Reduce(TermNode& term, _tIn first, _tIn last, _fTokenize tokenize)
{
	const auto a(term.get_allocator());
	YSLib::stack<TermNode> tms(a);
	const auto gd(ystdex::make_guard([&]() ynothrowv{
		YAssert(!tms.empty(), "Invalid state found.");
		term = std::move(tms.top());
	}));

	tms.push(std::move(term));
	for(; first != last; ++first)
		if(ToLexeme(*first) == "(")
			tms.push(NPL::AsTermNode(a));
		else if(ToLexeme(*first) != ")")
		{
			YAssert(!tms.empty(), "Invalid state found.");
			tms.top().Add(tokenize(*first));
		}
		else if(tms.size() != 1)
		{
			auto tm(std::move(tms.top()));

			tms.pop();
			YAssert(!tms.empty(), "Invalid state found.");
			tms.top().Add(std::move(tm));
		}
		else
			return first;
	if(tms.size() == 1)
		return first;
	throw LoggedEvent("Redundant '(' found.", YSLib::Alert);
}
//@}
//@}
//@}

/*!
\brief 分析指定源，取抽象语法树储存至指定值类型节点。
\throw LoggedEvent 警报：找到冗余的 ')' 。
*/
//@{
template<class _tParseResult>
void
Analyze(TermNode& root, const _tParseResult& parse_result)
{
	if(SContext::Reduce(root, parse_result.cbegin(), parse_result.cend())
		!= parse_result.cend())
		throw LoggedEvent("Redundant ')' found.", YSLib::Alert);
}
//! \since build 890
template<typename _fParse>
inline void
Analyze(TermNode& root, const Session& sess, const _fParse& parse)
{
	Analyze(root, sess.GetResult(parse));
}
template<class _tParseResult, typename _fTokenize>
void
Analyze(TermNode& root, _fTokenize tokenize, const _tParseResult& parse_result)
{
	if(SContext::Reduce(root, parse_result.cbegin(), parse_result.cend(),
		std::move(tokenize)) != parse_result.cend())
		throw LoggedEvent("Redundant ')' found.", YSLib::Alert);
}
template<typename _fParse, typename _fTokenize>
inline void
Analyze(TermNode& root, _fTokenize tokenize, const Session& sess,
	const _fParse& parse)
{
	Analyze(root, std::move(tokenize), sess.GetResult(parse));
}
//@}
//! \note 调用 ADL Analyze 分析节点。
//@{
//! \since build 889
template<typename _tParam, typename... _tParams,
	yimpl(ystdex::exclude_self_t<TermNode, _tParam, int> = 0)>
YB_ATTR_nodiscard
	yimpl(ystdex::exclude_self_t)<std::allocator_arg_t, _tParam, TermNode>
Analyze(_tParam&& arg, _tParams&&... args)
{
	TermNode root;

	Analyze(root, yforward(arg), yforward(args)...);
	return root;
}
//! \since build 891
template<typename... _tParams>
YB_ATTR_nodiscard TermNode
Analyze(std::allocator_arg_t, TermNode::allocator_type a, _tParams&&... args)
{
	TermNode root(a);

	Analyze(root, yforward(args)...);
	return root;
}
//@}

} // namespace SContext;

} // namespace NPL;

#endif

