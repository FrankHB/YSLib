// YSLib::Adaptor::YReference by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-03-21 23:09:06 + 08:00;
// UTime = 2010-10-24 19:56 + 08:00;
// Version = 0.2347;


#ifndef INCLUDED_YREF_HPP_
#define INCLUDED_YREF_HPP_

// YReference ：用于提供指针和引用访问的间接访问类模块。

#include "yadaptor.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Design)

YSL_BEGIN_NAMESPACE(Policies)

YSL_BEGIN_NAMESPACE(Operations)

//智能指针简单操作策略。
template<class P>
struct SmartPtr_Simple
{
	enum
	{
		destructiveCopy = false
	};

	//********************************
	//名称:		SmartPtr_Simple
	//全名:		YSLib::Design::Policies::Operations
	//			::SmartPtr_Simple<P>::SmartPtr_Simple
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	构造。
	//备注:		
	//********************************
	SmartPtr_Simple()
	{}

	//********************************
	//名称:		SmartPtr_Simple
	//全名:		YSLib::Design::Policies::Operations
	//			::SmartPtr_Simple<P1>::SmartPtr_Simple
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const SmartPtr_Simple<P1> & rhs
	//功能概要:	复制构造。
	//备注:		
	//********************************
	template<class P1>
	SmartPtr_Simple(const SmartPtr_Simple<P1>& rhs)
	{}

	//********************************
	//名称:		Clone
	//全名:		YSLib::Design::Policies::Operations
	//			::SmartPtr_Simple<P>::Clone
	//可访问性:	public static 
	//返回类型:	P
	//修饰符:	
	//形式参数:	const P & val
	//功能概要:	复制。
	//备注:		
	//********************************
	static P
	Clone(const P& val)
	{
		return val;
	}

	//********************************
	//名称:		Release
	//全名:		YSLib::Design::Policies::Operations
	//			::SmartPtr_Simple<P>::Release
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	const P &
	//功能概要:	释放。
	//备注:		
	//********************************
	bool
	Release(const P&)
	{
		return false;
	}

	//********************************
	//名称:		Swap
	//全名:		YSLib::Design::Policies::Operations
	//			::SmartPtr_Simple<P>::Swap
	//可访问性:	public static 
	//返回类型:	void
	//修饰符:	
	//形式参数:	SmartPtr_Simple &
	//功能概要:	交换。
	//备注:		
	//********************************
	static void
	Swap(SmartPtr_Simple&)
	{}
};

//智能指针引用计数操作策略。
template<class P>
class SmartPtr_RefCounted
{
private:
	uintptr_t* pCount_;

protected:
	//********************************
	//名称:		AddRef
	//全名:		YSLib::Design::Policies::Operations
	//			::SmartPtr_RefCounted<P>::AddRef
	//可访问性:	protected 
	//返回类型:	void
	//修饰符:	
	//功能概要:	增加引用计数。
	//备注:		
	//********************************
	void
	AddRef()
	{
		++*pCount_;
	}

public:
	enum { destructiveCopy = false };

	//********************************
	//名称:		SmartPtr_RefCounted
	//全名:		YSLib::Design::Policies::Operations
	//			::SmartPtr_RefCounted<P>::SmartPtr_RefCounted
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	构造。
	//备注:		
	//********************************
	SmartPtr_RefCounted()
		: pCount_(static_cast<uintptr_t*>(
		SmallObject<>::operator new(sizeof(uintptr_t))))
	{
	//	assert(pCount_!=0);
		*pCount_ = 1;
	}

	//********************************
	//名称:		SmartPtr_RefCounted
	//全名:		YSLib::Design::Policies::Operations
	//			::SmartPtr_RefCounted<P>::SmartPtr_RefCounted
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const SmartPtr_RefCounted & rhs
	//功能概要:	复制构造。
	//备注:		
	//********************************
	SmartPtr_RefCounted(const SmartPtr_RefCounted& rhs)
		: pCount_(rhs.pCount_)
	{}

	//********************************
	//名称:		SmartPtr_RefCounted
	//全名:		YSLib::Design::Policies::Operations
	//			::SmartPtr_RefCounted<P1>::SmartPtr_RefCounted
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const SmartPtr_RefCounted<P1> & rhs
	//功能概要:	复制构造。
	//备注:		
	//********************************
	template<typename P1>
	SmartPtr_RefCounted(const SmartPtr_RefCounted<P1>& rhs)
		: pCount_(reinterpret_cast<const SmartPtr_RefCounted&>(rhs).pCount_)
	{}

	//********************************
	//名称:		Clone
	//全名:		YSLib::Design::Policies::Operations
	//			::SmartPtr_RefCounted<P>::Clone
	//可访问性:	public 
	//返回类型:	P
	//修饰符:	
	//形式参数:	const P & val
	//功能概要:	复制。
	//备注:		
	//********************************
	P
	Clone(const P& val)
	{
		AddRef();
		return val;
	}

