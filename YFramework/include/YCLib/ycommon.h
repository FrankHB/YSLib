/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup YCLib YCommonLib
\brief YSLib 基础库。
*/

/*!	\file ycommon.h
\ingroup YCLib
\brief 平台相关的公共组件无关函数与宏定义集合。
\version r3642;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-12 22:14:28 +0800;
\par 修改时间:
	2012-04-30 22:21 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::YCommon;
*/


#ifndef YCL_INC_YCOMMON_H_
#define YCL_INC_YCOMMON_H_

//平台定义。
#include "Platform.h"

//平台无关部分。
#include <ydef.h>
#include <ystdex/cstdio.h>
#include <cstdlib>
#include <initializer_list> // for well-formedly using of std::initialize_list;
#include <bitset>

//平台相关部分。
#include "NativeAPI.h"

//#define HEAP_SIZE (mallinfo().uordblks)

//! \brief 默认平台命名空间。
namespace platform
{

//平台相关的全局常量。

//最大路径长度。
#ifdef PATH_MAX
#	define YCL_MAX_PATH_LENGTH PATH_MAX
#elif MAXPATHLEN
#	define YCL_MAX_PATH_LENGTH MAXPATHLEN
#else
#	define YCL_MAX_PATH_LENGTH 256
#endif

//最大文件名长度。
#ifdef NAME_MAX
#	define YCL_MAX_FILENAME_LENGTH NAME_MAX
#else
#	define YCL_MAX_FILENAME_LENGTH YCL_MAX_PATH_LENGTH
#endif

#ifdef YCL_API_FILESYSTEM_POSIX
	/*!
	\brief 文件路径分隔符。
	\since build 298 。
	*/
#	define YCL_PATH_DELIMITER '/'
	/*!
	\brief 文件路径分隔字符串。
	*/
#	define YCL_PATH_SEPERATOR "/"
	/*!
	\brief 根目录路径。
	*/
#	define YCL_PATH_ROOT YCL_PATH_SEPERATOR

/*!
\brief 本机路径字符类型。
\since build 286 。
*/
typedef char NativePathCharType;
#elif defined(YCL_MINGW32)
	/*!
	\brief 文件路径分隔符。
	\since build 296 。
	*/
#	define YCL_PATH_DELIMITER '\\'
//	#define YCL_PATH_DELIMITER L'\\'
	/*!
	\brief 文件路径分隔字符串。
	\since build 296 。
	*/
#	define YCL_PATH_SEPERATOR "\\"
//	#define YCL_PATH_SEPERATOR L"\\"
	/*!
	\brief 虚拟根目录路径。
	\since build 297 。
	*/
#	define YCL_PATH_ROOT YCL_PATH_SEPERATOR

/*!
\brief 本机路径字符类型。
\since build 296 。
\todo 解决 const_path_t 冲突。
*/
//	typedef wchar_t NativePathCharType;
typedef char NativePathCharType;
#else
#	error Unsupported platform found!
#endif

//类型定义。
/*!
\brief 本机路径字符串类型。
\since build 286 。
*/
typedef NativePathCharType PATHSTR[YCL_MAX_PATH_LENGTH];
/*!
\brief 本机文件名类型。
\since build 286 。
*/
typedef NativePathCharType FILENAMESTR[YCL_MAX_FILENAME_LENGTH];

// using ystdex;
using ystdex::const_path_t;
using ystdex::path_t;

/*!
\brief 主内存块设置。

满足条件时使用平台特定算法覆盖内存区域的每个字节，否则使用 std::memset 。
\note 参数和返回值语义同 std::memset 。
\warning 仅进行内存区域检查，不进行空指针或其它检查。
*/
void*
mmbset(void*, int, std::size_t);

/*!
\brief 主内存块复制。

满足条件时使用平台特定算法复制内存区域，否则使用 std::memcpy 。
\note 参数和返回值语义同 std::memcpy 。
\warning 仅进行内存区域检查，不进行空指针或其它检查。
*/
void*
mmbcpy(void*, const void*, std::size_t);


/*!
\brief 以 UTF-8 文件名打开文件。
\param filename 文件名，意义同 std::fopen 。
\param mode 打开模式，基本语义同 ISO C99 ，具体行为取决于实现。
\pre 断言检查：<tt>filename && mode && *mode != 0</tt> 。
\since build 299 。
*/
std::FILE*
ufopen(const char* filename, const char* mode);
/*!
\brief 以 UCS-2LE 文件名打开文件。
\param filename 文件名，意义同 std::fopen 。
\param mode 打开模式，基本语义同 ISO C99，具体行为取决于实现。
\pre 断言检查：<tt>filename && mode && *mode != 0</tt> 。
\since build 305 。
*/
std::FILE*
ufopen(const char16_t* filename, const char16_t* mode);

/*!
\brief 判断指定 UTF-8 文件名的文件是否存在。
\note 使用 ufopen 实现。
\pre 断言检查：参数非空。
\since build 299 。
*/
bool
ufexists(const char*);
/*!
\brief 判断指定 UCS-2 文件名的文件是否存在。
\note 使用 ufopen 实现。
\pre 断言检查：参数非空。
\since build 305 。
*/
bool
ufexists(const char16_t*);
/*!
\brief 判断指定字符串为文件名的文件是否存在。
\note 使用 NTCTS 参数 ufexists 实现。
\since build 305 。
*/
template<class _tString>
inline bool
ufexists(const _tString& s)
{
	return ufexists(s.c_str());
}

/*!
\brief 当第一参数非空时取当前工作目录复制至指定缓冲区中。
\param buf 缓冲区起始指针。
\param size 缓冲区长。
\return 若成功为 buf ，否则为空指针。
\deprecated 特定平台上的编码不保证是 UTF-8 。
*/
char*
getcwd_n(char* buf, std::size_t size);

/*!
\brief 当第一参数非空时取当前工作目录（ UCS2-LE 编码）复制至指定缓冲区中。
\param buf 缓冲区起始指针。
\param size 缓冲区长。
\return 若成功为 buf ，否则为空指针。
\since build 304 。
*/
char16_t*
u16getcwd_n(char16_t* buf, std::size_t size);

/*!
\brief 判断指定目录是否存在。
*/
bool
direxists(const_path_t);

using ::mkdir;
using ::chdir;

/*!
\brief 按路径新建一个或多个目录。
*/
bool
mkdirs(const_path_t);


/*!
\brief 异常终止函数。
*/
void
terminate();

typedef s16 SPos; //!< 屏幕坐标度量。
typedef u16 SDst; //!< 屏幕坐标距离。

#ifdef YCL_DS
	/*!
	\brief 标识 AXYZ1555 像素格式。
	\since build 297 。
	*/
#	define YCL_PIXEL_FORMAT_AXYZ1555

/*!
\brief LibNDS 兼容像素。
\note ABGR1555 。
*/
typedef u16 PixelType;
/*!
\brief 取像素 Alpha 值。
\since build 297 。
*/
yconstfn auto
FetchAlpha(PixelType px) -> decltype(px & BIT(15))
{
	return px & BIT(15);
}
/*!
\brief 取不透明像素。
\since build 297 。
*/
yconstfn PixelType
FetchOpaque(PixelType px)
{
	return px | BIT(15);
}
#	define DefColorH_(hex, name) name = \
	RGB8((((hex) >> 16) & 0xFF), (((hex) >> 8) & 0xFF), ((hex) & 0xFF)) \
	| BIT(15)
#elif defined(YCL_MINGW32)
/*!
\brief Windows DIB 格式兼容像素。
\note MSDN 注明此处第 4 字节保留为 0 ，但此处使用作为 8 位 Alpha 值使用。
	即 ARGB8888 （考虑字节序为BGRA8888）。
\warning 仅用于屏幕绘制，不保证无条件兼容于所有 DIB 。
\note 转换 DIB 在设备上下文绘制时无需转换格式，比 ::COLORREF 更高效。
\since build 296 。
*/
typedef ::RGBQUAD PixelType;
/*!
\brief 取像素 Alpha 值。
\since build 297 。
*/
yconstfn ::BYTE
FetchAlpha(PixelType px)
{
	return px.rgbReserved;
}
/*!
\brief 取不透明像素。
\since build 297 。
*/
yconstfn PixelType
FetchOpaque(PixelType px)
{
	return {px.rgbBlue, px.rgbGreen, px.rgbRed, 0xFF};
}
/*!
\brief 定义 Windows DIB 格式兼容像素。
\note 得到的 32 位整数和 ::RGBQUAD 在布局上兼容。
\note Alpha 值为 0xFF 。
\since build 296 。
*/
#	define DefColorH_(hex, name) name = (RGB((((hex) >> 16) & 0xFF), \
	(((hex) >> 8) & 0xFF), ((hex) & 0xFF)) << 8 | 0xFF)
