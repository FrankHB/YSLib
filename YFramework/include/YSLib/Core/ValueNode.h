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
\version r890
\author FrankHB<frankhb1989@gmail.com>
\since build 338
\par 创建时间:
	2012-08-03 23:03:44 +0800
\par 修改时间:
	2012-09-13 22:06 +0800
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

包含 ValueObject 对象和以 string 为键的子 ValueObject 容器的对象。
*/
class ValueNode
{
public:
	typedef set<ValueNode> Container;

private:
	string name;
	//! \since build 336
	mutable ValueObject value;
	//! \since build 337
	mutable unique_ptr<Container> p_nodes;

public:
	DefDeCtor(ValueNode)
	/*!
	\brief 构造：使用字符串左值引用和节点容器指针。
	\since build 338
	*/
	inline
	ValueNode(const string& str, unique_ptr<Container> p = {})
		: name(str), value(), p_nodes(std::move(p))
	{}
	/*!
	\brief 构造：使用字符串右值引用和节点容器指针。
	\since build 338
	*/
	inline
	ValueNode(string&& str, unique_ptr<Container> p)
		: name(std::move(str)), value(), p_nodes(std::move(p))
	{}
	/*!
	\brief 构造：使用字符串引用、值类型对象引用和节点容器指针。
	\since build 338
	*/
	template<typename _tString, typename _tValue, typename = typename
		std::enable_if<std::is_constructible<string, _tString&&>::value
		&& !std::is_constructible<unique_ptr<Container>, _tValue&&>::value,
		int>::type>
	inline
	ValueNode(_tString&& str, _tValue&& val, unique_ptr<Container> p = {})
		: name(yforward(str)), value(yforward(val)), p_nodes(std::move(p))
	{}
	/*!
	\brief 构造：使用输入迭代器对。
	\since build 337
	*/
	template<typename _tIn>
	inline
	ValueNode(const pair<_tIn, _tIn>& pr)
		: name(), value(), p_nodes(new Container(pr.first, pr.second))
	{}
	/*!
	\brief 构造：使用输入迭代器对、字符串引用和值参数。
	\since build 337
	*/
	template<typename _tIn, typename _tString, typename... _tParams>
	inline
	ValueNode(const pair<_tIn, _tIn>& pr, _tString&& str, _tParams&&... args)
		: name(yforward(str)), value(yforward(args)...),
		p_nodes(new Container(pr.first, pr.second))
	{}
	ValueNode(const ValueNode&);
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

	Container::iterator
	GetBegin()
	{
		if(p_nodes)
			return p_nodes->begin();
		throw std::out_of_range("No child value node found.");
	}
	Container::const_iterator
	GetBegin() const
	{
		if(p_nodes)
			return p_nodes->begin();
		throw std::out_of_range("No child value node found.");
	}
	Container::iterator
	GetEnd()
	{
		if(p_nodes)
			return p_nodes->end();
		throw std::out_of_range("No child value node found.");
	}
	Container::const_iterator
	GetEnd() const
	{
		if(p_nodes)
			return p_nodes->end();
		throw std::out_of_range("No child value node found.");
	}
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
	DefGetter(const ynothrow, size_t, Size, p_nodes ? p_nodes->size() : 0)
	DefGetter(ynothrow, ValueObject&, Value, value)
	//! \since build 334
	//@{
	DefGetter(const ynothrow, const ValueObject&, Value, value)

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
	void
	CheckNodes();

public:
	PDefH(void, Clear)
		ImplExpr(p_nodes.reset())

	PDefH(bool, Remove, const ValueNode& node)
		ImplRet(p_nodes ? p_nodes->erase(node.name) != 0 : false)
	PDefH(bool, Remove, const string& str)
		ImplRet(p_nodes ? p_nodes->erase(str) != 0 : false)
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
\since build 338
*/
template<typename... _tParams>
inline unique_ptr<ValueNode::Container>
CollectNodes(_tParams&&... args)
{
	const auto p(new ValueNode::Container());

	ystdex::seq_insert(*p, yforward(args)...);
	return unique_ptr<ValueNode::Container>(p);
}

/*!
\brief 取以指定分量为参数对应初始化得到的值类型节点为子节点的值类型节点。
\since build 338
*/
template<typename _tString, typename... _tParams>
inline ValueNode
PackNodes(_tString&& name, _tParams&&... args)
{
	return ValueNode(yforward(name),
		CollectNodes(UnpackToNode(yforward(args))...));
}

YSL_END

#endif

