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
\version r169
\author FrankHB <frankhb1989@gmail.com>
\since build 402
\par 创建时间:
	2013-05-05 12:34:03 +0800
\par 修改时间:
	2013-06-26 22:00 +0800
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

//! \since build 417
class BitmapBuffer;


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
	~ImageMemory();
	
	DefGetter(const ynothrow, ImageFormat, Format, format)
	DefGetter(const ynothrow, NativeHandle, NativeHandle, handle)
};
//@}


//! \since build 402
class YF_API NativePixmap final
{
public:
	typedef ::FIBITMAP* DataPtr;

private:
	DataPtr bitmap;

public:
	//! \throw BadImageAlloc 分配空间失败。
	NativePixmap(const Size&, BitPerPixel = 0);
	//! \since build 417
	//@{
	/*
	\brief 构造：使用现有数据指针。
	\post 断言检查： bitmap 非空。
	\note 取得所有权。
	*/
	NativePixmap(DataPtr) ynothrow;
	/*
	\brief 构造：使用指定文件路径。
	\throw UnknownImageFormat 未知图像格式。
	\throw LoggedEvent 读取失败。
	\since build 417
	*/
	NativePixmap(const string&);
	//! \throw LoggedEvent 读取失败。
	NativePixmap(const ImageMemory&);
	//! \throw BadImageAlloc 分配空间失败。
	NativePixmap(const NativePixmap&);
	NativePixmap(NativePixmap&&) ynothrow;
	~NativePixmap();
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
	~ImageCodec();

	static BitmapBuffer
	Load(const vector<octet>&);

	static BitmapBuffer
	Convert(const NativePixmap&);
//	static BitmapBuffer
//	Convert(NativePixmap&&);
};

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