#else
#	error Unsupported platform found!
#endif
typedef PixelType* BitmapPtr;
typedef const PixelType* ConstBitmapPtr;


//! \brief 系统默认颜色空间。
namespace ColorSpace
{
//	#define DefColorA(r, g, b, name) name = ARGB16(1, r, g, b),
#define	HexAdd0x(hex) 0x##hex
#define DefColorH(hex_, name) DefColorH_(HexAdd0x(hex_), name)

/*!
\brief 默认颜色集。
\see http://www.w3schools.com/html/html_colornames.asp 。
*/
typedef enum : u32
{
	DefColorH(00FFFF, Aqua),
	DefColorH(000000, Black),
	DefColorH(0000FF, Blue),
	DefColorH(FF00FF, Fuchsia),
	DefColorH(808080, Gray),
	DefColorH(008000, Green),
	DefColorH(00FF00, Lime),
	DefColorH(800000, Maroon),
	DefColorH(000080, Navy),
	DefColorH(808000, Olive),
	DefColorH(800080, Purple),
	DefColorH(FF0000, Red),
	DefColorH(C0C0C0, Silver),
	DefColorH(008080, Teal),
	DefColorH(FFFFFF, White),
	DefColorH(FFFF00, Yellow)
} ColorSet;

#undef DefColorH
#undef DefColorH_
#undef HexAdd0x
} // namespace ColorSpace;

