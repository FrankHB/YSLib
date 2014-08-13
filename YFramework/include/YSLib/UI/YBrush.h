/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YBrush.h
\ingroup UI
\brief 图形用户界面画刷。
\version r395
\author FrankHB <frankhb1989@gmail.com>
\since build 293
\par 创建时间:
	2012-01-10 19:55:30 +0800
\par 修改时间:
	2014-08-12 02:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YBrush
*/


#ifndef YSL_INC_UI_YBrush_h_
#define YSL_INC_UI_YBrush_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YWidgetEvent
#include YFM_YSLib_Service_YResource
#include YFM_YSLib_Service_YBlit // for Drawing::BlitPixels;

namespace YSLib
{

namespace UI
{

/*!	\defgroup UIBrushes User Interface Brushes
\ingroup UI
\brief 用户界面画刷。
*/

/*!
\ingroup UIBrushes
\brief 单色画刷。
\warning 非虚析构。
\since build 293
*/
class YF_API SolidBrush
{
public:
	Drawing::Color Color;

	SolidBrush(Drawing::Color c)
		: Color(c)
	{}

	void
	operator()(PaintEventArgs&&);
};


/*!
\brief 画刷更新器类型。
\since build 525

按目标绘制上下文、源和附加偏移更新目标图像的回调接口。
*/
template<typename... _types>
using GBrushUpdater = GHEvent<void(const PaintContext&, _types...)>;


/*!
\ingroup UIBrushes
\brief 图像画刷。
\warning 非虚析构。
\since build 294
*/
class YF_API ImageBrush
{
public:
	/*!
	\brief 当前使用的图像更新器。
	\note 第三个模板参数为目标偏移，第四个模板参数为源偏移。
	\since build 525
	*/
	GBrushUpdater<const Drawing::Image&, const Point&, const Point&>
		Update{DefaultUpdate};

	//! \since build 441
	mutable shared_ptr<Drawing::Image> ImagePtr;
	//! \since build 443
	Point DstOffset{};
	//! \since build 443
	Point SrcOffset{};

	yconstfn DefDeCtor(ImageBrush)
	ImageBrush(const shared_ptr<Drawing::Image>& h)
		: ImagePtr(h)
	{}
	ImageBrush(shared_ptr<Drawing::Image>&& h)
		: ImagePtr(std::move(h))
	{}
	DefDeCopyCtor(ImageBrush)
	DefDeMoveCtor(ImageBrush)

	DefDeCopyAssignment(ImageBrush)
	DefDeMoveAssignment(ImageBrush)

	/*!
	\brief 描画：调用图像更新器。
	\note 图像指针为空指针时忽略操作。
	*/
	void
	operator()(PaintEventArgs&&);

	/*!
	\brief 默认更新：复制图像。
	\since build 443
	*/
	static void
	DefaultUpdate(const PaintContext&, const Drawing::Image&, const Point&,
		const Point&);

	/*!
	\brief 更新：Alpha 组合图像。
	\since build 448
	*/
	static void
	UpdateComposite(const PaintContext&, const Drawing::Image&, const Point&,
		const Point&);
};


/*!
\brief 更新：逐像素操作。
\sa BlitPixels
\since build 448
*/
template<typename _fPixelShader, bool _bSwapLR = false, bool _bSwapUD = false>
void
UpdatePixels(_fPixelShader shader, const PaintContext& pc,
	const Drawing::Image& img, const Point& dst_offset, const Point& src_offset)
{
	const auto& g(pc.Target);
	const Rect& bounds(pc.ClipArea);
	const auto& src(img.GetContext());

	Drawing::BlitPixels<_bSwapLR, _bSwapUD>(shader, g.GetBufferPtr(),
		src.GetBufferPtr(), g.GetSize(), src.GetSize(), bounds.GetPoint(),
		bounds.GetPoint() + src_offset - dst_offset, bounds.GetSize());
}

} // namespace UI;

} // namespace YSLib;

#endif

