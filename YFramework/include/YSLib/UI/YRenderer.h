/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YRenderer.h
\ingroup UI
\brief 样式无关的 GUI 部件渲染器。
\version r649
\author FrankHB <frankhb1989@gmail.com>
\since build 566
\par 创建时间:
	2011-09-03 23:47:32 +0800
\par 修改时间:
	2015-03-21 21:28 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YRenderer
*/


#ifndef YSL_INC_UI_YRenderer_h_
#define YSL_INC_UI_YRenderer_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YComponent

namespace YSLib
{

namespace UI
{

/*!
\brief 部件渲染器。
\since build 237

无缓冲渲染策略：不保存部件渲染状态和有效的图形接口上下文。
*/
class YF_API Renderer : public ystdex::cloneable
{
public:
	DefDeCtor(Renderer)
	DefDeCopyMoveCtorAssignment(Renderer)

	/*!
	\brief 设置缓冲区大小。
	\note 空实现。
	*/
	virtual void
	SetSize(const Size&)
	{}

	//! \since build 409
	DefClone(const ImplI(ystdex::cloneable), Renderer)

	/*!
	\brief 提交无效区域。
	*/
	virtual PDefH(Rect, CommitInvalidation, const Rect& r)
		ImplRet(r)

	/*!
	\brief 按参数绘制。
	\pre 断言： <tt>&e.GetSender().GetRenderer() == this</tt> 。
	\note 调用 wgt 的 Paint 事件绘制。
	\note 不调用 wgt.IsVisible() 检查可见性。
	\note 派生类行为可能改变，不直接调用事件。
	\since build 263
	*/
	virtual Rect
	Paint(IWidget& wgt, PaintEventArgs&&);
};


/*!
\brief 伪渲染器：静态类型的所有实际操作不产生可观察的副作用。
\since build 588
*/
class YF_API PseudoRenderer : public Renderer
{
public:
	DefDeCtor(PseudoRenderer)
	DefDeCopyMoveCtorAssignment(PseudoRenderer)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~PseudoRenderer() override;

	DefClone(const override, PseudoRenderer)

	PDefH(Rect, Paint, IWidget&, PaintEventArgs&&) override
		ImplRet({})
};


/*!
\brief 带缓冲的部件渲染器。
\note 除了明确可能导致缓冲区失效的操作，缓冲区指针保持不变。
\since build 237

缓冲渲染策略：保存部件渲染状态和有效的图形接口上下文。
*/
class YF_API BufferedRenderer : public Renderer
{
protected:
	mutable Rect rInvalidated; \
		//!< 无效区域：包含所有新绘制请求的区域（不一定是最小的）。
	/*!
	\brief 显示图像缓冲区指针。
	\since build 406
	*/
	unique_ptr<Drawing::IImage> pImageBuffer;

public:
	/*!
	\brief 指定验证时忽略上层缓冲区背景。
	\since build 278
	*/
	bool IgnoreBackground;

	/*!
	\brief 构造：指定是否忽略上层缓冲区背景。
	\note 当指针为空时新建缓冲区。
	\since build 407
	*/
	BufferedRenderer(bool = {}, unique_ptr<Drawing::IImage> = {});
	BufferedRenderer(const BufferedRenderer&);
	DefDeMoveCtor(BufferedRenderer)

	/*!
	\brief 判断是否需要刷新。
	\note 若无效区域长宽都不为零，则需要刷新。
	*/
	bool
	RequiresRefresh() const;

	//! \since build 406
	DefGetter(const ynothrow, Drawing::IImage&, ImageBuffer, *pImageBuffer)
	/*!
	\brief 取无效区域。
	*/
	DefGetter(const ynothrow, const Rect&, InvalidatedArea, rInvalidated)
	/*!
	\brief 取图形接口上下文。
	\return 缓冲区图形接口上下文。
	\since build 566
	*/
	DefGetterMem(const ynothrow, Graphics, Context, GetImageBuffer())

	/*!
	\brief 设置缓冲区大小。
	\warning 可能导致原缓冲区指针失效。
	*/
	void
	SetSize(const Size&) override;
	//! \since build 406
	void
	SetImageBuffer(unique_ptr<Drawing::IImage>);

	//! \since build 409
	DefClone(const override, BufferedRenderer)

	/*!
	\brief 提交无效区域，使之合并至现有无效区域中。
	\return 新的无效区域。
	\note 由于无效区域的形状限制，可能会存在部分有效区域被合并。
	*/
	Rect
	CommitInvalidation(const Rect&) override;

	/*!
	\brief 按参数绘制。
	\pre 断言： <tt>&e.GetSender().GetRenderer() == this</tt> 。
	\note 在 Validate 后 Update 。
	\note 不调用 wgt.IsVisible() 检查可见性。
	\since build 263
	*/
	Rect
	Paint(IWidget& wgt, PaintEventArgs&&) override;

	/*!
	\brief 更新至指定图形设备上下文的指定点。
	\note 复制显示缓冲区内容。
	*/
	void
	UpdateTo(const PaintContext&) const;

	/*!
	\brief 验证并按需绘制。
	\return 验证中被刷新的区域边界。
	\since build 293

	验证 sender 的指定图形接口上下文的关联的缓冲区，
	若存在无效区域则新建 PaintEventArgs ， 调用 wgt 的 Paint 事件绘制。
	*/
	Rect
	Validate(IWidget& wgt, IWidget& sender, const PaintContext&);
};

} // namespace UI;

} // namespace YSLib;

#endif

