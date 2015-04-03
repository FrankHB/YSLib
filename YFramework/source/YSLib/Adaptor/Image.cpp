/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Image.cpp
\ingroup Adaptor
\brief 平台中立的图像输入和输出。
\version r1107
\author FrankHB <frankhb1989@gmail.com>
\since build 402
\par 创建时间:
	2013-05-05 12:33:51 +0800
\par 修改时间:
	2015-03-29 12:25 +0800
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

//! \since build 584
using Timers::TimeSpan;

namespace Drawing
{

//! \since build 430
static_assert(int(SamplingFilter::Box) == ::FILTER_BOX
	&& int(SamplingFilter::Bicubic) == ::FILTER_BICUBIC
	&& int(SamplingFilter::Bilinear) == FILTER_BILINEAR
	&& int(SamplingFilter::BSpline) == ::FILTER_BSPLINE
	&& int(SamplingFilter::CatmullRom) == ::FILTER_CATMULLROM
	&& int(SamplingFilter::Lanczos3) == ::FILTER_LANCZOS3,
	"Incompatible filter found.");

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

//! \since build 556
//@{
::FIBITMAP*
LoadImage(ImageFormat fmt, std::FILE* fp, ImageDecoderFlags flags)
{
	if(fp)
	{
		const auto bitmap(::FreeImage_LoadFromHandle(::FREE_IMAGE_FORMAT(fmt),
			&u8_io, ::fi_handle(fp), int(flags)));

		std::fclose(fp);
		if(bitmap)
			return bitmap;
		throw GeneralEvent("Loading image failed.");
	}
	throw std::invalid_argument("Invalid file found on loading image.");
}
::FIBITMAP*
LoadImage(ImageFormat fmt, const char* filename, ImageDecoderFlags flags)
{
	return LoadImage(fmt, ufopen(filename, "rb"), flags);
}
::FIBITMAP*
LoadImage(ImageFormat fmt, const char16_t* filename, ImageDecoderFlags flags)
{
	return LoadImage(fmt, ufopen(filename, u"rb"), flags);
}

void
SaveImage(ImageFormat fmt, ::FIBITMAP* dib, std::FILE* fp,
	ImageDecoderFlags flags) ynothrow
{
	if(dib)
	{
		const bool res(::FreeImage_SaveToHandle(::FREE_IMAGE_FORMAT(fmt),
			dib, &u8_io, ::fi_handle(fp), int(flags)));

		std::fclose(fp);
		if(!res)
			throw GeneralEvent("Failed saving image");
	}
	throw std::invalid_argument("No valid bitmap data found on saving image.");
}
void
SaveImage(ImageFormat fmt, ::FIBITMAP* dib, const char* filename,
	ImageDecoderFlags flags) ynothrow
{
	SaveImage(fmt, dib, ufopen(filename, "wb"), flags);
}
void
SaveImage(ImageFormat fmt, ::FIBITMAP* dib, const char16_t* filename,
	ImageDecoderFlags flags) ynothrow
{
	SaveImage(fmt, dib, ufopen(filename, u"wb"), flags);
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
	const auto len(ystdex::ntctslen(filename));
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
			throw GeneralEvent("Invalid plugin node found.");
	}
	else
		throw GeneralEvent("No proper plugin found.");
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


ImplDeDtor(BadImageAlloc)


ImplDeDtor(UnsupportedImageFormat)


ImplDeDtor(UnknownImageFormat)


ImageMemory::ImageMemory(const HBitmap& pixmap, ImageFormat fmt,
	ImageDecoderFlags flags)
	: buffer(), handle(), format(fmt)
{
	if(fmt == ImageFormat::Unknown)
		throw UnknownImageFormat("Unknown image format found when saving.");

	const auto p_data(pixmap.GetDataPtr());

	if(!p_data)
		throw GeneralEvent("Source image is empty.");
	handle = ::FreeImage_OpenMemory();
	if(!::FreeImage_SaveToMemory(::FREE_IMAGE_FORMAT(format), p_data,
		handle, int(flags)))
		throw GeneralEvent("Saving image to memory failed.");
}
ImageMemory::ImageMemory(Buffer buf)
	: ImageMemory(std::move(buf), ImageFormat::Unknown)
{
	format = ImageCodec::DetectFormat(handle, buffer.size());
}
ImageMemory::ImageMemory(Buffer buf, ImageFormat fmt)
	: buffer([&]{
		if(buf.empty())
			throw GeneralEvent("Null buffer found.");
		return std::move(buf);
	}()), handle(::FreeImage_OpenMemory(static_cast<byte*>(buffer.data()),
	static_cast<unsigned long>(buffer.size()))), format(fmt)
{
	if(!handle)
		throw GeneralEvent("Opening image memory failed.");
}
ImageMemory::~ImageMemory()
{
	if(!buffer.empty())
		::FreeImage_CloseMemory(handle);
}


HBitmap::HBitmap(const Size& s, BitPerPixel bpp)
	: p_bitmap(::FreeImage_Allocate(CheckScalar<int>(s.Width),
	CheckScalar<int>(s.Height), bpp, 0, 0, 0))
{
	if(!p_bitmap)
		throw BadImageAlloc();
}
HBitmap::HBitmap(BitmapPtr src, const Size& s, size_t pitch_delta)
	: p_bitmap([&]{
		return ::FreeImage_ConvertFromRawBits(reinterpret_cast<byte*>(
			Nonnull(src)), CheckScalar<int>(s.Width),
			CheckScalar<int>(s.Height), CheckScalar<int>(
			s.Width * sizeof(Pixel) + pitch_delta), YF_PixConvSpec, true);
	}())
{
	if(!p_bitmap)
		throw GeneralEvent("Converting compact pixmap failed.");
}
HBitmap::HBitmap(const CompactPixmap& buf)
	: HBitmap(buf.GetBufferPtr(), buf.GetSize())
{}
HBitmap::HBitmap(const char* filename, ImageDecoderFlags flags)
	: HBitmap(filename, ImageCodec::DetectFormat(filename), flags)
{}
HBitmap::HBitmap(const char* filename, ImageFormat fmt, ImageDecoderFlags flags)
	: p_bitmap(Nonnull(LoadImage(fmt, filename, flags)))
{}
HBitmap::HBitmap(const char16_t* filename, ImageDecoderFlags flags)
	: HBitmap(filename, ImageCodec::DetectFormat(filename), flags)
{}
HBitmap::HBitmap(const char16_t* filename, ImageFormat fmt,
	ImageDecoderFlags flags)
	: p_bitmap(Nonnull(LoadImage(fmt, filename, flags)))
{}
HBitmap::HBitmap(const ImageMemory& mem, ImageDecoderFlags flags)
	: p_bitmap(::FreeImage_LoadFromMemory(::FREE_IMAGE_FORMAT(mem.GetFormat()),
	mem.GetNativeHandle(), int(flags)))
{
	if(!p_bitmap)
		throw GeneralEvent("Loading image failed.");
}
HBitmap::HBitmap(const HBitmap& pixmap, BitPerPixel bpp)
	: p_bitmap([bpp](::FIBITMAP* p_bmp){
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
			throw UnsupportedImageFormat("Unsupported bit per pixel found.");
		}
	}(pixmap.p_bitmap))
{
	if(!p_bitmap)
		throw GeneralEvent("Converting bitmap failed.");
}
HBitmap::HBitmap(const HBitmap& pixmap, const Size& s, SamplingFilter sf)
	: p_bitmap(::FreeImage_Rescale(pixmap.p_bitmap, CheckScalar<int>(s.Width),
	CheckScalar<int>(s.Height), ::FREE_IMAGE_FILTER(sf)))
{
	if(!p_bitmap)
		throw GeneralEvent("Rescaling image failed.");
}
HBitmap::HBitmap(const HBitmap& pixmap)
	: p_bitmap(::FreeImage_Clone(pixmap.p_bitmap))
{
	if(!p_bitmap)
		throw BadImageAlloc();
}
HBitmap::HBitmap(HBitmap&& pixmap) ynothrow
	: p_bitmap(pixmap.p_bitmap)
{
	pixmap.p_bitmap = {};
}
HBitmap::~HBitmap()
{
	::FreeImage_Unload(p_bitmap);
}

