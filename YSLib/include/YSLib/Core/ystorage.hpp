
/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ystorage.hpp
\ingroup Core
\brief 全局公用存储管理。
\version r1352;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-03-14 20:17:34 +0800;
\par 修改时间:
	2011-09-13 23:43 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YStorage;
*/


#ifndef INCLUDED_CORE_YSTORAGE_HPP_
#define INCLUDED_CORE_YSTORAGE_HPP_

#include "ycutil.h"

YSL_BEGIN

/*!
\brief 取指定类型的默认构造的对象。
*/
PDefTH1(_type)
inline _type
FetchInstance()
{
	return _type();
}

/*!
\brief 取指定类型的静态默认对象。
*/
PDefTH1(_type)
inline _type&
FetchStaticRef()
{
	static _type _obj;

	return _obj;
}

/*!
\brief 取指定类型的静态原型对象。
*/
PDefTH1(_type)
inline const _type&
FetchPrototype()
{
	static _type _obj;

	return _obj;
}


//! \brief 全局静态单例存储器。
template<typename _type, typename _tPointer = _type*>
class GStaticCache
{
public:
	typedef _tPointer PointerType;

private:
	static PointerType _ptr;

	GStaticCache();

	/*!
	\brief 检查是否已经初始化，否则构造新对象。
	*/
	static void
	Check()
	{
		if(!_ptr)
			_ptr = PointerType(new _type());
	}

public:
	static DefMutableGetter(PointerType, PointerRaw, _ptr)
	/*!
	\brief 取指针。
	*/
	static PointerType
	GetPointer()
	{
		Check();
		return GetPointerRaw();
	}
	/*!
	\brief 取实例引用。
	*/
	static _type&
	GetInstance()
	{
		Check();
		return *GetPointer();
	}

	/*!
	\brief 删除对象并置指针为空值。
	*/
	inline static void
	Release()
	{
		safe_delete_obj()(_ptr);
	}
};

template<typename _type, typename _tPointer>
typename GStaticCache<_type, _tPointer>::PointerType
	GStaticCache<_type, _tPointer>::_ptr;


//! \brief 全局局部静态单例存储器。
template<typename _type, typename _tPointer = _type*>
class GLocalStaticCache
{
public:
	typedef _tPointer PointerType;

private:
	static PointerType _ptr;

	GLocalStaticCache();

	/*!
	\brief 取静态指针引用。
	*/
	inline static PointerType&
	GetStaticPtrRef()
	{
		static PointerType ptr;

		return ptr;
	}

	/*!
	\brief 检查是否已经初始化，否则构造新对象。
	*/
	static void
	Check()
	{
		PointerType& ptr(GetStaticPtrRef());

		if(!ptr)
			ptr = PointerType(new _type());
	}

public:
	static DefMutableGetter(PointerType, PointerRaw, GetStaticPtrRef())
	/*!
	\brief 取指针。
	*/
	static PointerType
	GetPointer()
	{
		Check();
		return GetPointerRaw();
	}
	/*!
	\brief 取实例引用。
	*/
	static _type&
	GetInstance()
	{
		Check();
		return *GetPointer();
	}

	/*!
	\brief 删除对象并置指针为空值。
	*/
	inline static void
	Release()
	{
		safe_delete_obj()(GetStaticPtrRef());
	}
};

YSL_END

#endif

