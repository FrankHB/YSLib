/*
	Copyright by FrankHB 2012-2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YBrush.h
\ingroup UI
\brief 图形用户界面画刷。
\version r340
\author FrankHB <frankhb1989@gmail.com>
\since build 293
\par 创建时间:
	2012-01-10 19:55:30 +0800
\par 修改时间:
	2013-09-06 23:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YBrush
*/


#ifndef YSL_INC_UI_YBrush_h_
#define YSL_INC_UI_YBrush_h_ 1

#include "ywgtevt.h"
#include "../Service/yres.h"

namespace YSLib
{

namespace UI
{

/*!
\brief 单色画刷。
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
\tparam _type 源类型。
\note 第三个参数为目标偏移，第四个参数为源偏移。
\since build 441

按目标绘制上下文、源和附加偏移更新目标图像的回调接口。
*/
template<typename _type>
using GBrushUpdater = std::function<void(const PaintContext&, _type,
	const Point&, const Point&)>;


/*!
\brief 图像画刷。
\since build 294
*/
class YF_API ImageBrush
{
public:
	/*!
	\brief 当前使用的图像更新器。
	\since build 441
	*/
	GBrushUpdater<const Drawing::Image&> Update = DefaultUpdate;

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
	\pre 断言：更新器非空。
	\note 图像指针为空指针忽略操作。
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
};

} // namespace UI;

} // namespace YSLib;

#endif

