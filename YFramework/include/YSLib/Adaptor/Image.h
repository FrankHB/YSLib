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
\version r198
\author FrankHB <frankhb1989@gmail.com>
\since build 402
\par 创建时间:
	2013-05-05 12:34:03 +0800
\par 修改时间:
	2013-07-15 06:22 +0800
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

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

//! \since build 418
class CompactPixmap;


//! \since build 402
typedef u8 BitPerPixel;


/*!
\brief 图像格式。
\note ::FREE_IMAGE_FORMAT 。
\see FreeImage 宏 FI_ENUM 。
\since build 417
*/
typedef int ImageFormat;


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
	typedef ::FIMEMORY* NativeHandle;

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
	typedef ::FIBITMAP* DataPtr;

private:
	DataPtr bitmap;

public:
	//! \throw BadImageAlloc 分配空间失败。
	HBitmap(const Size&, BitPerPixel = 0);
	//! \since build 417
	//@{
	/*
	\brief 构造：使用现有数据指针。
	\post 断言检查： bitmap 非空。
	\note 取得所有权。
	*/
	HBitmap(DataPtr) ynothrow;
	/*
	\brief 构造：使用指定文件路径。
	\throw UnknownImageFormat 未知图像格式。
	\throw LoggedEvent 读取失败。
	*/
	HBitmap(const string&);
	//! \throw LoggedEvent 读取失败。
	HBitmap(const ImageMemory&);
	//! \throw BadImageAlloc 分配空间失败。
	HBitmap(const HBitmap&);
	HBitmap(HBitmap&&) ynothrow;
	//! \since build 428
	~HBitmap() ynothrow;
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
};


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

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

