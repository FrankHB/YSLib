/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Image.cpp
\ingroup Adaptor
\brief 平台中立的图像输入和输出。
\version r294
\author FrankHB <frankhb1989@gmail.com>
\since build 402
\par 创建时间:
	2013-05-05 12:33:51 +0800
\par 修改时间:
	2013-08-05 21:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::Image
*/


#include "YSLib/Adaptor/Image.h"
#include <FreeImage.h>
#include "YSLib/Service/yblit.h"
#include "YSLib/Service/ygdi.h"
#include <CHRLib/chrproc.h> // for CHRLib::ToASCII;

namespace YSLib
{

namespace Drawing
{

//! \since build 430
static_assert(int(SamplingFilter::Box) == FILTER_BOX && int(
	SamplingFilter::Bicubic) == FILTER_BICUBIC && int(SamplingFilter::Bilinear)
	== FILTER_BILINEAR && int(SamplingFilter::BSpline) == FILTER_BSPLINE && int(
	SamplingFilter::CatmullRom) == FILTER_CATMULLROM && int(
	SamplingFilter::Lanczos3) == FILTER_LANCZOS3, "Incompatible filter found.");

//! \since build 431
namespace
{

::FreeImageIO u8_io{
	[](void *buffer, unsigned size, unsigned nmemb, fi_handle h){
		return unsigned(std::fread(buffer, std::size_t(size),
			std::size_t(nmemb), static_cast<std::FILE*>(h)));
	},
	[](void *buffer, unsigned size, unsigned nmemb, fi_handle h){
		return unsigned(std::fwrite(buffer, std::size_t(size),
			std::size_t(nmemb), static_cast<std::FILE*>(h)));
	},
	[](::fi_handle h, long offset, int whence){
		return std::fseek(static_cast<std::FILE*>(h), offset, whence);
	},
	[](::fi_handle h){
		return std::ftell(static_cast<std::FILE*>(h));
	}
};

::FIBITMAP*
LoadImage(ImageFormat fmt, std::FILE* fp, int flags) ynothrow
{
	if(fp)
	{
		const auto bitmap(::FreeImage_LoadFromHandle(::FREE_IMAGE_FORMAT(fmt),
			&u8_io, ::fi_handle(fp), flags));

		std::fclose(fp);
		return bitmap;
	}
	return {};
}
::FIBITMAP*
LoadImage(ImageFormat fmt, const char* filename, int flags = 0) ynothrow
{
	return LoadImage(fmt, ufopen(filename, "rb"), flags);
}
::FIBITMAP*
LoadImage(ImageFormat fmt, const char16_t* filename, int flags = 0) ynothrow
{
	return LoadImage(fmt, ufopen(filename, u"rb"), flags);
}

ImageFormat
GetFormatFromFilename(const char16_t* filename)
{
	const auto len(std::char_traits<char16_t>::length(filename));
	const unique_ptr<char[]> p(new char[len]);
	const auto str(p.get());

	for(size_t i{}; i < len; ++i)
		str[i] = CHRLib::ToASCII(filename[i]);
	str[len] = u'\0';
	return ::FreeImage_GetFIFFromFilename(str);
}

} // unnamed namespace;


ImageMemory::ImageMemory(octet* p, size_t size)
	: handle(::FreeImage_OpenMemory(static_cast<byte*>(p), static_cast<
	::DWORD>(size))), format(::FreeImage_GetFileTypeFromMemory(handle, size))
{
	if(!handle)
		throw LoggedEvent("Opening image memory failed.");
}
ImageMemory::~ImageMemory() ynothrow
{
	::FreeImage_CloseMemory(handle);
}


HBitmap::HBitmap(const Size& s, BitPerPixel bpp)
	: bitmap(::FreeImage_Allocate(s.Width, s.Height, bpp, 0, 0, 0))
{
	if(!bitmap)
		throw BadImageAlloc();
}
HBitmap::HBitmap(const char* filename)
	: HBitmap(filename, ::FreeImage_GetFIFFromFilename(filename))
{}
HBitmap::HBitmap(const char* filename, ImageFormat fmt)
	: bitmap(LoadImage(fmt, filename))
{
	if(!bitmap)
		throw LoggedEvent("Loading image failed.");
}
HBitmap::HBitmap(const char16_t* filename)
	: HBitmap(filename, GetFormatFromFilename(filename))
{}
HBitmap::HBitmap(const char16_t* filename, ImageFormat fmt)
	: bitmap(LoadImage(fmt, filename))
{
	if(!bitmap)
		throw LoggedEvent("Loading image failed.");
}
HBitmap::HBitmap(const ImageMemory& mem)
	: bitmap(::FreeImage_LoadFromMemory(::FREE_IMAGE_FORMAT(mem.GetFormat()),
	mem.GetNativeHandle(), 0))
{
	if(!bitmap)
		throw LoggedEvent("Loading image failed.");
}
HBitmap::HBitmap(const HBitmap& pixmap, const Size& s, SamplingFilter sf)
	: bitmap(::FreeImage_Rescale(pixmap.bitmap, s.Width, s.Height,
	::FREE_IMAGE_FILTER(sf)))
{
	if(!bitmap)
		throw LoggedEvent("Rescaling image failed.");
}
HBitmap::HBitmap(const HBitmap& pixmap)
	: bitmap(::FreeImage_Clone(pixmap.bitmap))
{
	if(!bitmap)
		throw BadImageAlloc();
}
HBitmap::HBitmap(HBitmap&& pixmap) ynothrow
	: bitmap(pixmap.bitmap)
{
	pixmap.bitmap = {};
}
HBitmap::~HBitmap() ynothrow
{
	::FreeImage_Unload(bitmap);
}

BitPerPixel
HBitmap::GetBPP() const ynothrow
{
	return ::FreeImage_GetBPP(bitmap);
}
SDst
HBitmap::GetHeight() const ynothrow
{
	return ::FreeImage_GetHeight(bitmap);
}
SDst
HBitmap::GetPitch() const ynothrow
{
	return ::FreeImage_GetPitch(bitmap);
}
SDst
HBitmap::GetWidth() const ynothrow
{
	return ::FreeImage_GetWidth(bitmap);
}

void
HBitmap::Rescale(const Size& s, SamplingFilter sf)
{
	*this = HBitmap(*this, s, sf);
}


ImageCodec::ImageCodec()
{
	::FreeImage_Initialise(false);
}
ImageCodec::~ImageCodec() ynothrow
{
	::FreeImage_DeInitialise();
}

CompactPixmap
ImageCodec::Load(const vector<octet>& vec)
{
	ImageMemory mem(const_cast<octet*>(&vec[0]), vec.size());

	if(mem.GetFormat() == FIF_UNKNOWN)
		throw UnknownImageFormat("Unknown image format found when loading.");

	return Convert(HBitmap(mem));
}

CompactPixmap
ImageCodec::Convert(const HBitmap& pixmap)
{
#if (YCL_PIXEL_FORMAT_XYZ555 & 0x00FFFFFF) == 0x00BBCCDD
	const Size& s(pixmap.GetSize());
	unique_ptr<PixelType[]> pixels(new PixelType[GetAreaOf(s)]);

	FreeImage_ConvertToRawBits(reinterpret_cast<byte*>(&pixels[0]),
		pixmap.GetDataPtr(), s.Width * sizeof(PixelType), 16, FI16_555_RED_MASK,
		FI16_555_GREEN_MASK, FI16_555_BLUE_MASK, true);
	return CompactPixmap(std::move(pixels), s);
//#elif (YCL_PIXEL_FORMAT_XYZ555 & 0x00FFFFFF) == 0x00DDCCBB
//#elif (YCL_PIXEL_FORMAT_XYZ888 & 0x00FFFFFF) == 0x00BBCCDD
#elif (YCL_PIXEL_FORMAT_XYZ888 & 0x00FFFFFF) == 0x00DDCCBB
	const Size& s(pixmap.GetSize());
	unique_ptr<PixelType[]> pixels(new PixelType[GetAreaOf(s)]);

	FreeImage_ConvertToRawBits(reinterpret_cast<byte*>(&pixels[0]),
		pixmap.GetDataPtr(), s.Width * sizeof(PixelType), 32, FI_RGBA_RED_MASK,
		FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, true);
#	if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
//#	elif FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
	// TODO: Add support for different color order.
#	else
#		error "No supported FreeImage pixel format for RGB888 found."
#	endif
	return CompactPixmap(std::move(pixels), s);
#else
#	error "Unsupported pixel format found."
#endif
}

} // namespace Drawing;

} // namespace YSLib;

