/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ValueNode.h
\ingroup Core
\brief 值类型节点。
\version r1351
\author FrankHB <frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:03:44 +0800
\par 修改时间:
	2014-02-18 00:54 +0800
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
	string name;

public:
	//! \since build 399
	mutable ValueObject Value;

	DefDeCtor(ValueNode)
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
	\brief 构造：使用输入迭代器对。
	\since build 337
	*/
	template<typename _tIn>
	inline
	ValueNode(const pair<_tIn, _tIn>& pr)
		: name(), Value(Container(pr.first, pr.second))
	{}
	/*!
	\brief 构造：使用输入迭代器对、字符串引用和值参数。
	\since build 340
	*/
	template<typename _tIn, typename _tString>
	inline
	ValueNode(const pair<_tIn, _tIn>& pr, _tString&& str)
		: name(yforward(str)), Value(Container(pr.first, pr.second))
	{}
	DefDeCopyCtor(ValueNode)
	DefDeMoveCtor(ValueNode)

	DefDeCopyAssignment(ValueNode)
	DefDeMoveAssignment(ValueNode)

	//! \since build 336
	PDefHOp(bool, !, ) const ynothrow
		ImplRet(!Value)

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
	operator%=(const ValueNode& node) const
	{
		const auto& n((*this)[node.name]);

		return n.Value = node.Value, n;
	}
	const ValueNode&
	operator%=(const ValueNode&& node) const
	{
		const auto& n((*this)[node.name]);

		return n.Value = std::move(node.Value), n;
	}
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

	//! \since build 336
	explicit DefCvt(const ynothrow, bool, bool(Value))
	DefCvt(const ynothrow, const string&, name);

	//! \since build 340
	DefGetter(const, Container&, Container, Value.Access<Container>())
	//! \since build 398
	DefGetter(const ynothrow, Container*, ContainerPtr,
		Value.AccessPtr<Container>())
	DefGetter(const ynothrow, const string&, Name, name)
	size_t
	GetSize() const ynothrow;

	//! \since build 403
	bool
	Add(const ValueNode&) const;
	//! \since build 403
	bool
	Add(ValueNode&&) const;

private:
	//! \since build 398
	Container&
	CheckNodes() const;

public:
	PDefH(void, Clear, )
		ImplExpr(Value.Clear())

	//! \since build 403
	bool
	Remove(const ValueNode&) const;
	//! \since build 403
	PDefH(bool, Remove, const string& str) const
		ImplRet(Remove({0, str}))

	//! \since build 433
	const ValueNode&
	at(const string&) const;

	//! \since build 460
	//@{
	PDefH(iterator, begin, )
		ImplRet(GetContainer().begin())
	PDefH(const_iterator, begin, ) const
		ImplRet(GetContainer().begin())

	PDefH(iterator, end, )
		ImplRet(GetContainer().end())
	PDefH(const_iterator, end, ) const
		ImplRet(GetContainer().end())
	//@}
};

/*!
\brief 访问节点的指定类型对象。
\exception std::bad_cast 空实例或类型检查失败 。
\relates ValueNode
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
\brief 访问容器中的节点。
\since build 399
*/
//@{
//! \since build 433
YF_API const ValueNode&
AccessNode(const ValueNode::Container*, const string&);
inline const ValueNode&
AccessNode(const ValueNode::Container& con, const string& name)
{
	return AccessNode(&con, name);
}
//@}

/*!
\brief 访问容器中的节点指针。
\since build 432
*/
//@{
YF_API const ValueNode*
AccessNodePtr(const ValueNode::Container&, const string&);
inline const ValueNode*
AccessNodePtr(const ValueNode::Container* p_con, const string& name)
{
	return p_con ? AccessNodePtr(*p_con, name) : nullptr;
}
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
	return AccessPtr<_type>(AccessNodePtr(node.GetContainerPtr(), name));
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
inline const ValueNode&
UnpackToNode(const ValueNode& arg)
{
	return arg;
}
inline ValueNode&&
UnpackToNode(ValueNode&& arg)
{
	return std::move(arg);
}
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
\since build 399
*/
template<typename... _tParams>
inline unique_ptr<ValueNode::Container>
CollectNodes(_tParams&&... args)
{
	auto p(make_unique<ValueNode::Container>());

	ystdex::seq_insert(*p, yforward(args)...);
	return p;
}

/*!
\brief 取以指定分量为参数对应初始化得到的值类型节点为子节点的值类型节点。
\since build 338
*/
template<typename _tString, typename... _tParams>
inline ValueNode
PackNodes(_tString&& name, _tParams&&... args)
{
	return {0, yforward(name), CollectNodes(UnpackToNode(yforward(args))...),
		PointerTag()};
}


/*!
\brief 判断字符串是否是一个指定字符和非负整数的组合。
\note 要求整数能被 unsigned long 表示。
\since build 450
*/
YF_API bool
IsPrefixedIndex(const string&, char = '$');

} // namespace YSLib;

#endif

