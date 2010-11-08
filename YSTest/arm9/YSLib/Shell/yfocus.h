// YSLib::Shell::YFocus by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-05-01 13:52:56 + 08:00;
// UTime = 2010-11-08 18:07 + 08:00;
// Version = 0.2034;


#ifndef INCLUDED_YMODULE_H_
#define INCLUDED_YMODULE_H_

// YFocus ： GUI 焦点特性实现。

#include "yobject.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

class AFocusRequester;


//焦点响应器模板。
template<class _type = AFocusRequester>
class GMFocusResponser// : implements GIContainer<_type>
{
	friend class AFocusRequester;

protected:
	_type* pFocusing; //焦点对象指针。
	GContainer<_type> sFOs; //焦点对象组。

	typedef typename GContainer<_type>::ContainerType FOs; \
		//焦点对象组类型。

	//********************************
	//名称:		GMFocusResponser
	//全名:		YSLib::Components::GMFocusResponser<_type>::GMFocusResponser
	//可访问性:	protected 
	//返回类型:	
	//修饰符:	
	//功能概要:	无参数构造。
	//备注:		
	//********************************
	inline
	GMFocusResponser()
	: pFocusing(NULL), sFOs()
	{}

public:
	//判断指定指针是否和焦点对象指针相等。
	inline PDefH(bool, IsFocusing, _type* p) const
		ImplRet(pFocusing == p)

	//取焦点对象指针。
	inline DefGetter(_type*, FocusingPtr, pFocusing)
	//取焦点对象组（只读）。
	inline DefGetter(const FOs&, FocusingSet, sFOs)

protected:
	//********************************
	//名称:		SetFocusingPtr
	//全名:		YSLib::Components::GMFocusResponser<_type>::SetFocusingPtr
	//可访问性:	protected 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	_type * p
	//功能概要:	设置焦点对象指针。
	//备注:		
	//********************************
	bool
	SetFocusingPtr(_type* p)
	{
		if(p && sFOs.find(p) == sFOs.end())
			return false;
		if(pFocusing != p)
		{
			EventArgs e;

			if(pFocusing && pFocusing->IsFocused())
				pFocusing->ReleaseFocus(e);
			pFocusing = p;
		}
		return pFocusing;
	}

	//向焦点对象组添加焦点对象。
	inline PDefHOperator(void, +=, _type& c)
		ImplExpr(sFOs += c)
	//从焦点对象组移除焦点对象。
	inline PDefHOperator(bool, -=, _type& c)
		ImplRet(sFOs -= c)

public:
	//清空焦点指针。
	inline PDefH(bool, ClearFocusingPtr)
		ImplRet(SetFocusingPtr(NULL))
};


//焦点申请器接口模板。
template<template<class> class _tResponser = GMFocusResponser,
	class _type = AFocusRequester>
DeclInterface(GIFocusRequester)
	DeclIEntry(bool IsFocused() const)
	DeclIEntry(bool IsFocusOfContainer(_tResponser<_type>&) const)

	DeclIEntry(bool CheckRemoval(_tResponser<_type>&) const)

	DeclIEntry(void ReleaseFocus(EventArgs&))
EndDecl


//焦点申请器。
class AFocusRequester : implements GIFocusRequester<GMFocusResponser, AFocusRequester>
{
protected:
	bool bFocused; //是否为所在容器的焦点。

public:
	//********************************
	//名称:		AFocusRequester
	//全名:		YSLib::Components::AFocusRequester::AFocusRequester
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	无参数构造。
	//备注:		
	//********************************
	AFocusRequester();
	virtual DefEmptyDtor(AFocusRequester)

	//********************************
	//名称:		IsFocused
	//全名:		YSLib::Components::AFocusRequester::IsFocused
	//可访问性:	ImplI(GIFocusRequester<GMFocusResponser, AFocusRequester>)
	//			public 
	//返回类型:	bool
	//修饰符:	const
	//功能概要:	判断是否为获得焦点状态。
	//备注:		
	//********************************
	ImplI(GIFocusRequester<GMFocusResponser, AFocusRequester>) bool
	IsFocused() const;
	//********************************
	//名称:		IsFocusOfContainer
	//全名:		YSLib::Components::AFocusRequester::IsFocusOfContainer
	//可访问性:	ImplI(GIFocusRequester<GMFocusResponser, AFocusRequester>)
	//			public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	GMFocusResponser<AFocusRequester> &
	//功能概要:	判断是否已在指定响应器中获得焦点。
	//备注:		
	//********************************
	ImplI(GIFocusRequester<GMFocusResponser, AFocusRequester>) bool
	IsFocusOfContainer(GMFocusResponser<AFocusRequester>&) const;
	//********************************
	//名称:		IsFocusOfContainer
	//全名:		YSLib::Components::AFocusRequester
	//				::IsFocusOfContainer<_tResponser, _type>
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	_tResponser<_type> &
	//功能概要:	判断是否已在指定响应器中获得焦点。
	//备注:		
	//********************************
	template<template<class> class _tResponser, class _type>
	bool
	IsFocusOfContainer(_tResponser<_type>&) const;

