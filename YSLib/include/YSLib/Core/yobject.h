
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
\version r3245;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-09-14 17:11 +0800;
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
PDefTH1(_type)
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

	//! \brief 指示指针构造的标签类型。
	struct PointerConstructTag
	{};

private:
	PDefTH1(_type)
	struct GManager
	{
		static bool
		Do(void*& x, void*& y, OpType op)
		{
			switch(op)
			{
		//	case Create:
		//		lhs = new _type();
		//		return false;
			case Destroy:
				delete static_cast<_type*>(x);
				return false;
			case Clone:
				YAssert(y, "Null pointer found"
					" @ ValueObject::GManager::Do#Clone;");

				x = new _type(*static_cast<_type*>(y));
				return false;
			case Equality:
				YAssert(x && y, "Null pointer found"
					" @ ValueObject::GManager::Do#Equlitiy;");

				return AreEqual(*static_cast<_type*>(x),
					*static_cast<_type*>(y));
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
	\brief 构造：使用对象左值引用。
	\note 对象需要是可复制构造的。
	\note 得到包含指定对象的实例。
	*/
	PDefTH1(_type)
	ValueObject(const _type& obj)
		: manager(&GManager<_type>::Do), obj_ptr(new _type(obj))
	{}
	/*!
	\brief 构造：使用对象指针。
	\note 得到包含指针指向的指定对象的实例，并获得所有权。
	*/
	PDefTH1(_type)
	ValueObject(_type* p, PointerConstructTag)
		: manager(&GManager<_type>::Do), obj_ptr(p)
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

	DefPredicate(Empty, !obj_ptr)

	PDefTH1(_type)
	const _type&
	GetObject() const
	{
		YAssert(obj_ptr, "Null pointer found @ ValueObject::GetObject;");
		YAssert(GManager<_type>::Do == manager, "Invalid type found"
			" @ ValueObject::GetObject;");

		return *static_cast<const _type*>(obj_ptr);
	}
	PDefTH1(_type)
	_type&
	GetObject()
	{
		YAssert(obj_ptr, "Null pointer found @ ValueObject::GetObject;");
		YAssert(GManager<_type>::Do == manager, "Invalid type found"
			" @ ValueObject::GetObject;");

		return *static_cast<_type*>(obj_ptr);
	}

private:
	PDefTH1(_type)
	inline static bool
	AreEqual(_type& x, _type& y, decltype(x == y) = false)
	{
		return x == y;
	}
	PDefTH2(_type, _tUnused)
	inline static bool
	AreEqual(_type&, _tUnused&)
	{
		return true;
	}

public:
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


/*!
\ingroup HelperFunction
\brief 使用指针构造 ValueObject 实例。
*/
PDefTH1(_type)
inline ValueObject
MakeValueObjectByPtr(_type* p)
{
	return ValueObject(p, ValueObject::PointerConstructTag());
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
	DefDeCtor(YCountableObject)
};


/*!
\brief 依赖项类模板。

基于被依赖的默认对象，可通过写时复制策略创建新对象；可能为空。
\tparam _type 被依赖的对象类型，需能被无参数构造。
\tparam _tOwnerPointer 依赖所有者指针类型。
\warning 依赖所有者指针需要实现所有权语义，
	否则出现无法释放资源导致内存泄漏或其它非预期行为。
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
	{}
	DefDeCopyAssignment(GDependency)
	DefDeMoveAssignment(GDependency)

	DefConverter(ConstReferenceType, *ptr)
	DefMutableConverter(ReferenceType, *ptr)
	DefConverter(bool, static_cast<bool>(ptr))

	DefGetter(ConstReferenceType, Ref, operator ConstReferenceType())
	DefMutableGetter(ReferenceType, Ref, operator ReferenceType())
	DefMutableGetter(ReferenceType, NewRef, *GetCopyOnWritePtr())

	PointerType
	GetCopyOnWritePtr()
	{
		if(!ptr)
			ptr = PointerType(new DependentType());
		else if(!ptr.unique())
			ptr = PointerType(CloneNonpolymorphic(ptr));

		YAssert(is_not_null(ptr),
			"Null pointer found @ GDependency::GetCopyOnWritePtr;");

		return ptr;
	}

	inline
	void Reset()
	{
		reset(ptr);
	}
};

//! \brief 范围模块类。
PDefTH1(_type)
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

