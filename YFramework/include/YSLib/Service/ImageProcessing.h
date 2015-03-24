/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ImageProcessing.h
\ingroup Service
\brief 图像处理。
\version r262
\author FrankHB <frankhb1989@gmail.com>
\since build 554
\par 创建时间:
	2014-11-16 16:33:35 +0800
\par 修改时间:
	2015-03-22 23:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::ImageProcessing
*/


#ifndef YSL_INC_Service_ImageProcessing_h_
#define YSL_INC_Service_ImageProcessing_h_ 1

#include "YModules.h"
#include <ystdex/rational.hpp> // for ystdex::fixed_point;
#include YFM_YSLib_Service_YGDI // for Drawing::CompactPimap;
#include YFM_YSLib_Service_YBrush // for Drawing::Image, Drawing::ImageCodec,
//	Drawing::ImageBrush;
#include YFM_YSLib_Adaptor_Image // for Drawing::HBitmap;
#include <ystdex/cache.hpp>
#include YFM_YSLib_Adaptor_YContainer

namespace YSLib
{

namespace Drawing
{

//! \since build 555
//@{
//! \brief 图像缩放类型。
using ImageScale = ystdex::fixed_point<yimpl(std::uint32_t, 12U)>;

/*!
\brief 缩放位图。
\pre 断言：位图有效且非空。
*/
YF_API CompactPixmap
Zoom(const HBitmap&, ImageScale);
//@}


/*!
\brief 缩放图像缓冲。
\since build 554
*/
class YF_API ZoomedImageCache
{
public:
	using Container = vector<HBitmap>;
	using CacheKey = pair<ImageScale, size_t>;
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
	//! \since build 555
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_ctor_t<ZoomedImageCache, _type>)>
	explicit
	ZoomedImageCache(const _type& path)
		: bitmaps(ImageCodec::LoadSequence<Container>(path)),
		cache(bitmaps.size() * 2)
	{}
	//! \since build 555
	DefDeMoveCtor(ZoomedImageCache)

	//! \since build 555
	DefDeMoveAssignment(ZoomedImageCache)

	DefGetter(const ynothrow, const Container&, Bitmaps, bitmaps)

	//! \since build 555
	shared_ptr<Image>
	Lookup(ImageScale, size_t);
};


/*!
\brief 图像页面集。
\warning 若 MinScale 不小于 MaxScale 则可能引起未定义行为。
\warning 若缩放比例小于 MaxScale 或 MaxScale 小于缩放比例则不保证状态符合预期。
\warning 非虚析构。
\since build 555
*/
class YF_API ImagePages
{
public:
	ImageScale MinScale{.1F};
	ImageScale MaxScale{16.F};

private:
	//! \since build 457
	size_t index = 0;
	ZoomedImageCache cache;
	/*!
	\brief 图像基础大小。
	\since build 557
	*/
	Size base_size;
	/*!
	\brief 缩放比例。
	\since build 555
	*/
	ImageScale scale;
	//! \since build 461
	Size view_size{};

public:
	//! \since build 443
	ImageBrush Brush{};

	/*
	\pre 断言：区域上限严格非空。
	\pre 断言：区域下限分量不大于区域上限分量。
	\note 若起始缩放比例小于下限则由图像大小计算。
	*/
	//@{
	//! \brief 构造：使用指定路径、显示区域下限、显示区域上限和起始缩放比例。
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_ctor_t<ImagePages, _type>)>
	ImagePages(const _type& path, const Size& min_size, const Size& max_size,
		ImageScale init_scale = {})
		: ImagePages(ZoomedImageCache(path), min_size, max_size, init_scale)
	{}
	//! \brief 构造：使用指定缓存、显示区域下限、显示区域上限和起始缩放比例。
	ImagePages(ZoomedImageCache&&, const Size&, const Size&, ImageScale);
	//@}
	DefDeMoveCtor(ImagePages)

	DefDeMoveAssignment(ImagePages)

	//! \since build 558
	DefGetterMem(const ynothrow, const vector<HBitmap>&, Bitmaps, cache)
	//! \since build 557
	//@{
	DefGetter(const ynothrow, size_t, Count, cache.GetBitmaps().size())
	DefGetter(const ynothrow, size_t, Index, index)
	//@}
	//! \since build 576
	DefGetter(const ynothrow, ImageScale, Scale, scale)
	DefGetter(const ynothrow, const Size&, ViewSize, view_size)

	//! \pre 断言： <tt>Brush.ImagePtr</tt> 。
	void
	AdjustOffset(const Size&);

private:
	//! \since build 554
	void
	LoadContent();

public:
	/*
	\pre 断言： <tt>Brush.ImagePtr</tt> 。
	\since build 554
	*/
	void
	Resize(const Size&);

	/*!
	\return 是否切换了不同页面。
	\since build 461
	*/
	bool
	SwitchPage(size_t);

	//!\return 是否切换了不同页面。
	PDefH(bool, SwitchPageDiff, ptrdiff_t diff)
		ImplRet(SwitchPage(size_t(ptrdiff_t(index) + diff) % GetCount()))

	/*!
	\return 是否成功进行了缩放。
	\note 偏移量相对于画刷左上角。
	*/
	//@{
	/*!
	\brief 按指定的比例变化量和偏移量缩放。
	\since build 558
	*/
	bool
	Zoom(float, const Point&);

	//! \brief 按指定的比例和偏移量缩放。
	bool
	ZoomByRatio(float, const Point&);

	/*!
	\brief 按指定的比例和偏移量缩放。
	\since build 580
	*/
	bool
	ZoomTo(float, const Point&);
	//@}
};

} // namespace Drawing;

} // namespace YSLib;

#endif

