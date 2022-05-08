/*
	© 2012-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SContext.h
\ingroup NPL
\brief S 表达式上下文。
\version r4233
\author FrankHB <frankhb1989@gmail.com>
\since build 304
\par 创建时间:
	2012-08-03 19:55:41 +0800
\par 修改时间:
	2022-04-28 03:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SContext
*/


#ifndef NPL_INC_SContext_h_
#define NPL_INC_SContext_h_ 1

#include "YModules.h"
#include YFM_NPL_Lexical // for ystdex::copy_and_swap, std::swap, pmr,
//	ByteParser, ystdex::expand_proxy, ystdex::unref, ystdex::as_const,
//	LexemeList;
#include YFM_YSLib_Core_ValueNode // for YSLib::Deref, YSLib::MakeIndex,
//	YSLib::NoContainer, YSLib::NoContainerTag, YSLib::ValueNode,
//	YSLib::ValueObject, YSLib::forward_as_tuple, YSLib::get,
//	YSLib::make_observer, YSLib::make_pair, YSLib::make_shared,
//	YSLib::make_weak, YSLib::observer_ptr, YSLib::share_move, YSLib::tuple,
//	YSLib::weak_ptr, ystdex::enable_if_t, list, std::allocator_arg_t,
//	YSLib::ListContainerTag, std::initializer_list,
//	ystdex::enable_if_same_param_t, ystdex::exclude_self_t,
//	ystdex::enable_if_inconvertible_t, ystdex::forward_like, ystdex::invoke,
//	YSLib::AccessPtr, ystdex::head_of_t, ystdex::addrof, ystdex::compose, pair,
//	std::is_lvalue_reference, YSLib::Alert, YSLib::stack;
#include YFM_YSLib_Core_YException // for YSLib::LoggedEvent;
#include <ystdex/operators.hpp> // for ystdex::equality_comparable;
#include <ystdex/deref_op.hpp> // for ystdex::call_value_or;
#include <ystdex/range.hpp> // for std::iterator_traits,
//	ystdex::range_iterator_t, ystdex::begin, ystdex::end,
//	std::input_iterator_tag, std::random_access_iterator_tag;
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
//! \since build 894
using YSLib::Nonnull;
//! \since build 335
using YSLib::ValueNode;
//! \since build 675
using YSLib::ValueObject;
//! \since build 899
using YSLib::forward_as_tuple;
//! \since build 882
using YSLib::get;
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
//! \since build 899
using YSLib::tuple;
//! \since build 928
//@{
using YSLib::type_id;
using YSLib::type_index;
using YSLib::type_info;
//@}
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
		而不引入项引用。
	不取得范式的项中，派生语言可指定指示被绑定对象以外的语义。
	*/
	Temporary = 1 << TemporaryIndex
};

//! \relates TermTags
//@{
DefBitmaskEnum(TermTags)

/*!
\brief 判断标签是否表示可转移值。
\since build 939
*/
YB_ATTR_nodiscard YB_STATELESS yconstfn
	PDefH(bool, IsMovable, TermTags tags) ynothrow
	ImplRet((tags & (TermTags::Unique | TermTags::Nonmodifying))
		== TermTags::Unique)

/*!
\brief 取转发推断为左值表达式时保留的标签。
\sa TermTags::Temporary

取按对象语言引用推断规则转发操作数时，允许使实际参数被推断为左值表达式的标签。
当前设计中，即除临时对象以外的标签。
*/
YB_ATTR_nodiscard YB_STATELESS yconstfn
	PDefH(TermTags, GetLValueTagsOf, const TermTags& tags) ynothrow
	ImplRet(tags & ~TermTags::Temporary)

/*!
\brief 传播标签。
\since build 915

向指定的标签添加可向被引用对象传播的标签。
传播的标签实现引用值上的限定符向列表引用子对象的传播或者间接引用的合并。
适用的范围类似宿主语言引用的限定符的传播。
当前可传播的标签只有实现限定符的 TermTags::Nonmodifying 。
*/
YB_ATTR_nodiscard YB_STATELESS yconstfn
	PDefH(TermTags, PropagateTo, TermTags dst, TermTags tags) ynothrow
	ImplRet(dst | (tags & TermTags::Nonmodifying))

