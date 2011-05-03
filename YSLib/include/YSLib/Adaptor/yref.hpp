/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yref.hpp
\ingroup Adaptor
\brief 用于提供指针和引用访问的间接访问类模块。
\version 0.3457;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-21 23:09:06 +0800;
\par 修改时间:
	2011-05-03 17:28 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YReference;
*/


#ifndef INCLUDED_ADAPTOR_YREF_HPP_
#define INCLUDED_ADAPTOR_YREF_HPP_

#include "yadaptor.h"
#include <memory>
#include <utility>

YSL_BEGIN

using std::bad_weak_ptr;
using std::const_pointer_cast;
using std::dynamic_pointer_cast;
using std::enable_shared_from_this;
using std::get_deleter;
using std::shared_ptr;
using std::static_pointer_cast;
using std::unique_ptr;
using std::weak_ptr;


//! \brief 句柄：强指针。
template<typename T, class SPT = shared_ptr<T>>
class GHandle : public SPT
{
public:
	typedef SPT PtrType;

	/*!
	\brief 无参数构造。
	\note 得到空实例。
	*/
	GHandle(nullptr_t = nullptr)
		: SPT()
	{}

	/*!
	\brief 构造：使用内建指针。
	*/
	GHandle(T* p)
		: SPT(p)
	{}
	/*!
	\brief 构造：使用句柄引用。
	*/
	GHandle(const SPT& p)
		: SPT(p)
	{}
	/*!
	\brief 构造：使用其它类型句柄引用。
	*/
	template<typename C, class CS>
	inline
	GHandle(const GHandle<C, CS>& h)
		: SPT(h)
	{}

	bool
	operator!() const
	{
		return this->get() == nullptr;
	}

	bool
	operator==(const T* p) const
	{
		return this->get() == p;
	}

	bool
	operator!=(const T* p) const
	{
		return !(*this == p);
	}

	operator bool() const
	{
		return !!*this;
	}
};


//! \brief 弱句柄：弱指针。
template<typename T, class SPT = weak_ptr<T>>
class GHWeak : public SPT
{
public:
	typedef SPT PtrType;
	typedef GHandle<T, shared_ptr<T>> StrongPtrType;

	/*!
	\brief 无参数构造。
	\note 得到空实例。
	*/
	GHWeak(nullptr_t = nullptr)
		: SPT()
	{}
	/*!
	\brief 构造：使用内建指针。
	*/
	GHWeak(T* p)
		: SPT(StrongPtrType(p))
	{}
	/*!
	\brief 构造：使用强指针引用。
	*/
	GHWeak(const StrongPtrType& p)
		: SPT(p)
	{}
	/*!
	\brief 构造：使用其它类型弱句柄引用。
	*/
	template<typename C, class CS>
	inline
	GHWeak(const GHWeak<C, CS>& h)
		: SPT(h)
	{}

	bool
	operator!() const
	{
		return this->expired();
	}

	typename std::add_lvalue_reference<T>::type
	operator*() const
	{
		return std::move(*this->lock());
	}

	/*!
	\brief 转换：强指针引用。
	*/
	operator const StrongPtrType&() const
	{
		return this->lock();
	}

	operator bool() const
	{
		return !!*this;
	}
};


/*!	\defgroup raw Get Raw Pointers
\brief 取内建指针。
*/
//@{
template<typename _type>
inline _type*
raw(_type* const& p)
{
	return p;
}
template<typename _type>
inline _type*
raw(const unique_ptr<_type>& p)
{
	return p.get();
}
template<typename _type>
inline _type*
raw(const shared_ptr<_type>& p)
{
	return p.get();
}
template<typename _type>
inline _type*
raw(const GHandle<_type>& h)
{
	return h.get();
}
template<class _type>
inline _type*
raw(const GHWeak<_type>& h)
{
	return h.lock().get();
}

//@}


/*!	\defGroup reset_pointer Reset Pointers
\brief 安全删除指定引用的句柄指向的对象。
\post 指定引用的句柄值等于 nullptr 。
*/
//@{
template<typename _type>
inline bool
reset_pointer(_type*& p) ynothrow
{
	bool b(p);

	ydelete(p);
	p = nullptr;
	return b;
}
template<typename _type>
inline bool
reset_pointer(unique_ptr<_type>& p) ynothrow
{
	if(p.get())
	{
		p.reset();
		return true;
	}
	return false;
}
template<typename _type>
inline bool
reset_pointer(shared_ptr<_type>& p) ynothrow
{
	if(p.get())
	{
		p.reset();
		return true;
	}
	return false;
}
template<typename _type>
inline bool
reset_pointer(GHandle<_type>& p) ynothrow
{
	if(p.get())
	{
		p.reset();
		return true;
	}
	return false;
}
template<typename _type>
inline bool
reset_pointer(GHWeak<_type>& h) ynothrow
{
	bool b(!h.expired());

	if(b)
		h = GHWeak<_type>();
	return b;
}

//@}

YSL_END

#endif