//! \brief 颜色。
class Color
{
public:
	typedef ColorSpace::ColorSet ColorSet;
	typedef u8 MonoType;
	typedef u8 AlphaType;

private:
	/*!
	\brief RGB 分量。
	\since build 276 。
	*/
	MonoType r, g, b;
	/*!
	\brief Alpha 分量。
	\since build 276 。
	*/
	AlphaType a;

public:
	/*!
	\brief 无参数构造：所有分量为 0 的默认颜色。
	\since build 297 。
	*/
	yconstfn
	Color();
	/*!
	\brief 构造：使用本机颜色对象。
	\since build 297 。
	*/
	yconstfn
	Color(PixelType);
#ifdef YCL_MINGW32
	/*!
	\brief 构造：使用默认颜色。
	\since build 297 。
	*/
	yconstfn
	Color(ColorSet);
#endif
	/*!
	\brief 使用 RGB 值和 alpha 位构造 Color 对象。
	*/
	yconstfn
	Color(MonoType, MonoType, MonoType, AlphaType = 0xFF);

	/*!
	\brief 转换：本机颜色对象。
	*/
	yconstfn
	operator PixelType() const;

	/*!
	\brief 取红色分量。
	*/
	yconstfn MonoType
	GetR() const
	{
		return r;
	}
	/*!
	\brief 取绿色分量。
	*/
	yconstfn MonoType
	GetG() const
	{
		return g;
	}
	/*!
	\brief 取蓝色分量。
	*/
	yconstfn MonoType
	GetB() const
	{
		return b;
	}
	/*!
	\brief 取 alpha 分量。
	*/
	yconstfn AlphaType
	GetA() const
	{
		return a;
	}
};

yconstfn
Color::Color()
	: r(0), g(0), b(0), a(0)
{}

