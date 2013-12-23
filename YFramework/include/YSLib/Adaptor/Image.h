/*
	© 2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Image.h
\ingroup Adaptor
\brief 平台中立的图像输入和输出。
\version r605
\author FrankHB <frankhb1989@gmail.com>
\since build 402
\par 创建时间:
	2013-05-05 12:34:03 +0800
\par 修改时间:
	2013-12-14 23:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::Image
*/


#ifndef YSL_INC_Adaptor_Image_h_
#define YSL_INC_Adaptor_Image_h_ 1

#include "../Core/ygdibase.h"
#include "ycont.h"

//包含 FreeImage 。
//#include <FreeImage.h>

//! \since build 402
struct FIBITMAP;
//! \since build 417
struct FIMEMORY;

namespace YSLib
{

namespace Drawing
{

//! \since build 418
class CompactPixmap;


//! \since build 402
using BitPerPixel = u8;


/*!
\brief 图像格式。
\note 枚举值和 ::FREE_IMAGE_FORMAT 兼容。
\see FreeImage 宏 FI_ENUM 。
\since build 457
*/
enum class ImageFormat : int
{
	Unknown = -1,
	BMP = 0,
	ICO = 1,
	JPEG = 2,
	GIF = 25
};


/*!
\brief 图像解码器标识。
\note 数值对应 FreeImage 实现。
\since build 457
*/
enum class ImageDecoderFlags : int
{
	Default = 0,
	GIF_Load256 = 1,
	GIF_Playback = 2,
	ICO_MakeAlpha = 1,
	JPEG_Fast = 0x0001,
	JPEG_Accurate = 0x0002,
	JPEG_CMYK = 0x0004,
	JPEG_EXIFRotate = 0x0008,
	JPEG_GreyScale = 0x0010,
	JPEG_QualitySuperb = 0x80,
	JPEG_QualityGood = 0x0100,
	JPEG_QualityNormal = 0x0200,
	JPEG_QualityAverage = 0x0400,
	JPEG_QualityBad = 0x0800,
	JPEG_Progressive = 0x2000,
	JPEG_Subsampling_411 = 0x1000,
	JPEG_Subsampling_420 = 0x4000,
	JPEG_Subsampling_422 = 0x8000,
	JPEG_Subsampling_444 = 0x10000,
	JPEG_Optimize = 0x20000,
	JPEG_Baseline = 0x40000
};

/*!
\relates ImageDecoderFlags
\since build 457
*/
DefBitmaskEnum(ImageDecoderFlags)


/*!
\brief 采样过滤算法。
\note 和 ::FREE_IMAGE_FILTER 兼容。
\see FreeImage 宏 FI_ENUM 。
\since build 430
*/
enum class SamplingFilter
{
	Box = 0,
	Bicubic = 1,
	Bilinear = 2,
	BSpline = 3,
	CatmullRom = 4,
	Lanczos3 = 5
};


//! \since build 402
class YF_API BadImageAlloc : public std::bad_alloc
{};


//! \since build 417
//@{
class YF_API UnknownImageFormat : public LoggedEvent
{
public:
	UnknownImageFormat(const std::string& str)
		: LoggedEvent(str)
	{}
};


class YF_API ImageMemory final
{
public:
	using NativeHandle = ::FIMEMORY*;

private:
	NativeHandle handle;
	ImageFormat format;

public:
	ImageMemory(octet* = {}, size_t = 0);
	//! \since build 461
	~ImageMemory();