	//********************************
	//名称:		Release
	//全名:		YSLib::Design::Policies::Operations
	//			::SmartPtr_RefCounted<P>::Release
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	const P &
	//功能概要:	释放引用计数。
	//备注:		
	//********************************
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

	//********************************
	//名称:		Swap
	//全名:		YSLib::Design::Policies::Operations
	//			::SmartPtr_RefCounted<P>::Swap
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	SmartPtr_RefCounted & rhs
	//功能概要:	交换。
	//备注:		
	//********************************
	void
	Swap(SmartPtr_RefCounted& rhs)
	{
		std::swap(pCount_, rhs.pCount_);
	}
};

YSL_END_NAMESPACE(Operations)

YSL_END_NAMESPACE(Policies)

YSL_END_NAMESPACE(Design)


template<typename T,
	template<class> class OP = Design::Policies::Operations::SmartPtr_RefCounted,
	class CP = AllowConversion,
	template<class> class KP = RejectNull,
	template<class> class SP = DefaultSPStorage,
	typename SPType = SmartPtr<T, OP, CP, KP, SP> >
class GHResource : public SPType //资源指针类。
{
public:
	//********************************
	//名称:		GHResource
	//全名:		YSLib::GHResource<T, OP, CP, KP, SP, SPType>::GHResource
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	T * p
	//功能概要:	构造：使用内建指针。
	//备注:		
	//********************************
	GHResource(T* p = NULL) : SPType(p) {};
	//********************************
	//名称:		GHResource
	//全名:		YSLib::GHResource<T, OP, CP, KP, SP, SPType>::GHResource
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	T & rhs
	//功能概要:	构造：使用对象引用。
	//备注:		
	//********************************
	GHResource(T& rhs) : SPType(rhs) {}
	//********************************
	//名称:		GHResource
	//全名:		YSLib::GHResource<T, OP, CP, KP, SP, SPType>::GHResource
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	RefToValue<GHResource> rhs
	//功能概要:	构造：使用智能指针引用。
	//备注:		
	//********************************
	GHResource(RefToValue<GHResource> rhs) : SPType(rhs) {}

	//********************************
	//名称:		operator RefToValue<GHResource>
	//全名:		YSLib::GHResource<T, OP, CP, KP, SP, SPType>
	//			::operator RefToValue<GHResource>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	转换：智能指针引用。
	//备注:		
	//********************************
	operator RefToValue<GHResource>()
	{
		return RefToValue<GHResource>(*this);
	}

	//********************************
	//名称:		operator->
	//全名:		YSLib::GHResource<T, OP, CP, KP, SP, SPType>::operator->
	//可访问性:	public 
	//返回类型:	T*
	//修饰符:	const
	//功能概要:	取内建指针。
	//备注:		
	//********************************
	T*
	operator->() const
	{
		return GHResource<T>::GetPointer(*this);
	}

	//********************************
	//名称:		operator=
	//全名:		YSLib::GHResource<T, OP, CP, KP, SP, SPType>::operator=
	//可访问性:	public 
	//返回类型:	GHResource&
	//修饰符:	
	//形式参数:	T & rhs
	//功能概要:	赋值复制。
	//备注:		
	//********************************
	GHResource&
	operator=(T& rhs)
	{
		GHResource temp(rhs);
		temp.Swap(*this);
		return *this;
	}

	//********************************
	//名称:		GetPtr
	//全名:		YSLib::GHResource<T, OP, CP, KP, SP, SPType>::GetPtr
	//可访问性:	public 
	//返回类型:	T*
	//修饰符:	const
	//功能概要:	取内建指针。
	//备注:		
	//********************************
	T*
	GetPtr() const
	{
		return operator->();
	}

	//********************************
	//名称:		Swap
	//全名:		YSLib::GHResource<T, OP, CP, KP, SP, SPType>::Swap
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	GHResource & rhs
	//功能概要:	交换。
	//备注:		
	//********************************
	void
	Swap(GHResource& rhs)
	{
		Swap(rhs);
	}
};


//********************************
//名称:		GetPointer
//全名:		YSLib<_type>::GetPointer
//可访问性:	public 
//返回类型:	_type*
//修饰符:	
//形式参数:	GHResource<_type> h
//功能概要:	取内建指针。
//备注:		
//********************************
template<class _type>
inline _type*
GetPointer(GHResource<_type> h)
{
	return h.GetPtr();
}


#define YHandleOP Design::Policies::Operations::SmartPtr_Simple
//句柄不会被自动回收，需要手动释放。
#define YDelete(h) delete h


template<typename T,
	template<class> class OP = YHandleOP,
	class CP = AllowConversion,
	template<class> class KP = RejectNull,
	template<class> class SP = DefaultSPStorage,
	typename SPType = SmartPtr<T, OP, CP, KP, SP> >
