/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfocus.h
\ingroup Shell
\brief GUI 焦点特性实现。
\version 0.2119;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-01 13:52:56 + 08:00;
\par 修改时间:
	2010-12-17 19:29 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YFocus;
*/


#ifndef INCLUDED_YMODULE_H_
#define INCLUDED_YMODULE_H_

#include "yobject.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

class AFocusRequester;


//! \brief 简单焦点响应器。
class MSimpleFocusResponser
{
protected:
	IVisualControl* pFocusing; //!< 焦点指针。

	MSimpleFocusResponser();

public:
	/*!
	\brief 取焦点指针。
	*/
	DefGetter(IVisualControl*, FocusingPtr, pFocusing)
	/*!
	\brief 清除焦点指针。
	*/
	void
	ClearFocusingPtr();

	/*!
	\brief 响应焦点请求。
	*/
	bool
	ResponseFocusRequest(AFocusRequester&);

	/*!
	\brief 响应焦点释放。
	*/
	bool
	ResponseFocusRelease(AFocusRequester&);
};

inline MSimpleFocusResponser::MSimpleFocusResponser()
	: pFocusing(NULL)
{}


//! \brief 焦点响应器模板。
template<class _type = AFocusRequester>
class GMFocusResponser
{
	friend class AFocusRequester;

protected:
	_type* pFocusing; //!< 焦点对象指针。
	GContainer<_type> sFOs; //!< 焦点对象组。

	typedef typename GContainer<_type>::ContainerType FOs; \
		//!< 焦点对象组类型。

	/*!
	\brief 无参数构造。
	*/
	inline
	GMFocusResponser()
		: pFocusing(NULL), sFOs()
	{}

public:
	//! \brief 判断指定指针是否和焦点对象指针相等。
	inline PDefH(bool, IsFocusing, _type* p) const
		ImplRet(pFocusing == p)

	//! \brief 取焦点对象指针。
	inline DefGetter(_type*, FocusingPtr, pFocusing)
	//! \brief 取焦点对象组（只读）。
	inline DefGetter(const FOs&, FocusingSet, sFOs)

protected:
	/*!
	\brief 设置焦点对象指针。
	*/
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

	//! \brief 向焦点对象组添加焦点对象。
	inline PDefHOperator(void, +=, _type& c)
		ImplExpr(sFOs += c)
	//! \brief 从焦点对象组移除焦点对象。
	inline PDefHOperator(bool, -=, _type& c)
		ImplRet(sFOs -= c)

public:
	//! \brief 清空焦点指针。
	inline PDefH(bool, ClearFocusingPtr)
		ImplRet(SetFocusingPtr(NULL))
};


//! \brief 焦点申请器接口模板。
template<template<class> class _tResponser = GMFocusResponser,
	class _type = AFocusRequester>
DeclInterface(GIFocusRequester)
	DeclIEntry(bool IsFocused() const)
	DeclIEntry(bool IsFocusOfContainer(_tResponser<_type>&) const)

	DeclIEntry(bool CheckRemoval(_tResponser<_type>&) const)

	DeclIEntry(void ReleaseFocus(EventArgs&))
EndDecl


//! \brief 焦点申请器。
class AFocusRequester
	: implements GIFocusRequester<GMFocusResponser, AFocusRequester>
{
protected:
	bool bFocused; //!< 是否为所在容器的焦点。

public:
	/*!
	\brief 无参数构造。
	*/
	AFocusRequester();
	virtual DefEmptyDtor(AFocusRequester)

	/*!
	\brief 判断是否为获得焦点状态。
	*/
	ImplI(GIFocusRequester<GMFocusResponser, AFocusRequester>) bool
	IsFocused() const;
	/*!
	\brief 判断是否已在指定响应器中获得焦点。
	*/
	ImplI(GIFocusRequester<GMFocusResponser, AFocusRequester>) bool
	IsFocusOfContainer(GMFocusResponser<AFocusRequester>&) const;
	/*!
	\brief 判断是否已在指定响应器中获得焦点。
	*/
	template<template<class> class _tResponser, class _type>
	bool
	IsFocusOfContainer(_tResponser<_type>&) const;

	/*!
	\brief 判断是否已在指定响应器中获得焦点，若是则释放焦点。
	*/
	ImplI(GIFocusRequester<GMFocusResponser, AFocusRequester>) bool
	CheckRemoval(GMFocusResponser<AFocusRequester>&) const;
	/*!
	\brief 判断是否已在指定响应器中获得焦点，若是则释放焦点。
	*/
	template<template<class> class _tResponser, class _type>
	bool
	CheckRemoval(_tResponser<_type>&) const;

	/*!
	\brief 向指定响应器对应的容器申请获得焦点。
	*/
	bool
	RequestFocus(GMFocusResponser<AFocusRequester>&);
	/*!
	\brief 向指定响应器对应的容器申请获得焦点。
	*/
	template<template<class> class _tResponser, class _type>
	bool
	RequestFocus(_tResponser<_type>&);

	/*!
	\brief 释放焦点。
	*/
	ImplI(GIFocusRequester<GMFocusResponser, AFocusRequester>) bool
	ReleaseFocus(GMFocusResponser<AFocusRequester>&);
	/*!
	\brief 释放焦点。
	*/
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