/*!
\brief 确保参数可被一等对象的值的表示包含。
\since build 925

清除参数中不被一等对象的值的表示支持的位。
当前实现中，标签的所有的位都不被右值支持，而所有左值中的标签都不在项而在项引用上；
	因此，可直接修改参数为非限定对象标签。
但因为当前实现中只引入具有临时对象标签的非一等对象（在被绑定对象中支持），
	为减少错误的使用，仅保守清除这些标签。
*/
inline PDefH(void, EnsureValueTags, TermTags& tags) ynothrow
	ImplExpr(tags &= ~yimpl(TermTags::Temporary))
//@}
//@}


/*!
\brief 项节点：存储语法分析结果的值类型节点。
\waning 非虚析构。
\since build 852

类似 ValueNode 的节点类型，但没有名称数据成员、按键比较和按键访问，
	且使用 list 而不是 ystdex::mapped_set 作为容器。
*/
class YF_API TermNode : private ystdex::equality_comparable<TermNode>
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
		: TermNode(con, con.get_allocator())
	{}
	TermNode(Container&& con)
		: container(std::move(con))
	{}
	/*!
	\note 除非 Value 的构造非嵌套调用安全，支持构造任意子节点时的嵌套调用安全。
	\since build 853
	*/
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
		: TermNode(std::allocator_arg, con.get_allocator(), con,
		yforward(args)...)
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
		: container(ConSub(con, a)), Value(yforward(args)...)
	{}
	template<typename... _tParams,
		yimpl(typename = enable_value_constructible_t<_tParams...>)>
	inline
	TermNode(std::allocator_arg_t, allocator_type a, Container&& con,
		_tParams&&... args)
		: container(std::move(con), a), Value(yforward(args)...)
	{}
	//@}
	//! \warning 不保证嵌套调用安全。
	TermNode(const ValueNode& nd, allocator_type a)
		: container(ConCons(nd.GetContainer(), a)), Value(nd.Value)
	{}
	//! \warning 不保证嵌套调用安全。
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
	//! \warning 不保证嵌套调用安全。
	explicit
	TermNode(const ValueNode& nd)
		: container(ConCons(nd.GetContainer())), Value(nd.Value)
	{}
	//! \warning 不保证嵌套调用安全。
	explicit
	TermNode(ValueNode&& nd)
		: container(ConCons(std::move(nd.GetContainer()))),
		Value(std::move(nd.Value))
	{}
	//! \note 除非 Value 的构造非嵌套调用安全，支持构造任意子节点时的嵌套调用安全。
	//@{
	/*!
	\brief 复制构造：使用参数和参数的分配器。
	\since build 879
	*/
	TermNode(const TermNode& nd)
		: TermNode(nd, nd.get_allocator())
	{}
	//! \brief 复制构造：使用参数和参数指定的分配器。
	TermNode(const TermNode& nd, allocator_type a)
		: container(ConSub(nd.container, a)), Value(nd.Value), Tags(nd.Tags)
	{}
	DefDeMoveCtor(TermNode)
	TermNode(TermNode&& nd, allocator_type a)
		: container(std::move(nd.container), a), Value(std::move(nd.Value)),
		Tags(nd.Tags)
	{}
	//@}

	/*!
	\brief 复制赋值：使用参数副本和交换操作。
	\since build 879
	*/
	PDefHOp(TermNode&, =, const TermNode& nd)
		ImplRet(ystdex::copy_and_swap(*this, nd))
	/*!
	\pre 被转移的参数不是被子节点容器直接或间接所有的其它节点。
	\warning 违反前置条件的转移可能引起循环引用。
	*/
	DefDeMoveAssignment(TermNode)
	/*
	\brief 析构：类定义外默认实现。
	\note 除非 Value 的析构非嵌套调用安全，支持移除任意子节点时的嵌套调用安全。
	\since build 916
	*/
	~TermNode()
	{
		// NOTE: Preprocess the node to prevent any deep nested node constructed
		//	in the object language overflow in C++ calls, which violates the
		//	nested call safety guarantee.
		// XXX: Assume the %Value has no such deep nested nodes as it is not
		//	constructible in the object language. Note the %Clear call is
		//	inefficient if the underlying destructor implementation of
		//	%any has branch. Keep it as-is at current.
		Clear();
	}

	/*!
	\brief 比较相等。
	\warning 不保证嵌套调用安全。
	\since build 944
	*/
	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const TermNode& x, const TermNode& y) ynothrow
		ImplRet(x.Tags == y.Tags && x.GetContainer() == y.GetContainer()
			&& x.Value == y.Value)

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
	PDefH(void, SetContent, const TermNode& nd)
		ImplExpr(SetContent(nd.container, nd.Value), Tags = nd.Tags)
	//! \pre 间接断言：容器分配器和参数的容器分配器相等。
	PDefH(void, SetContent, TermNode&& nd)
		ImplExpr(SwapContainer(nd), Value = std::move(nd.Value), Tags = nd.Tags)
	//@}
	//! \since build 918
	//@{
	template<typename _tParam>
	inline yimpl(ystdex::enable_if_same_param_t<ValueObject, _tParam>)
	SetValue(_tParam&& arg) ynoexcept_spec(yforward(arg))
	{
		Value = yforward(arg);
	}
	template<typename _tParam, typename... _tParams,
		yimpl(ystdex::enable_if_t<sizeof...(_tParams) != 0
		|| !ystdex::is_same_param<ValueObject, _tParam>::value, int> = 0,
		ystdex::exclude_self_t<std::allocator_arg_t, _tParam, int> = 0)>
	inline yimpl(ystdex::enable_if_inconvertible_t)<_tParam,
		TermNode::allocator_type, void>
	SetValue(_tParam&& arg, _tParams&&... args) ynoexcept_spec(Value.assign(
		std::allocator_arg, std::declval<TermNode::allocator_type&>(),
		yforward(arg), yforward(args)...))
	{
		SetValue(get_allocator(), yforward(arg), yforward(args)...);
	}
	template<typename... _tParams>
	inline void
	SetValue(TermNode::allocator_type a, _tParams&&... args)
		ynoexcept_spec(Value.assign(std::allocator_arg, a, yforward(args)...))
	{
		Value.assign(std::allocator_arg, a, yforward(args)...);
	}
	//@}

	//! \since build 853
	PDefH(void, Add, const TermNode& nd)
		ImplExpr(container.push_back(nd))
	//! \since build 853
	PDefH(void, Add, TermNode&& nd)
		ImplExpr(container.push_back(std::move(nd)))

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

	//! \note 除非 Value 的析构非嵌套调用安全，支持移除任意子节点时的嵌套调用安全。
	//@{
	//! \note 不访问 Tags 。
	PDefH(void, Clear, ) ynothrow
		// XXX: The order can be siginificant.
		ImplExpr(Value.Clear(), ClearContainer())

	/*!
	\brief 清除容器。
	\post 断言：\c IsLeaf(*this) 。
	*/
	void
	ClearContainer() ynothrow;
	//@}

	/*!
	\note 允许被参数中被复制的对象直接或间接地被目标引用。
	\since build 913
	*/
	//@{
	PDefH(void, CopyContainer, const TermNode& nd)
		ImplExpr(GetContainerRef() = Container(nd.GetContainer()))

	PDefH(void, CopyContent, const TermNode& nd)
		ImplExpr(SetContent(TermNode(nd)))

	PDefH(void, CopyValue, const TermNode& nd)
		ImplExpr(Value = ValueObject(nd.Value))
	//@}

