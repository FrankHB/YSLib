
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
\version 0.3067;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-16 20:06:58 +0800;
\par 修改时间:
	2011-05-03 19:32 +0800;
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

//基本对象定义。

//! \brief 基本对象类：所有类名以 Y 作前缀的类的公共基类。
class YObject : private noncopyable
{
public:
	/*!
	\brief 析构：空实现。必要的虚函数以构造多态基类。
	*/
	virtual
	~YObject()
	{}
};


//! \brief 基本可数对象类：所有可数的基本对象类的公共基类。
class YCountableObject : public GMCounter<YCountableObject>, public YObject
{
protected:
	/*!
	\brief 无参数构造。
	\note 保护实现。
	*/
	YCountableObject();
};

inline
YCountableObject::YCountableObject()
{}


/*!
\brief 依赖项类模板。

基于被依赖的默认对象，可通过写时复制策略创建新对象。
\param _type 被依赖的对象类型，需能被默认构造。
\param _tOwnerPointer 依赖所有者指针类型。
\warning 依赖所有者指针需要实现所有权语义，
	否则出现无法释放资源导致内存泄漏或其它非预期行为。
\todo 线程模型及安全性。
*/
template<typename _type, class _tOwnerPointer = GHandle<_type>>
class GDependency
{
public:
	typedef _type T;
	typedef _tOwnerPointer PointerType;

private:
	PointerType ptr;

public:
	GDependency(PointerType p = nullptr)
		: ptr(p)
	{
		GetCopyOnWritePtr();
	}

	DefConverter(const T&, *ptr)
	DefMutableConverter(T&, *ptr)

	DefGetter(const T&, Ref, operator const T&())
	DefMutableGetter(T&, Ref, operator T&())
	DefMutableGetter(T&, NewRef, *GetCopyOnWritePtr())

	PointerType
	GetCopyOnWritePtr()
	{
		if(!ptr)
			ptr = new T();
		else if(!ptr.unique())
			ptr = new T(*ptr);

		YAssert(ptr, "Null pointer found @ GDependency::GetCopyOnWritePtr;");

		return ptr;
	}
};

YSL_END

#endif

