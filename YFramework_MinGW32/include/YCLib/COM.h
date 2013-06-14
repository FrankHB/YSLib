/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file COM.h
\ingroup YCLib
\ingroup MinGW32
\brief COM 接口。
\version r426
\author FrankHB <frankhb1989@gmail.com>
\since build 412
\par 创建时间:
	2012-06-07 10:29:30 +0800
\par 修改时间:
	2013-06-11 12:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::COM
*/


#ifndef YCL_MinGW32_INC_COMPtr_h_
#define YCL_MinGW32_INC_COMPtr_h_ 1

#include "MinGW32.h"
#include <objbase.h>

namespace platform_ex
{

//! \since build 412
//@{
//! \brief COM 全局状态。
class YF_API COM
{
protected:
	//! \since build 413
	::HRESULT hResult;

public:
	COM()
		: hResult(::CoInitialize(nullptr))
	{}
	~COM()
	{
		if(hResult == S_OK)
			::CoUninitialize();
	}

	//! \since build 413
	DefGetter(ynothrow, ::HRESULT, HResult, hResult)
};


//! \brief COM 异常基类。
class YF_API COMException : public std::runtime_error
{
protected:
	::HRESULT hResult;

public:
	COMException(::HRESULT h)
		: runtime_error("COMException"), hResult(h)
	{
		yconstraint(FAILED(h) || h == S_FALSE);
	}

	DefGetter(ynothrow, ::HRESULT, HResult, hResult)
};


/*!
\brief 检查 ::HRESULT 值，若表示失败则抛出 COMException 。
\return 表示成功的值。
*/
inline ::HRESULT
CheckHResult(::HRESULT h) ythrow(COMException)
{
	if(FAILED(h))
		throw COMException(h);
	return h;
}

/*!
\brief 检查指针值，若为空则抛出 COMException 。
*/
inline void
EnsureNonNull(void* p) ythrow(COMException)
{
	if(!p)
		throw COMException(S_FALSE);
}


/*
\brief COM 指针。
\warning 非虚析构。
*/
template<class _iCOM>
class COMPtr
{
	template<class _iOther>
	friend class COMPtr;

public:
	typedef _iCOM InterfaceType;

protected:
	InterfaceType* pInterface;

public:
	COMPtr() ynothrow
		: pInterface()
	{}
	COMPtr(std::nullptr_t) ynothrow
		: pInterface()
	{}
	template<class _iOther>
	COMPtr(_iOther* ptr) ynothrow
		: pInterface(ptr)
	{}
	template<class _iOther>
	COMPtr(_iOther& intf, typename std::enable_if<!std::is_convertible<_iOther&,
		COMPtr&>::value, int>::type = 0) ynothrow
		: pInterface(&intf)
	{
		pInterface->AddRef();
	}
	COMPtr(const COMPtr& ptr) ynothrow
		: pInterface(ptr.pInterface)
	{
		InternalAddRef();
	}
	template<class _iOther>
	COMPtr(const COMPtr<_iOther>& ptr,
		typename std::enable_if<std::is_convertible<_iOther*, _iCOM*>::value,
		int>::type = 0) ynothrow
		: pInterface(ptr.pInterface)
	{
		InternalAddRef();
	}
	COMPtr(COMPtr&& ptr) ynothrow
		: pInterface()
	{
		ptr.swap(*this);
	}
	template<class _iOther>
	COMPtr(COMPtr<_iOther>&& ptr,
		typename std::enable_if<std::is_convertible<_iOther*, _iCOM*>::value,
		int>::type = 0) ynothrow
		: pInterface(ptr.pInterface)
	{
		ptr.pInterface = nullptr;
	}
	~COMPtr() ynothrow
	{
		InternalRelease();
	}

	COMPtr&
	operator=(std::nullptr_t) ynothrow
	{
		InternalRelease();
		return *this;
	}
	COMPtr&
	operator=(_iCOM* p) ynothrow
	{
		if(pInterface != p)
			COMPtr(p).swap(*this);
		return *this;
	}
	COMPtr&
	operator=(const COMPtr& ptr) ynothrow
	{
		COMPtr(ptr).swap(*this);
		return *this;
	}
	COMPtr&
	operator=(COMPtr&& ptr) ynothrow
	{
		ptr.swap(*this);
		return *this;
	}

	_iCOM&
	operator*() const ynothrow
	{
		yconstraint(pInterface);

		return *pInterface;
	}

	_iCOM*
	operator->() const ynothrow
	{
		return pInterface;
	}

	explicit
	operator bool() const ynothrow
	{
		return Get() != nullptr;
	}

	DefGetter(const ynothrow, _iCOM*, , pInterface)
	DefGetter(const, _iCOM&, Object, EnsureNonNull(pInterface), *pInterface)
	DefGetter(ynothrow, _iCOM*&, Ref, pInterface)

