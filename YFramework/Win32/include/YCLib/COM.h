/*
	© 2013-2016, 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file COM.h
\ingroup YCLib
\ingroup Win32
\brief COM 接口。
\version r539
\author FrankHB <frankhb1989@gmail.com>
\since build 412
\par 创建时间:
	2012-06-07 10:29:30 +0800
\par 修改时间:
	2020-12-24 12:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Win32)::COM
*/


#ifndef YCL_Win32_INC_COMPtr_h_
#define YCL_Win32_INC_COMPtr_h_ 1

#include "YCLib/YModules.h"
#include YFM_Win32_YCLib_MinGW32 // for platform_ex, std::runtime_error,
//	ystdex::enable_if_t, std::is_convertible, std::nullptr_t,
//	ystdex::replace_cast;
#include <objbase.h> // for ::HRESULT, ::CoInitialize, S_OK, ::CoUnitialize,
//	FAILED, S_FALSE, ::IID;

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
		YAssert(FAILED(h) || h == S_FALSE, "Invalid argument found.");
	}

	DefGetter(ynothrow, ::HRESULT, HResult, hResult)
};


/*!
\brief 检查 ::HRESULT 值，若表示失败则抛出 COMException 。
\return 表示成功的值。
\since build 563
*/
inline ::HRESULT
CheckHResult(::HRESULT h)
{
	if(FAILED(h))
		throw COMException(h);
	return h;
}

/*!
\brief 检查指针值，若为空则抛出 COMException 。
\since build 563
*/
inline PDefH(void, EnsureNonNull, void* p)
	ImplExpr(p ? void() : throw COMException(S_FALSE))


/*!
\brief COM 指针。
\warning 非虚析构。
*/
template<class _iCOM>
class COMPtr
{
	template<class _iOther>
	friend class COMPtr;

public:
	using InterfaceType = _iCOM;

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
	//! \since build 439
	template<class _iOther>
	COMPtr(_iOther& intf, ystdex::enable_if_t<!std::is_convertible<_iOther&,
		COMPtr&>::value, int> = 0) ynothrow
		: pInterface(&intf)
	{
		pInterface->AddRef();
	}
	COMPtr(const COMPtr& ptr) ynothrow
		: pInterface(ptr.pInterface)
	{
		InternalAddRef();
	}
	//! \since build 439
	template<class _iOther>
	COMPtr(const COMPtr<_iOther>& ptr, ystdex::enable_if_t<
		std::is_convertible<_iOther*, _iCOM*>::value, int> = 0) ynothrow
		: pInterface(ptr.pInterface)
	{
		InternalAddRef();
	}
	COMPtr(COMPtr&& ptr) ynothrow
		: pInterface()
	{
		ptr.swap(*this);
	}
	//! \since build 439
	template<class _iOther>
	COMPtr(COMPtr<_iOther>&& ptr, ystdex::enable_if_t<
		std::is_convertible<_iOther*, _iCOM*>::value, int> = 0) ynothrow
		: pInterface(ptr.pInterface)
	{
		ptr.pInterface = nullptr;
	}
	//! \since build 461
	~COMPtr()
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

	//! \since build 461
	_iCOM&
	operator*() const ynothrowv
	{
		return Deref(pInterface);
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

	/*!
	\throw COMException 转换失败。
	\since build 563
	*/
	//@{
	//! \since build 631
	COMPtr<IUnknown>
	As(const ::IID& riid) const
	{
		COMPtr<IUnknown> res;

		CheckHResult(Deref(pInterface).QueryInterface(riid,
			ystdex::replace_cast<void**>(&res.ReleaseAndGetRef())));
		return res;
	}
	template<class _iOther>
	COMPtr<_iOther>
	As() const
	{
		COMPtr<_iOther> res;

		CheckHResult(Deref(pInterface).QueryInterface(__uuidof(_iOther),
			ystdex::replace_cast<void**>(&res.ReleaseAndGetRef())));
		return res;
	}
	//@}

	//! \since build 631
	::HRESULT
	Cast(const ::IID& riid, COMPtr<IUnknown>& ptr) const ynothrow
	{
		return Deref(pInterface).QueryInterface(riid,
			ystdex::replace_cast<void**>(&ptr.ReleaseAndGetRef()));
	}
	template<class _iOther>
	::HRESULT
	Cast(COMPtr<_iOther>& ptr) const ynothrow
	{
		YAssertNonnull(pInterface);
		return pInterface->QueryInterface(__uuidof(_iOther),
			ystdex::replace_cast<void**>(&ptr.ReleaseAndGetRef()));
	}

	InterfaceType*
	Copy() const ynothrow
	{
		InternalAddRef();
		return pInterface;
	}
	//! \since build 631
	void*
	Copy(const ::IID& riid) const
	{
		void* p;

		CheckHResult(Deref(pInterface).QueryInterface(riid, &p));
		return p;
	}

	//! \since build 631
	::HRESULT
	CopyTo(const ::IID& riid, void** ptr) const ynothrow
	{
		return Deref(pInterface).QueryInterface(riid, ptr);
	}
	template<typename _type>
	::HRESULT
	CopyTo(_type*& p) const ynothrow
	{
		return pInterface->QueryInterface(__uuidof(_type),
			ystdex::replace_cast<void**>(&p));
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
			pInterface = {};
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

	//! \since build 710
	PDefH(void, swap, COMPtr& x, COMPtr& y) ynothrow
		ImplExpr(std::swap(x.pInterface, y.pInterface))
};

//! \relates COMPtr
//@{
template<class _iCOM1, class _iCOM2>
inline bool
operator==(const COMPtr<_iCOM1>& x, const COMPtr<_iCOM2>& y) ynothrow
{
	static_assert(ystdex::or_<std::is_base_of<_iCOM1, _iCOM2>,
		std::is_base_of<_iCOM1, _iCOM2>>(),
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
	static_assert(ystdex::or_<std::is_base_of<_iCOM1, _iCOM2>,
		std::is_base_of<_iCOM1, _iCOM2>>(),
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

		yunused(ref);
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
		ptr.GetRef() = {};
		n = tmp->Release();
	}
	return n;
}
//@}
//@}

} // namespace platform_ex;

#endif

