/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ImageProcessing.cpp
\ingroup Service
\brief 图像处理。
\version r256
\author FrankHB <frankhb1989@gmail.com>
\since build 554
\par 创建时间:
	2014-11-16 16:37:27 +0800
\par 修改时间:
	2015-02-27 23:07 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::ImageProcessing
*/


#include "YSLib/UI/YModules.h"
#include YFM_YSLib_Service_ImageProcessing
#include YFM_YSLib_UI_YComponent // for YSLib::to_string;

namespace YSLib
{

namespace Drawing
{

CompactPixmap
Zoom(const HBitmap& bitmap, ImageScale ratio)
{
	YAssert(bool(bitmap), "Invalid bitmap found.");

	const Size bmp_size(bitmap.GetSize());

	YAssert(!bmp_size.IsUnstrictlyEmpty(), "Invalid size found.");
	YTraceDe(Informative, "Unscaled image size = %s, requested ratio = %f.",
		to_string(bmp_size).c_str(), float(ratio));
	if(abs(ratio - 1) < std::numeric_limits<ImageScale>::epsilon())
		return bitmap;

	const Size zoomed_size(round(bmp_size.Width * float(ratio)),
		round(bmp_size.Height * float(ratio)));

	YTraceDe(Informative, "Zoomed image ratio = %f, with size = %s.",
		float(ratio), to_string(zoomed_size).c_str());
	return HBitmap(bitmap, zoomed_size,
		ratio < 2 ? SamplingFilter::Bilinear : SamplingFilter::Bicubic);
}


shared_ptr<Image>
ZoomedImageCache::Lookup(ImageScale scale, size_t idx)
{
	YAssert(idx < bitmaps.size(), "Invalid index found.");

	return ystdex::cache_lookup(cache, CacheKey(scale, idx), [&]{
		return make_shared<Image>(Zoom(bitmaps[idx], scale));
	});
}


ImagePages::ImagePages(ZoomedImageCache&& c, const Size& min_size,
	const Size& max_size, ImageScale init_scale)
	: cache(std::move(c)),
	base_size([&](const ZoomedImageCache::Container& bmps)->Size{
		YAssert(!max_size.IsUnstrictlyEmpty(), "Empty maximum size found.");

		if(bmps.empty())
			return max_size;

		const auto& first_size(bmps[0].GetSize());

		TryRet(GetLogicalSizeOf(bmps[0]) | first_size)
		CatchIgnore(GeneralEvent&)
		return first_size;
	}(cache.GetBitmaps())), scale([&, init_scale]()->ImageScale{
		return init_scale < MinScale ? ImageScale(cache.GetBitmaps().empty()
			? 1.F : ScaleMin(max_size, base_size)) : init_scale;
	}())
{
	YAssert((min_size & max_size) == min_size, "Invalid size arguments found.");
	YTraceDe(Informative, "Base size = %s.", to_string(base_size).c_str());
	YTraceDe(Informative, "Automatically rescaled, scale = %f.", double(scale));
	LoadContent();
//	YTraceDe(Informative, "Format = %d.", bitmap.GetFormat());
	yunseq(view_size = min_size | Brush.ImagePtr->GetSize(),
		Brush.Update = Drawing::UpdateRotatedBrush<>);
	AdjustOffset(view_size);
}

void
ImagePages::AdjustOffset(const Size& cont_size)
{
	YAssertNonnull(Brush.ImagePtr);

	auto& dst(Brush.DstOffset);
	const auto& size(Brush.ImagePtr->GetSize());

	YSLib::HalfDifferenceIfGreater<SPos>(dst.X, cont_size.Width, size.Width),
	YSLib::HalfDifferenceIfGreater<SPos>(dst.Y, cont_size.Height, size.Height);
	YTraceDe(Informative, "Adjusted destination offset = %s.",
		to_string(Brush.DstOffset).c_str());
}

void
ImagePages::Resize(const Size& new_size)
{
	YTraceDe(Informative, "Requested resized size = %s.",
		to_string(new_size).c_str());
	YAssertNonnull(Brush.ImagePtr);

	const Point offset(view_size.Width / 2, view_size.Height / 2);
	const Point new_offset(new_size.Width / 2, new_size.Height / 2);

	YTraceDe(Informative, "Zoomed ratio = %f, fixed offset = %s, "
		"fixed new offset = %s.", double(scale), to_string(offset).c_str(),
		to_string(new_offset).c_str());
	Brush.DstOffset += new_offset - offset;
	AdjustOffset(view_size = new_size);
}

void
ImagePages::LoadContent()
{
	YAssert(std::numeric_limits<ImageScale>::epsilon() < abs(scale),
		"Invalid ratio value found.");
	Brush.ImagePtr = cache.Lookup(scale, index);
}

bool
ImagePages::SwitchPage(size_t page)
{
	YAssert(ZoomedImageCache::Container::size_type(page) < GetCount(),
		"Invalid index found.");
	if(page != index)
	{
		index = ZoomedImageCache::Container::size_type(page);
		YTraceDe(Informative, "Page switched: %u/%u.", unsigned(index + 1),
			unsigned(cache.GetBitmaps().size()));
		LoadContent();
		return true;
	}
	return {};
}

bool
ImagePages::Zoom(float delta, const Point& offset)
{
	YTraceDe(Informative, "Action: zoom, with delta = %f%%.", delta * 100.F);
	return ZoomTo(scale + delta, offset);
}

bool
ImagePages::ZoomByRatio(float ratio, const Point& offset)
{
	YTraceDe(Informative, "Action: zoom, with ratio = %f%%.", ratio * 100.F);
	if(ratio > 0)
		return Zoom((ratio - 1.F) * scale, offset);
	else
		YTraceDe(Warning, "Invalid ratio found.");
	return {};
}

bool
ImagePages::ZoomTo(float dst_scale, const Point& offset)
{
	ImageScale new_scale(dst_scale);

	RestrictInClosedInterval(new_scale, MinScale, MaxScale);
	YTraceDe(Informative, "Requested zoomed ratio = %f, fixed offset = %s.",
		double(new_scale), to_string(offset).c_str());
	if(std::numeric_limits<ImageScale>::epsilon() < abs(new_scale - scale))
	{
		const auto old(scale);

		scale = new_scale;
		LoadContent();
		Brush.DstOffset = offset - (offset - Brush.DstOffset) * (scale / old);
		AdjustOffset(view_size);
		return true;
	}
	return {};
}

} // namespace Drawing;

} // namespace YSLib;

