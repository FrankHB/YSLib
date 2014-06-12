/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Image.cpp
\ingroup Adaptor
\brief 平台中立的图像输入和输出。
\version r762
\author FrankHB <frankhb1989@gmail.com>
\since build 402
\par 创建时间:
	2013-05-05 12:33:51 +0800
\par 修改时间:
	2014-06-12 19:05 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::Image
*/


#include "YSLib/Service/YModules.h"
#include "CHRLib/YModules.h"
#include YFM_YSLib_Adaptor_Image
#include <FreeImage.h>
#include YFM_YSLib_Service_YBlit
#include YFM_YSLib_Service_YGDI
#include YFM_CHRLib_CharacterProcessing // for CHRLib::ToASCII;

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
	YTraceDe(Warning, "FreeImage failed, format = %d: %s.", int(fif), msg);

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

//! \since build 470
//@{
::FIBITMAP*
LoadImage(ImageFormat fmt, std::FILE* fp, ImageDecoderFlags flags) ynothrow
{
	if(fp)
	{
		const auto bitmap(::FreeImage_LoadFromHandle(::FREE_IMAGE_FORMAT(fmt),
			&u8_io, ::fi_handle(fp), int(flags)));

		std::fclose(fp);
		return bitmap;
	}
	return {};
}
::FIBITMAP*
LoadImage(ImageFormat fmt, const char* filename, ImageDecoderFlags flags)
	ynothrow
{
	return LoadImage(fmt, ufopen(filename, "rb"), flags);
}
::FIBITMAP*
LoadImage(ImageFormat fmt, const char16_t* filename, ImageDecoderFlags flags)
	ynothrow
{
	return LoadImage(fmt, ufopen(filename, u"rb"), flags);
}

bool
SaveImage(ImageFormat fmt, ::FIBITMAP* dib, std::FILE* fp,
	ImageDecoderFlags flags) ynothrow
{
	if(dib)
	{
		const bool res(::FreeImage_SaveToHandle(::FREE_IMAGE_FORMAT(fmt),
			dib, &u8_io, ::fi_handle(fp), int(flags)));

		std::fclose(fp);
		return res;
	}
	return {};
}
bool
SaveImage(ImageFormat fmt, ::FIBITMAP* dib, const char* filename,
	ImageDecoderFlags flags) ynothrow
{
	return SaveImage(fmt, dib, ufopen(filename, "wb"), flags);
}
bool
SaveImage(ImageFormat fmt, ::FIBITMAP* dib, const char16_t* filename,
	ImageDecoderFlags flags) ynothrow
{
	return SaveImage(fmt, dib, ufopen(filename, u"wb"), flags);
}
//@}

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
	const auto p(make_unique<char[]>(len + 1));
	const auto str(p.get());

	for(size_t i{}; i < len; ++i)
		str[i] = CHRLib::ToASCII(filename[i]);
	str[len] = u'\0';
	return ImageFormat(::FreeImage_GetFIFFromFilename(str));
}

::FI_PluginRec&
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


#if (YCL_PIXEL_FORMAT_XYZ555 & 0x00FFFFFF) == 0x00BBCCDD
#	define YF_PixConvSpec \
	16, FI16_555_RED_MASK, FI16_555_GREEN_MASK, FI16_555_BLUE_MASK
#elif (YCL_PIXEL_FORMAT_XYZ555 & 0x00FFFFFF) == 0x00DDCCBB
#	define YF_PixConvSpec \
	16, FI16_555_BLUE_MASK, FI16_555_GREEN_MASK, FI16_555_RED_MASK
//#elif (YCL_PIXEL_FORMAT_XYZ555 & 0x00FFFFFF) == 0x00DDCCBB
#elif (YCL_PIXEL_FORMAT_XYZ888 & 0x00FFFFFF) == 0x00BBCCDD
#	define YF_PixConvSpec \
	32, FI_RGBA_BLUE_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_RED_MASK
#elif (YCL_PIXEL_FORMAT_XYZ888 & 0x00FFFFFF) == 0x00DDCCBB
#	define YF_PixConvSpec \
	32, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK
#	if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_BGR
//#	elif FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
	// TODO: Add support for different color orders.
#	else
#		error "No supported FreeImage pixel format found."
#	endif
#else
#	error "Unsupported pixel format found."
#endif

} // unnamed namespace;


ImageMemory::ImageMemory(const HBitmap& pixmap, ImageFormat fmt,
	ImageDecoderFlags flags)
	: buffer(), handle(), format(fmt)
{
	if(fmt == ImageFormat::Unknown)
		throw UnknownImageFormat("Unknown image format found when saving.");

	const auto p_data(pixmap.GetDataPtr());

	if(!p_data)
		throw LoggedEvent("Source image is empty.");
	handle = ::FreeImage_OpenMemory();
	if(!::FreeImage_SaveToMemory(::FREE_IMAGE_FORMAT(format), p_data,
		handle, int(flags)))
		throw LoggedEvent("Saving image to memory failed.");
}
ImageMemory::ImageMemory(Buffer buf)
	: ImageMemory(std::move(buf), ImageFormat::Unknown)
{
	format = ImageCodec::DetectFormat(handle, buffer.size());
}
ImageMemory::ImageMemory(Buffer buf, ImageFormat fmt)
	: buffer([&]{
		if(buf.empty())
			throw LoggedEvent("Null buffer found.");
		return std::move(buf);
	}()), handle(::FreeImage_OpenMemory(static_cast<byte*>(buffer.data()),
	static_cast< ::DWORD>(buffer.size()))), format(fmt)
{
	if(!handle)
		throw LoggedEvent("Opening image memory failed.");
}
ImageMemory::~ImageMemory()
{
	if(!buffer.empty())
		::FreeImage_CloseMemory(handle);
}


