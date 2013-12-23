/*
	© 2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Image.cpp
\ingroup Adaptor
\brief 平台中立的图像输入和输出。
\version r555
\author FrankHB <frankhb1989@gmail.com>
\since build 402
\par 创建时间:
	2013-05-05 12:33:51 +0800
\par 修改时间:
	2013-12-14 00:26 +0800
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

//! \since build 456
void DLL_CALLCONV
FI_OutputMessage(::FREE_IMAGE_FORMAT fif, const char* msg)
{
	YTraceDe(Warning, "FreeImage failed, format = %d: %s.\n", int(fif), msg);

	yunused(fif),
	yunused(msg);
}

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

//! \since build 457
//@{
ImageFormat
GetFileType(std::FILE* fp)
{
	if(fp)
	{
		const auto
			fif(::FreeImage_GetFileTypeFromHandle(&u8_io, ::fi_handle(fp)));

		std::fclose(fp);
		return ImageFormat(fif);
	}
	return ImageFormat::Unknown;
}
ImageFormat
GetFileType(const char* filename)
{
	return GetFileType(ufopen(filename, "rb"));
}
ImageFormat
GetFileType(const char16_t* filename)
{
	return GetFileType(ufopen(filename, u"rb"));
}
//@}

ImageFormat
GetFormatFromFilename(const char16_t* filename)
{
	const auto len(std::char_traits<char16_t>::length(filename));
	const unique_ptr<char[]> p(new char[len + 1]);
	const auto str(p.get());

	for(size_t i{}; i < len; ++i)
		str[i] = CHRLib::ToASCII(filename[i]);
	str[len] = u'\0';
	return ImageFormat(::FreeImage_GetFIFFromFilename(str));
}

FI_PluginRec&
LookupPlugin(::FREE_IMAGE_FORMAT fif)
{
	if(auto p_node = ::FreeImageEx_GetPluginNodeFromFIF(fif))
	{
		if(const auto p_plugin = p_node->m_plugin)
			return *p_plugin;
		else
			throw LoggedEvent("Invalid plugin node found.");
	}
	else
		throw LoggedEvent("No proper plugin found.");
}

} // unnamed namespace;


ImageMemory::ImageMemory(octet* p, size_t size)
	: handle(::FreeImage_OpenMemory(static_cast<byte*>(p), static_cast<
	::DWORD>(size))), format(ImageCodec::DetectFormat(handle, size))
{
	if(!handle)
		throw LoggedEvent("Opening image memory failed.");
}
ImageMemory::~ImageMemory()
{
	::FreeImage_CloseMemory(handle);
}


HBitmap::HBitmap(const Size& s, BitPerPixel bpp)
	: bitmap(::FreeImage_Allocate(s.Width, s.Height, bpp, 0, 0, 0))
{
	if(!bitmap)
		throw BadImageAlloc();
}
HBitmap::HBitmap(const char* filename, ImageDecoderFlags flags)
	: HBitmap(filename, ImageCodec::DetectFormat(filename), flags)
{}
HBitmap::HBitmap(const char* filename, ImageFormat fmt, ImageDecoderFlags flags)
	: bitmap(LoadImage(fmt, filename, int(flags)))
{
	if(!bitmap)
		throw LoggedEvent("Loading image failed.");
}
HBitmap::HBitmap(const char16_t* filename, ImageDecoderFlags flags)
	: HBitmap(filename, ImageCodec::DetectFormat(filename), flags)
{}
HBitmap::HBitmap(const char16_t* filename, ImageFormat fmt,
	ImageDecoderFlags flags)
	: bitmap(LoadImage(fmt, filename, int(flags)))
{
	if(!bitmap)
		throw LoggedEvent("Loading image failed.");
}
HBitmap::HBitmap(const ImageMemory& mem, ImageDecoderFlags flags)
	: bitmap(::FreeImage_LoadFromMemory(::FREE_IMAGE_FORMAT(mem.GetFormat()),
	mem.GetNativeHandle(), int(flags)))
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
HBitmap::~HBitmap()
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


class MultiBitmapData final : private noncopyable
{
private:
	bool read;
	::fi_handle handle;
	int load_flags;
	std::reference_wrapper< ::FreeImageIO> io_ref;
	std::reference_wrapper< ::FI_PluginRec> plugin_ref;
	size_t page_count = 0;
	void* data = {};

public:
	MultiBitmapData(::fi_handle, int, ::FI_PluginRec&, ::FreeImageIO& = u8_io,
		bool = true) ynothrow;
	MultiBitmapData(ImageFormat, std::FILE&, int = 0, ::FreeImageIO& = u8_io,
		bool = true) ynothrow;
	//! \since build 461
	~MultiBitmapData();

	DefPred(const ynothrow, OpenForRead, read)

	DefGetter(const ynothrow, size_t, PageCount, page_count)

	::FIBITMAP*
	LockPage(size_t = 0) const ynothrow;
};

MultiBitmapData::MultiBitmapData(::fi_handle h, int flags,
	::FI_PluginRec& plugin, ::FreeImageIO& io, bool open_for_reading) ynothrow
	: read(open_for_reading), handle(h), load_flags(flags), io_ref(io),
	plugin_ref(plugin)
{
	io.seek_proc(handle, 0, SEEK_SET);
	if(const auto open = plugin.open_proc)
	{
		data = open(&io_ref.get(), handle, open_for_reading);
		if(const auto proc = plugin_ref.get().pagecount_proc)
			page_count = proc(&io_ref.get(), handle, data);
		else
			page_count = 1;
	}
}
MultiBitmapData::MultiBitmapData(ImageFormat fmt, std::FILE& f, int flags,
	::FreeImageIO& io, bool open_for_reading) ynothrow
	: MultiBitmapData(::fi_handle(&f), flags,
	LookupPlugin(::FREE_IMAGE_FORMAT(fmt)), io, open_for_reading)
{}
MultiBitmapData::~MultiBitmapData()
{
	if(const auto close = plugin_ref.get().close_proc)
		close(&io_ref.get(), handle, data);
}

::FIBITMAP*
MultiBitmapData::LockPage(size_t index) const ynothrow
{
	YAssert(index < page_count, "Invalid page index found.");

	if(const auto load = plugin_ref.get().load_proc)
		return load(&io_ref.get(), handle, int(index), load_flags, data);
	return {};
}


//! \since build 456
namespace
{

MultiBitmapData*
LoadImagePages(ImageFormat fmt, std::FILE* fp, int flags) ynothrow
{
	if(fp)
		try
		{
			return new MultiBitmapData(fmt, *fp, flags);
		}
		catch(std::exception&)
		{}
	return {};
}
MultiBitmapData*
LoadImagePages(ImageFormat fmt, const char* filename, int flags = 0) ynothrow
{
	return LoadImagePages(fmt, ufopen(filename, "rb"), flags);
}
MultiBitmapData*
LoadImagePages(ImageFormat fmt, const char16_t* filename, int flags = 0)
	ynothrow
{
	return LoadImagePages(fmt, ufopen(filename, u"rb"), flags);
}

} // unnamed namespace;


bool
operator==(const HMultiBitmap::iterator& x, const HMultiBitmap::iterator& y)
	ynothrow
{
	return x.p_bitmaps == y.p_bitmaps && (!x.p_bitmaps || x.index == y.index);
}

HMultiBitmap::HMultiBitmap(const char* filename, ImageDecoderFlags flags)
	: HMultiBitmap(filename, ImageCodec::DetectFormat(filename), flags)
{}
HMultiBitmap::HMultiBitmap(const char* filename, ImageFormat fmt,
	ImageDecoderFlags flags)
	: pages(LoadImagePages(fmt, filename, int(flags)))
{
	if(!pages)
		throw LoggedEvent("Loading image pages failed.");
}
HMultiBitmap::HMultiBitmap(const char16_t* filename, ImageDecoderFlags flags)
	: HMultiBitmap(filename, ImageCodec::DetectFormat(filename), flags)
{}
HMultiBitmap::HMultiBitmap(const char16_t* filename, ImageFormat fmt,
	ImageDecoderFlags flags)
	: pages(LoadImagePages(fmt, filename, int(flags)))
{
	if(!pages)
		throw LoggedEvent("Loading image pages failed.");
}

size_t
HMultiBitmap::GetPageCount() const ynothrow
{
	return pages ? pages->GetPageCount() : 0;
}

HBitmap
HMultiBitmap::Lock(size_t i) const
{
	return pages ? pages->LockPage(i) : nullptr;
}


ImageCodec::ImageCodec()
{
	// XXX: Thread safety of errno.
	const auto old_errno(errno);

	::FreeImage_Initialise(false);
	::FreeImage_SetOutputMessageStdCall(FI_OutputMessage);
	errno = old_errno;
}
ImageCodec::~ImageCodec()
{
	::FreeImage_DeInitialise();
}

CompactPixmap
ImageCodec::Convert(const HBitmap& pixmap)
{
#if (YCL_PIXEL_FORMAT_XYZ555 & 0x00FFFFFF) == 0x00BBCCDD
	const Size& s(pixmap.GetSize());
	unique_ptr<PixelType[]> pixels(new PixelType[GetAreaOf(s)]);

	::FreeImage_ConvertToRawBits(reinterpret_cast<byte*>(&pixels[0]),
		pixmap.GetDataPtr(), s.Width * sizeof(PixelType), 16, FI16_555_RED_MASK,
		FI16_555_GREEN_MASK, FI16_555_BLUE_MASK, true);
	return CompactPixmap(std::move(pixels), s);
#elif (YCL_PIXEL_FORMAT_XYZ555 & 0x00FFFFFF) == 0x00DDCCBB
	const Size& s(pixmap.GetSize());
	unique_ptr<PixelType[]> pixels(new PixelType[GetAreaOf(s)]);

	::FreeImage_ConvertToRawBits(reinterpret_cast<byte*>(&pixels[0]),
		pixmap.GetDataPtr(), s.Width * sizeof(PixelType), 16,
		FI16_555_BLUE_MASK, FI16_555_GREEN_MASK, FI16_555_RED_MASK, true);
	return CompactPixmap(std::move(pixels), s);
//#elif (YCL_PIXEL_FORMAT_XYZ555 & 0x00FFFFFF) == 0x00DDCCBB
//#elif (YCL_PIXEL_FORMAT_XYZ888 & 0x00FFFFFF) == 0x00BBCCDD
#elif (YCL_PIXEL_FORMAT_XYZ888 & 0x00FFFFFF) == 0x00DDCCBB
	const Size& s(pixmap.GetSize());
	unique_ptr<PixelType[]> pixels(new PixelType[GetAreaOf(s)]);

	::FreeImage_ConvertToRawBits(reinterpret_cast<byte*>(&pixels[0]),
		pixmap.GetDataPtr(), s.Width * sizeof(PixelType), 32, FI_RGBA_RED_MASK,
		FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, true);
#	if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
//#	elif FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
	// TODO: Add support for different color orders.
#	else
#		error "No supported FreeImage pixel format found."
#	endif
	return CompactPixmap(std::move(pixels), s);
#else
#	error "Unsupported pixel format found."
#endif
}

ImageFormat
ImageCodec::DetectFormat(ImageMemory::NativeHandle handle, size_t size)
{
	return ImageFormat(::FreeImage_GetFileTypeFromMemory(handle, size));
}
ImageFormat
ImageCodec::DetectFormat(const char* filename)
{
	const auto fmt(GetFileType(filename));

	return fmt == ImageFormat::Unknown
		? ImageFormat(::FreeImage_GetFIFFromFilename(filename)) : fmt;
}
ImageFormat
ImageCodec::DetectFormat(const char16_t* filename)
{
	const auto fmt(GetFileType(filename));

	return fmt == ImageFormat::Unknown ? GetFormatFromFilename(filename) : fmt;
}

CompactPixmap
ImageCodec::Load(const vector<octet>& vec)
{
	ImageMemory mem(const_cast<octet*>(&vec[0]), vec.size());

	if(mem.GetFormat() == ImageFormat::Unknown)
		throw UnknownImageFormat("Unknown image format found when loading.");

	return Convert(HBitmap(mem));
}

HMultiBitmap
ImageCodec::LoadForPlaying(const char* path)
{
	const auto fmt(DetectFormat(path));

	return HMultiBitmap(path, fmt, fmt == ImageFormat::GIF
		? ImageDecoderFlags::GIF_Playback : ImageDecoderFlags::Default);
}
HMultiBitmap
ImageCodec::LoadForPlaying(const char16_t* path)
{
	const auto fmt(DetectFormat(path));

	return HMultiBitmap(path, fmt, fmt == ImageFormat::GIF
		? ImageDecoderFlags::GIF_Playback : ImageDecoderFlags::Default);
}

} // namespace Drawing;

} // namespace YSLib;