private:
	//! \warning 不保证嵌套调用安全。
	//@{
	YB_ATTR_nodiscard YB_PURE static TermNode::Container
	ConCons(const ValueNode::Container&);
	YB_ATTR_nodiscard YB_PURE static TermNode::Container
	ConCons(ValueNode::Container&&);
	YB_ATTR_nodiscard YB_PURE static TermNode::Container
	ConCons(const ValueNode::Container&, allocator_type);
	YB_ATTR_nodiscard YB_PURE static TermNode::Container
	ConCons(ValueNode::Container&&, allocator_type);
	//@}

	//! \since build 934
	YB_ATTR_nodiscard YB_PURE static Container
	ConSub(const Container&, allocator_type);

public:
	template<class _tCon, typename _fCallable,
		yimpl(typename = ystdex::enable_if_t<
		std::is_same<Container&, ystdex::remove_cvref_t<_tCon>&>::value>)>
	static Container
	CreateRecursively(_tCon&& con, _fCallable f)
	{
		Container res(con.get_allocator());

		for(auto&& nd : con)
			res.emplace_back(CreateRecursively(
				ystdex::forward_like<_tCon>(nd.container), f),
				ystdex::invoke(f, ystdex::forward_like<_tCon>(nd.Value)));
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
	\pre 断言：参数不是 \c *this 。
	\note 允许被参数中被转移的对象直接或间接地被目标引用。
	\since build 913
	*/
	//@{
	/*!
	\brief 转移容器。

	转移参数指定的节点的容器到对象。
	转移后的节点的容器是转移前的参数的容器。
	*/
	PDefH(void, MoveContainer, TermNode&& nd)
		ImplExpr(
			YAssert(!ystdex::ref_eq<>()(*this, nd), "Invalid self move found."),
			GetContainerRef() = Container(std::move(nd.GetContainerRef())))

	/*!
	\brief 转移内容。
	\since build 853

	转移参数指定的节点的内容到对象。
	转移后的节点的内容是转移前的参数的内容。
	*/
	PDefH(void, MoveContent, TermNode&& nd)
		ImplExpr(YAssert(!ystdex::ref_eq<>()(*this, nd),
			"Invalid self move found."), SetContent(TermNode(std::move(nd))))

	/*!
	\brief 转移值数据成员。

	转移参数指定的节点的值数据成员到对象。
	转移后的节点的值数据成员是转移前的参数内容。
	*/
	PDefH(void, MoveValue, TermNode&& nd)
		ImplExpr(
			YAssert(!ystdex::ref_eq<>()(*this, nd), "Invalid self move found."),
			Value = ValueObject(std::move(nd.Value)))
	//@}

	PDefH(void, Remove, const_iterator i)
		ImplExpr(erase(i))

	//! \pre 断言：容器分配器和参数的容器分配器相等。
	PDefH(void, SwapContainer, TermNode& nd) ynothrowv
		ImplExpr(YAssert(get_allocator() == nd.get_allocator(),
			"Invalid allocator found."), container.swap(nd.container))

	PDefH(void, SwapContent, TermNode& term) ynothrowv
		ImplExpr(SwapContainer(term), swap(Value, term.Value),
			std::swap(Tags, term.Tags))

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

	friend PDefH(void, swap, TermNode& x, TermNode& y) ynothrowv
		ImplExpr(x.SwapContent(y))
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
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsBranch, const TermNode& nd) ynothrow
	ImplRet(!nd.empty())

