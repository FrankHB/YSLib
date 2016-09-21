/*
	© 2015-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YBlend.h
\ingroup Service
\brief 平台中立的图像混合操作。
\version r164
\author FrankHB <frankhb1989@gmail.com>
\since build 584
\par 创建时间:
	2015-03-17 06:17:06 +0800
\par 修改时间:
	2016-09-20 10:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YBlend
*/


#ifndef YSL_INC_Service_YBlend_h_
#define YSL_INC_Service_YBlend_h_ 1

#include "YModules.h"
#include YFM_YSLib_Service_YPixel
#include YFM_YSLib_Service_YBlit

namespace YSLib
{

namespace Drawing
{

namespace Shaders
{

/*!
\ingroup PixelShaders
\brief 像素迭代器透明操作。
\warning 不检查迭代器有效性。
\since build 440
*/
struct BlitTransparentPoint
{
	//! \note 使用源迭代器对应像素的第 15 位表示透明性。
	template<typename _tOut, typename _tIn>
	inline void
	operator()(_tOut dst_iter, _tIn src_iter)
	{
		if(FetchAlpha(*src_iter))
			*dst_iter = *src_iter;
	}
	/*!
	\note 使用像素自身的 Alpha 通道表示透明性。
	\since build 584
	*/
	template<typename _tOut, typename _tInAlpha>
	inline void
	operator()(_tOut dst_iter,
		ystdex::pair_iterator<ConstBitmapPtr, _tInAlpha> src_iter)
	{
		*dst_iter = *get<1>(src_iter.base()) & 0x80 ? FetchOpaque(*src_iter)
			: FetchOpaque(Pixel());
	}
};


/*!
\ingroup PixelShaders
\brief 像素计算：Alpha 混合。
\since build 440
*/
struct BlitAlphaPoint
{
private:
	//! \since build 729
	template<typename _type>
	using ABitTraits = typename ystdex::decay_t<_type>::Traits;

public:
	//! \since build 584
	template<typename _tOut, typename _tInPixel, typename _tInAlpha>
	inline void
	operator()(_tOut dst_iter,
		ystdex::pair_iterator<_tInPixel, _tInAlpha> src_iter)
	{
		static_assert(std::is_convertible<ystdex::remove_reference_t<
			decltype(*dst_iter)>, Pixel>(), "Wrong type found.");

		*dst_iter = Shaders::BlendAlpha<ABitTraits<decltype(*dst_iter)>::ABitsN,
			8>(*dst_iter, *src_iter, AlphaType(*get<1>(src_iter.base())));
	}
	//! \since build 448
	template<typename _tOut, typename _tIn>
	inline void
	operator()(_tOut dst_iter, _tIn src_iter)
	{
		static_assert(std::is_convertible<ystdex::remove_reference_t<
			decltype(*dst_iter)>, Pixel>(), "Wrong type found.");

		*dst_iter = Shaders::Composite<ABitTraits<decltype(*dst_iter)>::ABitsN,
			ABitTraits<decltype(*src_iter)>::ABitsN>(*dst_iter, *src_iter);
	}
};

} // namespace Shaders;

//! \since build 584
//@{
//! \brief 使用指定像素 Alpha 混合指定的标准矩形区域。
//@{
template<typename _tPixel, typename _tOut, typename... _tParams>
inline void
BlendRectRaw(_tOut dst, _tPixel c, _tParams&&... args)
{
	using namespace ystdex;
	using bl_it = pseudo_iterator<const Pixel>;

	Drawing::BlitRectPixels(Shaders::BlitAlphaPoint(), dst,
		pair_iterator<bl_it, ConstBitmapPtr>(bl_it(c), dst), yforward(args)...);
}

YF_API void
BlendRect(const Graphics&, const Rect&, Color);
//@}


//! \brief 使用指定像素 Alpha 组合指定的标准矩形区域。
//@{
template<typename _tPixel, typename _tOut, typename... _tParams>
inline void
CompositeRectRaw(_tOut dst, _tPixel c, _tParams&&... args)
{
	Drawing::BlitRectPixels(Shaders::BlitAlphaPoint(), dst,
		ystdex::pseudo_iterator<const _tPixel>(c), yforward(args)...);
}

YF_API void
CompositeRect(const Graphics&, const Rect&, Color);
//@}
//@}

} // namespace Drawing;

} // namespace YSLib;

#endif

