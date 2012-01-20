/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShellHelper.h
\ingroup Helper
\brief Shell 助手模块。
\version r2238;
\author FrankHB<frankhb1989@gmail.com>
\since build 278 。
\par 创建时间:
	2010-03-14 14:07:22 +0800;
\par 修改时间:
	2012-01-20 09:29 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Helper::ShellHelper;
*/


#ifndef YSL_INC_HELPER_SHELLHELPER_H_
#define YSL_INC_HELPER_SHELLHELPER_H_

#include "../../ysbuild.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 序列设置仿函数。
\since build 278 。
*/
struct ContainerSetter
{
public:
	IWidget& Container;

	yconstfn
	ContainerSetter(IWidget& con)
		: Container(con)
	{}

	void
	operator()(IWidget& wgt)
	{
		SetContainerPtrOf(wgt, &Container);
	}
};


/*!
\brief 子部件绘制仿函数。
\since build 278 。
*/
struct ChildPainter
{
public:
	const PaintContext& Context;

	yconstfn
	ChildPainter(const PaintContext& pc)
		: Context(pc)
	{}

	void
	operator()(IWidget& wgt)
	{
		PaintChild(wgt, Context);
	}
};

YSL_END_NAMESPACE(Components)


/*!
\ingroup HelperFunctions
*/
//@{

//句柄语法糖。

/*!
\brief 句柄转换：对象引用。
\since 早于 build 132 。
*/
template<class _type, class _tHandle>
inline _type&
HandleToReference(_tHandle h) ythrow(std::bad_cast)
{
	_type* _tmp(dynamic_cast<_type*>(raw(h)));

	if(!_tmp)
		throw std::bad_cast();
	return *_tmp;
}

/*!
\brief 取指定 Shell 句柄对应的 Shell 引用 。
\since build 205 。
*/
template<class _tShell>
inline _tShell&
FetchShell()
{
	auto hShl(FetchShellHandle());

	YAssert(bool(hShl), "Null handle found @ FetchShell;");

	return dynamic_cast<_tShell&>(*hShl);
}


//全局函数。

/*!
\brief 取全局 Shell 句柄。
\since build 195 。
*/
template<class _tShl>
inline shared_ptr<Shell>
FetchStored()
{
	return GLocalStaticCache<_tShl, shared_ptr<Shell>>::GetPointer();
}

/*!
\brief 释放全局 Shell 。
\since build 195 。
*/
template<class _tShl>
inline void
ReleaseStored()
{
	GLocalStaticCache<_tShl, shared_ptr<Shell>>::Release();
}


/*!
\brief 判断句柄指定的 Shell 是否为当前线程空间中运行的 Shell 。
\since 早于 build 132 。
*/
inline bool
IsNowShell(const shared_ptr<Shell>& hShl)
{
	return FetchAppInstance().GetShellHandle() == hShl;
}

/*!
\brief 向句柄指定的 Shell 对象转移线程控制权。
\since 早于 build 132 。
*/
inline errno_t
NowShellTo(const shared_ptr<Shell>& hShl)
{
	return -!Activate(hShl);
}

/*!
\brief 向新建 Shell 对象转移控制权。
\since 早于 build 132 。
*/
template<class _tShl>
inline errno_t
NowShellToNew()
{
	return NowShellTo(new _tShl());
}

/*!
\brief 向全局 Shell 管理器的对象转移控制权。
\since 早于 build 132 。
*/
template<class _tShl>
inline errno_t
NowShellToStored()
{
	return NowShellTo(FetchStored<_tShl>());
}

/*!
\brief 通过主消息队列向指定 Shell 对象转移控制权。
\since 早于 build 132 。
*/
inline void
SetShellTo(const shared_ptr<Shell>& hShl, Messaging::Priority p = 0x80)
{
	SendMessage<SM_SET>(FetchShellHandle(), p, hShl);
}

/*!
\brief 通过主消息队列向新建 Shell 对象转移控制权。
\since 早于 build 132 。
*/
template<class _tShl>
inline void
SetShellToNew()
{
	SetShellTo(new _tShl());
}

/*!
\brief 通过主消息队列向全局 Shell 管理器内的对象转移控制权。
\since 早于 build 132 。
*/
template<class _tShl>
inline void
SetShellToStored()
{
	SetShellTo(FetchStored<_tShl>());
}

/*!
\brief 封装向全局 Shell 管理器内的对象转移控制权。
\since 早于 build 132 。
*/
template<class _tShl>
inline void
CallStoredAtOnce()
{
	NowShellToStored<_tShl>();
}

/*!
\brief 封装通过消息队列向全局 Shell 管理器内的对象转移控制权。
\since 早于 build 132 。
*/
template<class _tShl>
inline void
CallStored()
{
	SetShellToStored<_tShl>();
}


//资源相关定义和函数。

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 简单屏幕绘图函数指针类型。
\since 早于 build 132 。
*/
typedef void (*PPDRAW)(BitmapPtr, SDst, SDst);

/*!
\brief 全屏幕描点。
\note 颜色由坐标决定。
\since 早于 build 132 。
*/
inline void
ScrDraw(BitmapPtr buf, PPDRAW f)
{
	for(SDst y(0); y < MainScreenHeight; ++y)
		for(SDst x(0); x < MainScreenWidth; ++x)
			f(buf, x, y);
}

/*!
\brief 新建屏幕图像。
\since build 213 。
*/
inline shared_ptr<Image>
CreateSharedScreenImage(ConstBitmapPtr p)
{
	return share_raw(new Image(p, MainScreenWidth, MainScreenHeight));
}

/*!
\brief 使用 new 分配空间并复制无压缩位图。
\since build 213 。
*/
PDefTmplH1(_tPixel)
_tPixel*
CreateRawBitmap(const _tPixel* s, size_t n)
{
	if(s && n)
	{
		size_t size(sizeof(_tPixel) * n);
		_tPixel* d(new _tPixel[size]);

		if(d)
			mmbcpy(d, s, size);
		return d;
	}
	return nullptr;
}

YSL_END_NAMESPACE(Drawing)

//@}

YSL_END;

#endif