/*!
\brief 判断项是否为分支列表节点。
\since build 858
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsBranchedList, const TermNode& nd) ynothrow
	ImplRet(!(nd.empty() || nd.Value))

//! \brief 判断项是否为空节点。
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsEmpty, const TermNode& nd) ynothrow
	ImplRet(!nd)

/*!
\brief 判断项是否为扩展列表节点。
\since build 858
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsExtendedList, const TermNode& nd) ynothrow
	ImplRet(!(nd.empty() && nd.Value))

//! \brief 判断项是否为叶节点。
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsLeaf, const TermNode& nd) ynothrow
	ImplRet(nd.empty())

/*!
\brief 判断项是否为列表节点。
\since build 774
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsList, const TermNode& nd) ynothrow
	ImplRet(!nd.Value)

/*!
\brief 判断项是否为正规节点。
\since build 918
*/
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, IsRegular, const TermNode& nd) ynothrow
	ImplRet(IsLeaf(nd) || IsList(nd))
//@}

//! \since build 928
using YSLib::IsTyped;
/*!
\brief 判断项节点的值数据成员具有指定的目标类型。
\since build 924
*/
template<typename _type>
YB_ATTR_nodiscard YB_ATTR_always_inline YB_PURE inline bool
IsTyped(const TermNode& nd)
{
	return IsTyped<_type>(nd.Value);
}

/*!
\brief 判断项节点是具有指定目标类型的值数据成员的正规节点。
\since build 924
*/
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline bool
IsTypedRegular(const TermNode& nd)
{
	return IsLeaf(nd) && IsTyped<_type>(nd);
}