HBitmap::HBitmap(const Size& s, BitPerPixel bpp)
	: bitmap(::FreeImage_Allocate(s.Width, s.Height, bpp, 0, 0, 0))
{
	if(!bitmap)
		throw BadImageAlloc();
}
HBitmap::HBitmap(BitmapPtr src, const Size& s, size_t pitch_delta)
	: bitmap([&]{
		YAssertNonnull(src);
		return ::FreeImage_ConvertFromRawBits(reinterpret_cast<byte*>(src),
			s.Width, s.Height, s.Width * sizeof(PixelType) + pitch_delta,
			YF_PixConvSpec, true);
	}())
{
	if(!bitmap)
		throw LoggedEvent("Converting compact pixmap failed.");
}
HBitmap::HBitmap(const CompactPixmap& buf)
	: HBitmap(buf.GetBufferPtr(), buf.GetSize())
{}
HBitmap::HBitmap(const char* filename, ImageDecoderFlags flags)
	: HBitmap(filename, ImageCodec::DetectFormat(filename), flags)
{}
HBitmap::HBitmap(const char* filename, ImageFormat fmt, ImageDecoderFlags flags)
	: bitmap(LoadImage(fmt, filename, flags))
{
	if(!bitmap)
		throw LoggedEvent("Loading image failed.");
}
HBitmap::HBitmap(const char16_t* filename, ImageDecoderFlags flags)
	: HBitmap(filename, ImageCodec::DetectFormat(filename), flags)
{}
HBitmap::HBitmap(const char16_t* filename, ImageFormat fmt,
	ImageDecoderFlags flags)
	: bitmap(LoadImage(fmt, filename, flags))
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
HBitmap::HBitmap(const HBitmap& pixmap, BitPerPixel bpp)
	: bitmap([](::FIBITMAP* p_bmp, BitPerPixel bpp){
		switch(bpp)
		{
		case 32:
			return FreeImage_ConvertTo32Bits(p_bmp);
		case 24:
			return FreeImage_ConvertTo24Bits(p_bmp);
		case 16:
			return FreeImage_ConvertTo16Bits555(p_bmp);
		case 8:
			return FreeImage_ConvertTo8Bits(p_bmp);
		case 4:
			return FreeImage_ConvertTo4Bits(p_bmp);
		default:
			throw UnsupportedImageFormat("Unsupported bit for pixel found.");
		}
	}(pixmap.bitmap, bpp))
{
	if(!bitmap)
		throw LoggedEvent("Converting bitmap failed.");
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

byte*
HBitmap::operator[](size_t idx) const ynothrow
{
	YAssertNonnull(bitmap);
	YAssert(idx < GetHeight(), "Index out of range.");
	return ::FreeImage_GetScanLine(bitmap, idx);
}

HBitmap::operator CompactPixmap() const
{
	const Size& s(GetSize());
	auto pixels(make_unique<PixelType[]>(GetAreaOf(s)));

	::FreeImage_ConvertToRawBits(reinterpret_cast<byte*>(&pixels[0]),
		GetDataPtr(), s.Width * sizeof(PixelType), YF_PixConvSpec, true);
	return CompactPixmap(std::move(pixels), s);
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
byte*
HBitmap::GetPixels() const ynothrow
{
	return ::FreeImage_GetBits(bitmap);
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

bool
HBitmap::SaveTo(const char* filename, ImageFormat fmt, ImageDecoderFlags flags)
	const ynothrow
{
	return SaveImage(fmt, GetDataPtr(), filename, flags);
}
bool
HBitmap::SaveTo(const char16_t* filename, ImageFormat fmt,
	ImageDecoderFlags flags) const ynothrow
{
	return SaveImage(fmt, GetDataPtr(), filename, flags);
}


class MultiBitmapData final : private noncopyable
{
private:
	bool read;
	::fi_handle handle;
	int load_flags;
	std::reference_wrapper<::FreeImageIO> io_ref;
	std::reference_wrapper<::FI_PluginRec> plugin_ref;
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

//! \since build 470
//@{
MultiBitmapData*
LoadImagePages(ImageFormat fmt, std::FILE* fp, ImageDecoderFlags flags) ynothrow
{
	if(fp)
		try
		{
			return new MultiBitmapData(fmt, *fp, int(flags));
		}
		catch(std::exception&)
		{}
	return {};
}
MultiBitmapData*
LoadImagePages(ImageFormat fmt, const char* filename, ImageDecoderFlags flags)
	ynothrow
{
	return LoadImagePages(fmt, ufopen(filename, "rb"), flags);
}
MultiBitmapData*
LoadImagePages(ImageFormat fmt, const char16_t* filename,
	ImageDecoderFlags flags) ynothrow
{
	return LoadImagePages(fmt, ufopen(filename, u"rb"), flags);
}
//@}

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
	: pages(LoadImagePages(fmt, filename, flags))
{
	if(!pages)
		throw LoggedEvent("Loading image pages failed.");
}
HMultiBitmap::HMultiBitmap(const char16_t* filename, ImageDecoderFlags flags)
	: HMultiBitmap(filename, ImageCodec::DetectFormat(filename), flags)
{}
HMultiBitmap::HMultiBitmap(const char16_t* filename, ImageFormat fmt,
	ImageDecoderFlags flags)
	: pages(LoadImagePages(fmt, filename, flags))
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
ImageCodec::Load(ImageMemory::Buffer buf)
{
	ImageMemory mem(std::move(buf));

	if(mem.GetFormat() == ImageFormat::Unknown)
		throw UnknownImageFormat("Unknown image format found when loading.");

	return HBitmap(mem);
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

#undef YF_PixConvSpec

} // namespace Drawing;

} // namespace YSLib;

