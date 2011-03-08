/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfocus.h
\ingroup Shell
\brief GUI 焦点特性实现。
\version 0.2283;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-01 13:52:56 +0800;
\par 修改时间:
	2011-03-07 10:28 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Shell::YFocus;
*/


#ifndef INCLUDED_YMODULE_H_
#define INCLUDED_YMODULE_H_

#include "../Core/ysdef.h"
#include "../Core/ycutil.h"
#include "../Core/yobject.h"
#include "ycomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

class AFocusRequester;

//! \brief 简单焦点响应器。
class MSimpleFocusResponser : public NonCopyable
{
protected:
	IControl* pFocusing; //!< 焦点指针。

	MSimpleFocusResponser();

public:
	/*!
	\brief 取焦点指针。
	*/
	DefGetter(IControl*, FocusingPtr, pFocusing)
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
template<class _type>
class GMFocusResponser : public NonCopyable
{
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
	inline PDefH1(bool, IsFocusing, _type* p) const
		ImplRet(pFocusing == p)

	//! \brief 取焦点对象指针。
	inline DefGetter(_type*, FocusingPtr, pFocusing)
	//! \brief 取焦点对象组（只读）。
	inline DefGetter(const FOs&, FocusingSet, sFOs)

	/*!
	\brief 设置焦点对象指针。
	*/
	bool
	SetFocusingPtr(_type* p)
	{
		if(!p)
			return (pFocusing = NULL);
		if(sFOs.find(p) == sFOs.end())
			return false;
		if(pFocusing != p)
		{
			if(pFocusing && pFocusing->IsFocused())
				pFocusing->ReleaseFocus(GetStaticRef<EventArgs>());
			pFocusing = p;
		}
		return pFocusing;
	}

	//! \brief 向焦点对象组添加焦点对象。
	inline PDefHOperator1(void, +=, _type& c)
		ImplRet(sFOs += c)
	//! \brief 从焦点对象组移除焦点对象。
	inline PDefHOperator1(bool, -=, _type& c)
		ImplRet(sFOs -= c)

	//! \brief 清空焦点指针。
	inline PDefH0(void, ClearFocusingPtr)
		ImplRet(static_cast<void>(SetFocusingPtr(NULL)))
};


//! \brief 焦点申请器。
class AFocusRequester
{
public:
	/*!
	\brief 无参数构造。
	*/
	AFocusRequester();
	virtual DefEmptyDtor(AFocusRequester)

	/*!
	\brief 判断是否已在指定响应器中获得焦点。
	*/
	template<template<class> class _tResponser, class _type>
	bool
	IsFocusOfContainer(_tResponser<_type>&) const;

	/*!
	\brief 向指定响应器对应的容器申请获得焦点。
	*/
	template<template<class> class _tResponser, class _type>
	bool
	RequestFocus(_tResponser<_type>&);

	/*!
	\brief 释放焦点。
	*/
	template<template<class> class _tResponser, class _type>
	bool
	ReleaseFocus(_tResponser<_type>&);
};

inline
AFocusRequester::AFocusRequester()
{}

template<template<class> class _tResponser, class _type>
inline bool
AFocusRequester::IsFocusOfContainer(_tResponser<_type>& c) const
{
	return c.GetFocusingPtr() == dynamic_cast<const _type*>(this);
}

template<template<class> class _tResponser, class _type>
bool
AFocusRequester::RequestFocus(_tResponser<_type>& c)
{
	return !(IsFocusOfContainer(c))
		&& c.SetFocusingPtr(dynamic_cast<_type*>(this));
}

template<template<class> class _tResponser, class _type>
bool
AFocusRequester::ReleaseFocus(_tResponser<_type>& c)
{
	return IsFocusOfContainer(c) && !(c.SetFocusingPtr(NULL));
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