byte*
HBitmap::operator[](size_t idx) const ynothrowv
{
	YAssertNonnull(*this);
	YAssert(idx < GetHeight(), "Index is out of range.");
	return ::FreeImage_GetScanLine(Nonnull(p_bitmap), int(idx));
}

HBitmap::operator CompactPixmap() const
{
	const Size& s(GetSize());
	auto pixels(make_unique<Pixel[]>(size_t(GetAreaOf(s))));

	::FreeImage_ConvertToRawBits(reinterpret_cast<byte*>(&pixels[0]),
		GetDataPtr(), CheckScalar<int>(s.Width * sizeof(Pixel)), YF_PixConvSpec,
		true);
	return CompactPixmap(std::move(pixels), s);
}

BitPerPixel
HBitmap::GetBPP() const ynothrow
{
	return ::FreeImage_GetBPP(p_bitmap);
}
SDst
HBitmap::GetHeight() const ynothrow
{
	return ::FreeImage_GetHeight(p_bitmap);
}
SDst
HBitmap::GetPitch() const ynothrow
{
	return ::FreeImage_GetPitch(p_bitmap);
}
byte*
HBitmap::GetPixels() const ynothrow
{
	return ::FreeImage_GetBits(p_bitmap);
}
SDst
HBitmap::GetWidth() const ynothrow
{
	return ::FreeImage_GetWidth(p_bitmap);
}

