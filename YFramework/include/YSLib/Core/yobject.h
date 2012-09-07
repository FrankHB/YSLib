
/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yobject.h
\ingroup Core
\brief 平台无关的基础对象。
\version r3255
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2012-09-07 21:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YObject
*/


#ifndef YSL_INC_CORE_YOBJECT_H_
#define YSL_INC_CORE_YOBJECT_H_ 1

#include "ycutil.h"
#include "yexcept.h"
#include "../Adaptor/ycont.h"
#include <ystdex/any.h> // for ystdex::any_holder, ystdex::any;

YSL_BEGIN

//特征类策略：对象类型标签模版。

/*!
\brief 指定对于参数指定类型的成员具有所有权的标签。
\since build 218
*/
template<typename>
struct OwnershipTag
{};


/*!
\brief 标签类型元运算。
\since build 218
*/
template<class _tOwner, typename _type>
struct HasOwnershipOf : public std::integral_constant<bool,
	std::is_base_of<OwnershipTag<_type>, _tOwner>::value>
{};


/*!
\brief 带等于接口的动态泛型持有者接口。
\see ystdex::any_holder 。
\since build 332
*/
DeclDerivedI(IValueHolder, ystdex::any_holder)
	DeclIEntry(bool operator==(const IValueHolder&) const)

protected:
	template<typename _type>
	static inline bool
	AreEqual(_type& x, _type& y, decltype(x == y) = false)
	{
		return x == y;
	}
	template<typename _type, typename _tUnused>
	static inline bool
	AreEqual(_type&, _tUnused&)
	{
		return true;
	}
EndDecl


/*!
\brief 带等于接口的值类型动态泛型持有者。
\see ystdex::value_holder 。
\since build 332
*/
template<typename _type>
class ValueHolder : implements IValueHolder
{
public:
	_type held;

	ValueHolder(const _type& value)
		: held(value)
	{}
	DefDeCopyCtor(ValueHolder)
	DefDeMoveCtor(ValueHolder)

	DefDelCopyAssignment(ValueHolder)

	ImplI(IValueHolder) bool
	operator==(const IValueHolder& obj) const
	{
		return AreEqual(held, static_cast<const ValueHolder&>(obj).held);
	}

	ImplI(IValueHolder) DefClone(ValueHolder, clone)

	ImplI(IValueHolder) void*
	get() override
	{
		return std::addressof(held);
	}

	ImplI(IValueHolder) const std::type_info&
	type() const override
	{
		return typeid(_type);
	}
};


/*!
\brief 带等于接口的指针类型动态泛型持有者。
\see ystdex::pointer_holder 。
\since build 332
*/
template<typename _type>
class PointerHolder : implements IValueHolder
{
	static_assert(std::is_object<_type>::value, "Invalid type found.");

public:
	_type* p_held;

	PointerHolder(_type* value)
		: p_held(value)
	{}
	virtual
	~PointerHolder() ynothrow
	{
		delete p_held;
	}

	DefDelCopyAssignment(PointerHolder)

	ImplI(IValueHolder) PointerHolder*
	clone() const override
	{
		return new PointerHolder(p_held ? new _type(*p_held) : nullptr);
	}

	ImplI(IValueHolder) bool
	operator==(const IValueHolder& obj) const
	{
		return AreEqual(*p_held,
			*static_cast<const PointerHolder&>(obj).p_held);
	}

	ImplI(IValueHolder) void*
	get() override
	{
		return p_held;
	}

	ImplI(IValueHolder) const std::type_info&
	type() const override
	{
		return p_held ? typeid(_type) : typeid(void);
	}
};


/*!
\brief 值类型对象类。
\pre 满足 CopyConstructible 。
\warning 非虚析构。
\warning 若修改子节点的 name 则行为未定义。
\since build 217

具有值语义和深复制语义的对象。
*/
class ValueObject
{
public:
	//! \brief 指示指针构造的标记。
	struct PointerConstructTag
	{};

	//! \since build 332
	ystdex::any content;

public:
	/*!
	\brief 无参数构造。
	\note 得到空实例。
	\since build 296
	*/
	DefDeCtor(ValueObject)
	/*!
	\brief 构造：使用对象左值引用。
	\pre obj 可被复制构造。
	\note 得到包含指定对象副本的实例。
	*/
	template<typename _type>
	ValueObject(const _type& obj)
		: content(new ValueHolder<_type>(obj), nullptr)
	{}
	/*!
	\brief 构造：使用对象指针。
	\note 得到包含指针指向的指定对象的实例，并获得所有权。
	*/
	template<typename _type>
	ValueObject(_type* p, PointerConstructTag)
		: content(new PointerHolder<_type>(p), nullptr)
	{}
	/*!
	\brief 复制构造：默认实现。
	\since build 332
	*/
	DefDeCopyCtor(ValueObject)
	/*!
	\brief 转移构造：默认实现。
	\since build 332
	*/
	DefDeMoveCtor(ValueObject)
	/*!
	\brief 析构：默认实现。
	\since build 332
	*/
	DefDeDtor(ValueObject)

	//! \since build 332
	//@{
	DefDeCopyAssignment(ValueObject)
	DefDeMoveAssignment(ValueObject)
	//@}

	/*!
	\brief 判断是否为空。
	\since build 320
	*/
	PDefHOp(bool, !) const ynothrow
		ImplRet(!content)

	bool
	operator==(const ValueObject&) const;

