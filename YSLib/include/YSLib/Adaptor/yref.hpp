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
\version 0.3203;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-21 23:09:06 +0800;
\par 修改时间:
	2011-04-13 11:22 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Adaptor::YReference;
*/


#ifndef INCLUDED_YREF_HPP_
#define INCLUDED_YREF_HPP_

#include "yadaptor.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Policies)

/*!
\brief 智能指针所有权策略：使用 ystdex::general_cast 的引用计数所有权策略。
\note ystdex::general_cast 在调试模式下可能会断言失败。
*/
template <class P>
class GeneralCastRefCounted
{
public:
	enum
	{
		destructiveCopy = false
	};

private:
	uintptr_t* pCount_;

public:
	GeneralCastRefCounted()
		: pCount_(static_cast<uintptr_t*>(
		SmallObject<>::operator new(sizeof(uintptr_t))))
	{
		assert(pCount_);
		*pCount_ = 1;
	}
	GeneralCastRefCounted(const GeneralCastRefCounted& rhs)
		: pCount_(rhs.pCount_)
	{}
	template <typename P1>
	GeneralCastRefCounted(const GeneralCastRefCounted<P1>& rhs)
		: pCount_(reinterpret_cast<const GeneralCastRefCounted&>(rhs).pCount_)
	{}

	bool
	IsUnique() const
	{
		return *pCount_ == 1;
	}

	uintptr_t
	GetCount() const
	{
		return *pCount_;
	}

	P
	Clone(const P& val)
	{
		++*pCount_;
		return val;
	}

	template <typename P1>
	P
	Clone(const P1& val)
	{
		P r(ystdex::general_cast<P>(val));

		++*pCount_;
		return r;
	}

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

	void
	Swap(GeneralCastRefCounted& rhs)
	{
		std::swap(pCount_, rhs.pCount_);
	}
};

YSL_END_NAMESPACE(Policies)


//! \brief 强指针：资源指针。
template<typename T,
	template<class> class DP = Policies::DeleteSingle,
	class OP = Policies::TwoRefCounts,
	class CP = Policies::DisallowConversion,
	template<class> class KP = Policies::AssertCheck,
	template<class> class RP = Policies::CantResetWithStrong,
	typename SPT = StrongPtr<T, true, OP, CP, KP, RP, DP> >
class GHStrong : public SPT
{
public:
	typedef SPT PtrType;
	typedef SPT StrongPtrType;
	typedef StrongPtr<T, false, OP, CP, KP, RP, DP> WeakPtrType;

	/*!
	\brief 构造：使用内建指针。
	*/
	GHStrong(T* p = NULL)
		: SPT(p)
	{};
	/*!
	\brief 构造：使用强指针引用。
	*/
	GHStrong(RefToValue<GHStrong> p)
		: SPT(p)
	{}

	/*!
	\brief 取内建指针。
	*/
	T*
	operator->() const
	{
		return GHStrong<T>::GetPointer();
	}

	/*!
	\brief 释放指针。
	\note Policies::CantResetWithStrong 策略决定
		无法释放所有指向同一对象的强指针；这里仅影响单独的 GHStrong 对象实例。
	*/
	T*
	Release()
	{
		T* p(this->operator->());

		*this = NULL;
		return p;
	}

	/*!
	\brief 复位指针。
	\note Policies::CantResetWithStrong 策略决定
		无法复位所有指向同一对象的强指针；这里仅影响单独的 GHStrong 对象实例。
	*/
	bool
	Reset(T* p = NULL)
	{
		if(this->operator->() != p)
		{
			GHStrong hTemp(NULL);

			this->Swap(hTemp);
			return true;
		}
		return false;
	}
};


//! \brief 弱指针：资源引用指针。
template<typename T,
	template<class> class DP = Policies::DeleteSingle,
	class OP = Policies::TwoRefCounts,
	class CP = Policies::DisallowConversion,
	template<class> class KP = Policies::AssertCheck,
	template<class> class RP = Policies::CantResetWithStrong,
	typename SPT = StrongPtr<T, false, OP, CP, KP, RP, DP> >
class GHWeak : public SPT
{
public:
	typedef SPT PtrType;
	typedef StrongPtr<T, true, OP, CP, KP, RP, DP> StrongPtrType;
	typedef SPT WeakPtrType;

	/*!
	\brief 构造：使用内建指针。
	*/
	GHWeak(T* p = NULL)
		: SPT(p)
	{}
	/*!
	\brief 构造：使用强指针。
	*/
	GHWeak(StrongPtrType p)
		: SPT(p)
	{}
	/*!
	\brief 构造：使用强指针引用。
	*/
	GHWeak(RefToValue<StrongPtrType> p)
		: SPT(p)
	{}
	/*!
	\brief 构造：使用弱指针引用。
	*/
	GHWeak(RefToValue<GHWeak> p)
		: SPT(p)
	{}

	/*!
	\brief 取内建指针。
	*/
	T*
	operator->() const
	{
		return GHWeak<T>::GetPointer();
	}