yconstfn
Color::Color(PixelType px)
#ifdef YCL_DS
	: r(px << 3 & 248), g(px >> 2 & 248), b(px >> 7 & 248),
	a(FetchAlpha(px) ? 0xFF : 0x00)
#elif defined(YCL_MINGW32)
	: r(px.rgbRed), g(px.rgbGreen), b(px.rgbBlue), a(px.rgbReserved)
#else
#	error Unsupport platform found!
#endif
{}
yconstfn
Color::Color(MonoType r_, MonoType g_, MonoType b_, AlphaType a_)
	: r(r_), g(g_), b(b_), a(a_)
{}
#ifdef YCL_MINGW32
yconstfn
Color::Color(ColorSet cs)
	: r(GetRValue(cs >> 8)), g(GetGValue(cs >> 8)), b(GetBValue(cs >> 8)),
	a(0xFF)
{}
#endif

yconstfn
Color::operator PixelType() const
{
#ifdef YCL_DS
	return ARGB16(int(a != 0), r >> 3, g >> 3, b >> 3);
#elif defined(YCL_MINGW32)
	return {b, g, r, a};
#else
#	error Unsupport platform found!
#endif
}

#ifdef YCL_DS
/*!
\brief 按键并行位宽。
\note 不少于实际表示的 KeyPad 按键数。
\since build 298 。
*/
yconstexpr std::size_t KeyBitsetWidth(32);
#elif defined(YCL_MINGW32)
/*!
\brief 按键并行位宽。
\note 不少于实际表示的键盘按键数。
\since build 298 。
*/
yconstexpr std::size_t KeyBitsetWidth(256);
#else
#	error Unsupport platform found!
#endif

/*!
\brief 本机输入类型。
\since build 298 。

支持并行按键的缓冲区数据类型。
*/
typedef std::bitset<KeyBitsetWidth> KeyInput;


/*!
\brief 本机按键编码。
\note MinGW32 下的 KeyCodes 中仅含每个实现默认需要支持的基本集合。
\since build 298 。
*/
namespace KeyCodes
{

#ifdef YCL_DS
typedef enum
{
	A		= 0,
	B		= 1,
	Select	= 2,
	Start	= 3,
	Right	= 4,
	Left	= 5,
	Up		= 6,
	Down	= 7,
	R		= 8,
	L		= 9,
	X		= 10,
	Y		= 11,
	Touch	= 12,
	Lid		= 13
} NativeSet;

//按键别名。
const NativeSet
	Enter = A,
	Esc = B,
	PgUp = L,
	PgDn = R;
#elif defined(YCL_MINGW32)
/*!
\brief 基本公用按键集合。
\since build 297 。
*/
//@{
typedef enum
{
	Empty	= 0,
	Enter	= VK_RETURN,
	Esc		= VK_ESCAPE,
	PgUp	= VK_PRIOR,
	PgDn	= VK_NEXT,
	Left	= VK_LEFT,
	Up		= VK_UP,
	Right	= VK_RIGHT,
	Down	= VK_DOWN
} NativeSet;
//@}
#else
#	error Unsupport platform found!
#endif

} // namespace KeyCodes;


#ifdef YCL_DS
//! \brief 屏幕指针设备光标信息。
typedef struct CursorInfo : public ::touchPosition
{
	/*!
	\brief 取横坐标。
	*/
	SDst
	GetX() const
	{
		return px;
	}
	/*!
	\brief 取纵坐标。
	*/
	SDst
	GetY() const
	{
		return py;
	}
} CursorInfo;

#elif defined(YCL_MINGW32)
/*!
\brief 屏幕指针设备光标信息。
\since build 296 。
*/
typedef ::CURSORINFO CursorInfo;
#else
#	error Unsupport platform found!
#endif


//断言。
#ifdef YCL_USE_YASSERT

#undef YAssert

/*!
\brief YCLib 默认断言函数。
\note 当定义 YCL_USE_YASSERT 宏时，宏 YAssert 操作由此函数实现。
*/
void
yassert(bool, const char*, const char*, int, const char*);