/*!
\brief 判断项节点是否具有指定的值。
\since build 753
*/
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline bool
HasValue(const TermNode& nd, const _type& x)
{
	return nd.Value == x;
}

//! \since build 853
using YSLib::Access;
//! \since build 854
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline _type&
Access(TermNode& nd)
{
	return nd.Value.Access<_type>();
}
//! \since build 854
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline const _type&
Access(const TermNode& nd)
{
	return nd.Value.Access<_type>();
}

//! \since build 853
using YSLib::AccessPtr;
//! \since build 852
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline observer_ptr<_type>
AccessPtr(TermNode& nd) ynothrow
{
	return nd.Value.AccessPtr<_type>();
}
//! \since build 852
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline observer_ptr<const _type>
AccessPtr(const TermNode& nd) ynothrow
{
	return nd.Value.AccessPtr<_type>();
}

/*!
\brief 断言枝节点。
\pre 断言：参数指定的项是枝节点。
\since build 761
*/
YB_NONNULL(2) inline PDefH(void, AssertBranch, const TermNode& nd,
	const char* msg = "Invalid term found.") ynothrowv
	ImplExpr(yunused(nd), yunused(msg), YAssert(IsBranch(nd), msg))

/*!
\brief 断言分支列表节点。
\pre 断言：参数指定的项是分支列表节点。
\since build 918
*/
YB_NONNULL(2) inline PDefH(void, AssertBranchedList, const TermNode& nd,
	const char* msg = "Invalid term found.") ynothrowv
	ImplExpr(yunused(nd), yunused(msg), YAssert(IsBranchedList(nd), msg))

/*!
\brief 断言具有可表示一等对象的值的标签节点。
\pre 断言：参数的标签可表示一等对象的值。
\note 较 EnsureValueTags 更严格，对不符合要求的项总是断言失败。
\sa EnsureValueTags
\since build 939
*/
YB_NONNULL(2) inline
	PDefH(void, AssertValueTags, const TermNode& nd, const char* msg
	= "Invalid term of first-class value found.") ynothrowv
	ImplExpr(yunused(nd), yunused(msg),
		YAssert(nd.Tags == TermTags::Unqualified, msg))

//! \brief 创建项节点。
//@{
//! \since build 942
template<typename... _tParam, typename... _tParams>
YB_ATTR_nodiscard YB_PURE inline
yimpl(ystdex::enable_if_inconvertible_t)<ystdex::head_of_t<_tParams...>,
	TermNode::allocator_type, TermNode>
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

//! \since build 852
//@{
// NOTE: Like %YSLib::GetValueOf.
YB_ATTR_nodiscard YB_PURE inline
	PDefH(ValueObject, GetValueOf, observer_ptr<const TermNode> p_term)
	ImplRet(ystdex::call_value_or([](const TermNode& nd) -> const ValueObject&{
		return nd.Value;
	}, p_term))

// NOTE: Like %YSLib::GetValuePtrOf.
YB_ATTR_nodiscard YB_PURE inline PDefH(observer_ptr<const ValueObject>,
	GetValuePtrOf, observer_ptr<const TermNode> p_term)
	ImplRet(ystdex::call_value_or(
		ystdex::compose(make_observer<const ValueObject>, ystdex::addrof<>(),
		[](const TermNode& nd) -> const ValueObject&{
		return nd.Value;
	}), p_term))

//! \since build 872
//@{
//! \pre 断言：参数指定的项是枝节点。
//@{
YB_ATTR_nodiscard YB_PURE inline
	PDefH(TermNode&, AccessFirstSubterm, TermNode& nd)
	ImplRet(AssertBranch(nd), NPL::Deref(nd.begin()))
YB_ATTR_nodiscard YB_PURE inline
	PDefH(const TermNode&, AccessFirstSubterm, const TermNode& nd)
	ImplRet(AssertBranch(nd), NPL::Deref(nd.begin()))

YB_ATTR_nodiscard YB_PURE inline
	PDefH(TermNode&&, MoveFirstSubterm, TermNode& nd)
	ImplRet(std::move(AccessFirstSubterm(nd)))
//@}

YB_ATTR_nodiscard inline
	PDefH(shared_ptr<TermNode>, ShareMoveTerm, TermNode& nd)
	ImplRet(YSLib::share_move(nd.get_allocator(), nd))
