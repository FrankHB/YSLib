/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YStorage.hpp
\ingroup Core
\brief 全局公用存储管理。
\version r446
\author FrankHB <frankhb1989@gmail.com>
\since build 195
\par 创建时间:
	2011-03-14 20:17:34 +0800
\par 修改时间:
	2015-01-19 20:53 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YStorage
*/


#ifndef YSL_INC_Core_ystorage_hpp_
#define YSL_INC_Core_ystorage_hpp_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YCoreUtilities // for YSLib::safe_delete_obj;

namespace YSLib
{

/*!
\brief 取指定类型的无参数构造的对象。
\since build 241
*/
template<typename _type>
inline _type
FetchInstance()
{
	return _type();
}

/*!
\brief 取指定类型的静态默认对象。
\since build 240
*/
template<typename _type>
inline _type&
FetchStaticRef()
{
	return ystdex::parameterize_static_object<_type>();
}

/*!
\brief 取指定类型的静态原型对象。
\since build 240
*/
template<typename _type>
inline const _type&
FetchPrototype()
{
	return ystdex::parameterize_static_object<const _type>();
}


/*!
\brief 全局静态单态存储器。
\pre <tt>std::is_nothrow_constructible<_tPointer>::value</tt> 。
\warning 非线程安全。
\since 早于 build 132
*/
template<typename _type, typename _tPointer = _type*>
class GStaticCache : private noncopyable
{
	static_assert(std::is_nothrow_constructible<_tPointer>::value,
		"Invalid pointer type found");

public:
	using PointerType = _tPointer;

private:
	static PointerType _ptr;

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
	//! \since build 567
	DefDelCtor(GStaticCache)

	static DefGetter(ynothrow, PointerType, PointerRaw, _ptr)
	/*!
	\brief 取指针。
	*/
	static const PointerType&
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
	\since build 319
	*/
	static void
	Release() ynothrow
	{
		safe_delete_obj()(_ptr);
	}
};

template<typename _type, typename _tPointer>
typename GStaticCache<_type, _tPointer>::PointerType
	GStaticCache<_type, _tPointer>::_ptr;


/*!
\brief 全局局部静态单态存储器。
\pre <tt>std::is_nothrow_constructible<_tPointer>::value</tt> 。
\note 存储使用以本类实例作为附加参数类型实例化的
	ystdex::parameterize_static_object 模板函数提供。
\warning 非线程安全。
\since build 205
*/
template<typename _type, typename _tPointer = _type*>
class GLocalStaticCache : private noncopyable
{
	static_assert(std::is_nothrow_constructible<_tPointer>::value,
		"Invalid pointer type found");

public:
	using PointerType = _tPointer;

private:
	/*!
	\brief 取静态指针引用。
	\since build 319
	*/
	static PointerType&
	GetStaticPtrRef() ynothrow
	{
		return ystdex::parameterize_static_object<PointerType,
			GLocalStaticCache>();
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
	//! \since build 567
	DefDelCtor(GLocalStaticCache)

	static DefGetter(ynothrow, PointerType, PointerRaw, GetStaticPtrRef())
	/*!
	\brief 取指针。
	\since build 567
	*/
	static const PointerType&
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
	\since build 319
	*/
	static void
	Release() ynothrow
	{
		safe_delete_obj()(GetStaticPtrRef());
	}
};

} // namespace YSLib;

#endif

