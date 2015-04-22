/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Image.h
\ingroup Adaptor
\brief 平台中立的图像输入和输出。
\version r1336
\author FrankHB <frankhb1989@gmail.com>
\since build 402
\par 创建时间:
	2013-05-05 12:34:03 +0800
\par 修改时间:
	2015-04-19 12:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Adaptor::Image
*/


#ifndef YSL_INC_Adaptor_Image_h_
#define YSL_INC_Adaptor_Image_h_ 1

#include "../Core/YModules.h"
#include YFM_YSLib_Core_YGraphics
#include YFM_YSLib_Adaptor_YContainer
#include <ystdex/exception.h> // for ystdex::unsupported;
#include YFM_YSLib_Core_YClock // for TimeSpan;
//#include <FreeImage.h>

//! \since build 402
struct FIBITMAP;
//! \since build 556
struct FIMETADATA;
//! \since build 556
struct FITAG;
//! \since build 417
struct FIMEMORY;

namespace YSLib
{

namespace Drawing
{

//! \since build 418
class CompactPixmap;
//! \since build 470
class HBitmap;

//! \since build 402
using BitPerPixel = std::uint8_t;


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
	//! \since build 470
	PNG = 13,
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


/*!
\brief 图像元数据模型标识。
\note 数值对应 FreeImage 实现的 \c ::FREE_IMAGE_MDMODEL 类型。
\since build 557
*/
enum class ImageMetadataModel
{
	NoData = -1,
	Comments = 0,
	EXIF_Main = 1,
	EXIF_EXIF = 2,
	EXIF_GPS = 3,
	EXIF_MakerNote = 4,
	EXIF_Interop = 5,
	IPTC = 6,
	XMP = 7,
	GeoTIFF = 8,
	Animation = 9,
	Custom = 10,
	EXIF_RAW = 11
};


/*!
\brief 图像资源分配失败异常：表示存储等资源不足导致无法创建图像。
\since build 402
*/
class YF_API BadImageAlloc : public std::bad_alloc
{
public:
	//! \since build 586
	//@{
	DefDeCtor(BadImageAlloc)
	DefDeCopyCtor(BadImageAlloc)
	//! \brief 虚析构：类定义外默认实现。
	~BadImageAlloc() override;
	//@}
};


/*!
\brief 未被支持的图像格式异常：表示请求的操作涉及的图像格式不受库的支持。
\since build 471
*/
class YF_API UnsupportedImageFormat
	: public GeneralEvent, public ystdex::unsupported
{
public:
	//! \since build 566
	using GeneralEvent::GeneralEvent;

	//! \since build 586
	DefDeCopyCtor(UnsupportedImageFormat)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~UnsupportedImageFormat() override;
};


//! \since build 417
//@{
/*!
\brief 未知图像格式异常：表示请求的操作涉及的图像格式因为不明确而不受库的支持。
*/
class YF_API UnknownImageFormat : public UnsupportedImageFormat
{
public:
	//! \since build 566
	using UnsupportedImageFormat::UnsupportedImageFormat;

	//! \since build 586
	DefDeCopyCtor(UnknownImageFormat)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~UnknownImageFormat() override;
};


/*!
\brief 图像内存对象；保存图像内容的数据缓冲对象。
\note 仅当新建缓冲区时可访问缓冲区数据。
*/
class YF_API ImageMemory final
{
public:
	using NativeHandle = ::FIMEMORY*;
	//! \since build 470
	using Buffer = vector<octet>;

private:
	Buffer buffer;
	NativeHandle handle;
	ImageFormat format;

public:
	/*!
	\exception GeneralEvent 打开内存缓冲区失败。
	\note 需要校验图像格式正确。
	\since build 470
	*/
	//@{
	/*!
	\brief 构造：从现有图像打开。
	\post <tt>GetBuffer().empty()</tt> 。
	\throw UnknownImageFormat 未知图像格式。
	\throw GeneralEvent 图像为空。
	\throw GeneralEvent 图像保存到缓冲区失败。
	*/
	explicit
	ImageMemory(const HBitmap&, ImageFormat = ImageFormat::BMP,
		ImageDecoderFlags = ImageDecoderFlags::Default);
	/*!
	\post <tt>!GetBuffer().empty()</tt> 。
	\exception GeneralEvent 缓冲区大小等于 0 。
	\exception GeneralEvent 打开内存缓冲区失败。
	*/
	//@{
	//! \brief 构造：打开指定的内存缓冲区。
	ImageMemory(Buffer);
	//! \brief 构造：以指定格式打开指定的内存缓冲区。
	ImageMemory(Buffer, ImageFormat);
	//! \brief 构造：新建并打开内存缓冲区。
	template<typename _func>
	ImageMemory(_func f)
		: ImageMemory(f())
	{}
	//! \brief 构造：以指定格式新建并打开内存缓冲区。
	template<typename _func>
	ImageMemory(_func f, ImageFormat fmt)
		: ImageMemory(f(), fmt)
	{}
	//@}
	DefDelMoveCtor(ImageMemory)
	//@}
	//! \since build 461
	~ImageMemory();