	/*!
	\brief 判断是否非空。
	\since build 320
	*/
	explicit DefCvt(const ynothrow, bool, content.get_holder())

private:
	/*!
	\brief 取指定类型的对象。
	\tparam _type 指定类型。
	\pre 断言检查： bool(content) && content.type() == typeid(_type) 。
	\since build 337
	*/
	template<typename _type>
	inline _type&
	GetMutableObject() const
	{
		YAssert(bool(content), "Null pointer found.");
		YAssert(content.type() == typeid(_type), "Invalid type found.");

		return *content.get<_type>();
	}

public:
	template<typename _type>
	inline _type&
	GetObject()
	{
		return GetMutableObject<_type>();
	}
	template<typename _type>
	inline const _type&
	GetObject() const
	{
		return GetMutableObject<_type>();
	}

	/*!
	\brief 访问指定类型对象。
	\throw std::bad_cast 空实例或类型检查失败 。
	\since build 334
	*/
	template<typename _type>
	inline _type&
	Access()
	{
		return ystdex::any_cast<_type&>(content);
	}
	/*!
	\brief 访问指定类型 const 对象。
	\throw std::bad_cast 空实例或类型检查失败 。
	\since build 306
	*/
	template<typename _type>
	inline const _type&
	Access() const
	{
		return ystdex::any_cast<_type&>(content);
	}

	/*
	\brief 清除。
	\post <tt>*this == ValueObject()</tt> 。
	\since build 296
	*/
	PDefH(void, Clear) ynothrow
		ImplBodyMem(content, clear)

	/*!
	\brief 交换。
	\since build 296
	*/
	PDefH(void, Swap, ValueObject& vo) ynothrow
		ImplBodyMem(content, swap, vo.content)
};

/*!
\ingroup HelperFunctions
\brief 使用指针构造 ValueObject 实例。
\since build 233
*/
template<typename _type>
inline ValueObject
MakeValueObjectByPtr(_type* p)
{
	return ValueObject(p, ValueObject::PointerConstructTag());
}


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
	\since build 337
	*/
	inline
	ValueNode(const string& str, unique_ptr<Container>&& p = {})
		: name(str), value(), p_nodes(std::move(p))
	{}
	/*!
	\brief 构造：使用字符串右值引用和节点容器指针。
	\since build 337
	*/
	inline
	ValueNode(string&& str, unique_ptr<Container>&& p)
		: name(str), value(), p_nodes(std::move(p))
	{}
	/*!
	\brief 构造：使用字符串引用、值类型对象引用和节点容器指针。
	\since build 337
	*/
	template<typename _tString, typename _tValue, typename = typename
		std::enable_if<std::is_constructible<string, _tString&&>::value
		&& !std::is_constructible<unique_ptr<Container>, _tValue&&>::value,
		int>::type>
	inline
	ValueNode(_tString&& str, _tValue&& val, unique_ptr<Container>&& p = {})
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
\brief 依赖项类模板。

基于被依赖的默认对象，可通过写时复制策略创建新对象；可能为空。
\tparam _type 被依赖的对象类型，需能被无参数构造。
\tparam _tOwnerPointer 依赖所有者指针类型。
\warning 依赖所有者指针需要实现所有权语义，
	否则出现无法释放资源导致内存泄漏或其它非预期行为。
\since build 195
\todo 线程模型及安全性。
*/
template<typename _type, class _tOwnerPointer = shared_ptr<_type>>
class GDependency
{
public:
	typedef _type DependentType;
	typedef _tOwnerPointer PointerType;
	typedef decltype(*PointerType()) ConstReferenceType;
	typedef typename std::remove_const<typename std::remove_reference<
		ConstReferenceType>::type>::type ReferentType;
	typedef ReferentType& ReferenceType;

private:
	PointerType ptr;

public:
	inline
	GDependency(PointerType p = PointerType())
		: ptr(p)
	{
		GetCopyOnWritePtr();
	}

	DefDeCopyAssignment(GDependency)
	DefDeMoveAssignment(GDependency)

	DefCvt(const ynothrow, ConstReferenceType, *ptr)
	DefCvt(ynothrow, ReferenceType, *ptr)
	DefCvt(const ynothrow, bool, bool(ptr))

	DefGetter(const ynothrow, ConstReferenceType, Ref, operator ConstReferenceType())
	DefGetter(ynothrow, ReferenceType, Ref, operator ReferenceType())
	DefGetter(ynothrow, ReferenceType, NewRef, *GetCopyOnWritePtr())

	PointerType
	GetCopyOnWritePtr()
	{
		if(!ptr)
			ptr = PointerType(new DependentType());
		else if(!ptr.unique())
			ptr = PointerType(CloneNonpolymorphic(ptr));

		YAssert(bool(ptr), "Null pointer found.");

		return ptr;
	}

	inline
	void Reset()
	{
		reset(ptr);
	}
};


/*!
\brief 范围模块类。
\warning 非虚析构。
\since build 193
*/
template<typename _type>
class GMRange
{
public:
	typedef _type ValueType;

protected:
	ValueType max_value; //!< 最大取值。
	ValueType value; //!< 值。

	/*!
	\brief 构造：使用指定最大取值和值。
	*/
	GMRange(ValueType m, ValueType v)
		: max_value(m), value(v)
	{}

public:
	DefGetter(const ynothrow, ValueType, MaxValue, max_value)
	DefGetter(const ynothrow, ValueType, Value, value)
};

YSL_END

#endif