	/*!
	\brief 释放指针。
	\note Policies::CantResetWithStrong 策略决定可以释放弱指针；
		会影响所有指向同一对象的 GHStrong 和 GHWeak 的对象实例。
	*/
	T*
	Release()
	{
		T* p(NULL);

		Loki::ReleaseAll(static_cast<SPT&>(*this), p);
		return p;
	}

	/*!
	\brief 复位指针。
	\note Policies::CantResetWithStrong 策略决定可以复位弱指针；
		会影响所有指向同一对象的 GHStrong 和 GHWeak 的对象实例。
	*/
	bool
	Reset(T* p = NULL)
	{
		return Loki::ResetAll(static_cast<SPT&>(*this), p);
	}
};


//! \brief 句柄。
template<typename T,
	template<class> class OP = Policies::GeneralCastRefCounted,
	class CP = Policies::DisallowConversion,
	template<class> class KP = Policies::AssertCheck,
	template<class> class SP = Policies::DefaultSPStorage,
	typename SPT = SmartPtr<T, OP, CP, KP, SP> >
class GHandle : public SPT
{
public:
	typedef SPT PtrType;

	/*!
	\brief 构造：使用内建指针。
	*/
	GHandle(T* p = NULL)
		: SPT(p)
	{}
	/*!
	\brief 构造：使用句柄引用。
	*/
	GHandle(RefToValue<GHandle> p)
		: SPT(p)
	{}
	/*!
	\brief 构造：使用其它类型句柄引用。
	*/
	template<class C>
	inline explicit
	GHandle(const GHandle<C>& h)
		: SPT(h)
	{}

	/*!
	\brief 转换：句柄引用。
	*/
	operator RefToValue<GHandle>()
	{
		return RefToValue<GHandle>(*this);
	}

	/*!
	\brief 取内建指针。
	*/
	T*
	operator->() const
	{
		return GHandle<T>::GetPointer(*this);
	}

	/*!
	\brief 释放指针。
	*/
	T*
	Release()
	{
		T* p(NULL);

		Loki::Release(*this, p);
		return p;
	}

	/*!
	\brief 复位指针。
	*/
	bool
	Reset(T* p = NULL)
	{
		if(p != operator->())
		{
			Loki::Reset(*this, p);
			return true;
		}
		return false;
	}
};


/*!	\defgroup GetPointer GetPointer
\brief 取内建指针。
*/
//@{
template<typename _type>
inline _type*
GetPointer(_type* h)
{
	return h;
}
template<typename _type>
inline _type*
GetPointer(SmartPtr<_type> h)
{
	return SmartPtr<_type>::GetPointer(h);
}
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
template<typename _type>
inline _type*
GetPointer(GHandle<_type> h)
{
	return h.operator->();
}
//@}

/*!
\brief 转换：指定类型句柄为整数。
*/
template<typename _tIntegral, typename _tReference>
inline _tIntegral
handle2int(GHandle<_tReference> h)
{
	return reinterpret_cast<_tIntegral>(GetPointer(h));
}

/*!
\brief 转换：指定类型整数为句柄。
*/
template<typename _tIntegral, typename _tReference>
inline GHandle<_tReference>
handle2int(_tIntegral i)
{
	return GHandle<_tReference>(reinterpret_cast<_tIntegral>(i));
}

/*!
\brief 转换：指定类型句柄。
*/
template<typename _type, typename _tReference>
inline GHandle<_type>
static_handle_cast(const GHandle<_tReference>& h)
{
	return GHandle<_type>(h);
}

/*!
\brief 转换：指定类型句柄。
*/
template<typename _type, typename _tReference>
inline GHandle<_type>
general_handle_cast(GHandle<_tReference> h)
{
	return GHandle<_type>(h);
}


/*!	\defGroup YReset YReset
\brief 安全删除指定引用的句柄指向的对象。
\post 指定引用的句柄值等于 NULL 。
*/
//@{
template<typename _type>
inline bool
YReset_ndebug(_type*& h) ythrow()
{
	bool b(h);

	delete h;
	h = NULL;
	return b;
}
template<typename _type>
inline bool
YReset_ndebug(GHandle<_type>& h) ythrow()
{
	return h.Reset();
}
template<typename _type>
inline bool
YReset_ndebug(GHStrong<_type>& h) ythrow()
{
	return h.Reset();
}
template<typename _type>
inline bool
YReset_ndebug(GHWeak<_type>& h) ythrow()
{
	return h.Reset();
}

#ifdef YSL_USE_MEMORY_DEBUG

template<typename _type>
inline bool
YReset_debug(_type*& h) ythrow()
{
	bool b(h);

	ydelete(h);
	h = NULL;
	return b;
}
template<typename _type>
inline bool
YReset_debug(GHandle<_type>& h) ythrow()
{
//	DebugMemory.Unregister(GetPointer(h), __FILE__, __LINE__);
	return h.Reset();
}
template<typename _type>
inline bool
YReset_debug(GHStrong<_type>& h) ythrow()
{
	return h.Reset();
}
template<typename _type>
inline bool
YReset_debug(GHWeak<_type>& h) ythrow()
{
	return h.Reset();
}

#	define YReset YReset_debug

#else

#	define YReset YReset_ndebug

#endif

//@}

YSL_END

#endif