	//! \since build 470
	DefGetter(const ynothrow, const Buffer&, Buffer, buffer)
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
	//! \since build 556
	DataPtr p_bitmap;

public:
	/*!
	\post <tt>!*this</tt> 。
	\since build 556
	*/
	DefDeCtor(HBitmap)
	/*
	\brief 构造：使用现有数据指针。
	\note 取得所有权。
	\since build 556
	*/
	HBitmap(DataPtr ptr) ynothrow
		: p_bitmap(ptr)
	{}
	//! \exception LoggedEvent 异常中立：输入的大小分量过大。
	//@{
	//! \throw BadImageAlloc 分配空间失败。
	HBitmap(const Size&, BitPerPixel = 0);
	/*!
	\brief 构造：从矩形像素图缓冲区按指定大小和扫描线跨距增量复制并转换图像数据。
	\pre 间接断言：指针参数非空。
	\throw GeneralEvent 转换失败。
	\exception LoggedEvent 异常中立：由跨距计算的偏移值范围检查失败。
	\note 扫描线跨距的单位为字节，
		等于图像的宽乘以每像素字节数与输入的扫描线跨距增量之和。
	\since build 471
	*/
	explicit YB_NONNULL(1)
	HBitmap(BitmapPtr, const Size&, size_t = 0);
	//@}
	/*!
	\brief 构造：从标准矩形像素图缓冲区复制并转换图像数据。
	\exception GeneralEvent 转换失败。
	\since build 471
	*/
	HBitmap(const CompactPixmap&);
	/*!
	\throw std::invalid_argument 文件打开失败。
	\throw GeneralEvent 读取失败。
	\since build 556
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
	/*!
	\brief 构造：使用指定 UCS-2 文件名和解码器标识。
	\throw UnknownImageFormat 未知图像格式。
	*/
	HBitmap(const char16_t*, ImageDecoderFlags = ImageDecoderFlags::Default);
	//! \brief 构造：使用指定 UCS-2 文件名、指定格式和解码器标识。
	HBitmap(const char16_t*, ImageFormat,
		ImageDecoderFlags = ImageDecoderFlags::Default);
	/*!
	\brief 构造：使用指定字符串文件名和解码器标识。
	\throw UnknownImageFormat 未知图像格式。
	\since build 483
	*/
	template<class _tString,
		yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
	HBitmap(const _tString& filename,
		ImageDecoderFlags = ImageDecoderFlags::Default)
		: HBitmap(&filename[0])
	{}
	/*!
	\brief 构造：使用指定字符串文件名和解码器标识。
	\since build 483
	*/
	template<class _tString,
		yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
	HBitmap(const _tString& filename, ImageFormat fmt,
		ImageDecoderFlags = ImageDecoderFlags::Default)
		: HBitmap(&filename[0], fmt)
	{}
	//@}
	/*!
	\throw GeneralEvent 读取失败。
	\since build 457
	*/
	HBitmap(const ImageMemory&, ImageDecoderFlags = ImageDecoderFlags::Default);
	/*!
	\brief 构造指定图像转换为指定色深的基于 RGB 像素格式的位图副本。
	\throw UnsupportedImageFormat 指定的色深对指定图形不被支持。
	\throw GeneralEvent 转换失败（包括色深被支持但具体格式不被实现支持的情形）。
	\note 对 16 位位图使用 RGB555 。对 32 位位图使用 RGBA8888 。
	\since build 471
	*/
	HBitmap(const HBitmap&, BitPerPixel);
	//! \since build 417
	//@{
	/*!
	\brief 构造指定图像缩放至指定大小的副本。
	\throw GeneralEvent 缩放失败。
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
	//! \brief 合一赋值：使用值参数和交换函数进行复制或转移赋值。
	HBitmap&
	operator=(HBitmap pixmap) ynothrow
	{
		pixmap.swap(*this);
		return *this;
	}

	DefBoolNeg(explicit, p_bitmap)

	/*!
	\brief 取扫描线数据。
	\pre 断言： <tt>bool(*this)</tt> 。
	\pre 断言： 参数值小于高。
	\return 扫描线数据的起始指针。
	\note 扫描线宽为跨距。
	\since build 566
	*/
	byte*
	operator[](size_t) const ynothrowv;
	//@}