	COMPtr<IUnknown>
	As(REFIID riid) const ythrow(COMException)
	{
		yconstraint(pInterface);

		COMPtr<IUnknown> res;

		CheckHResult(pInterface->QueryInterface(riid,
			reinterpret_cast<void**>(&res.ReleaseAndGetRef())));
		return std::move(res);
	}
	template<class _iOther>
	COMPtr<_iOther>
	As() const ythrow(COMException)
	{
		yconstraint(pInterface);

		COMPtr<_iOther> res;

		CheckHResult(pInterface->QueryInterface(__uuidof(_iOther),
			reinterpret_cast<void**>(&res.ReleaseAndGetRef())));
		return std::move(res);
	}

	::HRESULT
	Cast(REFIID riid, COMPtr<IUnknown>& ptr) const ynothrow
	{
		yconstraint(pInterface);

		return pInterface->QueryInterface(riid,
			reinterpret_cast<void**>(&ptr.ReleaseAndGetRef()));
	}
	template<class _iOther>
	::HRESULT
	Cast(COMPtr<_iOther>& ptr) const ynothrow
	{
		yconstraint(pInterface);

		return pInterface->QueryInterface(__uuidof(_iOther),
			reinterpret_cast<void**>(&ptr.ReleaseAndGetRef()));
	}

	InterfaceType*
	Copy() const ynothrow
	{
		InternalAddRef();
		return pInterface;
	}
	void*
	Copy(REFIID riid) const ythrow(COMException)
	{
		yconstraint(pInterface);

		void* p;

		CheckHResult(pInterface->QueryInterface(riid, &p));
		return p;
	}

	::HRESULT
	CopyTo(REFIID riid, void** ptr) const ynothrow
	{
		yconstraint(pInterface);

		return pInterface->QueryInterface(riid, ptr);
	}
	template<typename _type>
	::HRESULT
	CopyTo(_type*& p) const ynothrow
	{
		return pInterface->QueryInterface(__uuidof(_type),
			reinterpret_cast<void**>(&p));
	}

protected:
	void
	InternalAddRef() const ynothrow
	{
		if(pInterface)
			pInterface->AddRef();
	}

	void
	InternalRelease() ynothrow
	{
		if(const auto tmp = pInterface)
		{
			pInterface = nullptr;
			tmp->Release();
		}
	}

public:
	_iCOM*&
	ReleaseAndGetRef() ynothrow
	{
		InternalRelease();
		return pInterface;
	}

	void
	swap(COMPtr& ptr) ynothrow
	{
		std::swap(pInterface, ptr.pInterface);
	}
};

template<class _iCOM1, class _iCOM2>
inline bool
operator==(const COMPtr<_iCOM1>& x, const COMPtr<_iCOM2>& y) ynothrow
{
	static_assert(std::is_base_of<_iCOM1, _iCOM2>::value
		|| std::is_base_of<_iCOM1, _iCOM2>::value,
		"'_iCOM1' and '_iCOM2' pointers must be comparable");

	return x.Get() == y.Get();
}
template<class _iCOM>
inline bool
operator==(const COMPtr<_iCOM>& x, std::nullptr_t) ynothrow
{
	return !x.Get();
}
template<class _iCOM>
inline bool
operator==(std::nullptr_t, const COMPtr<_iCOM>& x) ynothrow
{
	return !x.Get();
}

template<class _iCOM1, class _iCOM2>
inline bool
operator!=(const COMPtr<_iCOM1>& x, const COMPtr<_iCOM2>& y) ynothrow
{
	return !(x == y);
}
template<class _iCOM>
inline bool
operator!=(const COMPtr<_iCOM>& x, std::nullptr_t) ynothrow
{
	return x.Get();
}

template<class _iCOM>
inline bool
operator!=(std::nullptr_t, const COMPtr<_iCOM>& x) ynothrow
{
	return x.Get();
}

template<class _iCOM1, class _iCOM2>
inline bool
operator<(const COMPtr<_iCOM1>& x, const COMPtr<_iCOM2>& y) ynothrow
{
	static_assert(std::is_base_of<_iCOM1, _iCOM2>::value
		|| std::is_base_of<_iCOM1, _iCOM2>::value,
		"'_iCOM1' and '_iCOM2' pointers must be comparable");

	return x.Get() < y.Get();
}

template<class _iCOM>
void
Attach(COMPtr<_iCOM>& ptr, typename COMPtr<_iCOM>::InterfaceType* p) ynothrow
{
	if(const auto p_interface = ptr.Get())
	{
		const auto ref(p_interface->Release());

		static_cast<void>(ref);

		yassume(ref != 0 || p_interface != p);
	}
	ptr.GetRef() = ptr;
}

template<class _iCOM>
_iCOM*
Detach(COMPtr<_iCOM>& ptr) ynothrow
{
	auto p(ptr.Get());

	ptr.GetRef() = nullptr;
	return p;
}

template<class _iCOM>
unsigned long
Reset(COMPtr<_iCOM>& ptr) ynothrow
{
	auto n(0UL);

	if(const auto tmp = ptr.Get())
	{
		ptr.GetRef() = nullptr;
		n = tmp->Release();
	}
	return n;
}

template<class _iCOM>
inline DefSwap(ynothrow, COMPtr<_iCOM>)
//@}

} // namespace platform_ex;

#endif

