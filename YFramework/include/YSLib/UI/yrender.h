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
\version r1527;
\author FrankHB<frankhb1989@gmail.com>
\since build 237 。
\par 创建时间:
	2011-09-03 23:47:32 +0800;
\par 修改时间:
	2011-12-05 09:24 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::UI::YRenderer;
*/


#ifndef YSL_INC_UI_YRENDER_H_
#define YSL_INC_UI_YRENDER_H_

#include "ycomp.h"
#include "../Service/ygdi.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//前向声明。
struct PaintContext;
struct PaintEventArgs;


/*!
\brief 部件渲染器。

无缓冲渲染策略：不保存部件渲染状态和有效的图形接口上下文。
\since build 237 。
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
	\brief 设置缓冲区大小。
	\note 空实现。
	*/
	virtual void
	SetSize(const Size&)
	{}

	/*!
	\brief 提交无效区域。
	*/
	virtual Rect
	CommitInvalidation(const Rect& r)
	{
		return r;
	}

	/*!
	\brief 按参数刷新。
	\pre 断言： <tt>&wgt.GetRenderer() == this</tt> 。
	\note 调用 wgt 的 Refresh 刷新，仅适用于指定部件包含自身。
	\since build 263 。
	*/
	virtual Rect
	Refresh(IWidget& wgt, PaintContext&&);
};


/*!
\brief 带缓冲的部件渲染器。

缓冲渲染策略：保存部件渲染状态和有效的图形接口上下文。
\since build 237 。
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
	bool
	RequiresRefresh() const;

	/*!
	\brief 取无效区域。
	*/
	DefGetter(const ynothrow, const Rect&, InvalidatedArea, rInvalidated)
	/*!
	\brief 取图形接口上下文。
	\return 缓冲区图形接口上下文。
	*/
	DefGetter(const ynothrow, const Graphics&, Context, Buffer)

	/*!
	\brief 设置缓冲区大小。
	*/
	virtual void
	SetSize(const Size&);

	/*!
	\brief 提交无效区域，使之合并至现有无效区域中。
	\return 新的无效区域。
	\note 由于无效区域的形状限制，可能会存在部分有效区域被合并。
	*/
	virtual Rect
	CommitInvalidation(const Rect&);

	/*!
	\brief 按参数刷新。
	\pre 断言： <tt>&wgt.GetRenderer() == this</tt> 。
	\pre 断言： <tt>GetContext().IsValid()</tt> 。
	\note 在 Validate 后 Update ，仅适用于指定部件包含自身。
	\since build 263 。
	*/
	virtual Rect
	Refresh(IWidget& wgt, PaintContext&&);


	/*!
	\brief 更新至指定图形设备上下文的指定点。
	\note 调用 wgt 的 Refresh 刷新。
	*/
	void
	UpdateTo(const PaintContext&) const;

	/*!
	\brief 验证指定部件的指定区域的缓冲区，若存在无效区域则刷新。
	\param r 相对部件的指定区域边界。
	\return 验证中被刷新的区域边界。
	\since build 263 。
	*/
	Rect
	Validate(IWidget&, const Rect& r);
};

/*!
\brief 设置部件的无效区域。
\since build 231 。
*/
void
SetInvalidationOf(IWidget&);

/*!
\brief 在容器设置部件的无效区域。
\note 若容器不存在则忽略。
\since build 229 。
*/
void
SetInvalidationToParent(IWidget&);


/*!
\brief 无效化：使部件区域在窗口缓冲区中无效。
\since build 224 。
*/
void
Invalidate(IWidget&);

/*!
\brief 级联无效化：使相对于部件的指定区域在直接和间接的窗口缓冲区中无效。
\since build 226 。
*/
void
InvalidateCascade(IWidget&, const Rect&);

/*
\brief 调用指定子部件所有的 Paint 事件绘制参数指定的事件发送者。
\since build 263 。
*/
void
PaintChild(IWidget&, PaintEventArgs&&);
/*
\brief 调用 Paint 事件绘制指定子部件。
\note 使用指定子部件作为事件发送者并复制参数。
\since build 263 。
*/
void
PaintChild(IWidget&, const PaintContext&);


/*
\brief 渲染：更新，若缓冲存储不可用则在更新前使用渲染器的 Refresh 方法刷新指定部件。

以 wgt 作为绘制目标，判断其边界是否和区域 pc.ClipArea 相交，
若相交区域非空则调用 wgt 的渲染器的 Refresh 方法绘制 。
调用结束后，e.ClipArea 被覆盖为 Refresh 方法的返回结果。
\since build 243 。
*/
void
Render(IWidget& wgt, PaintContext&& pc);
/*
\brief 渲染：更新，若缓冲存储不可用则在更新前刷新发送者。

以 e.GetSender() 作为绘制目标，判断其边界是否和区域 e.ClipArea 相交，
若相交区域非空则调用 e.GetSender() 的渲染器的 Refresh 方法绘制 。
调用结束后，e.ClipArea 被覆盖为 Refresh 方法的返回结果。
\since build 256 。
*/
void
Render(PaintEventArgs&& e);

YSL_END_NAMESPACE(Components)

YSL_END

#endif