	/*!
	\brief 转换为标准矩形像素图缓冲区。
	\since build 471
	*/
	operator CompactPixmap() const;

	BitPerPixel
	GetBPP() const ynothrow;
	//! \since build 417
	DefGetter(const ynothrow, DataPtr, DataPtr, p_bitmap)
	SDst
	GetHeight() const ynothrow;
	//! \since build 417
	DefGetter(const ynothrow, Size, Size, {GetWidth(), GetHeight()})
	//! \since build 417
	SDst
	GetPitch() const ynothrow;
	/*!
	\brief 取像素数据。
	\return 若数据指针为空则为空指针，否则为像素数据起始非空指针。
	\note 像素数据由连续的扫面线数据构成，数量等于高度值。
	\since build 471
	*/
	byte*
	GetPixels() const ynothrow;
	/*!
	\brief 取扫描线数据。
	\pre 间接断言：参数值小于高。
	\return 若数据指针为空则为空指针，否则为扫描线数据起始非空指针。
	\note 使用 <tt>operator[]</tt> 实现。
	\sa operator[]
	\since build 471
	*/
	PDefH(byte*, GetScanLine, size_t idx) const ynothrow
		ImplRet(p_bitmap ? (*this)[idx] : nullptr)
	SDst
	GetWidth() const ynothrow;

	/*!
	\brief 释放所有权。
	\post <tt>!*this</tt>
	\since build 566
	*/
	DataPtr
	Release() ynothrow;

	/*!
	\brief 缩放为指定大小。
	\since build 430
	*/
	void
	Rescale(const Size&, SamplingFilter = SamplingFilter::Box);

	/*!
	\throw std::invalid_argument 不存在可被保存的位图数据。
	\throw GeneralEvent 保存失败。
	\since build 556
	*/
	//@{
	//!\ brief 保存：使用指定 UTF-8 文件名、格式和解码器标识。
	void
	SaveTo(const char*, ImageFormat = ImageFormat::BMP,
		ImageDecoderFlags = ImageDecoderFlags::Default) const;
	//!\ brief 保存：使用指定 UTF-16 文件名、格式和解码器标识。
	void
	SaveTo(const char16_t*, ImageFormat = ImageFormat::BMP,
		ImageDecoderFlags = ImageDecoderFlags::Default) const;
	/*!
	\brief 保存：使用指定字符串文件名、格式和解码器标识。
	\since build 483
	*/
	template<class _tString,
		yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
	void
	SaveTo(const _tString& filename, ImageFormat fmt = ImageFormat::BMP,
		ImageDecoderFlags flags = ImageDecoderFlags::Default) const
	{
		SaveTo(&filename[0], fmt, flags);
	}
	//@}