HBitmap::DataPtr
HBitmap::Release() ynothrow
{
	const auto ptr(p_bitmap);

	p_bitmap = {};
	return ptr;
}

void
HBitmap::Rescale(const Size& s, SamplingFilter sf)
{
	*this = HBitmap(*this, s, sf);
}

void
HBitmap::SaveTo(const char* filename, ImageFormat fmt, ImageDecoderFlags flags)
	const
{
	SaveImage(fmt, GetDataPtr(), filename, flags);
}
void
HBitmap::SaveTo(const char16_t* filename, ImageFormat fmt,
	ImageDecoderFlags flags) const
{
	SaveImage(fmt, GetDataPtr(), filename, flags);
}

TimeSpan
GetFrameTimeOf(const HBitmap& bmp)
{
	return TimeSpan(CheckNonnegativeScalar<long>(ImageTag(bmp,
		ImageMetadataModel::Animation, "FrameTime").TryGetValue<long>(),
		"frame time", Warning));
}

Size
GetLogicalSizeOf(const HBitmap& bmp)
{
	return {CheckPositiveScalar<SDst>(ImageTag(bmp,
		ImageMetadataModel::Animation, "LogicalWidth").TryGetValue<short>(),
		"logical width", Warning), CheckPositiveScalar<SDst>(ImageTag(bmp,
		ImageMetadataModel::Animation, "LogicalHeight").TryGetValue<short>(),
		"logical height", Warning)};
}


class MultiBitmapData final : private noncopyable
{
private:
	bool read;
	::fi_handle handle;
	int load_flags;
	//! \since build 554
	lref<::FreeImageIO> io_ref;
	//! \since build 554
	lref<::FI_PluginRec> plugin_ref;
	size_t page_count = 1;
	void* data = {};

public:
	//! \since build 556
	MultiBitmapData(::fi_handle, int, ::FI_PluginRec&, ::FreeImageIO& = u8_io,
		bool = true);
	//! \since build 556
	MultiBitmapData(ImageFormat, std::FILE&, int = 0, ::FreeImageIO& = u8_io,
		bool = true);
	//! \since build 461
	~MultiBitmapData();

	DefPred(const ynothrow, OpenForRead, read)

	DefGetter(const ynothrow, size_t, PageCount, page_count)

	::FIBITMAP*
	LockPage(size_t = 0) const ynothrow;
};

