/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yref.hpp
\ingroup Adaptor
\brief 用于提供指针和引用访问的间接访问类模块。
\version 0.2370;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-21 23:09:06 + 08:00;
\par 修改时间:
	2010-11-12 18:56 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YReference;
*/


#ifndef INCLUDED_YREF_HPP_
#define INCLUDED_YREF_HPP_

#include "yadaptor.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Design)

YSL_BEGIN_NAMESPACE(Policies)

YSL_BEGIN_NAMESPACE(Operations)

//! \brief 智能指针简单操作策略。
template<class P>
struct SmartPtr_Simple
{
	enum
	{
		destructiveCopy = false
	};

	/*!
	\brief 构造。
	*/
	SmartPtr_Simple()
	{}

	/*!
	\brief 复制构造。
	*/
	template<class P1>
	SmartPtr_Simple(const SmartPtr_Simple<P1>& rhs)
	{}

	/*!
	\brief 复制。
	*/
	static P
	Clone(const P& val)
	{
		return val;
	}

	/*!
	\brief 释放。
	*/
	bool
	Release(const P&)
	{
		return false;
	}

	/*!
	\brief 交换。
	*/
	static void
	Swap(SmartPtr_Simple&)
	{}
};

//! \brief 智能指针引用计数操作策略。
template<class P>
class SmartPtr_RefCounted
{
private:
	uintptr_t* pCount_;

protected:
	/*!
	\brief 增加引用计数。
	*/
	void
	AddRef()
	{
		++*pCount_;
	}

public:
	enum { destructiveCopy = false };

	/*!
	\brief 构造。
	*/
	SmartPtr_RefCounted()
		: pCount_(static_cast<uintptr_t*>(
		SmallObject<>::operator new(sizeof(uintptr_t))))
	{
	//	assert(pCount_!=0);
		*pCount_ = 1;
	}

	/*!
	\brief 复制构造。
	*/
	SmartPtr_RefCounted(const SmartPtr_RefCounted& rhs)
		: pCount_(rhs.pCount_)
	{}

	/*!
	\brief 复制构造。
	*/
	template<typename P1>
	SmartPtr_RefCounted(const SmartPtr_RefCounted<P1>& rhs)
		: pCount_(reinterpret_cast<const SmartPtr_RefCounted&>(rhs).pCount_)
	{}

	/*!
	\brief 复制。
	*/
	P
	Clone(const P& val)
	{
		AddRef();
		return val;
	}

	/*!
	\brief 释放引用计数。
	*/
	bool
	Release(const P&)
	{
		if(!--*pCount_)
		{
			SmallObject<>::operator delete(pCount_, sizeof(uintptr_t));
			pCount_ = NULL;
			return true;
		}
		return false;
	}

	/*!
	\brief 交换。
	*/
	void
	Swap(SmartPtr_RefCounted& rhs)
	{
		std::swap(pCount_, rhs.pCount_);
	}
};

YSL_END_NAMESPACE(Operations)

YSL_END_NAMESPACE(Policies)

YSL_END_NAMESPACE(Design)


//! \brief 资源指针。
template<typename T,
	template<class> class OP = Design::Policies::Operations::SmartPtr_RefCounted,
	class CP = AllowConversion,
	template<class> class KP = RejectNull,
	template<class> class SP = DefaultSPStorage,
	typename SPType = SmartPtr<T, OP, CP, KP, SP> >
class GHResource : public SPType
{
public:
	/*!
	\brief 构造：使用内建指针。
	*/
	GHResource(T* p = NULL) : SPType(p) {};
	/*!
	\brief 构造：使用对象引用。
	*/
	GHResource(T& rhs) : SPType(rhs) {}
	/*!
	\brief 构造：使用智能指针引用。
	*/
	GHResource(RefToValue<GHResource> rhs) : SPType(rhs) {}

	/*!
	\brief 转换：智能指针引用。
	*/
	operator RefToValue<GHResource>()
	{
		return RefToValue<GHResource>(*this);
	}

	/*!
	\brief 取内建指针。
	*/
	T*
	operator->() const
	{
		return GHResource<T>::GetPointer(*this);
	}

	/*!
	\brief 赋值复制。
	*/
	GHResource&
	operator=(T& rhs)
	{
		GHResource temp(rhs);
		temp.Swap(*this);
		return *this;
	}

	/*!
	\brief 取内建指针。
	*/
	T*
	GetPtr() const
	{
		return operator->();
	}

	/*!
	\brief 交换。
	*/
	void
	Swap(GHResource& rhs)
	{
		Swap(rhs);
	}
};


/*!
\brief 取内建指针。
*/
template<class _type>
inline _type*
GetPointer(GHResource<_type> h)
{
	return h.GetPtr();
}


#define YHandleOP Design::Policies::Operations::SmartPtr_Simple
//! \warning 句柄不会被自动回收，需要手动释放。
#define YDelete(h) delete h


//! \brief 句柄类。
template<typename T,
	template<class> class OP = YHandleOP,
	class CP = AllowConversion,
	template<class> class KP = RejectNull,
	template<class> class SP = DefaultSPStorage,
	typename SPType = SmartPtr<T, OP, CP, KP, SP> >
class GHHandle : public SPType
{
public:
	/*!
	\brief 构造：使用内建指针。
	*/
	GHHandle(T* p = NULL) : SPType(p) {}
	/*!
	\brief 构造：使用对象引用。
	*/
	GHHandle(T& rhs) : SPType(rhs) {}
	/*!
	\brief 构造：使用句柄引用。
	*/
	GHHandle(RefToValue<GHHandle> rhs) : SPType(rhs) {}
	/*!
	\brief 构造：使用其它类型句柄引用。
	*/
	template<class C>
	explicit
	GHHandle(GHHandle<C>& h) : SPType(static_cast<T*>(GHHandle<C>::GetPointer(h))) {}

	/*!
	\brief 转换：句柄引用。
	*/
	operator RefToValue<GHHandle>()
	{
		return RefToValue<GHHandle>(*this);
	}

	/*!
	\brief 取内建指针。
	*/
	T*
	operator->() const
	{
		return GHHandle<T>::GetPointer(*this);
	}

	/*!
	\brief 赋值复制。
	*/
	GHHandle&
	operator=(T& rhs)
	{
		GHHandle temp(rhs);
		temp.Swap(*this);
		return *this;
	}

	/*!
	\brief 取内建指针。
	*/
	T*
	GetPtr() const
	{
		return operator->();
	}

	/*!
	\brief 交换。
	*/
	void
	Swap(GHHandle& rhs)
	{
		Swap(rhs);
	}
};


#undef YHandleOP


/*!
\brief 取内建指针。
*/
template<typename _type>
inline _type*
GetPointer(_type* h)
{
	return h;
}

#ifndef YSL_USE_SIMPLE_HANDLE

/*!
\brief 取内建指针。
*/
template<typename _tReference>
inline _tReference*
GetPointer(GHHandle<_tReference> h)
{
	return h.GetPtr();
}

/*!
\brief 转换：指定类型句柄。
*/
template<typename _type, typename _tReference>
inline _type
handle_cast(GHHandle<_tReference> h)
{
	return reinterpret_cast<_type>(GHHandle<_tReference>::GetPointer(h));
}

#endif

YSL_END

#endif