	/*
	\brief 交换。
	\since build 430
	*/
	PDefH(void, swap, HBitmap& pixmap) ynothrow
		ImplExpr(std::swap(p_bitmap, pixmap.p_bitmap))
};

//! \relates HBitmap
//@{

/*!
\brief 取图像延时。
\return CheckNonnegativeScalar 检查后的元数据指定的被显示为帧的图像的时间间隔。
\exception GeneralEvent 指定数据不存在。
\exception LoggedEvent 数据小于 0 。
\since build 584
*/
YF_API Timers::TimeSpan
GetFrameTimeOf(const HBitmap&);

/*!
\brief 取图像逻辑大小。
\return CheckPositiveScalar 检查后的元数据指定分量的大小，保证分量大于 0 。
\exception GenerlError 元数据读取失败或不完整。
\exception LoggedEvent 大小分量溢出。
\note 当前只支持 ImageMetadataModel::Animation 模型。
\since build 557
*/
YF_API Size
GetLogicalSizeOf(const HBitmap&);

//! \since build 430
inline DefSwap(ynothrow, HBitmap)
//@}


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
\warning 非虚析构。
\todo 增加使用 ImageMemory 的构造函数。
*/
class YF_API HMultiBitmap final
{
public:
	using DataPtr = shared_ptr<MultiBitmapData>;
	//! \since build 556
	using iterator = ystdex::subscriptive_iterator<const HMultiBitmap, HBitmap,
		ptrdiff_t, const HBitmap*, HBitmap>;
	//! \since build 460
	using const_iterator = iterator;

private:
	DataPtr pages{};

public:
	/*!
	\brief 无参数构造：默认实现。
	\post <tt>*!this</tt> 。
	\since build 556
	*/
	DefDeCtor(HMultiBitmap)
	/*!
	\post <tt>bool(*this)</tt> 。
	\throw std::bad_alloc 存储分配失败。
	\throw std::invalid_argument 文件打开失败。
	\since build 457
	*/
	//@{
	/*!
	\brief 构造：使用指定 UTF-8 文件名和解码器标识。
	\throw UnknownImageFormat 未知图像格式。
	*/
	HMultiBitmap(const char*, ImageDecoderFlags = ImageDecoderFlags::Default);
	//! \brief 构造：使用指定 UTF-8 文件名、指定格式和解码器标识。
	HMultiBitmap(const char*, ImageFormat,
		ImageDecoderFlags = ImageDecoderFlags::Default);
	/*!
	\brief 构造：使用指定 UCS-2 文件名和解码器标识。
	\throw UnknownImageFormat 未知图像格式。
	*/
	HMultiBitmap(const char16_t*,
		ImageDecoderFlags = ImageDecoderFlags::Default);
	//! \brief 构造：使用指定 UCS-2 文件名、指定格式和解码器标识。
	HMultiBitmap(const char16_t*, ImageFormat,
		ImageDecoderFlags = ImageDecoderFlags::Default);
	//! \since build 483
	//@{
	/*!
	\brief 构造：使用指定字符串文件名和解码器标识。
	\throw UnknownImageFormat 未知图像格式。
	*/
	template<class _tString,
		yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
	HMultiBitmap(const _tString& filename,
		ImageDecoderFlags flags = ImageDecoderFlags::Default)
		: HMultiBitmap(&filename[0], flags)
	{}
	//! \brief 构造：使用指定字符串文件名和解码器标识。
	template<class _tString,
		yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
	HMultiBitmap(const _tString& filename, ImageFormat fmt,
		ImageDecoderFlags flags = ImageDecoderFlags::Default)
		: HMultiBitmap(&filename[0], fmt, flags)
	{}
	//@}
	//@}
	DefDeCopyMoveCtorAssignment(HMultiBitmap)

	DefBoolNeg(explicit, bool(pages))

	//! \since build 556
	PDefHOp(HBitmap, [], size_t idx) const ynothrowv
		ImplRet(Lock(idx))

	size_t
	GetPageCount() const ynothrow;

	HBitmap
	Lock(size_t = 0) const ynothrowv;

	//! \brief 交换。
	PDefH(void, swap, HMultiBitmap& multi_pixmap) ynothrow
		ImplExpr(std::swap(pages, multi_pixmap.pages))

	//! \since build 461
	//@{
	PDefH(iterator, begin, ) const ynothrow
		ImplRet(HMultiBitmap::iterator(*this, 0))

