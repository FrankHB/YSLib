/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ValueNode.h
\ingroup Core
\brief 值类型节点。
\version r1673
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:03:44 +0800
\par 修改时间:
	2015-05-20 13:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::ValueNode
*/


#ifndef YSL_INC_Core_ValueNode_h_
#define YSL_INC_Core_ValueNode_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YObject
#include <ystdex/path.hpp>
#include <numeric> // for std::accumulate;

namespace YSLib
{

/*!
\brief 值类型节点。
\warning 非虚析构。
\since build 330

包含名称字符串和值类型对象的对象节点。
*/
class YF_API ValueNode
{
public:
	using Container = set<ValueNode>;
	//! \since build 460
	using iterator = Container::iterator;
	//! \since build 460
	using const_iterator = Container::const_iterator;

private:
	string name{};
	/*!
	\brief 子节点容器指针。
	\since build 598
	*/
	mutable Container container{};

public:
	//! \since build 399
	mutable ValueObject Value{};

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
	\note 第一个参数不使用，仅用于避免参与单参数重载，便于其它类使用转换函数。
	\since build 376
	*/
	template<typename _tString, typename... _tParams>
	inline
	ValueNode(int, _tString&& str, _tParams&&... args)
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
	DefDeCopyMoveCtor(ValueNode)

	/*
	\brief 合一赋值：使用值参数和交换函数进行复制或转移赋值。
	\since build 502
	*/
	PDefHOp(ValueNode&, =, ValueNode node) ynothrow
		ImplRet(node.swap(*this), *this)

	//! \since build 336
	DefBoolNeg(explicit, bool(Value) || !container.empty())

	//! \since build 403
	//@{
	PDefHOp(const ValueNode&, +=, const ValueNode& node) const
		ImplRet(Add(node), *this)
	PDefHOp(const ValueNode&, +=, ValueNode&& node) const
		ImplRet(Add(std::move(node)), *this)

	PDefHOp(const ValueNode&, -=, const ValueNode& node) const
		ImplRet(Remove(node), *this)
	PDefHOp(const ValueNode&, -=, const string& str) const
		ImplRet(Remove(str), *this)
	//@}
	/*!
	\brief 替换同名子节点。
	\return 自身引用。
	\since build 398
	*/
	//@{
	PDefHOp(const ValueNode&, /=, const ValueNode& node) const
		ImplRet(*this %= node, *this)
	PDefHOp(const ValueNode&, /=, ValueNode&& node) const
		ImplRet(*this %= std::move(node), *this)
	//@}
	/*!
	\brief 替换同名子节点。
	\return 子节点引用。
	\since build 399
	*/
	//@{
	const ValueNode&
	operator%=(const ValueNode&) const;
	const ValueNode&
	operator%=(const ValueNode&&) const;
	//@}

	PDefHOp(bool, ==, const ValueNode& node) const
		ImplRet(name == node.name)

	PDefHOp(bool, <, const ValueNode& node) const
		ImplRet(name < node.name)

	//! \since build 398
	const ValueNode&
	operator[](const string&) const;
	//! \since build 497
	template<class _tCon>
	const ValueNode&
	operator[](const ystdex::path<_tCon>& pth) const
	{
		auto p(this);

		for(const auto& n : pth)
			p = &(*p)[n];
		return *p;
	}

	DefCvt(const ynothrow, const string&, name)

	/*!
	\brief 取子节点容器引用。
	\since build 598
	*/
	DefGetter(const ynothrow, Container&, ContainerRef, container)
	DefGetter(const ynothrow, const string&, Name, name)

	/*!
	\brief 设置子节点容器内容。
	\since build 503
	*/
	PDefH(void, SetChildren, Container con) const
		ImplExpr(container = std::move(con))

	//! \since build 403
	PDefH(bool, Add, const ValueNode& node) const
		ImplRet(insert(node).second)
	//! \since build 403
	PDefH(bool, Add, ValueNode&& node) const
		ImplRet(insert(std::move(node)).second)

	/*!
	\brief 清除节点。
	\post <tt>!Value && GetContainerRef().empty()</tt> 。
	*/
	PDefH(void, Clear, ) const ynothrow
		ImplExpr(Value.Clear(), ClearContainer())

	/*!
	\brief 清除节点容器。
	\post <tt>GetContainerRef().empty()</tt> 。
	\since build 592
	*/
	PDefH(void, ClearContainer, ) const ynothrow
		ImplExpr(container.clear())

	//! \since build 403
	bool
	Remove(const ValueNode&) const;
	//! \since build 403
	PDefH(bool, Remove, const string& str) const
		ImplRet(Remove({0, str}))

	//! \since build 595
	//@{
	//! \brief 交换容器。
	PDefH(void, SwapContainer, const ValueNode& node) const ynothrow
		ImplExpr(container.swap(node.container))

	//! \brief 交换容器和值。
	void
	SwapContent(const ValueNode&) const ynothrow;
	//@}

	/*!
	\exception ystdex::bad_any_cast 容器不存在。
	\throw std::out_of_range 未找到对应节点。
	\since build 433
	*/
	const ValueNode&
	at(const string&) const;

	//! \since build 460
	//@{
	PDefH(iterator, begin, )
		ImplRet(GetContainerRef().begin())
	PDefH(const_iterator, begin, ) const
		ImplRet(GetContainerRef().begin())

	//! \since build 598
	//@{
	DefFwdTmpl(const, pair<iterator YPP_Comma bool>, emplace,
		container.emplace(yforward(args)...))

	//! \since build 599
	DefFwdTmpl(const, iterator, emplace_hint,
		container.emplace_hint(yforward(args)...))

	PDefH(bool, empty, ) const ynothrow
		ImplRet(container.empty())
	//@}

