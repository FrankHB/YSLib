/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ImageProcessing.cpp
\ingroup Service
\brief 图像处理。
\version r75
\author FrankHB <frankhb1989@gmail.com>
\since build 554
\par 创建时间:
	2014-11-16 16:37:27 +0800
\par 修改时间:
	2014-11-18 00:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::ImageProcessing
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_Service_ImageProcessing
#include YFM_YSLib_UI_YComponent // for YSLib::to_string;
#include <cmath>
#include <limits>

namespace YSLib
{

namespace Drawing
{

CompactPixmap
Zoom(const HBitmap& bitmap, float ratio)
{
	YAssert(bool(bitmap), "Invalid bitmap found.");

	const Size bmp_size(bitmap.GetSize());

	YAssert(!bmp_size.IsUnstrictlyEmpty(), "Invalid size found.");
	YTraceDe(Informative, "Unscaled image size = %s, requested ratio = %f.",
		to_string(bmp_size).c_str(), ratio);
	if(std::abs(ratio - 1) < std::numeric_limits<float>::epsilon())
		return bitmap;

	const Size zoomed_size(round(bmp_size.Width * ratio),
		round(bmp_size.Height * ratio));

	YTraceDe(Informative, "Zoomed image ratio = %f, with size = %s.", ratio,
		to_string(zoomed_size).c_str());
	return HBitmap(bitmap, zoomed_size, ratio < 2 ? SamplingFilter::Bilinear
		: SamplingFilter::Bicubic);
}


shared_ptr<Image>
ZoomedImageCache::Lookup(ScaleType scale, size_t idx)
{
	YAssert(idx < bitmaps.size(), "Invalid index found.");

	return ystdex::cache_lookup(cache, CacheKey(scale, idx), [&]{
		return make_shared<Image>(Zoom(bitmaps[idx], scale));
	});
}

} // namespace Drawing;

} // namespace YSLib;