	PDefH(iterator, end, ) const ynothrow
		ImplRet(HMultiBitmap::iterator(*this, GetPageCount()))
	//@}
};

//! \relates HMultiBitmap
inline DefSwap(ynothrow, HMultiBitmap)
//@}


/*!
\brief 图像元数据标签。
\note 若不具有所有权则依赖特定的位图。
\warning 被依赖的位图在标签生存期内不存在则程序行为未定义。
\since build 556
*/
class YF_API ImageTag final
{
public:
	using DataPtr = ::FITAG*;
	using ID = std::uint16_t;
	/*!
	\brief 元数据类型标识。
	\note 数值对应 FreeImage 实现的 \c ::FREE_IMAGE_MDTYPE 类型。
	*/
	enum Type
	{
		NoType = 0,
		Byte = 1,
		ASCII = 2,
		Short = 3,
		Long = 4,
		Rational = 5,
		SByte = 6,
		Undefined = 7,
		SShort = 8,
		SLong = 9,
		SRational = 10,
		Float = 11,
		Double = 12,
		IFD = 13,
		Palette = 14,
		Long8 = 16,
		SLong8 = 17,
		IFD8 = 18
	};

private:
	DataPtr p_tag = {};
	//! \since build 556
	bool owns = {};

public:
	/*!
	\post <tt>!*this</tt> 。
	\post <tt>!owns</tt> 。
	*/
	DefDeCtor(ImageTag)
	/*
	\brief 构造：使用现有数据指针。
	\param o 所有权标签，若为 true 取得所有权。
	\post <tt>owns == o</tt> 。
	\since build 557
	*/
	ImageTag(DataPtr ptr, bool o = true) ynothrow
		: p_tag(ptr), owns(o)
	{}
	/*!
	\pre 断言：表示名称的指针参数非空。
	\post <tt>!owns</tt> 。
	\throw GeneralEvent 没有找到指定的标签。
	\throw std::invalid_argument 位图为空。
	\since build 557
	\todo 使用 <tt>YB_NONNULL(3)</tt> 。
	*/
	//@{
	//! \brief 构造：使用依赖的位图指针、模型和元数据名称。
	ImageTag(HBitmap::DataPtr, ImageMetadataModel, const char*);
	//! \brief 构造：使用依赖的位图、模型和元数据名称。
	ImageTag(const HBitmap&, ImageMetadataModel, const char*);
	template<typename _tBitmap, class _tString,
		yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
	ImageTag(const _tBitmap& bmp, ImageMetadataModel model, _tString& name)
		: ImageTag(bmp, model, &name[0])
	{}
	//@}
	//! \post <tt>owns</tt> 。
	ImageTag(const ImageTag&) ythrow(BadImageAlloc);
	/*!
	\post <tt>!owns</tt> 。
	\since build 557
	*/
	ImageTag(const ImageTag& tag, std::false_type) ynothrow
		: ImageTag(tag.p_tag, {})
	{}
	~ImageTag();

	DefBoolNeg(explicit, p_tag)

	size_t
	GetCount() const ynothrow;
	//! \since build 557
	DefGetter(const ynothrow, DataPtr, DataPtr, p_tag)
	const char*
	GetDescription() const ynothrow;
	ID
	GetID() const ynothrow;
	const char*
	GetKey() const ynothrow;
	size_t
	GetLength() const ynothrow;
	Type
	GetType() const ynothrow;
	//! \since build 557
	const void*
	GetValuePtr() const ynothrow;

	//! \warning 参数截断至 <tt>unsigned long</tt> 。
	bool
	SetCount(size_t) const ynothrow;
	bool
	SetDescription(const char*) const ynothrow;
	bool
	SetID(ID) const ynothrow;
	bool
	SetKey(const char*) const ynothrow;
	//! \warning 参数截断至 <tt>unsigned long</tt> 。
	bool
	SetLength(size_t) const ynothrow;
	bool
	SetType(Type) const ynothrow;
	bool
	SetValue(const void*) const ynothrow;

	/*!
	\brief 释放所有权。
	\post <tt>!*this</tt> 。
	*/
	DataPtr
	Release() ynothrow;

	/*!
	\brief 取指定类型的标签值。
	\warning 不检查类型。
	\since build 557
	*/
	template<typename _type>
	const _type&
	TryGetValue() const ythrow(GeneralEvent)
	{
		if(const auto p = GetValuePtr())
			return Deref(static_cast<const _type*>(p));
		throw GeneralEvent("Null tag value found.");
	}
};

/*!
\brief 取按指定图像元数据模型解释的标签值字符串表示。
\note 线程安全：仅在不调用 \c ::FreeImage_TagToString 时保证。
\relates ImageTag
\since build 557
*/
YF_API string
to_string(const ImageTag&, ImageMetadataModel);


/*!
\brief 图像元数据。
\warning 若参与构造的图像被释放则继续操作的行为未定义。
\warning 非虚析构。
\since build 557
*/
class YF_API ImageMetadataFindData final : private noncopyable
{
public:
	using DataPtr = ::FIMETADATA*;