YB_ATTR_nodiscard inline
	PDefH(shared_ptr<TermNode>, ShareMoveTerm, TermNode&& nd)
	ImplRet(YSLib::share_move(nd.get_allocator(), nd))
//@}

//! \pre 断言：项节点容器非空。
inline PDefH(void, RemoveHead, TermNode& nd) ynothrowv
	ImplExpr(YAssert(!nd.empty(), "Empty term container found."),
		nd.erase(nd.begin()))

/*!
\brief 根据节点和节点容器创建操作设置目标节点的值或子节点。
\note 可用于创建副本。
\warning 不保证嵌套调用安全。
*/
template<typename _fCallable, class _tNode>
void
SetContentWith(TermNode& dst, _tNode&& nd, _fCallable f)
{
	// NOTE: Similar to %YSLib::SetContentWith.
	auto con(yforward(nd).CreateWith(f));
	auto vo(ystdex::invoke(f, yforward(nd).Value));

	dst.SetContent(std::move(con), std::move(vo));
}
//@}
//@}

/*!
\brief 遍历子节点。
\note 使用 ADL AccessPtr 。
\since build 803

使用第一参数指定的遍历函数访问第二参数指定的节点容器中的子节点。
兼容 TermNode 和 ValueNode ，主要用于语法变换。
*/
template<typename _fCallable, class _tNode>
void
TraverseSubnodes(_fCallable f, const _tNode& nd)
{
	// TODO: Null coalescing or variant value?
	if(const auto p = AccessPtr<YSLib::NodeSequence>(nd))
		for(const auto& sub : *p)
			ystdex::invoke(f, _tNode(sub));
	else
		for(const auto& sub : nd)
			ystdex::invoke(f, sub);
}


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
\brief 词素标记器：转换输入为以词素为作为值数据成员的节点。
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
\brief 读取器状态：支持解析一个记号或列表的解析器的辅助状态。
\warning 非虚析构。
\since build 899
*/
class YF_API ReaderState
{
public:
	char LeftDelimiter = '(';
	char RightDelimiter = ')';
	size_t LeftDelimiterCount = 0;
	size_t RightDelimiterCount = 0;

	DefDeCtor(ReaderState)

	DefPred(const ynothrow, Balanced, LeftDelimiterCount == RightDelimiterCount)
	DefPred(const ynothrow, Valid, LeftDelimiterCount >= RightDelimiterCount)

	PDefH(void, Reset, ) ynothrow
		ImplExpr(*this = ReaderState())

	//! \note 使用 ADL ToLexeme 。
	template<class _type>
	void
	Update(const _type& val)
	{
		UpdateLexeme(ToLexeme(val));
	}

	void
	UpdateLexeme(const string&);
};


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
	//! \since build 941
	YSLib::default_allocator<yimpl(byte)> allocator;

public:
	//! \since build 618
	DefDeCtor(Session)
	//! \since build 887
	Session(YSLib::default_allocator<yimpl(byte)> a)
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
	\exception LoggedEvent 关键失败：无法访问源内容。
	\sa DefaultParser
	\sa SContextParsers
	*/
	//@{
	/*!
	\brief 使用解析器处理输入范围。
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
		return ProcessSequence(first, last,
			typename std::iterator_traits<_tIn>::iterator_category());
	}
	template<typename _tIn, typename _fParse>
	YB_ATTR_nodiscard inline _fParse
	Process(_tIn first, _tIn last, _fParse parse)
	{
		return ProcessSequence(first, last, parse,
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

	/*!
	\brief 使用解析器处理输入范围读取列表或元素。
	\since build 899

	同 Process ，但维护读取器状态，当读取完整的列表或元素时终止，返回解析器和迭代器。
	*/
	//@{
	template<typename _tIn>
	YB_ATTR_nodiscard inline pair<DefaultParser, _tIn>
	ProcessOne(ReaderState& rs, _tIn first, _tIn last)
	{
		return ProcessSequenceOne(rs, first, last,
			typename std::iterator_traits<_tIn>::iterator_category());
	}
	template<typename _tIn, typename _fParse>
	YB_ATTR_nodiscard inline pair<_fParse, _tIn>
	ProcessOne(ReaderState& rs, _tIn first, _tIn last, _fParse parse)
	{
		return ProcessSequenceOne(rs, first, last, parse,
			typename std::iterator_traits<_tIn>::iterator_category());
	}
	template<typename _tRange>
	YB_ATTR_nodiscard inline
		pair<DefaultParser, ystdex::range_iterator_t<const _tRange>>
	ProcessOne(ReaderState& rs, const _tRange& c)
	{
		return ProcessOne(rs, ystdex::begin(c), ystdex::end(c));
	}
	template<typename _tRange, typename _fParse>
	YB_ATTR_nodiscard inline
		pair<_fParse, ystdex::range_iterator_t<const _tRange>>
	ProcessOne(ReaderState& rs, const _tRange& c, _fParse parse)
	{
		return ProcessOne(rs, ystdex::begin(c), ystdex::end(c), parse);
	}
	//@}
	//@}