MultiBitmapData::MultiBitmapData(::fi_handle h, int flags,
	::FI_PluginRec& plugin, ::FreeImageIO& io, bool open_for_reading)
	: read(open_for_reading), handle(h), load_flags(flags), io_ref(io),
	plugin_ref(plugin)
{
	io.seek_proc(handle, 0, SEEK_SET);
	if(const auto open = plugin.open_proc)
	{
		data = open(&io_ref.get(), handle, open_for_reading);
		if(const auto proc = plugin_ref.get().pagecount_proc)
		{
			const int cnt(proc(&io_ref.get(), handle, data));

			if(cnt > 0)
				page_count = size_t(cnt);
		}
	}
}
MultiBitmapData::MultiBitmapData(ImageFormat fmt, std::FILE& f, int flags,
	::FreeImageIO& io, bool open_for_reading)
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
	YAssert(index < page_count, ystdex::sfmt(
		"Invalid page index %u found, should be less than %u.",
		unsigned(index), unsigned(page_count)).c_str());
	if(const auto load = plugin_ref.get().load_proc)
		return load(&io_ref.get(), handle, int(index), load_flags, data);
	return {};
}


//! \since build 456
namespace
{

//! \since build 566
//@{
MultiBitmapData*
LoadImagePages(ImageFormat fmt, std::FILE* fp, ImageDecoderFlags flags)
{
	if(fp)
		return new MultiBitmapData(fmt, *fp, int(flags));
	throw std::invalid_argument("Invalid file found on loading image pages.");
}
MultiBitmapData*
LoadImagePages(ImageFormat fmt, const char* filename, ImageDecoderFlags flags)
{
	return LoadImagePages(fmt, ufopen(filename, "rb"), flags);
}
MultiBitmapData*
LoadImagePages(ImageFormat fmt, const char16_t* filename,
	ImageDecoderFlags flags)
{
	return LoadImagePages(fmt, ufopen(filename, u"rb"), flags);
}
//@}

} // unnamed namespace;


HMultiBitmap::HMultiBitmap(const char* filename, ImageDecoderFlags flags)
	: HMultiBitmap(filename, ImageCodec::DetectFormat(filename), flags)
{}
HMultiBitmap::HMultiBitmap(const char* filename, ImageFormat fmt,
	ImageDecoderFlags flags)
	: pages(Nonnull(LoadImagePages(fmt, filename, flags)))
{}
HMultiBitmap::HMultiBitmap(const char16_t* filename, ImageDecoderFlags flags)
	: HMultiBitmap(filename, ImageCodec::DetectFormat(filename), flags)
{}
HMultiBitmap::HMultiBitmap(const char16_t* filename, ImageFormat fmt,
	ImageDecoderFlags flags)
	: pages(Nonnull(LoadImagePages(fmt, filename, flags)))
{}

size_t
HMultiBitmap::GetPageCount() const ynothrow
{
	return pages ? pages->GetPageCount() : 0;
}

HBitmap
HMultiBitmap::Lock(size_t i) const ynothrowv
{
	return pages ? pages->LockPage(i) : nullptr;
}


ImageTag::ImageTag(const ImageTag& tag) ythrow(BadImageAlloc)
	: p_tag(::FreeImage_CloneTag(tag.p_tag)), owns(true)
{
	if(bool(p_tag) != bool(tag.p_tag))
		throw BadImageAlloc();
}
ImageTag::ImageTag(HBitmap::DataPtr p_bmp, ImageMetadataModel model,
	const char* name)
	: p_tag([=]{
		YAssertNonnull(name);
		if(!p_bmp)
			throw std::invalid_argument("Invalid bitmap found.");

		::FITAG* p_new_tag{};

		if(::FreeImage_GetMetadata(::FREE_IMAGE_MDMODEL(model), p_bmp, name,
			&p_new_tag) && p_new_tag)
			return p_new_tag;
		throw GeneralEvent("Specified tag not found.");
	}())
{}
ImageTag::ImageTag(const HBitmap& bmp, ImageMetadataModel model,
	const char* name)
	: ImageTag(bmp.GetDataPtr(), model, name)
{}
ImageTag::~ImageTag()
{
	if(owns)
		::FreeImage_DeleteTag(p_tag);
}

