// YSLib::Adapter::YRefrence by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-21 23:09:06;
// UTime = 2010-9-19 18:26;
// Version = 0.2159;


#ifndef INCLUDED_YREF_HPP_
#define INCLUDED_YREF_HPP_

// YRefrence ：用于提供指针和引用访问的间接访问类模块。

#include "yadapter.h"

namespace YSLib
{

template<class P>
struct YSP_OP_Simple
{
	enum
	{
		destructiveCopy = false
	};

	YSP_OP_Simple()
	{}

	template<class P1>
	YSP_OP_Simple(const YSP_OP_Simple<P1>& rhs)
	{}

	static P
	Clone(const P& val)
	{
		return val;
	}

	bool
	Release(const P&)
	{
		return false;
	}

	static void
	Swap(YSP_OP_Simple&)
	{}
};

template<class P>
class YSP_OP_RefCounted
{
private:
	uintptr_t* pCount_;

protected:
	void
	AddRef()
	{
		++*pCount_;
	}

public:
	enum { destructiveCopy = false };

	YSP_OP_RefCounted()
	: pCount_(static_cast<uintptr_t*>(SmallObject<>::operator new(sizeof(uintptr_t))))
	{
	//	assert(pCount_!=0);
		*pCount_ = 1;
	}

	YSP_OP_RefCounted(const YSP_OP_RefCounted& rhs)
		: pCount_(rhs.pCount_)
	{}

	template<typename P1>
	YSP_OP_RefCounted(const YSP_OP_RefCounted<P1>& rhs)
		: pCount_(reinterpret_cast<const YSP_OP_RefCounted&>(rhs).pCount_)
	{}

	P
	Clone(const P& val)
	{
		AddRef();
		return val;
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
	Swap(YSP_OP_RefCounted& rhs)
	{
		std::swap(pCount_, rhs.pCount_);
	}
};


template<typename T,
	template<class> class OP = YSP_OP_RefCounted,
	class CP = AllowConversion,
	template<class> class KP = RejectNull,
	template<class> class SP = DefaultSPStorage,
	typename SPType = SmartPtr<T, OP, CP, KP, SP> >
class GHResource : public SPType //资源指针类。
{
public:
	GHResource(T* p = NULL) : SPType(p) {};
	GHResource(T& rhs) : SPType(rhs) {}
	GHResource(RefToValue<GHResource> rhs) : SPType(rhs) {}

	operator RefToValue<GHResource>()
	{
		return RefToValue<GHResource>(*this);
	}

	T*
	operator->() const
	{
		return GHResource<T>::GetPointer(*this);
	}

	GHResource&
	operator=(T& rhs)
	{
		GHResource temp(rhs);
		temp.Swap(*this);
		return *this;
	}

	T*
	GetPtr() const
	{
		return operator->();
	}
	void
	Swap(GHResource& rhs)
	{
		Swap(rhs);
	}
};


template<class _Tp>
inline _Tp*
GetPointer(GHResource<_Tp> h)
{
	return h.GetPtr();
}


//#ifdef YSL_HANDLEPOLICY_SIMPLE
#	define YHandleOP YSP_OP_Simple
//句柄不会被自动回收，需要手动释放。
#	define YDelete(h) delete h
/*#else
#	define YHandleOP YSP_OP_RefCounted
#	define YDelete(h)
#endif*/


template<typename T,
	template<class> class OP = YHandleOP,
	class CP = AllowConversion,
	template<class> class KP = RejectNull,
	template<class> class SP = DefaultSPStorage,
	typename SPType = SmartPtr<T, OP, CP, KP, SP> >
class GHHandle : public SPType //句柄类。
{
public:
	GHHandle(T* p = NULL) : SPType(p) {}
	GHHandle(T& rhs) : SPType(rhs) {}
	GHHandle(RefToValue<GHHandle> rhs) : SPType(rhs) {}
	template<class C>
	explicit
	GHHandle(GHHandle<C>& h) : SPType(static_cast<T*>(GHHandle<C>::GetPointer(h))) {}

	operator RefToValue<GHHandle>()
	{
		return RefToValue<GHHandle>(*this);
	}

	T*
	operator->() const
	{
		return GHHandle<T>::GetPointer(*this);
	}

	GHHandle&
	operator=(T& rhs)
	{
		GHHandle temp(rhs);
		temp.Swap(*this);
		return *this;
	}

	T*
	GetPtr() const
	{
		return operator->();
	}
	void
	Swap(GHHandle& rhs)
	{
		Swap(rhs);
	}
};


#undef YHandleOP


#ifdef YSL_USE_SIMPLE_HANDLE

template<class _Tp>
inline _Tp*
GetPointer(_Tp* h)
{
	return h;
}

#else

template<typename _tReference>
inline _tReference*
GetPointer(GHHandle<_tReference> h)
{
	return h.GetPtr();
}

template<typename _type, typename _tReference>
inline _type
handle_cast(GHHandle<_tReference> h)
{
	return reinterpret_cast<_type>(GHHandle<_tReference>::GetPointer(h));
}

#endif

}

#endif