private:
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

	//! \since build 899
	//@{
	template<typename _tIn, class _tTag>
	YB_ATTR_nodiscard inline DefaultParser
	ProcessSequence(_tIn first, _tIn last, _tTag)
	{
		DefaultParser parse(Lexer, allocator);

		yunused(ProcessSequence(first, last, ystdex::ref(parse), _tTag()));
		// NOTE: Return the parser object with copy elimination.
		return parse;
	}
	template<typename _tIn, typename _fParse>
	YB_ATTR_nodiscard inline _fParse
	ProcessSequence(_tIn first, _tIn last, _fParse parse,
		std::input_iterator_tag)
	{
		std::for_each(first, last, parse);
		return parse;
	}
	template<typename _tRandom, typename _fParse>
	YB_ATTR_nodiscard inline _fParse
	ProcessSequence(_tRandom first, _tRandom last, _fParse parse,
		std::random_access_iterator_tag)
	{
		// XXX: This should be safe since there should be no more space out of
		//	the range of %size_t.
		ProcessReserve(parse, size_t(last - first));
		return ProcessSequence(first, last, parse, std::input_iterator_tag());
	}

	template<typename _tIn, class _tTag>
	YB_ATTR_nodiscard inline pair<DefaultParser, _tIn>
	ProcessSequenceOne(ReaderState& rs, _tIn first, _tIn last, _tTag)
	{
		DefaultParser parse(Lexer, allocator);

		return {std::move(parse), ProcessSequenceOne(rs, first, last,
			ystdex::ref(parse), _tTag()).second};
	}
	template<typename _tIn, typename _fParse>
	YB_ATTR_nodiscard inline pair<_fParse, _tIn>
	ProcessSequenceOne(ReaderState& rs, _tIn first, _tIn last, _fParse parse,
		std::input_iterator_tag)
	{
		static_assert(std::is_lvalue_reference<decltype(GetResult(parse))>(),
			"Reader requires a parser has the result in a fixed location.");

		const auto& res(GetResult(parse));
		auto size(res.size());

		while(first != last)
		{
			parse(*first);

			const auto new_size(res.size());

			if(new_size != size)
			{
				rs.Update(res.back());
				if(rs.IsBalanced() && !ystdex::unref(parse).IsUpdating())
					break;
				size = new_size;
			}
			++first;
		}
		return {parse, first};
	}
	template<typename _tRandom, typename _fParse>
	YB_ATTR_nodiscard inline pair<_fParse, _tRandom>
	ProcessSequenceOne(ReaderState& rs, _tRandom first, _tRandom last,
		_fParse parse, std::random_access_iterator_tag)
	{
		// XXX: This should be safe since there should be no more space out of
		//	the range of %size_t.
		ProcessReserve(parse, size_t(last - first));
		return ProcessSequenceOne(rs, first, last, parse,
			std::input_iterator_tag());
	}
	//@}

public:
	/*!
	\brief 取使用的分配器。
	\since build 890
	*/
	YB_ATTR_nodiscard YB_PURE PDefH(YSLib::default_allocator<yimpl(byte)>,
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
			tms.push(TermNode(a));
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
template<typename _tIn>
YB_ATTR_nodiscard inline _tIn
Reduce(TermNode& term, _tIn first, _tIn last)
{
	return SContext::Reduce(term, first, last,
		LexemeTokenizer{term.get_allocator()});
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