class GHHandle : public SPType //句柄类。
{
public:
	//********************************
	//名称:		GHHandle
	//全名:		YSLib::GHHandle<T, OP, CP, KP, SP, SPType>::GHHandle
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	T * p
	//功能概要:	构造：使用内建指针。
	//备注:		
	//********************************
	GHHandle(T* p = NULL) : SPType(p) {}
	//********************************
	//名称:		GHHandle
	//全名:		YSLib::GHHandle<T, OP, CP, KP, SP, SPType>::GHHandle
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	T & rhs
	//功能概要:	构造：使用对象引用。
	//备注:		
	//********************************
	GHHandle(T& rhs) : SPType(rhs) {}
	//********************************
	//名称:		GHHandle
	//全名:		YSLib::GHHandle<T, OP, CP, KP, SP, SPType>::GHHandle
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	RefToValue<GHHandle> rhs
	//功能概要:	构造：使用句柄引用。
	//备注:		
	//********************************
	GHHandle(RefToValue<GHHandle> rhs) : SPType(rhs) {}
	//********************************
	//名称:		GHHandle
	//全名:		YSLib::GHHandle<C>::GHHandle
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	GHHandle<C> & h
	//功能概要:	构造：使用其它类型句柄引用。
	//备注:		
	//********************************
	template<class C>
	explicit
	GHHandle(GHHandle<C>& h) : SPType(static_cast<T*>(GHHandle<C>::GetPointer(h))) {}

	//********************************
	//名称:		operator RefToValue<GHHandle>
	//全名:		YSLib::GHHandle<T, OP, CP, KP, SP, SPType>
	//			::operator RefToValue<GHHandle>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	转换：句柄引用。
	//备注:		
	//********************************
	operator RefToValue<GHHandle>()
	{
		return RefToValue<GHHandle>(*this);
	}

	//********************************
	//名称:		operator->
	//全名:		YSLib::GHHandle<T, OP, CP, KP, SP, SPType>::operator->
	//可访问性:	public 
	//返回类型:	T*
	//修饰符:	const
	//功能概要:	取内建指针。
	//备注:		
	//********************************
	T*
	operator->() const
	{
		return GHHandle<T>::GetPointer(*this);
	}

	//********************************
	//名称:		operator=
	//全名:		YSLib::GHHandle<T, OP, CP, KP, SP, SPType>::operator=
	//可访问性:	public 
	//返回类型:	GHHandle&
	//修饰符:	
	//形式参数:	T & rhs
	//功能概要:	赋值复制。
	//备注:		
	//********************************
	GHHandle&
	operator=(T& rhs)
	{
		GHHandle temp(rhs);
		temp.Swap(*this);
		return *this;
	}

	//********************************
	//名称:		GetPtr
	//全名:		YSLib::GHHandle<T, OP, CP, KP, SP, SPType>::GetPtr
	//可访问性:	public 
	//返回类型:	T*
	//修饰符:	const
	//功能概要:	取内建指针。
	//备注:		
	//********************************
	T*
	GetPtr() const
	{
		return operator->();
	}

	//********************************
	//名称:		Swap
	//全名:		YSLib::GHHandle<T, OP, CP, KP, SP, SPType>::Swap
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	GHHandle & rhs
	//功能概要:	交换。
	//备注:		
	//********************************
	void
	Swap(GHHandle& rhs)
	{
		Swap(rhs);
	}
};


#undef YHandleOP


//********************************
//名称:		GetPointer
//全名:		YSLib<_type>::GetPointer
//可访问性:	public 
//返回类型:	_type*
//修饰符:	
//形式参数:	_type * h
//功能概要:	取内建指针。
//备注:		
//********************************
template<typename _type>
inline _type*
GetPointer(_type* h)
{
	return h;
}

#ifndef YSL_USE_SIMPLE_HANDLE

//********************************
//名称:		GetPointer
//全名:		YSLib<_tReference>::GetPointer
//可访问性:	public 
//返回类型:	_tReference*
//修饰符:	
//形式参数:	GHHandle<_tReference> h
//功能概要:	取内建指针。
//备注:		
//********************************
template<typename _tReference>
inline _tReference*
GetPointer(GHHandle<_tReference> h)
{
	return h.GetPtr();
}

//********************************
//名称:		handle_cast
//全名:		YSLib<_type, _tReference>::handle_cast
//可访问性:	public 
//返回类型:	_type
//修饰符:	
//形式参数:	GHHandle<_tReference> h
//功能概要:	转换：指定类型句柄。
//备注:		
//********************************
template<typename _type, typename _tReference>
inline _type
handle_cast(GHHandle<_tReference> h)
{
	return reinterpret_cast<_type>(GHHandle<_tReference>::GetPointer(h));
}

#endif

YSL_END

#endif