	PDefH(iterator, end, )
		ImplRet(GetContainerRef().end())
	PDefH(const_iterator, end, ) const
		ImplRet(GetContainerRef().end())
	//@}

	//! \since build 598
	DefFwdTmpl(const -> decltype(container.insert(yforward(args)...)), auto,
		insert, container.insert(yforward(args)...))

	//! \since build 598
	PDefH(size_t, size, ) const ynothrow
		ImplRet(container.size())

	/*
	\brief 交换。
	\since build 501
	*/
	void
	swap(ValueNode&) ynothrow;
};

//! \relates ValueNode
//@{
/*!
\brief 访问节点的指定类型对象。
\exception std::bad_cast 空实例或类型检查失败 。
\since build 399
*/
template<typename _type>
inline _type&
Access(const ValueNode& node)
{
	return node.Value.Access<_type>();
}

/*!
\brief 访问节点的指定类型对象指针。
\since build 399
*/
template<typename _type>
inline _type*
AccessPtr(const ValueNode& node) ynothrow
{
	return node.Value.AccessPtr<_type>();
}
/*!
\brief 访问节点的指定类型对象指针。
\since build 432
*/
template<typename _type>
inline _type*
AccessPtr(const ValueNode* p_node) ynothrow
{
	return p_node ? AccessPtr<_type>(*p_node) : nullptr;
}

/*!
\brief 访问节点。
\exception ystdex::bad_any_cast 容器不存在。
\throw std::out_of_range 未找到对应节点。
\since build 531
*/
//@{
//! \note 时间复杂度 O(n) 。
YF_API const ValueNode&
at(const ValueNode&, size_t);
//! \note 调用对应成员函数。
inline PDefH(const ValueNode&, at, const ValueNode& node, const string& name)
	ImplRet(node.at(name))
//! \note 使用 ADL 指定使用的 at 调用。
template<typename _tIn>
const ValueNode&
at(const ValueNode& node, _tIn first, _tIn last)
{
	return std::accumulate(first, last, ystdex::ref(node),
		[](const ValueNode& nd, decltype(*first) c){
		return ystdex::ref(at(nd, c));
	});
}
/*!
\note 使用 ADL <tt>begin</tt> 和 <tt>end</tt> 指定范围迭代器。
\since build 597
*/
template<typename _tRange>
inline auto
at(const ValueNode& node, const _tRange& c)
	-> decltype(YSLib::at(node, begin(c), end(c)))
{
	return YSLib::at(node, begin(c), end(c));
}
//@}

//! \since build 501
inline DefSwap(ynothrow, ValueNode)
//@}


/*!
\brief 访问容器中的节点。
\since build 399
*/
//@{
//! \since build 433
YF_API const ValueNode&
AccessNode(const ValueNode::Container*, const string&);
inline PDefH(const ValueNode&, AccessNode, const ValueNode::Container& con,
	const string& name)
	ImplRet(AccessNode(&con, name))
//@}

/*!
\brief 访问容器中的节点指针。
\since build 432
*/
//@{
YF_API const ValueNode*
AccessNodePtr(const ValueNode::Container&, const string&);
inline PDefH(const ValueNode*, AccessNodePtr, const ValueNode::Container* p_con,
	const string& name)
	ImplRet(p_con ? AccessNodePtr(*p_con, name) : nullptr)
//@}

/*!
\brief 访问指定名称的子节点的指定类型对象。
\exception std::bad_cast 空实例或类型检查失败 。
\relates ValueNode
\since build 432
*/
template<typename _type>
inline _type&
AccessChild(const ValueNode& node, const string& name)
{
	return Access<_type>(node.at(name));
}

/*!
\brief 访问指定名称的子节点的指定类型对象的指针。
\exception std::bad_cast 空实例或类型检查失败 。
\relates ValueNode
\since build 432
*/
//@{
template<typename _type>
inline _type*
AccessChildPtr(const ValueNode& node, const string& name)
{
	return AccessPtr<_type>(AccessNodePtr(node.GetContainerRef(), name));
}
template<typename _type>
inline _type*
AccessChildPtr(const ValueNode* p_node, const string& name)
{
	return p_node ? AccessChildPtr<_type>(*p_node, name) : nullptr;
}
//@}


/*!
\brief 取指定名称和退化参数的值类型节点。
\since build 337
*/
template<typename _tString, typename... _tParams>
inline ValueNode
MakeNode(_tString&& name, _tParams&&... args)
{
	return {0, yforward(name), ystdex::decay_copy(args)...};
}

/*!
\brief 取指定名称和转换为字符串的值类型节点。
\note 使用非限定 to_string 转换。
\since build 344
*/
template<typename _tString, typename... _tParams>
inline ValueNode
StringifyToNode(_tString&& name, _tParams&&... args)
{
	return {0, yforward(name), to_string(yforward(args)...)};
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
template<typename _type>
inline ValueNode::Container
CollectNodes(std::initializer_list<_type> il)
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


/*!
\brief 判断字符串是否是一个指定字符和非负整数的组合。
\note 要求整数能被 unsigned long 表示。
\since build 450
*/
YF_API bool
IsPrefixedIndex(const string&, char = '$');

/*!
\brief 转换节点大小为新的节点索引值。
\return 保证 4 位十进制数内按字典序递增的字符串。
\note 重复使用作为新节点的名称，可用于插入不重复节点。
\since build 598
*/
//@{
YF_API string
MakeIndex(size_t);
inline PDefH(string, MakeIndex, const ValueNode::Container& con)
	ImplRet(MakeIndex(con.size()))
inline PDefH(string, MakeIndex, const ValueNode& node)
	ImplRet(MakeIndex(node.GetContainerRef()))
//@}

} // namespace YSLib;

#endif

