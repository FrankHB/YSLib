/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ImageProcessing.h
\ingroup Service
\brief 图像处理。
\version r92
\author FrankHB <frankhb1989@gmail.com>
\since build 554
\par 创建时间:
	2014-11-16 16:33:35 +0800
\par 修改时间:
	2014-11-18 00:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::ImageProcessing
*/


#ifndef YSL_INC_Service_ImageProcessing_h_
#define YSL_INC_Service_ImageProcessing_h_ 1

#include "YModules.h"
#include YFM_YSLib_Service_YGDI // for Drawing::CompactPimap;
#include YFM_YSLib_Service_YResource // for Drawing::Image, Drawing::ImageCodec;
#include YFM_YSLib_Adaptor_Image // for Drawing::HBitmap;
#include <ystdex/cache.hpp>
#include YFM_YSLib_Adaptor_YContainer

namespace YSLib
{

namespace Drawing
{

//! \since build 554
//@{
/*!
\brief 缩放位图。
\pre 断言：位图有效且非空。
*/
YF_API CompactPixmap
Zoom(const HBitmap&, float);


//! \brief 缩放图像缓冲。
class YF_API ZoomedImageCache
{
public:
	using Container = vector<HBitmap>;
	using ScaleType = float;
	using CacheKey = pair<ScaleType, size_t>;
	struct CacheHash
	{
		PDefHOp(size_t, (), const CacheKey& key) const ynothrow
			ImplRet(ystdex::hash_combine_seq(size_t(key.first), key.second))
	};
	//! \since build 521
	using BitmapCache = ystdex::used_list_cache<CacheKey, shared_ptr<Image>,
		CacheHash>;

private:
	Container bitmaps;
	BitmapCache cache;

public:
	explicit
	ZoomedImageCache(const char16_t* path)
		: bitmaps(ImageCodec::LoadSequence<Container>(path)),
		cache(bitmaps.size() * 2)
	{}

	DefGetter(const ynothrow, const Container&, Bitmaps, bitmaps)

	shared_ptr<Image>
	Lookup(ScaleType, size_t);
};
//@}

} // namespace Drawing;

} // namespace YSLib;

#endif

