/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yrender.h
\ingroup UI
\brief 样式无关的图形用户界面部件渲染器。
\version r1320;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-09-03 23:47:32 +0800;
\par 修改时间:
	2011-09-14 23:20 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YRenderer;
*/


#ifndef YSL_INC_UI_YRENDER_H_
#define YSL_INC_UI_YRENDER_H_

#include "../Service/ygdi.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//名称引用。
using Drawing::PixelType;
using Drawing::BitmapPtr;
using Drawing::ConstBitmapPtr;
using Drawing::ScreenBufferType;
using Drawing::Color;

using Drawing::Point;
using Drawing::Vec;
using Drawing::Size;
using Drawing::Rect;

using Drawing::Graphics;


/*!
\brief 部件渲染器。

无缓冲渲染策略：不保存部件渲染状态和有效的图形接口上下文。
*/
class Renderer
{
public:
	inline DefDeCtor(Renderer)
	inline DefDeCopyCtor(Renderer)
	inline DefDeMoveCtor(Renderer)
	virtual DefClone(Renderer, Clone)
	virtual DefEmptyDtor(Renderer)

	/*!
	\brief 判断是否需要刷新。
	\note 总是需要刷新。
	*/
	virtual PDefH0(bool, RequiresRefresh) const
		ImplRet(true)

	/*!
	\brief 取无效区域。
	\note 空实现。
	*/
	virtual void
	GetInvalidatedArea(Rect&) const
	{}
	/*!
	\brief 取图形接口上下文。
	\note 返回无效图形接口上下文。
	*/
	virtual DefGetter(const Graphics&, Context, Graphics::Invalid)

	/*!
	\brief 设置缓冲区大小。
	\note 空实现。
	*/
	virtual void
	SetSize(const Size&)
	{}

	/*!
	\brief 清除无效区域。
	\note 空实现。
	*/
	virtual void
	ClearInvalidation()
	{}

	/*!
	\brief 提交无效区域。
	\note 空实现。
	*/
	virtual void
	CommitInvalidation(const Rect&)
	{}

	/*
	\brief 以纯色填充无效区域。
	\note 空实现。
	*/
	virtual void
	FillInvalidation(Color)
	{}

	/*!
	\brief 更新至指定图形设备上下文的指定点。
	\param 意义同 IWidget::Refresh 。
	\note 空实现。
	*/
	virtual void
	UpdateTo(const Graphics&, const Point&,
		const Rect&) const
	{}
};


/*!
\brief 带缓冲的部件渲染器。

缓冲渲染策略：保存部件渲染状态和有效的图形接口上下文。
*/
class BufferedRenderer : public Renderer
{
protected:
	mutable Rect rInvalidated; \
		//!< 无效区域：包含所有新绘制请求的区域（不一定是最小的）。

public:
	Drawing::BitmapBuffer Buffer; //!< 显示缓冲区。

	DefDeCtor(BufferedRenderer)
	DefDeCopyCtor(BufferedRenderer)
	DefDeMoveCtor(BufferedRenderer)
	virtual DefClone(BufferedRenderer, Clone)

	/*!
	\brief 判断是否需要刷新。
	\note 若无效区域长宽都不为零，则需要刷新。
	*/
	virtual bool
	RequiresRefresh() const;

	/*!
	\brief 取无效区域。
	*/
	DefGetter(const Rect&, InvalidatedArea, rInvalidated)
	/*!
	\brief 取无效区域。
	\note 通过参数返回。
	*/
	virtual void
	GetInvalidatedArea(Rect&) const;
	/*!
	\brief 取图形接口上下文。
	\return 缓冲区图形接口上下文。
	*/
	virtual DefGetter(const Graphics&, Context, Buffer)

	/*!
	\brief 设置缓冲区大小。
	*/
	virtual void
	SetSize(const Size&);

	/*!
	\brief 清除无效区域。
	\note 通过设置大小为 Size::Zero 使无效区域被忽略。
	*/
	virtual void
	ClearInvalidation();

	/*!
	\brief 提交无效区域。
	\note 合并至现有无效区域中。
	\note 由于无效区域的形状限制，结果可能会包含部分有效区域。
	*/
	virtual void
	CommitInvalidation(const Rect&);

	/*
	\brief 以纯色填充无效区域。
	*/
	virtual void
	FillInvalidation(Color);

	/*!
	\brief 更新至指定图形设备上下文的指定点。
	\param 意义同 IWidget::Refresh 。
	*/
	virtual void
	UpdateTo(const Graphics&, const Point&, const Rect&) const;
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

