/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yfocus.h
\ingroup UI
\brief 图形用户界面焦点特性。
\version 0.2352;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-01 13:52:56 +0800;
\par 修改时间:
	2011-06-16 19:52 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YFocus;
*/


#ifndef YSL_INC_UI_YFOCUS_H_
#define YSL_INC_UI_YFOCUS_H_

#include "ycomp.h"
#include "../Core/ystatic.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

class AFocusRequester;

//! \brief 简单焦点响应器。
class MSimpleFocusResponser : public noncopyable
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
	: pFocusing()
{}


//! \brief 焦点响应器模板。
template<class _type>
class GMFocusResponser : public noncopyable
{
public:
	typedef set<_type*> FocusObjectSet; //!< 焦点对象组类型。

protected:
	_type* pFocusing; //!< 焦点对象指针。
	FocusObjectSet sFocusObjects; //!< 焦点对象组。

	/*!
	\brief 无参数构造。
	*/
	inline
	GMFocusResponser()
		: pFocusing(), sFocusObjects()
	{}

public:
	//! \brief 向焦点对象组添加焦点对象。
	inline PDefHOperator1(void, +=, _type& c)
		ImplRet(static_cast<void>(sFocusObjects.insert(&c)))

	/*!
	\brief 从焦点对象组移除焦点对象。
	\note 若为当前焦点，则先清除。
	*/
	bool operator-=(_type& c)
	{
		if(&c == pFocusing)
			ClearFocusingPtr();
		return sFocusObjects.erase(&c) != 0;
	}

	//! \brief 判断指定指针是否和焦点对象指针相等。
	inline PDefH1(bool, IsFocusing, _type* p) const
		ImplRet(pFocusing == p)

	//! \brief 取焦点对象指针。
	inline DefGetter(_type*, FocusingPtr, pFocusing)
	//! \brief 取焦点对象组（只读）。
	inline DefGetter(const FocusObjectSet&, FocusingSet, sFocusObjects)

	/*!
	\brief 设置焦点对象指针。
	*/
	bool
	SetFocusingPtr(_type* p)
	{
		if(!p)
			return (pFocusing = nullptr);
		if(sFocusObjects.find(p) == sFocusObjects.end())
			return false;
		if(pFocusing != p)
		{
			if(pFocusing && pFocusing->IsFocused())
				pFocusing->ReleaseFocus();
			pFocusing = p;
		}
		return pFocusing;
	}

	//! \brief 清空焦点指针。
	inline PDefH0(void, ClearFocusingPtr)
		ImplRet(static_cast<void>(SetFocusingPtr(nullptr)))
};


//! \brief 焦点申请器。
class AFocusRequester
{
public:
	DeclIEntry(~AFocusRequester())

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

ImplEmptyDtor(AFocusRequester)

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
	return IsFocusOfContainer(c) && !(c.SetFocusingPtr(nullptr));
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

