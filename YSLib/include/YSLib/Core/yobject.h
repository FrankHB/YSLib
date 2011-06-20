
/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yobject.h
\ingroup Core
\brief 平台无关的基础对象。
\version 0.3152;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-06-16 18:39 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YObject;
*/


#ifndef YSL_INC_CORE_YOBJECT_H_
#define YSL_INC_CORE_YOBJECT_H_

#include "ycutil.h"
#include "yexcept.h"
#include "ycounter.hpp"
#include "../Adaptor/cont.h"

YSL_BEGIN

//特征类策略：对象类型标签模版。

/*!
\brief 指定对于 _type 类型成员具有所有权的标签。
*/
template<typename _type>
struct OwnershipTag
{};

//标签类型元运算。

template<class _tOwner, typename _type>
struct HasOwnershipOf : public std::integral_constant<bool,
	std::is_base_of<OwnershipTag<_type>, _tOwner>::value>
{};


//基本对象定义。

/*!
\brief 值类型对象类。
\pre 满足 CopyConstructible 。
\warning \c public 析构函数非虚实现。

具有值语义和深复制语义的对象。
*/
class ValueObject
{
public:
	typedef enum
	{
	//	Create = 0,
		Destroy = 1,
		Clone = 2,
		Equality = 3
	} OpType;
	typedef bool (*ManagerType)(void*&, void*&, OpType);

private:
	template<typename _type>
	struct GManager
	{
		static bool
		Do(void*& lhs, void*& rhs, OpType op)
		{
			switch(op)
			{
		//	case Create:
		//		lhs = new _type();
		//		return false;
			case Destroy:
				delete static_cast<_type*>(lhs);
				return false;
			case Clone:
				YAssert(rhs, "Null pointer found"
					" @ ValueObject::GManager::Do#Clone;");

				lhs = new _type(*static_cast<_type*>(rhs));
				return false;
			case Equality:
				YAssert(lhs && rhs, "Null pointer found"
					" @ ValueObject::GManager::Do#Equlitiy;");

				return *static_cast<_type*>(lhs) == *static_cast<_type*>(rhs);
			default:
				return false;
			}
			return false;
		}
	};

	ManagerType manager;
	mutable void* obj_ptr;

public:
	/*!
	\brief 无参数构造。
	\note 得到空实例。
	*/
	ValueObject();
	/*!
	\brief 构造：使用对象引用。
	\note 对象需要是可复制构造的。
	\note 得到包含指定对象的实例。
	*/
	template<typename _type>
	ValueObject(const _type& obj)
		: manager(&GManager<_type>::Do), obj_ptr(new _type(obj))
	{}
	ValueObject(const ValueObject&);
	ValueObject(ValueObject&&);
	~ValueObject();

	ValueObject&
	operator=(const ValueObject&);
	ValueObject&
	operator=(ValueObject&&);

	bool
	operator==(const ValueObject&) const;

	template<typename _type>
	const _type&
	GetObject() const
	{
		YAssert(obj_ptr, "Null pointer found @ ValueObject::GetObject;");
		YAssert(GManager<_type>::Do == manager, "Invalid type found"
			" @ ValueObject::GetObject;");

		return *static_cast<const _type*>(obj_ptr);
	}

	void
	Clear();

	void
	Swap(ValueObject&);
};

inline
ValueObject::ValueObject()
	: manager(nullptr), obj_ptr(nullptr)
{}
inline
ValueObject::~ValueObject()
{
	Clear();
}

inline ValueObject&
ValueObject::operator=(const ValueObject& c)
{
	ValueObject(c).Swap(*this);
	return *this;
}


//! \brief 基本框架对象类：所有类名以 Y 作前缀的类的公共基类。
class YObject : public noncopyable
{
public:
	/*!
	\brief 析构：空实现。
	\note 必要的虚函数以构造多态基类。
	*/
	virtual
	~YObject()
	{}
};


//! \brief 基本可数对象类：所有可数的基本对象类的公共基类。
class YCountableObject : public GMCounter<YCountableObject>, public YObject
{
public:
	/*!
	\brief 无参数构造：默认实现。
	*/
	YCountableObject() = default;
};


/*!
\brief 依赖项类模板。

基于被依赖的默认对象，可通过写时复制策略创建新对象。
\tparam _type 被依赖的对象类型，需能被默认构造。
\tparam _tOwnerPointer 依赖所有者指针类型。
\warning 依赖所有者指针需要实现所有权语义，
	否则出现无法释放资源导致内存泄漏或其它非预期行为。
\todo 线程模型及安全性。
*/
template<typename _type, class _tOwnerPointer = shared_ptr<_type>>
class GDependency
{
public:
	typedef _type T;
	typedef _tOwnerPointer PointerType;

private:
	PointerType ptr;

public:
	GDependency(PointerType p = PointerType())
		: ptr(p)
	{
		GetCopyOnWritePtr();
	}
	GDependency(const GDependency&) = default;
	GDependency(GDependency&&) = default;


	DefConverter(const T&, *ptr)
	DefMutableConverter(T&, *ptr)

	DefGetter(const T&, Ref, operator const T&())
	DefMutableGetter(T&, Ref, operator T&())
	DefMutableGetter(T&, NewRef, *GetCopyOnWritePtr())

	PointerType
	GetCopyOnWritePtr()
	{
		if(!ptr)
			ptr = PointerType(new T());
		else if(!ptr.unique())
			ptr = PointerType(new T(*ptr));

		YAssert(is_valid(ptr),
			"Invalid pointer found @ GDependency::GetCopyOnWritePtr;");

		return ptr;
	}
};

//! \brief 范围模块类。
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
	DefGetter(ValueType, MaxValue, max_value)
	DefGetter(ValueType, Value, value)
};

YSL_END

#endif

