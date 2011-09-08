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
\version r2482;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-05-01 13:52:56 +0800;
\par 修改时间:
	2011-09-07 23:29 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YFocus;
*/


#ifndef YSL_INC_UI_YFOCUS_H_
#define YSL_INC_UI_YFOCUS_H_

#include "ycomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//! \brief 简单焦点响应器。
template<typename _type>
class GFocusResponser : public noncopyable
{
protected:
	_type* pFocusing; //!< 焦点对象指针。

public:
	virtual DefEmptyDtor(GFocusResponser)

	//! \brief 判断指定指针是否和焦点对象指针相等。
	inline PDefH1(bool, IsFocusing, _type* p) const
		ImplRet(pFocusing == p)

	/*!
	\brief 取焦点指针。
	*/
	DefGetter(_type*, FocusingPtr, pFocusing)

	/*!
	\brief 清除焦点指针。
	*/
	void
	ClearFocusingPtr()
	{
		pFocusing = nullptr;
	}

	/*!
	\brief 响应焦点请求。
	*/
	virtual bool
	ResponseFocusRequest(_type& obj)
	{
		pFocusing = &obj;
		return pFocusing;
	}

	/*!
	\brief 响应焦点释放。
	*/
	virtual bool
	ResponseFocusRelease(_type& obj)
	{
		if(pFocusing == &obj)
		{
			pFocusing = nullptr;
		//	w.ReleaseFocusRaw();
			return true;
		}
		return false;
	}
};


/*!
\brief 判断是否已在指定响应器中获得焦点。
*/
template<template<class> class _tResponser, class _type>
inline bool
IsFocusOfContainer(const _type& obj, _tResponser<_type>& rsp)
{
	return rsp.GetFocusingPtr() == static_cast<const _type*>(&obj);
}

/*!
\brief 向指定响应器对应的容器申请获得焦点。
*/
template<template<class> class _tResponser, class _type>
bool
RequestFocusOf(_type& obj, _tResponser<_type>& rsp)
{
	return !IsFocusOfContainer(obj, rsp)
		&& rsp.SetFocusingPtr(static_cast<_type*>(&obj));
}

/*!
\brief 释放焦点。
*/
template<template<class> class _tResponser, class _type>
bool
ReleaseFocusOf(_type& obj, _tResponser<_type>& rsp)
{
	return IsFocusOfContainer(obj, rsp) && !(rsp.SetFocusingPtr(nullptr));
}


//! \brief 焦点响应器模板。
template<class _type>
class GCheckedFocusResponser : public GFocusResponser<_type>
{
protected:
	using GFocusResponser<_type>::pFocusing;

public:
	/*!
	\brief 设置焦点对象指针。
	*/
	bool
	SetFocusingPtr(_type* p)
	{
		if(!p)
			return (pFocusing = nullptr);
		if(pFocusing != p)
		{
			if(pFocusing && IsFocused(*pFocusing))
				ReleaseFocusFrom(*pFocusing, *p);
			pFocusing = p;
		}
		return pFocusing;
	}

	/*!
	\brief 响应焦点请求。
	*/
	virtual bool
	ResponseFocusRequest(IWidget& wgt)
	{
		return RequestFocusOf<GCheckedFocusResponser, IWidget>(wgt, *this);
	}

	/*!
	\brief 响应焦点释放。
	*/
	virtual bool
	ResponseFocusRelease(IWidget& wgt)
	{
		return ReleaseFocusOf<GCheckedFocusResponser, IWidget>(wgt, *this);
	}
};

typedef GFocusResponser<IWidget> FocusResponser;
typedef GCheckedFocusResponser<IWidget> CheckedFocusResponser;

YSL_END_NAMESPACE(Components)

YSL_END

#endif