	//********************************
	//名称:		CheckRemoval
	//全名:		YSLib::Components::AFocusRequester::CheckRemoval
	//可访问性:	ImplI(GIFocusRequester<GMFocusResponser, AFocusRequester>)
	//			public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	GMFocusResponser<AFocusRequester> &
	//功能概要:	判断是否已在指定响应器中获得焦点，若是则释放焦点。
	//备注:		
	//********************************
	ImplI(GIFocusRequester<GMFocusResponser, AFocusRequester>) bool
	CheckRemoval(GMFocusResponser<AFocusRequester>&) const;
	//********************************
	//名称:		CheckRemoval
	//全名:		YSLib::Components::AFocusRequester
	//				::CheckRemoval<_tResponser, _type>
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	_tResponser<_type> &
	//功能概要:	判断是否已在指定响应器中获得焦点，若是则释放焦点。
	//备注:		
	//********************************
	template<template<class> class _tResponser, class _type>
	bool
	CheckRemoval(_tResponser<_type>&) const;

	//********************************
	//名称:		RequestFocus
	//全名:		YSLib::Components::AFocusRequester::RequestFocus
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	GMFocusResponser<AFocusRequester> &
	//功能概要:	向指定响应器对应的容器申请获得焦点。
	//备注:		
	//********************************
	bool
	RequestFocus(GMFocusResponser<AFocusRequester>&);
	//********************************
	//名称:		RequestFocus
	//全名:		YSLib::Components::AFocusRequester
	//				::RequestFocus<_tResponser, _type>
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	_tResponser<_type> &
	//功能概要:	向指定响应器对应的容器申请获得焦点。
	//备注:		
	//********************************
	template<template<class> class _tResponser, class _type>
	bool
	RequestFocus(_tResponser<_type>&);

	//********************************
	//名称:		ReleaseFocus
	//全名:		YSLib::Components::AFocusRequester::ReleaseFocus
	//可访问性:	ImplI(GIFocusRequester<GMFocusResponser, AFocusRequester>)
	//			public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	GMFocusResponser<AFocusRequester> &
	//功能概要:	释放焦点。
	//备注:		
	//********************************
	ImplI(GIFocusRequester<GMFocusResponser, AFocusRequester>) bool
	ReleaseFocus(GMFocusResponser<AFocusRequester>&);
	//********************************
	//名称:		ReleaseFocus
	//全名:		YSLib::Components::AFocusRequester
	//				::ReleaseFocus<_tResponser, _type>
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	_tResponser<_type> &
	//功能概要:	释放焦点。
	//备注:		
	//********************************
	template<template<class> class _tResponser, class _type>
	bool
	ReleaseFocus(_tResponser<_type>&);

	ImplA(GIFocusRequester<GMFocusResponser, AFocusRequester>)
	DeclIEntry(void ReleaseFocus(EventArgs&))
};

inline
AFocusRequester::AFocusRequester()
	: bFocused(false)
{}

inline bool
AFocusRequester::IsFocused() const
{
	return bFocused;
}

inline bool
AFocusRequester::IsFocusOfContainer(GMFocusResponser<AFocusRequester>& c) const
{
	return c.GetFocusingPtr() == this;
}
template<template<class> class _tResponser, class _type>
inline bool
AFocusRequester::IsFocusOfContainer(_tResponser<_type>& c) const
{
	return c.GetFocusingPtr() == dynamic_cast<const _type*>(this);
}

template<template<class> class _tResponser, class _type>
bool
AFocusRequester::CheckRemoval(_tResponser<_type>& c) const
{
	if(IsFocusOfContainer(c))
	{
		c.ClearFocusingPtr();
		return true;
	}
	return false;
}

template<template<class> class _tResponser, class _type>
bool
AFocusRequester::RequestFocus(_tResponser<_type>& c)
{
	return !(bFocused && IsFocusOfContainer(c))
		&& (bFocused = c.SetFocusingPtr(dynamic_cast<_type*>(this)));
}

template<template<class> class _tResponser, class _type>
bool
AFocusRequester::ReleaseFocus(_tResponser<_type>& c)
{
	return bFocused && IsFocusOfContainer(c)
		&& (bFocused = false, !(c.ClearFocusingPtr()));
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