	DefGetter(const ynothrow, ImageFormat, Format, format)
	DefGetter(const ynothrow, NativeHandle, NativeHandle, handle)
};
//@}


/*!
\brief 位图句柄：指向位图数据。
\note 内部数据的平凡容器：深复制且可转移。
\since build 418
*/
class YF_API HBitmap final
{
public:
	using DataPtr = ::FIBITMAP*;

private:
	DataPtr bitmap;

public:
	/*
	\brief 构造：使用现有数据指针。
	\note 取得所有权。
	\since build 430
	*/
	HBitmap(DataPtr ptr = {}) ynothrow
		: bitmap(ptr)
	{}
	//! \throw BadImageAlloc 分配空间失败。
	HBitmap(const Size&, BitPerPixel = 0);
	/*!
	\throw LoggedEvent 读取失败。
	\since build 457
	*/
	//@{
	/*
	\brief 构造：使用指定 UTF-8 文件名和解码器标识。
	\throw UnknownImageFormat 未知图像格式。
	*/
	HBitmap(const char*, ImageDecoderFlags = ImageDecoderFlags::Default);
	//! \brief 构造：使用指定 UTF-8 文件名、指定格式和解码器标识。
	HBitmap(const char*, ImageFormat,
		ImageDecoderFlags = ImageDecoderFlags::Default);
	/*
	\brief 构造：使用指定 UTF-16LE 文件名和解码器标识。
	\throw UnknownImageFormat 未知图像格式。
	*/
	HBitmap(const char16_t*, ImageDecoderFlags = ImageDecoderFlags::Default);
	//! \brief 构造：使用指定 UTF-16LE 文件名、指定格式和解码器标识。
	HBitmap(const char16_t*, ImageFormat,
		ImageDecoderFlags = ImageDecoderFlags::Default);
	/*
	\brief 构造：使用指定字符串文件名和解码器标识。
	\throw UnknownImageFormat 未知图像格式。
	*/
	template<class _tString, typename = decltype(&_tString()[0])>
	HBitmap(const _tString& filename,
		ImageDecoderFlags = ImageDecoderFlags::Default)
		: HBitmap(&filename[0])
	{}
	//! \brief 构造：使用指定字符串文件名和解码器标识。
	template<class _tString, typename = decltype(&_tString()[0])>
	HBitmap(const _tString& filename, ImageFormat fmt,
		ImageDecoderFlags = ImageDecoderFlags::Default)
		: HBitmap(&filename[0], fmt)
	{}
	//@}
	/*!
	\throw LoggedEvent 读取失败。
	\since build 457
	*/
	HBitmap(const ImageMemory&, ImageDecoderFlags = ImageDecoderFlags::Default);
	//! \since build 417
	//@{
	/*!
	\brief 构造指定图像缩放至指定大小的副本。
	\throw LoggedEvent 缩放失败。
	\since build 430
	*/
	HBitmap(const HBitmap&, const Size&, SamplingFilter);
	//! \throw BadImageAlloc 分配空间失败。
	HBitmap(const HBitmap&);
	HBitmap(HBitmap&&) ynothrow;
	//! \since build 461
	~HBitmap();
	//@}

	//! \since build 430
	//@{
	//! \brief 统一赋值：使用值参数和交换函数进行复制或转移赋值。
	HBitmap&
	operator=(HBitmap pixmap) ynothrow
	{
		pixmap.swap(*this);
		return *this;
	}

	PDefHOp(bool, !, ) const ynothrow
		ImplRet(!bitmap)

	explicit DefCvt(const ynothrow, bool, bitmap)
	//@}

	BitPerPixel
	GetBPP() const ynothrow;
	//! \since build 417
	DefGetter(const ynothrow, DataPtr, DataPtr, bitmap)
	SDst
	GetHeight() const ynothrow;
	//! \since build 417
	DefGetter(const ynothrow, Size, Size, {GetWidth(), GetHeight()});
	//! \since build 417
	SDst
	GetPitch() const ynothrow;
	SDst
	GetWidth() const ynothrow;

	/*!
	\brief 缩放为指定大小。
	\since build 430
	*/
	void
	Rescale(const Size&, SamplingFilter = SamplingFilter::Box);

