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
\version 0.2468;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-21 23:09:06 + 08:00;
\par 修改时间:
	2010-11-29 15:42 + 08:00;
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

YSL_END_NAMESPACE(Policies)

YSL_END_NAMESPACE(Design)


//! \brief 资源指针。
template<typename T,
	class OP = Policies::TwoRefCounts,
	class CP = Policies::DisallowConversion,
	template<class> class KP = Policies::AssertCheck,
	template<class> class RP = Policies::CantResetWithStrong,
	template<class> class DP = Policies::DeleteSingle,
	typename SPType = StrongPtr<T, true, OP, CP, KP, RP, DP> >
class GHStrong : public SPType
{
public:
	/*!
	\brief 构造：使用内建指针。
	*/
	GHStrong(T* p = NULL)
		: SPType(p)
	{};
	/*!
	\brief 构造：使用对象引用。
	*/
	GHStrong(T& rhs)
		: SPType(rhs)
	{}
	/*!
	\brief 构造：使用智能指针引用。
	*/
	GHStrong(RefToValue<GHStrong> rhs)
		: SPType(rhs)
	{}

	/*!
	\brief 取内建指针。
	*/
	T*
	operator->() const
	{
		return GHStrong<T>::GetPointer();
	}
};


//! \brief 资源引用指针。
template<typename T,
	class OP = Policies::TwoRefCounts,
	class CP = Policies::DisallowConversion,
	template<class> class KP = Policies::AssertCheck,
	template<class> class RP = Policies::CantResetWithStrong,
	template<class> class DP = Policies::DeleteSingle,
	typename SPType = StrongPtr<T, false, OP, CP, KP, RP, DP> >
class GHWeak : public SPType
{
public:
	/*!
	\brief 构造：使用内建指针。
	*/
	GHWeak(T* p = NULL)
		: SPType(p)
	{}
	/*!
	\brief 构造：使用对象引用。
	*/
	GHWeak(T& rhs)
		: SPType(rhs)
	{}
	/*!
	\brief 构造：使用强指针。
	*/
	GHWeak(GHStrong<T> p)
		: SPType(p)
	{}
	/*!
	\brief 构造：使用强指针引用。
	*/
	GHWeak(RefToValue<GHStrong<T> > rhs)
		: SPType(rhs)
	{}
	/*!
	\brief 构造：使用弱指针引用。
	*/
	GHWeak(RefToValue<GHWeak> rhs)
		: SPType(rhs)
	{}

	/*!
	\brief 取内建指针。
	*/
	T*
	operator->() const
	{
		return GHWeak<T>::GetPointer();
	}
};


/*!
\brief 取内建指针。
*/
template<class _type>
inline _type*
GetPointer(GHStrong<_type> h)
{
	return h.operator->();
}
template<class _type>
inline _type*
GetPointer(GHWeak<_type> h)
{
	return h.operator->();
}


#define YHandleOP Design::Policies::SmartPtr_Simple
//! \warning 句柄不会被自动回收，需要手动释放。
#define YDelete(h) delete GetPointer(h)


//! \brief 句柄类。
template<typename T,
	template<class> class OP = YHandleOP,
	class CP = Policies::DisallowConversion,
	template<class> class KP = Policies::AssertCheck,
	template<class> class SP = Policies::DefaultSPStorage,
	typename SPType = SmartPtr<T, OP, CP, KP, SP> >
class GHHandle : public SPType
{
public:
	/*!
	\brief 构造：使用内建指针。
	*/
	GHHandle(T* p = NULL)
		: SPType(p)
	{}
	/*!
	\brief 构造：使用对象引用。
	*/
	GHHandle(T& rhs)
		: SPType(rhs)
	{}
	/*!
	\brief 构造：使用句柄引用。
	*/
	GHHandle(RefToValue<GHHandle> rhs)
		: SPType(rhs)
	{}
	/*!
	\brief 构造：使用其它类型句柄引用。
	*/
	template<class C>
	explicit
	GHHandle(GHHandle<C>& h) : SPType(static_cast<T*>(
		GHHandle<C>::GetPointer(h)))
	{}

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
	return h.operator->();
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

