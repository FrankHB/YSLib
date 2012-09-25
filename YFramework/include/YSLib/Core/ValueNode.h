/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ValueNode.h
\ingroup Core
\brief 值类型节点。
\version r1006
\author FrankHB<frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:03:44 +0800
\par 修改时间:
	2012-09-25 08:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::ValueNode
*/


#ifndef YSL_INC_CORE_VALUENODE_H_
#define YSL_INC_CORE_VALUENODE_H_ 1

#include "yobject.h"

YSL_BEGIN

/*!
\brief 值类型节点。
\warning 非虚析构。
\since build 330

包含名称字符串和值类型对象的对象节点。
*/
class ValueNode
{
public:
	typedef set<ValueNode> Container;

private:
	string name;
	//! \since build 336
	mutable ValueObject value;

public:
	DefDeCtor(ValueNode)
	/*!
	\brief 构造：使用字符串引用和值类型对象构造参数。
	\since build 340
	*/
	template<typename _tString, typename... _tParams>
	inline
	ValueNode(_tString&& str, _tParams&&... args)
		: name(yforward(str)), value(yforward(args)...)
	{}
	/*!
	\brief 构造：使用输入迭代器对。
	\since build 337
	*/
	template<typename _tIn>
	inline
	ValueNode(const pair<_tIn, _tIn>& pr)
		: name(), value(Container(pr.first, pr.second))
	{}
	/*!
	\brief 构造：使用输入迭代器对、字符串引用和值参数。
	\since build 340
	*/
	template<typename _tIn, typename _tString>
	inline
	ValueNode(const pair<_tIn, _tIn>& pr, _tString&& str)
		: name(yforward(str)), value(Container(pr.first, pr.second))
	{}
	DefDeCopyCtor(ValueNode)
	DefDeMoveCtor(ValueNode)

	DefDeCopyAssignment(ValueNode)
	DefDeMoveAssignment(ValueNode)

	//! \since build 336
	//@{
	PDefHOp(bool, !) const ynothrow
		ImplRet(!value)

	PDefHOp(bool, +=, const ValueNode& node)
		ImplRet(Add(node))
	//@}
	PDefHOp(bool, +=, ValueNode&& node)
		ImplRet(Add(std::move(node)))

	PDefHOp(bool, -=, const ValueNode& node)
		ImplRet(Remove(node))
	PDefHOp(bool, -=, const string& str)
		ImplRet(Remove(str))

	PDefHOp(bool, ==, const ValueNode& node) const
		ImplRet(name == node.name)

	PDefHOp(bool, <, const ValueNode& node) const
		ImplRet(name < node.name)

	ValueNode&
	operator[](const string&);

	//! \since build 336
	explicit DefCvt(const ynothrow, bool, bool(value))
	DefCvt(const ynothrow, const string&, name);

	DefGetter(, Container::iterator, Begin, GetContainer().begin())
	DefGetter(const, Container::const_iterator, Begin, GetContainer().begin())
	//! \since build 340
	Container&
	GetContainer() const;
	DefGetter(, Container::iterator, End, GetContainer().end())
	DefGetter(const, Container::const_iterator, End, GetContainer().end())
	DefGetter(const ynothrow, const string&, Name, name)
	//! \since build 337
	//@{
	ValueNode&
	GetNode(const string& name)
	{
		return const_cast<ValueNode&>(
			const_cast<const ValueNode*>(this)->GetNode(name));
	}
	const ValueNode&
	GetNode(const string&) const;
	//@}
	size_t
	GetSize() const ynothrow;
	DefGetter(ynothrow, ValueObject&, Value, value)
	//! \since build 334
	//@{
	DefGetter(const ynothrow, const ValueObject&, Value, value)
	//! \since build 341
	DefGetter(const ynothrow, ValueObject&&, ValueRRef, std::move(value))

	DefSetter(const ValueObject&, Value, GetValue())
	DefSetter(ValueObject&&, Value, GetValue())
	//@}

	PDefH(bool, Add, ValueNode& node)
		ImplRet(Add(std::move(node)))
	//! \since build 336
	bool
	Add(const ValueNode&);
	bool
	Add(ValueNode&&);

private:
	//! \since build 340
	Container&
	CheckNodes();

public:
	PDefH(void, Clear)
		ImplExpr(value.Clear())

	bool
	Remove(const ValueNode&);
	PDefH(bool, Remove, const string& str)
		ImplRet(Remove(str))
};

/*!
\ingroup HelperFunctions
\brief 迭代器包装，用于 range-based for 。
\since build 330
*/
//@{
inline auto
begin(ValueNode& node) -> decltype(node.GetBegin())
{
	return node.GetBegin();
}
inline auto
begin(const ValueNode& node) -> decltype(node.GetBegin())
{
	return node.GetBegin();
}

inline auto
end(ValueNode& node) -> decltype(node.GetEnd())
{
	return node.GetEnd();
}
inline auto
end(const ValueNode& node) -> decltype(node.GetEnd())
{
	return node.GetEnd();
}
//@}

/*!
\brief 访问节点的指定类型对象。
\throw std::bad_cast 空实例或类型检查失败 。
\since build 336
*/
template<typename _type>
inline _type&
Access(ValueNode& node)
{
	return node.GetValue().Access<_type>();
}
/*!
\brief 访问节点的指定类型 const 对象。
\throw std::bad_cast 空实例或类型检查失败 。
\since build 336
*/
template<typename _type>
inline const _type&
Access(const ValueNode& node)
{
	return node.GetValue().Access<_type>();
}


/*!
\brief 访问指定名称的子节点的指定类型对象。
\throw std::bad_cast 空实例或类型检查失败 。
\since build 334
*/
template<typename _type>
inline _type&
AccessChild(ValueNode& node, const string& name)
{
	return Access<_type>(node.GetNode(name));
}
/*!
\brief 访问指定名称的子节点的指定类型 const 对象。
\throw std::bad_cast 空实例或类型检查失败 。
\since build 334
*/
template<typename _type>
inline const _type&
AccessChild(const ValueNode& node, const string& name)
{
	return Access<_type>(node.GetNode(name));
}


/*!
\brief 取指定名称和退化参数的值类型节点。
\since build 337
*/
template<typename _tString, typename... _tParams>
inline ValueNode
MakeNode(_tString&& name, _tParams&&... args)
{
	return ValueNode(yforward(name),
		ValueObject(ystdex::decay_copy(yforward(args))...));
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
	return ValueNode(get<0>(yforward(pk)),
		ValueObject(ystdex::decay_copy(get<1>(yforward(pk)))));
}

/*!
\brief 取指定值类型节点为成员的节点容器。
\since build 339
*/
template<typename... _tParams>
inline ValueNode::Container*
CollectNodes(_tParams&&... args)
{
	auto p(make_unique<ValueNode::Container>());

	ystdex::seq_insert(*p, yforward(args)...);
	return p.release();
}

/*!
\brief 取以指定分量为参数对应初始化得到的值类型节点为子节点的值类型节点。
\since build 338
*/
template<typename _tString, typename... _tParams>
inline ValueNode
PackNodes(_tString&& name, _tParams&&... args)
{
	return ValueNode(yforward(name), CollectNodes(UnpackToNode(
		yforward(args))...), PointerTag());
}

YSL_END

#endif