	/*
	\brief 交换。
	\since build 430
	*/
	PDefH(void, swap, HBitmap& pixmap) ynothrow
		ImplExpr(std::swap(bitmap, pixmap.bitmap))
};

/*!
\relates HBitmap
\since build 430
*/
inline DefSwap(ynothrow, HBitmap)


//! \since build 456
//@{
/*!
\brief 多页面位图数据。
\note 非公开实现。
*/
class MultiBitmapData;

/*!
\brief 多页面位图句柄：指向多页面位图数据。
\note 共享复制且可转移。
\todo 增加使用 ImageMemory 的构造函数。
*/
class YF_API HMultiBitmap final
{
public:
	using DataPtr = shared_ptr<MultiBitmapData>;
	//! \since build 460
	//@{
	class YF_API iterator : public std::iterator<std::input_iterator_tag,
		HBitmap, ptrdiff_t, const HBitmap*, HBitmap>
	{
	private:
		const HMultiBitmap* p_bitmaps;
		size_t index;

	public:
		iterator()
			: p_bitmaps()
		{}
		iterator(const HMultiBitmap& bmps, size_t idx = 0)
			: p_bitmaps(&bmps), index(idx)
		{}

		iterator&
		operator++() ynothrowv;

		reference
		operator*() const;

		YF_API friend bool
		operator==(const iterator&, const iterator&) ynothrow;

		DefGetter(const ynothrow, const HMultiBitmap*, HMultiBitmapPtr,
			p_bitmaps)
		DefGetter(const ynothrow, size_t, Index, index)
	};
	using const_iterator = iterator;
	//@}

private:
	DataPtr pages;

public:
	/*!
	\throw LoggedEvent 读取失败。
	\note 非多页面读取结果为空。
	\since build 457
	*/
	//@{
	/*
	\brief 构造：使用指定 UTF-8 文件名和解码器标识。
	\throw UnknownImageFormat 未知图像格式。
	*/
	HMultiBitmap(const char*, ImageDecoderFlags = ImageDecoderFlags::Default);
	//! \brief 构造：使用指定 UTF-8 文件名、指定格式和解码器标识。
	HMultiBitmap(const char*, ImageFormat,
		ImageDecoderFlags = ImageDecoderFlags::Default);
	/*
	\brief 构造：使用指定 UCS-2LE 文件名和解码器标识。
	\throw UnknownImageFormat 未知图像格式。
	*/
	HMultiBitmap(const char16_t*,
		ImageDecoderFlags = ImageDecoderFlags::Default);
	//! \brief 构造：使用指定 UCS-2LE 文件名、指定格式和解码器标识。
	HMultiBitmap(const char16_t*, ImageFormat,
		ImageDecoderFlags = ImageDecoderFlags::Default);
	/*
	\brief 构造：使用指定字符串文件名和解码器标识。
	\throw UnknownImageFormat 未知图像格式。
	*/
	template<class _tString, typename = decltype(&_tString()[0])>
	HMultiBitmap(const _tString& filename,
		ImageDecoderFlags flags = ImageDecoderFlags::Default)
		: HMultiBitmap(&filename[0], flags)
	{}
	//! \brief 构造：使用指定字符串文件名和解码器标识。
	template<class _tString, typename = decltype(&_tString()[0])>
	HMultiBitmap(const _tString& filename, ImageFormat fmt,
		ImageDecoderFlags flags = ImageDecoderFlags::Default)
		: HMultiBitmap(&filename[0], fmt, flags)
	{}
	//@}
	DefDeCopyCtor(HMultiBitmap)
	DefDeMoveCtor(HMultiBitmap)

	DefDeCopyAssignment(HMultiBitmap)
	DefDeMoveAssignment(HMultiBitmap)

	PDefHOp(bool, !, ) const ynothrow
		ImplRet(!pages)

	explicit DefCvt(const ynothrow, bool, bool(pages))

	size_t
	GetPageCount() const ynothrow;

	HBitmap
	Lock(size_t = 0) const;

