// YSLib::Core::YFunc by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-14 18:48:44 + 08:00;
// UTime = 2010-10-24 19:55 + 08:00;
// Version = 0.1682;


#ifndef INCLUDED_YFUNC_HPP_
#define INCLUDED_YFUNC_HPP_

// YFunc ：函数对象封装模块。

#include "ysdef.h"

YSL_BEGIN

//函数对象基类。
template<typename _tPointer>
class GHBase
{
private:
	mutable _tPointer _ptr; //指针。

protected:
	//********************************
	//名称:		GHBase
	//全名:		YSLib::GHBase<_tPointer>::GHBase
	//可访问性:	protected 
	//返回类型:	
	//修饰符:	
	//形式参数:	_tPointer pf
	//功能概要:	构造：使用指针。
	//备注:		
	//********************************
	inline explicit
	GHBase(_tPointer pf = NULL) : _ptr(pf)
	{}
	//********************************
	//名称:		GHBase
	//全名:		YSLib::GHBase<_tPointer>::GHBase<P>
	//可访问性:	protected 
	//返回类型:	
	//修饰符:	
	//形式参数:	GHBase<P> pf
	//功能概要:	构造：使用其它 GHBase 对象。
	//备注:		
	//********************************
	template<typename P>
	inline explicit
	GHBase(GHBase<P> pf) : _ptr(reinterpret_cast<_tPointer>(P(pf)))
	{}

public:
	//********************************
	//名称:		operator _tPointer
	//全名:		YSLib::GHBase::operator _tPointer
	//可访问性:	public 
	//返回类型:	
	//修饰符:	const
	//功能概要:	转换：指针。
	//备注:		
	//********************************
	inline
	operator _tPointer() const
	{
		return _ptr;
	}

	//********************************
	//名称:		empty
	//全名:		YSLib::GHBase::empty
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//功能概要:	判断是否为空指针。
	//备注:		
	//********************************
	inline bool
	empty() const
	{
		return _ptr == NULL;
	}
	//********************************
	//名称:		GetPtr
	//全名:		YSLib::GHBase::GetPtr
	//可访问性:	public 
	//返回类型:	_tPointer
	//修饰符:	const
	//功能概要:	取指针。
	//备注:		
	//********************************
	inline _tPointer
	GetPtr() const
	{
		return _ptr;
	}
};

YSL_END

#endif