#define YAssert(exp, message) \
	platform::yassert(exp, #exp, message, __LINE__, __FILE__)

#else

#	include <cassert>
#	define YAssert(exp, message) assert(exp)

#endif


/*!
\brief 文件系统节点迭代器。
\since build 298 。
*/
class HFileNode
{
public:
	typedef ::DIR* IteratorType; //!< 本机迭代器类型。

	static int LastError; //!< 上一次操作结果，0 为无错误。

private:
	IteratorType dir;
	/*!
	\brief 节点信息。
	\since build 298 。
	*/
	::dirent* p_dirent;

public:
	/*!
	\brief 构造：使用路径字符串。
	*/
	explicit
	HFileNode(const_path_t path = nullptr)
		: dir(), p_dirent()
	{
		Open(path);
	}
	/*!
	\brief 复制构造：默认实现。
	\note 浅复制。
	*/
	HFileNode(const HFileNode&) = default;
	/*!
	\brief 析构。
	*/
	~HFileNode()
	{
		Close();
	}

	/*!
	\brief 迭代：向后遍历。
	*/
	HFileNode&
	operator++();
	/*!
	\brief 迭代：向前遍历。
	*/
	HFileNode
	operator++(int)
	{
		return ++HFileNode(*this);
	}

	/*!
	\brief 判断文件系统节点有效性。
	*/
	bool
	IsValid() const
	{
		return dir;
	}
	/*!

	\brief 从节点状态信息判断是否为目录。
	*/
	bool
	IsDirectory() const;

	/*!
	\brief 取节点名称。
	\post 返回值非空。
	\since build 298 。
	*/
	const char*
	GetName() const
	{
		return p_dirent ? p_dirent->d_name : ".";
	}

	/*!
	\brief 打开。
	*/
	void
	Open(const_path_t);

	/*!
	\brief 关闭。
	*/
	void
	Close();

	/*!
	\brief 复位。
	*/
	void
	Reset()
	{
		::rewinddir(dir);
	}
};


/*!
\brief 判断指定路径字符串是否表示一个绝对路径。
*/
bool
IsAbsolute(const_path_t);

/*!
\brief 取指定路径的文件系统根节点名称的长度。
*/
std::size_t
GetRootNameLength(const_path_t);


/*!
\brief 启动控制台。
\note fc 为前景色，bc为背景色。
*/
void
YConsoleInit(u8 dspIndex,
	Color fc = ColorSpace::White, Color bc = ColorSpace::Black);


/*!
\brief 开始 tick 计时。
*/
void
StartTicks();

/*!
\brief 取 tick 数。
\note 单位为毫秒。
*/
u32
GetTicks();

/*!
\brief 取高精度 tick 数。
\note 单位为纳秒。
\since build 291 。
*/
u64
GetHighResolutionTicks();

/*!
\brief 初始化视频输出。
*/
bool
InitVideo();
} // namespace platform;

namespace platform_ex
{

#ifdef YCL_DS

//外部平台库名称引用。
using ::swiWaitForVBlank;

using ::lcdMainOnTop;
using ::lcdMainOnBottom;
using ::lcdSwap;
using ::videoSetMode;
using ::videoSetModeSub;

using ::touchRead;


/*!
\brief 设置允许设备进入睡眠的标识状态。
\return 旧状态。
\note 默认状态为 true 。
\since build 278 。
*/
bool
AllowSleep(bool);


/*!
\brief 复位屏幕显示模式。
*/
void
ResetVideo();


/*!
\brief 默认上屏初始化函数。
*/
platform::BitmapPtr
InitScrUp(int&);

/*!
\brief 默认下屏初始化函数。
*/
platform::BitmapPtr
InitScrDown(int&);

/*!
\brief 快速刷新缓存映像到显示屏缓冲区。
\note 第一参数为显示屏缓冲区，第二参数为源缓冲区。
\since build 184 。
*/
void
ScreenSynchronize(platform::PixelType*, const platform::PixelType*);

#elif defined(YCL_MINGW32)
// TODO: add WinAPIs;
#else
#	error Unsupport platform found!
#endif

} // namespace platform_ex;

#endif