	//! \brief 交换。
	PDefH(void, swap, HMultiBitmap& multi_pixmap) ynothrow
		ImplExpr(std::swap(pages, multi_pixmap.pages))

	//! \since build 461
	//@{
	PDefH(iterator, begin, ) const ynothrow
		ImplRet(GetPageCount() != 0 ? HMultiBitmap::iterator(*this)
			: HMultiBitmap::iterator())

	PDefH(iterator, end, ) const ynothrow
		ImplRet(HMultiBitmap::iterator())
	//@}
};

inline HMultiBitmap::iterator&
HMultiBitmap::iterator::operator++() ynothrowv
{
	YAssert(p_bitmaps, "Null pointer found.");

	if(++index == p_bitmaps->GetPageCount())
		p_bitmaps = {};
	return *this;
}

inline HMultiBitmap::iterator::reference
HMultiBitmap::iterator::operator*() const
{
	YAssert(p_bitmaps, "Null pointer found.");

	return p_bitmaps->Lock(index);
}

inline bool
operator!=(const HMultiBitmap::iterator& x, const HMultiBitmap::iterator& y)
	ynothrow
{
	return !(x == y);
}

//! \relates HMultiBitmap::iterator
inline bool
is_undereferenceable(const HMultiBitmap::iterator& i) ynothrow
{
	return !i.GetHMultiBitmapPtr();
}

//! \relates HMultiBitmap
inline DefSwap(ynothrow, HMultiBitmap)
//@}


//! \since build 417
class YF_API ImageCodec final
{
public:
	ImageCodec();
	//! \since build 461
	~ImageCodec();

	//! \since build 418
	static CompactPixmap
	Convert(const HBitmap&);
//	static CompactPixmap
//	Convert(HBitmap&&);

	/*!
	\brief 检测图像格式。
	\note 对于文件，若根据内容检测失败则根据扩展名判断（不保证正确性）。
	\since build 457
	*/
	//@{
	//! \note 使用图像内存的本机句柄和大小。
	static ImageFormat
	DetectFormat(ImageMemory::NativeHandle, size_t);
	//! \note 使用指定 UTF-8 文件名。
	static ImageFormat
	DetectFormat(const char*);
	//! \note 使用指定 UCS-2LE 文件名。
	static ImageFormat
	DetectFormat(const char16_t*);
	//@}

	//! \since build 418
	static CompactPixmap
	Load(const vector<octet>&);

	/*!
	\brief 读取指定路径的多页面图片文件为用于直接呈现的帧序列。
	\note 非多页面读取结果为空。
	\note 对多页面图片自动选择解码器标识（当前仅支持 GIF 格式）；其它为默认参数。
	\since build 458
	*/
	//@{
	//! \note 使用指定 UTF-8 文件名。
	static HMultiBitmap
	LoadForPlaying(const char*);
	//! \note 使用指定 UCS-2LE 文件名。
	static HMultiBitmap
	LoadForPlaying(const char16_t*);
	//! \brief 构造：使用指定字符串文件名。
	template<class _tString, typename = decltype(&_tString()[0])>
	static HMultiBitmap
	LoadForPlaying(const _tString& filename)
	{
		return LoadForPlaying(&filename[0]);
	}
	//@}

	/*!
	\brief 读取指定路径的图片文件为用于直接呈现的帧序列。
	\note 使用 LoadForPlaying 按多页面读取，若结果为空按单页面读取。
	\sa LoadForPlaying
	\since build 461
	*/
	template<class _tSeqCon, typename _type>
	static _tSeqCon
	LoadSequence(const _type& path)
	{
		const auto multi_bitmap(LoadForPlaying(path));
		_tSeqCon con{multi_bitmap.begin(), multi_bitmap.end()};

		if(con.empty())
			con.emplace_back(path);
		return std::move(con);
	}
};

} // namespace Drawing;

} // namespace YSLib;

#endif