size_t
ImageTag::GetCount() const ynothrow
{
	return ::FreeImage_GetTagCount(p_tag);
}
const char*
ImageTag::GetDescription() const ynothrow
{
	return ::FreeImage_GetTagDescription(p_tag);
}
ImageTag::ID
ImageTag::GetID() const ynothrow
{
	return ::FreeImage_GetTagID(p_tag);
}
const char*
ImageTag::GetKey() const ynothrow
{
	return ::FreeImage_GetTagKey(p_tag);
}
size_t
ImageTag::GetLength() const ynothrow
{
	return ::FreeImage_GetTagLength(p_tag);
}
ImageTag::Type
ImageTag::GetType() const ynothrow
{
	return ImageTag::Type(::FreeImage_GetTagType(p_tag));
}
const void*
ImageTag::GetValuePtr() const ynothrow
{
	return ::FreeImage_GetTagValue(p_tag);
}

bool
ImageTag::SetCount(size_t count) const ynothrow
{
	return ::FreeImage_SetTagCount(p_tag, static_cast<unsigned long>(count));
}
bool
ImageTag::SetDescription(const char* desc) const ynothrow
{
	return ::FreeImage_SetTagDescription(p_tag, desc);
}
bool
ImageTag::SetID(ImageTag::ID id) const ynothrow
{
	return ::FreeImage_SetTagID(p_tag, id);
}
bool
ImageTag::SetKey(const char* key) const ynothrow
{
	return ::FreeImage_SetTagKey(p_tag, key);
}
bool
ImageTag::SetLength(size_t len) const ynothrow
{
	return ::FreeImage_SetTagLength(p_tag, static_cast<unsigned long>(len));
}
bool
ImageTag::SetType(ImageTag::Type type) const ynothrow
{
	return ::FreeImage_SetTagType(p_tag, ::FREE_IMAGE_MDTYPE(type));
}
bool
ImageTag::SetValue(const void* value) const ynothrow
{
	return ::FreeImage_SetTagValue(p_tag, value);
}

ImageTag::DataPtr
ImageTag::Release() ynothrow
{
	const auto ptr(p_tag);

	p_tag = {};
	return ptr;
}

string
to_string(const ImageTag& tag, ImageMetadataModel model)
{
	static mutex mtx;
	lock_guard<mutex> lck(mtx);

	// XXX: The documentation does not guarantee non null return value, but
	//	The implementation actually use static string as buffer, as well as
	//	suggested "not thread safe".
	return string(Nonnull(::FreeImage_TagToString(::FREE_IMAGE_MDMODEL(model),
		tag.GetDataPtr())));
}


ImageMetadataFindData::ImageMetadataFindData(HBitmap::DataPtr ptr,
	ImageMetadataModel model)
	: CurrentModel(model), p_bitmap(ptr)
{
	if(!p_bitmap)
		throw std::invalid_argument("Invalid bitmap found.");
}
ImageMetadataFindData::ImageMetadataFindData(const HBitmap& bmp,
	ImageMetadataModel model)
	: ImageMetadataFindData(bmp.GetDataPtr(), model)
{}
ImageMetadataFindData::~ImageMetadataFindData()
{
	if(p_metadata)
		Close();
}

void
ImageMetadataFindData::Close() ynothrow
{
	::FreeImage_FindCloseMetadata(p_metadata);
}

void
ImageMetadataFindData::Read() ynothrow
{
	if(!p_metadata)
		p_metadata = ::FreeImage_FindFirstMetadata(
			::FREE_IMAGE_MDMODEL(CurrentModel), p_bitmap, &p_tag);
	else if(!::FreeImage_FindNextMetadata(p_metadata, &p_tag))
	{
		Close();
		p_metadata = {};
	}
}

void
ImageMetadataFindData::Rewind() ynothrow
{
	if(p_metadata)
	{
		Close();
		p_metadata = {};
	}
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
	return ImageFormat(::FreeImage_GetFileTypeFromMemory(handle,
		int(static_cast<unsigned long>(size))));
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

