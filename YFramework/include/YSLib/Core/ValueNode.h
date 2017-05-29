/*
	© 2012-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ValueNode.h
\ingroup Core
\brief 值类型节点。
\version r3158
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:03:44 +0800
\par 修改时间:
	2017-05-27 00:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::ValueNode
*/


#ifndef YSL_INC_Core_ValueNode_h_
#define YSL_INC_Core_ValueNode_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YObject // for ystdex::invoke;
#include <ystdex/path.hpp>
#include <ystdex/set.hpp> // for ystdex::mapped_set;
#include <numeric> // for std::accumulate;

namespace YSLib
{

//! \since build 674
//@{
//! \brief 标记列表构造容器。
yconstexpr const struct ListContainerTag{} ListContainer{};

//! \brief 标记不使用容器。
yconstexpr const struct NoContainerTag{} NoContainer{};
//@}


/*!
\brief 值类型节点。
\warning 非虚析构。
\warning 作为子节点时应保证修改操作不影响键（名称）的等价性，否则访问容器行为未定义。
\since build 330

包含名称字符串和值类型对象的对象节点。
*/
class YF_API ValueNode : private ystdex::totally_ordered<ValueNode>,
	private ystdex::totally_ordered<ValueNode, string>
{
public:
	using Container = ystdex::mapped_set<ValueNode, ystdex::less<>>;
	//! \since build 678
	using key_type = typename Container::key_type;
	//! \since build 460
	using iterator = Container::iterator;
	//! \since build 460
	using const_iterator = Container::const_iterator;
	//! \since build 696
	using reverse_iterator = Container::reverse_iterator;
	//! \since build 696
	using const_reverse_iterator = Container::const_reverse_iterator;

private:
	string name{};
	/*!
	\brief 子节点容器。
	\since build 667
	*/
	Container container{};

public:
	//! \since build 667
	ValueObject Value{};

	DefDeCtor(ValueNode)
	/*!
	\brief 构造：使用容器对象。
	\since build 502
	*/
	ValueNode(Container con)
		: container(std::move(con))
	{}
	/*!
	\brief 构造：使用字符串引用和值类型对象构造参数。
	\note 不使用容器。
	\since build 674
	*/
	template<typename _tString, typename... _tParams>
	inline
	ValueNode(NoContainerTag, _tString&& str, _tParams&&... args)
		: name(yforward(str)), Value(yforward(args)...)
	{}
	/*!
	\brief 构造：使用容器对象、字符串引用和值类型对象构造参数。
	\since build 502
	*/
	template<typename _tString, typename... _tParams>
	ValueNode(Container con, _tString&& str, _tParams&&... args)
		: name(yforward(str)), container(std::move(con)),
		Value(yforward(args)...)
	{}
	/*!
	\brief 构造：使用输入迭代器对。
	\since build 337
	*/
	template<typename _tIn>
	inline
	ValueNode(const pair<_tIn, _tIn>& pr)
		: container(pr.first, pr.second)
	{}
	/*!
	\brief 构造：使用输入迭代器对、字符串引用和值参数。
	\since build 340
	*/
	template<typename _tIn, typename _tString>
	inline
	ValueNode(const pair<_tIn, _tIn>& pr, _tString&& str)
		: name(yforward(str)), container(pr.first, pr.second)
	{}
	/*!
	\brief 原地构造：使用容器、名称和值的参数元组。
	\since build 767
	*/
	//@{
	template<typename... _tParams1>
	inline
	ValueNode(tuple<_tParams1...> args1)
		: container(ystdex::make_from_tuple<Container>(args1))
	{}
	template<typename... _tParams1, typename... _tParams2>
	inline
	ValueNode(tuple<_tParams1...> args1, tuple<_tParams2...> args2)
		: name(ystdex::make_from_tuple<string>(args2)),
		container(ystdex::make_from_tuple<Container>(args1))
	{}
	template<typename... _tParams1, typename... _tParams2,
		typename... _tParams3>
	inline
	ValueNode(tuple<_tParams1...> args1, tuple<_tParams2...> args2,
		tuple<_tParams3...> args3)
		: name(ystdex::make_from_tuple<string>(args2)),
		container(ystdex::make_from_tuple<Container>(args1)),
		Value(ystdex::make_from_tuple<ValueObject>(args3))
	{}
	//@}

	DefDeCopyMoveCtor(ValueNode)

	//! \since build 768
	//@{
	//! \brief 复制赋值：使用复制和交换。
	PDefHOp(ValueNode&, =, const ValueNode& node)
		ImplRet(ystdex::copy_and_swap(*this, node))
	DefDeMoveAssignment(ValueNode)
	//@}

	//! \since build 336
	DefBoolNeg(explicit, bool(Value) || !container.empty())

	//! \since build 730
	//@{
	PDefHOp(const ValueNode&, +=, const ValueNode& node)
		ImplRet(Add(node), *this)
	PDefHOp(const ValueNode&, +=, ValueNode&& node)
		ImplRet(Add(std::move(node)), *this)

	PDefHOp(const ValueNode&, -=, const ValueNode& node)
		ImplRet(Remove(node), *this)
	PDefHOp(const ValueNode&, -=, const string& str)
		ImplRet(Remove(str), *this)
	/*!
	\brief 替换同名子节点。
	\return 自身引用。
	*/
	//@{
	PDefHOp(ValueNode&, /=, const ValueNode& node)
		ImplRet(*this %= node, *this)
	PDefHOp(ValueNode&, /=, ValueNode&& node)
		ImplRet(*this %= std::move(node), *this)
	//@}
	/*!
	\brief 替换同名子节点。
	\return 子节点引用。
	*/
	//@{
	const ValueNode&
	operator%=(const ValueNode&);
	const ValueNode&
	operator%=(const ValueNode&&);
	//@}
	//@}

	//! \since build 673
	friend PDefHOp(bool, ==, const ValueNode& x, const ValueNode& y) ynothrow
		ImplRet(x.name == y.name)
	//! \since build 679
	friend PDefHOp(bool, ==, const ValueNode& x, const string& str) ynothrow
		ImplRet(x.name == str)
	//! \since build 730
	template<typename _tKey>
	friend bool
	operator==(const ValueNode& x, const _tKey& str) ynothrow
	{
		return x.name == str;
	}

	//! \since build 673
	friend PDefHOp(bool, <, const ValueNode& x, const ValueNode& y) ynothrow
		ImplRet(x.name < y.name)
	//! \since build 678
	friend PDefHOp(bool, <, const ValueNode& x, const string& str) ynothrow
		ImplRet(x.name < str)
	//! \since build 730
	template<typename _tKey>
	friend bool
	operator<(const ValueNode& x, const _tKey& str) ynothrow
	{
		return x.name < str;
	}
	//! \since build 730
	template<typename _tKey>
	friend bool
	operator<(const _tKey& str, const ValueNode& y) ynothrow
	{
		return str < y.name;
	}
	//! \since build 679
	friend PDefHOp(bool, >, const ValueNode& x, const string& str) ynothrow
		ImplRet(x.name > str)
	//! \since build 730
	template<typename _tKey>
	friend bool
	operator>(const ValueNode& x, const _tKey& str) ynothrow
	{
		return x.name > str;
	}
	//! \since build 730
	template<typename _tKey>
	friend bool
	operator>(const _tKey& str, const ValueNode& y) ynothrow
	{
		return str > y.name;
	}

	//! \since build 680
	template<typename _tString>
	ValueNode&
	operator[](_tString&& str)
	{
		return *ystdex::try_emplace(container, str, NoContainer, yforward(str))
			.first;
	}
	//! \since build 667
	template<class _tCon>
	const ValueNode&
	operator[](const ystdex::path<_tCon>& pth)
	{
		auto p(this);

		for(const auto& n : pth)
			p = &(*p)[n];
		return *p;
	}

	/*!
	\brief 取子节点容器引用。
	\since build 664
	*/
	DefGetter(const ynothrow, const Container&, Container, container)
	/*!
	\brief 取子节点容器引用。
	\since build 667
	*/
	DefGetter(ynothrow, Container&, ContainerRef, container)
	DefGetter(const ynothrow, const string&, Name, name)

	/*!
	\warning 设置子节点的容器可能导致未定义行为。
	\warning 被转移的内容不应被覆盖，否则行为未定义。
	*/
	//@{
	/*!
	\brief 设置子节点容器内容。
	\since build 774
	*/
	//@{
	PDefH(void, SetChildren, const Container& con)
		ImplExpr(container = con)
	PDefH(void, SetChildren, Container&& con)
		ImplExpr(container = std::move(con))
	//! \since build 776
	PDefH(void, SetChildren, ValueNode&& node)
		ImplExpr(container = std::move(node.container))
	//@}
	/*!
	\note 设置子节点容器和值的内容。
	\warning 除转移外非强异常安全。
	\since build 734
	*/
	//@{
	//! \since build 776
	template<class _tCon, class _tValue>
	yimpl(ystdex::enable_if_t)<
		ystdex::and_<std::is_assignable<Container, _tCon&&>,
		std::is_assignable<ValueObject, _tValue&&>>::value>
	SetContent(_tCon&& con, _tValue&& val) ynoexcept(ystdex::and_<
		std::is_nothrow_assignable<Container, _tCon&&>,
		std::is_nothrow_assignable<ValueObject, _tValue&&>>())
	{
		yunseq(container = yforward(con), Value = yforward(val));
	}
	PDefH(void, SetContent, const ValueNode& node)
		ImplExpr(SetContent(node.GetContainer(), node.Value))
	PDefH(void, SetContent, ValueNode&& node)
		ImplExpr(SwapContent(node))
	//@}
	//@}

	/*!
	\brief 设置子节点容器，并设置值的内容为指定对象的内容引用。
	\sa ValueObject::MakeIndirect
	\since build 747
	*/
	//@{
	void
	SetContentIndirect(Container, const ValueObject&) ynothrow;
	PDefH(void, SetContentIndirect, const ValueNode& node)
		ImplExpr(SetContentIndirect(node.GetContainer(), node.Value))
	//@}


	//! \since build 667
	PDefH(bool, Add, const ValueNode& node)
		ImplRet(insert(node).second)
	//! \since build 667
	PDefH(bool, Add, ValueNode&& node)
		ImplRet(insert(std::move(node)).second)

	/*!
	\brief 添加参数节点指定容器和值的子节点。
	\since build 775
	*/
	//@{
	//! \sa try_emplace
	//@{
	template<typename _tKey>
	bool
	AddChild(_tKey&& k, const ValueNode& node)
	{
		return
			try_emplace(k, node.GetContainer(), yforward(k), node.Value).second;
	}
	template<typename _tKey>
	bool
	AddChild(_tKey&& k, ValueNode&& node)
	{
		return try_emplace(k, std::move(node.GetContainerRef()), yforward(k),
			std::move(node.Value)).second;
	}
	//@}
	//! \sa try_emplace_hint
	//@{
	template<typename _tKey>
	void
	AddChild(const_iterator hint, _tKey&& k, const ValueNode& node)
	{
		return try_emplace_hint(hint, k, node.GetContainer(), yforward(k),
			node.Value);
	}
	template<typename _tKey>
	void
	AddChild(const_iterator hint, _tKey&& k, ValueNode&& node)
	{
		return try_emplace_hint(hint, k, std::move(node.GetContainerRef()),
			yforward(k), std::move(node.Value));
	}
	//@}
	//@}

	/*!
	\brief 添加参数指定值的子节点。
	\since build 757
	*/
	//@{
	//! \sa try_emplace
	template<typename _tKey, typename... _tParams>
	inline bool
	AddValue(_tKey&& k, _tParams&&... args)
	{
		return
			try_emplace(k, NoContainer, yforward(k), yforward(args)...).second;
	}
	//! \sa try_emplace_hint
	template<typename _tKey, typename... _tParams>
	inline bool
	AddValue(const_iterator hint, _tKey&& k, _tParams&&... args)
	{
		return try_emplace_hint(hint, k, NoContainer, yforward(k),
			yforward(args)...).second;
	}
	//@}

	/*!
	\brief 向容器添加参数指定值的子节点。
	\sa emplace_hint
	\since build 674
	*/
	template<typename _tString, typename... _tParams>
	static bool
	AddValueTo(Container& con, _tString&& str, _tParams&&... args)
	{
		return ystdex::try_emplace(con, str,
			NoContainer, yforward(str), yforward(args)...).second;
	}

	//! \note 清理容器和修改值的操作之间的顺序未指定。
	//@{
	/*!
	\brief 清除节点。
	\post <tt>!Value && empty()</tt> 。
	\since build 666
	*/
	PDefH(void, Clear, ) ynothrow
		ImplExpr(Value.Clear(), ClearContainer())

	/*!
	\brief 清除容器并设置值。
	\since build 776
	*/
	//@{
	PDefH(void, ClearTo, const ValueObject& vo) ynothrow
		ImplExpr(ClearContainer(), Value = vo)
	PDefH(void, ClearTo, ValueObject&& vo) ynothrow
		ImplExpr(ClearContainer(), Value = std::move(vo))
	//@}
	//@}

	/*!
	\brief 清除节点容器。
	\post \c empty() 。
	\since build 667
	*/
	PDefH(void, ClearContainer, ) ynothrow
		ImplExpr(container.clear())

	//! \brief 递归创建容器副本。
	//@{
	//! \since build 767
	static Container
	CreateRecursively(const Container&, IValueHolder::Creation);
	//! \since build 787
	template<typename _fCallable>
	static Container
	CreateRecursively(Container& con, _fCallable f)
	{
		Container res;

		for(auto& tm : con)
			res.emplace(CreateRecursively(tm.GetContainerRef(), f),
				tm.GetName(), ystdex::invoke(f, tm.Value));
		return res;
	}
	//! \since build 785
	template<typename _fCallable>
	static Container
	CreateRecursively(const Container& con, _fCallable f)
	{
		Container res;

		for(auto& tm : con)
			res.emplace(CreateRecursively(tm.GetContainer(), f), tm.GetName(),
				ystdex::invoke(f, tm.Value));
		return res;
	}

	//! \since build 767
	PDefH(Container, CreateWith, IValueHolder::Creation c) const
		ImplRet(CreateRecursively(container, c))
	//! \since build 787
	template<typename _fCallable>
	Container
	CreateWith(_fCallable f)
	{
		return CreateRecursively(container, f);
	}
	//! \since build 785
	template<typename _fCallable>
	Container
	CreateWith(_fCallable f) const
	{
		return CreateRecursively(container, f);
	}
	//@}

	/*!
	\brief 若指定名称子节点不存在则按指定值初始化。
	\since build 781
	*/
	//@{
	//! \return 按指定名称查找的指定类型的子节点的值的引用。
	//@{
	/*!
	\sa ValueObject::Access
	\since build 681
	*/
	template<typename _type, typename _tString, typename... _tParams>
	inline _type&
	Place(_tString&& str, _tParams&&... args)
	{
		return PlaceValue<_type>(yforward(str), yforward(args)...).template
			Access<_type>();
	}

	/*!
	\pre 间接断言：存储对象已存在时类型和访问的类型一致。
	\sa ValueObject::GetObject
	*/
	template<typename _type, typename _tString, typename... _tParams>
	inline _type&
	PlaceUnchecked(_tString&& str, _tParams&&... args)
	{
		return PlaceValue<_type>(yforward(str), yforward(args)...).template
			GetObject<_type>();
	}
	//@}

	//! \brief 初始化的值对象引用。
	template<typename _type, typename _tString, typename... _tParams>
	inline ValueObject&
	PlaceValue(_tString&& str, _tParams&&... args)
	{
		return try_emplace(str, NoContainer, yforward(str),
			ystdex::in_place<_type>, yforward(args)...).first->Value;
	}
	//@}

	PDefH(bool, Remove, const ValueNode& node)
		ImplRet(erase(node) != 0)
	//! \since build 754
	PDefH(iterator, Remove, const_iterator i)
		ImplRet(erase(i))
	//! \since build 680
	template<typename _tKey, yimpl(typename = ystdex::enable_if_t<
		ystdex::is_interoperable<const _tKey&, const string&>::value>)>
	inline bool
	Remove(const _tKey& k)
	{
		return ystdex::erase_first(container, k);
	}

	/*!
	\brief 复制满足条件的子节点。
	\since build 664
	*/
	template<typename _func>
	Container
	SelectChildren(_func f) const
	{
		Container res;

		ystdex::for_each_if(begin(), end(), f, [&](const ValueNode& nd){
			res.insert(nd);
		});
		return res;
	}

	//! \since build 667
	//@{
	//! \brief 转移满足条件的子节点。
	template<typename _func>
	Container
	SplitChildren(_func f)
	{
		Container res;

		std::for_each(begin(), end(), [&](const ValueNode& nd){
			res.emplace(NoContainer, nd.GetName());
		});
		ystdex::for_each_if(begin(), end(), f, [&, this](const ValueNode& nd){
			const auto& child_name(nd.GetName());

			Deref(res.find(child_name)).Value = std::move(nd.Value);
			Remove(child_name);
		});
		return res;
	}

	//! \warning 不检查容器之间的所有权，保持循环引用状态析构引起未定义行为。
	//@{
	//! \brief 交换容器。
	PDefH(void, SwapContainer, ValueNode& node) ynothrow
		ImplExpr(container.swap(node.container))

	//! \brief 交换容器和值。
	void
	SwapContent(ValueNode&) ynothrow;
	//@}
	//@}

	/*!
	\brief 抛出索引越界异常。
	\throw std::out_of_range 索引越界。
	\since build 730
	*/
	YB_NORETURN static void
	ThrowIndexOutOfRange();

	/*!
	\brief 抛出名称错误异常。
	\throw std::out_of_range 名称错误。
	\since build 730
	*/
	YB_NORETURN static void
	ThrowWrongNameFound();

	//! \since build 460
	//@{
	PDefH(iterator, begin, )
		ImplRet(GetContainerRef().begin())
	PDefH(const_iterator, begin, ) const
		ImplRet(GetContainer().begin())

	//! \since build 767
	DefFwdTmpl(, pair<iterator YPP_Comma bool>, emplace,
		container.emplace(yforward(args)...))

	//! \since build 667
	DefFwdTmpl(, iterator, emplace_hint,
		container.emplace_hint(yforward(args)...))

	//! \since build 598
	PDefH(bool, empty, ) const ynothrow
		ImplRet(container.empty())

	PDefH(iterator, end, )
		ImplRet(GetContainerRef().end())
	PDefH(const_iterator, end, ) const
		ImplRet(GetContainer().end())
	//@}

	//! \since build 761
	DefFwdTmpl(-> decltype(container.erase(yforward(args)...)), auto,
		erase, container.erase(yforward(args)...))

	//! \since build 667
	DefFwdTmpl(-> decltype(container.insert(yforward(args)...)), auto,
		insert, container.insert(yforward(args)...))

	//! \since build 681
	//@{
	template<typename _tKey, class _tParam>
	yimpl(ystdex::enable_if_inconvertible_t)<_tKey&&, const_iterator,
		std::pair<iterator, bool>>
	insert_or_assign(_tKey&& k, _tParam&& arg)
	{
		return ystdex::insert_or_assign(container, yforward(k), yforward(arg));
	}
	template<typename _tKey, class _tParam>
	iterator
	insert_or_assign(const_iterator hint, _tKey&& k, _tParam&& arg)
	{
		return ystdex::insert_or_assign_hint(container, hint, yforward(k),
			yforward(arg));
	}
	//@}

	//! \since build 696
	//@{
	PDefH(reverse_iterator, rbegin, )
		ImplRet(GetContainerRef().rbegin())
	PDefH(const_reverse_iterator, rbegin, ) const
		ImplRet(GetContainer().rbegin())

	PDefH(reverse_iterator, rend, )
		ImplRet(GetContainerRef().rend())
	PDefH(const_reverse_iterator, rend, ) const
		ImplRet(GetContainer().rend())
	//@}

	/*!
	\sa ystdex::mapped_set
	\sa ystdex::set_value_move
	\since build 681
	*/
	friend PDefH(ValueNode, set_value_move, ValueNode& node)
		ImplRet({std::move(node.GetContainerRef()), node.GetName(),
			std::move(node.Value)})

	//! \since build 598
	PDefH(size_t, size, ) const ynothrow
		ImplRet(container.size())

	/*!
	\brief 交换。
	\since build 710
	*/
	YF_API friend void
	swap(ValueNode&, ValueNode&) ynothrow;

	//! \since build 681
	template<typename _tKey, typename... _tParams>
	yimpl(ystdex::enable_if_inconvertible_t)<_tKey&&, const_iterator,
		std::pair<iterator, bool>>
	try_emplace(_tKey&& k, _tParams&&... args)
	{
		return ystdex::try_emplace(container, yforward(k), yforward(args)...);
	}
	//! \since build 681
	template<typename _tKey, typename... _tParams>
	iterator
	try_emplace(const_iterator hint, _tKey&& k, _tParams&&... args)
	{
		return ystdex::try_emplace_hint(container, hint, yforward(k),
			yforward(args)...);
	}
};

//! \relates ValueNode
//@{
//! \since build 666
//@{
/*!
\brief 访问节点的指定类型对象。
\exception std::bad_cast 空实例或类型检查失败 。
*/
//@{
template<typename _type>
inline _type&
Access(ValueNode& node)
{
	return node.Value.Access<_type>();
}
template<typename _type>
inline const _type&
Access(const ValueNode& node)
{
	return node.Value.Access<_type>();
}
//@}

//! \since build 670
//@{
//! \brief 访问节点的指定类型对象指针。
//@{
template<typename _type>
inline observer_ptr<_type>
AccessPtr(ValueNode& node) ynothrow
{
	return node.Value.AccessPtr<_type>();
}
template<typename _type>
inline observer_ptr<const _type>
AccessPtr(const ValueNode& node) ynothrow
{
	return node.Value.AccessPtr<_type>();
}
//@}
/*!
\brief 访问节点的指定类型对象指针。
\since build 749
*/
//@{
template<typename _type, typename _tNodeOrPointer>
inline auto
AccessPtr(observer_ptr<_tNodeOrPointer> p) ynothrow
	-> decltype(YSLib::AccessPtr<_type>(*p))
{
	return p ? YSLib::AccessPtr<_type>(*p) : nullptr;
}
//@}
//@}
//@}

//! \since build 749
//@{
//! \brief 取指定名称指称的值。
YF_API ValueObject
GetValueOf(observer_ptr<const ValueNode>);

//! \brief 取指定名称指称的值的指针。
YF_API observer_ptr<const ValueObject>
GetValuePtrOf(observer_ptr<const ValueNode>);
//@}
//@}


//! \since build 730
//@{
template<typename _tKey>
observer_ptr<ValueNode>
AccessNodePtr(ValueNode::Container*, const _tKey&) ynothrow;
template<typename _tKey>
observer_ptr<const ValueNode>
AccessNodePtr(const ValueNode::Container*, const _tKey&) ynothrow;

/*!
\brief 访问节点。
\throw std::out_of_range 未找到对应节点。
*/
//@{
//! \since build 670
YF_API ValueNode&
AccessNode(ValueNode::Container*, const string&);
//! \since build 670
YF_API const ValueNode&
AccessNode(const ValueNode::Container*, const string&);
template<typename _tKey>
ValueNode&
AccessNode(ValueNode::Container* p_con, const _tKey& name)
{
	if(const auto p = YSLib::AccessNodePtr(p_con, name))
		return *p;
	ValueNode::ThrowWrongNameFound();
}
template<typename _tKey>
const ValueNode&
AccessNode(const ValueNode::Container* p_con, const _tKey& name)
{
	if(const auto p = YSLib::AccessNodePtr(p_con, name))
		return *p;
	ValueNode::ThrowWrongNameFound();
}
template<typename _tKey>
inline ValueNode&
AccessNode(observer_ptr<ValueNode::Container> p_con, const _tKey& name)
{
	return YSLib::AccessNode(p_con.get(), name);
}
template<typename _tKey>
inline const ValueNode&
AccessNode(observer_ptr<const ValueNode::Container> p_con, const _tKey& name)
{
	return YSLib::AccessNode(p_con.get(), name);
}
template<typename _tKey>
inline ValueNode&
AccessNode(ValueNode::Container& con, const _tKey& name)
{
	return YSLib::AccessNode(&con, name);
}
template<typename _tKey>
inline const ValueNode&
AccessNode(const ValueNode::Container& con, const _tKey& name)
{
	return YSLib::AccessNode(&con, name);
}
/*!
\note 时间复杂度 O(n) 。
\since build 670
*/
//@{
YF_API ValueNode&
AccessNode(ValueNode&, size_t);
YF_API const ValueNode&
AccessNode(const ValueNode&, size_t);
//@}
template<typename _tKey, yimpl(typename = typename ystdex::enable_if_t<
	ystdex::is_interoperable<const _tKey&, const string&>::value>)>
inline ValueNode&
AccessNode(ValueNode& node, const _tKey& name)
{
	return YSLib::AccessNode(node.GetContainerRef(), name);
}
template<typename _tKey, yimpl(typename = typename ystdex::enable_if_t<
	ystdex::is_interoperable<const _tKey&, const string&>::value>)>
inline const ValueNode&
AccessNode(const ValueNode& node, const _tKey& name)
{
	return YSLib::AccessNode(node.GetContainer(), name);
}
//! \since build 670
//@{
//! \note 使用 ADL \c AccessNode 。
template<class _tNode, typename _tIn>
_tNode&&
AccessNode(_tNode&& node, _tIn first, _tIn last)
{
	return std::accumulate(first, last, ystdex::ref(node),
		[](_tNode&& nd, decltype(*first) c){
		return ystdex::ref(AccessNode(nd, c));
	});
}
//! \note 使用 ADL \c begin 和 \c end 指定范围迭代器。
template<class _tNode, typename _tRange,
	yimpl(typename = typename ystdex::enable_if_t<
	!std::is_constructible<const string&, const _tRange&>::value>)>
inline auto
AccessNode(_tNode&& node, const _tRange& c)
	-> decltype(YSLib::AccessNode(yforward(node), begin(c), end(c)))
{
	return YSLib::AccessNode(yforward(node), begin(c), end(c));
}
//@}
//@}

//! \brief 访问节点指针。
//@{
//! \since build 670
YF_API observer_ptr<ValueNode>
AccessNodePtr(ValueNode::Container&, const string&) ynothrow;
//! \since build 670
YF_API observer_ptr<const ValueNode>
AccessNodePtr(const ValueNode::Container&, const string&) ynothrow;
template<typename _tKey>
observer_ptr<ValueNode>
AccessNodePtr(ValueNode::Container& con, const _tKey& name) ynothrow
{
	return make_observer(ystdex::call_value_or<ValueNode*>(ystdex::addrof<>(),
		con.find(name), {}, end(con)));
}
template<typename _tKey>
observer_ptr<const ValueNode>
AccessNodePtr(const ValueNode::Container& con, const _tKey& name) ynothrow
{
	return make_observer(ystdex::call_value_or<const ValueNode*>(
		ystdex::addrof<>(), con.find(name), {}, end(con)));
}
template<typename _tKey>
inline observer_ptr<ValueNode>
AccessNodePtr(ValueNode::Container* p_con, const _tKey& name) ynothrow
{
	return p_con ? YSLib::AccessNodePtr(*p_con, name) : nullptr;
}
template<typename _tKey>
inline observer_ptr<const ValueNode>
AccessNodePtr(const ValueNode::Container* p_con, const _tKey& name) ynothrow
{
	return p_con ? YSLib::AccessNodePtr(*p_con, name) : nullptr;
}
template<typename _tKey>
inline observer_ptr<ValueNode>
AccessNodePtr(observer_ptr<ValueNode::Container> p_con, const _tKey& name)
	ynothrow
{
	return p_con ? YSLib::AccessNodePtr(*p_con, name) : nullptr;
}
template<typename _tKey>
inline observer_ptr<const ValueNode>
AccessNodePtr(observer_ptr<const ValueNode::Container> p_con, const _tKey& name)
	ynothrow
{
	return p_con ? YSLib::AccessNodePtr(*p_con, name) : nullptr;
}
/*!
\note 时间复杂度 O(n) 。
\since build 670
*/
//@{
YF_API observer_ptr<ValueNode>
AccessNodePtr(ValueNode&, size_t);
YF_API observer_ptr<const ValueNode>
AccessNodePtr(const ValueNode&, size_t);
//@}
template<typename _tKey, yimpl(typename = typename ystdex::enable_if_t<
	ystdex::is_interoperable<const _tKey&, const string&>::value>)>
inline observer_ptr<ValueNode>
AccessNodePtr(ValueNode& node, const _tKey& name)
{
	return YSLib::AccessNodePtr(node.GetContainerRef(), name);
}
template<typename _tKey, yimpl(typename = typename ystdex::enable_if_t<
	ystdex::is_interoperable<const _tKey&, const string&>::value>)>
inline observer_ptr<const ValueNode>
AccessNodePtr(const ValueNode& node, const _tKey& name)
{
	return YSLib::AccessNodePtr(node.GetContainer(), name);
}
//! \since build 670
//@{
//! \note 使用 ADL \c AccessNodePtr 。
template<class _tNode, typename _tIn>
auto
AccessNodePtr(_tNode&& node, _tIn first, _tIn last)
	-> decltype(make_obsrever(std::addressof(node)))
{
	// TODO: Simplified using algorithm template?
	for(auto p(make_observer(std::addressof(node))); p && first != last;
		++first)
		p = AccessNodePtr(*p, *first);
	return first;
}
//! \note 使用 ADL \c begin 和 \c end 指定范围迭代器。
template<class _tNode, typename _tRange,
	yimpl(typename = typename ystdex::enable_if_t<
	!std::is_constructible<const string&, const _tRange&>::value>)>
inline auto
AccessNodePtr(_tNode&& node, const _tRange& c)
	-> decltype(YSLib::AccessNodePtr(yforward(node), begin(c), end(c)))
{
	return YSLib::AccessNodePtr(yforward(node), begin(c), end(c));
}
//@}
//@}
//@}

//! \since build 670
//@{
/*!
\exception std::bad_cast 空实例或类型检查失败 。
\relates ValueNode
*/
//@{
/*!
\brief 访问子节点的指定类型对象。
\note 使用 ADL \c AccessNode 。
*/
//@{
template<typename _type, typename... _tParams>
inline _type&
AccessChild(ValueNode& node, _tParams&&... args)
{
	return Access<_type>(AccessNode(node, yforward(args)...));
}
template<typename _type, typename... _tParams>
inline const _type&
AccessChild(const ValueNode& node, _tParams&&... args)
{
	return Access<_type>(AccessNode(node, yforward(args)...));
}
//@}

//! \brief 访问指定名称的子节点的指定类型对象的指针。
//@{
template<typename _type, typename... _tParams>
inline observer_ptr<_type>
AccessChildPtr(ValueNode& node, _tParams&&... args) ynothrow
{
	return AccessPtr<_type>(
		AccessNodePtr(node.GetContainerRef(), yforward(args)...));
}
template<typename _type, typename... _tParams>
inline observer_ptr<const _type>
AccessChildPtr(const ValueNode& node, _tParams&&... args) ynothrow
{
	return AccessPtr<_type>(
		AccessNodePtr(node.GetContainer(), yforward(args)...));
}
template<typename _type, typename... _tParams>
inline observer_ptr<_type>
AccessChildPtr(ValueNode* p_node, _tParams&&... args) ynothrow
{
	return p_node ? AccessChildPtr<_type>(*p_node, yforward(args)...) : nullptr;
}
template<typename _type, typename... _tParams>
inline observer_ptr<const _type>
AccessChildPtr(const ValueNode* p_node, _tParams&&... args) ynothrow
{
	return p_node ? AccessChildPtr<_type>(*p_node, yforward(args)...) : nullptr;
}
//@}
//@}
//@}


//! \note 结果不含子节点。
//@{
//! \since build 678
inline PDefH(const ValueNode&, AsNode, const ValueNode& node)
	ImplRet(node)
/*!
\brief 传递指定名称和值参数构造值类型节点。
\since build 668
*/
template<typename _tString, typename... _tParams>
inline ValueNode
AsNode(_tString&& str, _tParams&&... args)
{
	return {NoContainer, yforward(str), yforward(args)...};
}

/*!
\brief 传递指定名称和退化值参数构造值类型节点。
\since build 337
*/
template<typename _tString, typename... _tParams>
inline ValueNode
MakeNode(_tString&& str, _tParams&&... args)
{
	return {NoContainer, yforward(str), ystdex::decay_copy(args)...};
}
//@}

/*!
\brief 取指定名称和转换为字符串的值类型节点。
\note 使用非限定 to_string 转换。
\since build 344
*/
template<typename _tString, typename... _tParams>
inline ValueNode
StringifyToNode(_tString&& str, _tParams&&... args)
{
	return {NoContainer, yforward(str), to_string(yforward(args)...)};
}

/*!
\brief 从引用参数取值类型节点：返回自身。
\since build 338
*/
//@{
inline PDefH(const ValueNode&, UnpackToNode, const ValueNode& arg)
	ImplRet(arg)
inline PDefH(ValueNode&&, UnpackToNode, ValueNode&& arg)
	ImplRet(std::move(arg))
//@}
/*!
\brief 从参数取以指定分量为初始化参数的值类型节点。
\note 取分量同 std::get ，但使用 ADL 。仅取前两个分量。
\since build 338
*/
template<class _tPack>
inline ValueNode
UnpackToNode(_tPack&& pk)
{
	return {0, get<0>(yforward(pk)),
		ValueObject(ystdex::decay_copy(get<1>(yforward(pk))))};
}

/*!
\brief 取指定值类型节点为成员的节点容器。
\since build 598
*/
//@{
template<typename _tElem>
inline ValueNode::Container
CollectNodes(std::initializer_list<_tElem> il)
{
	return il;
}
template<typename... _tParams>
inline ValueNode::Container
CollectNodes(_tParams&&... args)
{
	return {yforward(args)...};
}
//@}

/*!
\brief 取以指定分量为参数对应初始化得到的值类型节点为子节点的值类型节点。
\since build 338
*/
template<typename _tString, typename... _tParams>
inline ValueNode
PackNodes(_tString&& name, _tParams&&... args)
{
	return {CollectNodes(UnpackToNode(yforward(args))...), yforward(name)};
}


//! \since build 674
//@{
//! \brief 移除空子节点。
YF_API void
RemoveEmptyChildren(ValueNode::Container&) ynothrow;

/*!
\brief 移除首个子节点。
\pre 断言：节点容器非空。
*/
//@{
YF_API void
RemoveHead(ValueNode::Container&) ynothrowv;
inline PDefH(void, RemoveHead, ValueNode& term) ynothrowv
	ImplExpr(RemoveHead(term.GetContainerRef()))
//@}
//@}


/*!
\brief 判断字符串是否是前缀索引：一个指定字符和非负整数的组合。
\pre 断言：字符串参数的数据指针非空。
\note 仅测试能被 <tt>unsigned long</tt> 表示的整数。
\since build 659
*/
YF_API bool
IsPrefixedIndex(string_view, char = '$');

/*!
\brief 转换节点大小为新的节点索引值。
\return 保证 4 位十进制数内按字典序递增的字符串。
\throw std::invalid_argument 索引值过大，不能以 4 位十进制数表示。
\note 重复使用作为新节点的名称，可用于插入不重复节点。
\since build 598
*/
//@{
YF_API string
MakeIndex(size_t);
inline PDefH(string, MakeIndex, const ValueNode::Container& con)
	ImplRet(MakeIndex(con.size()))
inline PDefH(string, MakeIndex, const ValueNode& node)
	ImplRet(MakeIndex(node.GetContainer()))
//@}

/*!
\brief 取最后一个子节点名称的前缀索引。
\return 若不存在子节点则为 \c size_t(-1) ，否则为最后一个子节点的名称对应的索引。
\throw std::invalid_argument 存在子节点但名称不是前缀索引。
\sa IsPrefixedIndex
\since build 790
*/
//@{
YF_API size_t
GetLastIndexOf(const ValueNode::Container&);
inline PDefH(size_t, GetLastIndexOf, const ValueNode& term)
	ImplRet(GetLastIndexOf(term.GetContainer()))
//@}

/*!
\brief 传递作为索引名称的值和其它参数构造值类型节点。
\note 使用 ADL AsNode 和 MakeIndex 。
\since build 691
*/
template<typename _tParam, typename... _tParams>
inline ValueNode
AsIndexNode(_tParam&& arg, _tParams&&... args)
{
	return AsNode(MakeIndex(yforward(arg)), yforward(args)...);
}


/*!
\brief 节点序列容器。
\since build 597

除分配器外满足和 std::vector 相同的要求的模板的一个实例，元素为 ValueNode 类型。
*/
using NodeSequence = yimpl(YSLib::vector)<ValueNode>;


/*!
\brief 包装节点的组合字面量。
\since build 598
*/
class YF_API NodeLiteral final
{
private:
	ValueNode node;

public:
	//! \since build 599
	NodeLiteral(const ValueNode& nd)
		: node(nd)
	{}
	//! \since build 599
	NodeLiteral(ValueNode&& nd)
		: node(std::move(nd))
	{}
	NodeLiteral(const string& str)
		: node(NoContainer, str)
	{}
	NodeLiteral(const string& str, string val)
		: node(NoContainer, str, std::move(val))
	{}
	template<typename _tLiteral = NodeLiteral>
	NodeLiteral(const string& str, std::initializer_list<_tLiteral> il)
		: node(NoContainer, str, NodeSequence(il.begin(), il.end()))
	{}
	//! \since build 674
	template<typename _tLiteral = NodeLiteral, class _tString,
		typename... _tParams>
	NodeLiteral(ListContainerTag, _tString&& str,
		std::initializer_list<_tLiteral> il, _tParams&&... args)
		: node(ValueNode::Container(il.begin(), il.end()), yforward(str),
		yforward(args)...)
	{}
	DefDeCopyMoveCtorAssignment(NodeLiteral)

	DefCvt(ynothrow, ValueNode&, node)
	DefCvt(const ynothrow, const ValueNode&, node)
};

/*!
\brief 传递参数构造值类型节点字面量。
\relates NodeLiteral
\since build 668
*/
template<typename _tString, typename _tLiteral = NodeLiteral>
inline NodeLiteral
AsNodeLiteral(_tString&& name, std::initializer_list<_tLiteral> il)
{
	return {ListContainer, yforward(name), il};
}

} // namespace YSLib;

#endif

