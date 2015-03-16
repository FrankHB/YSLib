/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YBrush.h
\ingroup UI
\brief 画刷。
\version r546
\author FrankHB <frankhb1989@gmail.com>
\since build 293
\par 创建时间:
	2012-01-10 19:55:30 +0800
\par 修改时间:
	2015-03-17 06:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YBrush
*/


#ifndef YSL_INC_Service_YBrush_h_
#define YSL_INC_Service_YBrush_h_ 1

#include "YModules.h"
#include YFM_YSLib_Service_YResource
#include YFM_YSLib_Core_YEvent // for YSLib::GHEvent;
#include YFM_YSLib_Service_YBlend // for Drawing::BlitPixels,
//	Drawing::Shaders::BlitAlphaPoint;

namespace YSLib
{

namespace Drawing
{

/*!	\defgroup UIBrushes User Interface Brushes
\ingroup UI
\brief 用户界面画刷。
*/

/*!
\ingroup UIBrushes
\warning 非虚析构。
*/
//@{
/*!
\brief 单色画刷。
\sa FillRect
\since build 293
*/
class YF_API SolidBrush
{
public:
	Drawing::Color Color;

	SolidBrush(Drawing::Color c)
		: Color(c)
	{}

	//! \since build 555
	void
	operator()(const PaintContext&) const;
};


/*!
\brief 单色 Alpha 混合画刷。
\warning 非虚析构。
\sa BlendRect
\since build 584
*/
class YF_API SolidBlendBrush
{
public:
	Drawing::Color Color;

	SolidBlendBrush(Drawing::Color c)
		: Color(c)
	{}

	void
	operator()(const PaintContext&) const;
};

/*!
\brief 单色 Alpha 组合画刷。
\sa CompositeRect
\since build 584
*/
class YF_API SolidCompositeBrush
{
public:
	Drawing::Color Color;

	SolidCompositeBrush(Drawing::Color c)
		: Color(c)
	{}

	void
	operator()(const PaintContext&) const;
};
//@}


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
	\since build 555
	*/
	void
	operator()(const PaintContext&) const;

	/*!
	\brief 默认更新：复制图像。
	\since build 443
	*/
	static void
	DefaultUpdate(const PaintContext&, const Drawing::Image&, const Point&,
		const Point&);
};


/*!
\brief 更新：逐像素操作。
\sa Drawing::BlitPixels
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

/*!
\brief 更新：转置的逐像素操作。
\sa Drawing::BlitPixels
\since build 575
*/
template<typename _fPixelShader, bool _bSwapLR = false, bool _bSwapUD = false>
void
UpdateTranposedPixels(_fPixelShader shader, const PaintContext& pc,
	const Drawing::Image& img, const Point& dst_offset, const Point& src_offset)
{
	const auto& g(pc.Target);
	const Rect& bounds(pc.ClipArea);
	const auto& src(img.GetContext());

	Drawing::BlitPixels<_bSwapLR, _bSwapUD>(shader, g.GetBufferPtr(),
		ystdex::make_transposed(src.GetBufferPtr(), src.GetWidth(),
		src.GetHeight(), 0), g.GetSize(), Transpose(src.GetSize()),
		bounds.GetPoint(), bounds.GetPoint() + src_offset - dst_offset,
		bounds.GetSize());
}


/*!
\brief 更新：可能旋转源图像的像素操作。
\sa Drawing::DispatchTranspose
\sa Drawing::UpdatePixels
\sa Drawing::UpdateTranposedPixels
\since build 579
*/
template<Rotation _vRot = RDeg0,
	typename _fPixelShader = Shaders::BlitAlphaPoint>
void
UpdatePixelsWithRotation(_fPixelShader shader, const PaintContext& pc,
	const Drawing::Image& img, const Point& dst_offset, const Point& src_offset)
{
	using namespace ystdex;
	using trans_t
		= std::integral_constant<bool, _vRot == RDeg90 || _vRot == RDeg270>;

	Drawing::DispatchTranspose(trans_t(), [&](conditional_t<trans_t::value,
		transposed_iterator<ConstBitmapPtr>, ConstBitmapPtr> src_iter,
		const Size& ss, const Point& dst_offset_ex){
		const auto& g(pc.Target);
		const Rect& bounds(pc.ClipArea);

		Drawing::BlitPixels<_vRot == RDeg180 || _vRot == RDeg270,
			_vRot == RDeg90 || _vRot == RDeg180>(shader, g.GetBufferPtr(),
			src_iter, g.GetSize(), ss, bounds.GetPoint(), bounds.GetPoint()
			+ src_offset - (dst_offset + dst_offset_ex), bounds.GetSize());
	}, img.GetContext());
}


/*!
\brief 使用默认构造的画刷更新：可能旋转源图像的像素操作。
\sa Drawing::UpdatePixelsWithRotation
\since build 579
*/
template<Rotation _vRot = RDeg0,
	typename _fPixelShader = Shaders::BlitAlphaPoint>
void
UpdateRotatedBrush(const PaintContext& pc, const Drawing::Image& img,
	const Point& dst_offset, const Point& src_offset)
{
	Drawing::UpdatePixelsWithRotation<_vRot, _fPixelShader>(_fPixelShader(),
		pc, img, dst_offset, src_offset);
}


/*!
\brief 按指定的源图像旋转方向构造画刷。
\sa UpdateRotatedBrush
\since build 580
*/
template<typename _fPixelShader = Shaders::BlitAlphaPoint,
	typename _tImage = Drawing::Image>
GBrushUpdater<const _tImage&, const Point&, const Point&>
DispatchRotatedBrush(Rotation rot)
{
	switch(rot)
	{
	case RDeg0:
		return UpdateRotatedBrush<RDeg0, _fPixelShader>;
		break;
	case RDeg90:
 		return UpdateRotatedBrush<RDeg90, _fPixelShader>;
		break;
	case RDeg180:
		return UpdateRotatedBrush<RDeg180, _fPixelShader>;
		break;
	case RDeg270:
		return UpdateRotatedBrush<RDeg270, _fPixelShader>;
	default:
		break;
	}
	YAssert(false, "Invalid rotation found.");
	return {};
}

} // namespace Drawing;

} // namespace YSLib;

#endif