	/*!
	\brief 当前生效的模型。

	当前状态为空时开始第一次迭代后决定使用的模型。
	*/
	ImageMetadataModel CurrentModel;

private:
	HBitmap::DataPtr p_bitmap;
	ImageTag::DataPtr p_tag = {};
	DataPtr p_metadata = {};

public:
	//! \throw std::invalid_argument 图像指针为空。
	//@{
	ImageMetadataFindData(HBitmap::DataPtr, ImageMetadataModel);
	ImageMetadataFindData(const HBitmap&, ImageMetadataModel);
	//@}
	//! \brief 析构：关闭查找状态。
	~ImageMetadataFindData();

	DefBoolNeg(explicit, p_metadata)

	DefGetter(const ynothrow, ImageTag, Tag, {p_tag, {}})

private:
	void
	Close() ynothrow;

public:
	//! \brief 读取：迭代当前查找状态。
	void
	Read() ynothrow;

	void
	Rewind() ynothrow;
};


/*!
\brief 元数据句柄：表示打开的元数据和内容迭代状态。
\warning 若参与构造的图像被释放则继续操作的行为未定义。
\since build 557
*/
class YF_API HImageMetadata final : private ystdex::deref_self<HImageMetadata>
{
	friend deref_self<HImageMetadata>;

private:
	unique_ptr<ImageMetadataFindData> p_data;

public:
	template<typename... _tParams>
	explicit
	HImageMetadata(_tParams&&... args)
		: p_data(new ImageMetadataFindData(yforward(args)...))
	{}
	DefDeMoveCtor(HImageMetadata)

	DefDeMoveAssignment(HImageMetadata)

	/*!
	\brief 间接操作：取自身引用。
	\note 使用 ystdex::indirect_input_iterator 和转换函数访问。
	*/
	using deref_self<HImageMetadata>::operator*;

	//! \brief 迭代：向后遍历。
	PDefHOp(HImageMetadata&, ++, ) ynothrow
		ImplRet(Deref(p_data).Read(), *this)

	explicit DefCvt(const ynothrow, bool, bool(p_data))

	//! \brief 间接操作：取图像标签。
	operator ImageTag() const;

	PDefH(void, Rewind, )
		ImplRet(Deref(p_data).Rewind())
};


/*!
\brief 元数据迭代器。
\since build 557
*/
using HImageMetadataIterator = ystdex::indirect_input_iterator<HImageMetadata*>;


//! \since build 417
class YF_API ImageCodec final
{
public:
	ImageCodec();
	//! \since build 461
	~ImageCodec();

	/*!
	\brief 检测图像格式。
	\note 对文件，若根据内容检测失败则根据扩展名判断（不保证正确性）。
	\since build 457
	*/
	//@{
	/*!
	\note 使用图像内存的本机句柄和大小。
	\warning 参数截断至 <tt>unsigned long</tt> 。
	*/
	static ImageFormat
	DetectFormat(ImageMemory::NativeHandle, size_t);
	//! \note 使用指定 UTF-8 文件名。
	static ImageFormat
	DetectFormat(const char*);
	//! \note 使用指定 UCS-2 文件名。
	static ImageFormat
	DetectFormat(const char16_t*);
	//@}

	//! \since build 470
	static CompactPixmap
	Load(ImageMemory::Buffer);

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
	//! \note 使用指定 UCS-2 文件名。
	static HMultiBitmap
	LoadForPlaying(const char16_t*);
	/*!
	\brief 构造：使用指定字符串文件名。
	\since build 483
	*/
	template<class _tString,
		yimpl(typename = ystdex::enable_for_string_class_t<_tString>)>
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
		_tSeqCon con(multi_bitmap.begin(), multi_bitmap.end());

		if(con.empty())
			con.emplace_back(path);
		return con;
	}
};

} // namespace Drawing;

} // namespace YSLib;

#endif

