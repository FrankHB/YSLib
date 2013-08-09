/*
	Copyright by FrankHB 2009 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Image.h
\ingroup Adaptor
\brief 平台中立的图像输入和输出。
\version r310
\author FrankHB <frankhb1989@gmail.com>
\since build 402
\par 创建时间:
	2013-05-05 12:34:03 +0800
\par 修改时间:
	2013-08-05 21:03 +0800
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
\note 和 ::FREE_IMAGE_FORMAT 兼容。
\see FreeImage 宏 FI_ENUM 。
\since build 417
*/
using ImageFormat = int;


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
	//! \since build 428
	~ImageMemory() ynothrow;

	DefGetter(const ynothrow, ImageFormat, Format, format)
	DefGetter(const ynothrow, NativeHandle, NativeHandle, handle)
};
//@}


/*!
\brief 位图句柄：指向位图数据。
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
	\since build 431
	*/
	//@{
	/*
	\brief 构造：使用指定 UTF-8 文件名。
	\throw UnknownImageFormat 未知图像格式。
	*/
	HBitmap(const char*);
	//! \brief 构造：使用指定 UTF-8 文件名和指定格式。
	HBitmap(const char*, ImageFormat);
	/*
	\brief 构造：使用指定 UTF-16LE 文件名。
	\throw UnknownImageFormat 未知图像格式。
	*/
	HBitmap(const char16_t*);
	//! \brief 构造：使用指定 UTF-16LE 文件名和指定格式。
	HBitmap(const char16_t*, ImageFormat);
	/*
	\brief 构造：使用指定字符串文件名。
	\throw UnknownImageFormat 未知图像格式。
	*/
	template<class _tString, typename = decltype(&_tString()[0])>
	HBitmap(const _tString& filename)
		: HBitmap(&filename[0])
	{}
	//! \brief 构造：使用指定字符串文件名。
	template<class _tString, typename = decltype(&_tString()[0])>
	HBitmap(const _tString& filename, ImageFormat fmt)
		: HBitmap(&filename[0], fmt)
	{}
	//@}
	//! \since build 417
	//@{
	//! \throw LoggedEvent 读取失败。
	HBitmap(const ImageMemory&);
	/*!
	\brief 构造指定图像缩放至指定大小的副本。
	\throw LoggedEvent 缩放失败。
	\since build 430
	*/
	HBitmap(const HBitmap&, const Size&, SamplingFilter);
	//! \throw BadImageAlloc 分配空间失败。
	HBitmap(const HBitmap&);
	HBitmap(HBitmap&&) ynothrow;
	//! \since build 428
	~HBitmap() ynothrow;
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

//! \since build 430
inline DefSwap(ynothrow, HBitmap)


//! \since build 417
class YF_API ImageCodec final
{
public:
	ImageCodec();
	//! \since build 428
	~ImageCodec() ynothrow;

	//! \since build 418
	//@{
	static CompactPixmap
	Load(const vector<octet>&);

	static CompactPixmap
	Convert(const HBitmap&);
	//@}
//	static CompactPixmap
//	Convert(HBitmap&&);
};

} // namespace Drawing;

} // namespace YSLib;

#endif

