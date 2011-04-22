/*
	Copyright (C) by Franksoft 2009 - 2011.

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
\version 0.2698;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-12 22:14:28 +0800; 
\par 修改时间:
	2011-04-20 10:44 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YCommon;
*/


#ifndef YCL_INC_YCOMMON_H_
#define YCL_INC_YCOMMON_H_

#include <platform.h>

#ifndef UNICODE
#define UNICODE
#endif

//平台无关部分。
#include "ydef.h"
#include <cstdio>
#include "ystdex/cast.hpp"
#include "ystdex/iterator.hpp"
#include "ystdex/util.hpp"

//平台相关部分。
#include <api.h>

//#define HEAP_SIZE (mallinfo().uordblks)

//! \brief 默认平台命名空间。
namespace platform
{
	//外部平台库名称引用。
	using ::swiWaitForVBlank;

	using ::lcdMainOnTop;
	using ::lcdMainOnBottom;
	using ::lcdSwap;
	using ::videoSetMode;
	using ::videoSetModeSub;

	using ::scanKeys;
	using ::touchRead;

	//平台相关的全局常量。

	#define YCL_MAX_FILENAME_LENGTH MAXPATHLEN //!< 最大路径长度。
	#define YCL_MAX_PATH_LENGTH YCL_MAX_FILENAME_LENGTH

	const char DEF_PATH_DELIMITER = '/'; //!< 文件路径分隔符。
	const char* const DEF_PATH_SEPERATOR = "/"; //!< 文件路径分隔字符串。
	#define DEF_PATH_ROOT DEF_PATH_SEPERATOR

	//类型定义。
	typedef char PATHSTR[YCL_MAX_PATH_LENGTH];
	//typedef char PATHSTR[MAXPATHLEN];
	typedef char FILENAMESTR[YCL_MAX_FILENAME_LENGTH];

	using ::iprintf;

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
	\brief 当 buf 非空时取当前工作目录复制至 buf 起始的长为 t 的缓冲区中。
	\return buf 。
	*/
	char*
	getcwd_n(char* buf, std::size_t);

	/*!
	\brief 判断指定目录是否存在。
	*/
	bool
	direxists(CPATH);

	using ::mkdir;
	using ::chdir;

	/*!
	\brief 按路径新建一个或多个目录。
	*/
	bool
	mkdirs(CPATH);


	/*!
	\brief 异常终止函数。
	*/
	void
	terminate();

	typedef s16 SPos; //!< 屏幕坐标度量。
	typedef u16 SDst; //!< 屏幕坐标距离。
	typedef u16 PixelType; //!< 像素。
	typedef PixelType* BitmapPtr;
	typedef const PixelType* ConstBitmapPtr;
	typedef PixelType ScreenBufferType[SCREEN_WIDTH * SCREEN_HEIGHT]; \
		//!< 主显示屏缓冲区。
	#define BITALPHA BIT(15) //!<  Alpha 位。


	//! \brief 系统默认颜色空间。
	namespace ColorSpace
	{
	//	#define DefColorA(r, g, b, name) name = ARGB16(1, r, g, b),
		#define	HexAdd0x(hex) 0x##hex
		#define DefColorH_(hex, name) name = \
			RGB8(((hex >> 16) & 0xFF), ((hex >> 8) & 0xFF), (hex & 0xFF)) \
			| BITALPHA
		#define DefColorH(hex_, name) DefColorH_(HexAdd0x(hex_), name)

		//参考：http://www.w3schools.com/html/html_colornames.asp 。

		typedef enum
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
	}

	//! \brief 颜色。
	class Color
	{
	public:
		typedef ColorSpace::ColorSet ColorSet;
		typedef u8 MonoType;
		typedef bool AlphaType;

	private:
		PixelType _value;

	public:
		/*!
		\brief 构造：使用本机颜色对象。
		*/
		Color(PixelType = 0);
		/*!
		\brief 使用 RGB 值和 alpha 位构造 Color 对象。
		*/
		Color(MonoType, MonoType, MonoType, AlphaType = true);

		/*!
		\brief 转换：本机颜色对象。
		*/
		operator PixelType() const;

		/*!
		\brief 取红色分量。
		*/
		MonoType
		GetR() const;
		/*!
		\brief 取绿色分量。
		*/
		MonoType
		GetG() const;
		/*!
		\brief 取蓝色分量。
		*/
		MonoType
		GetB() const;
		/*!
		\brief 取 alpha 分量。
		*/
		AlphaType
		GetA() const;
	};

	inline
	Color::Color(PixelType p)
		: _value(p)
	{}
	inline
	Color::Color(MonoType r, MonoType g, MonoType b, AlphaType a)
		: _value(ARGB16(int(a), r >> 3, g >> 3, b >> 3))
	{}

	inline
	Color::operator PixelType() const
	{
		return _value;
	}

	inline Color::MonoType
	Color::GetR() const
	{
		return _value << 3 & 248;
	}
	inline Color::MonoType
	Color::GetG() const
	{
		return _value >> 2 & 248;
	}
	inline Color::MonoType
	Color::GetB() const
	{
		return _value >> 7 & 248;
	}
	inline Color::AlphaType
	Color::GetA() const
	{
		return _value & BITALPHA;
	}

	//! \brief 本机按键空间。
	namespace KeySpace
	{
		//按键集合。
		typedef enum
		{
			Empty	= 0,
			A		= KEY_A,
			B		= KEY_B,
			Select	= KEY_SELECT,
			Start	= KEY_START,
			Right	= KEY_RIGHT,
			Left	= KEY_LEFT,
			Up		= KEY_UP,
			Down	= KEY_DOWN,
			R		= KEY_R,
			L		= KEY_L,
			X		= KEY_X,
			Y		= KEY_Y,
			Touch	= KEY_TOUCH,
			Lid		= KEY_LID
		} KeySet;

		//按键别名。
		const KeySet
			Enter = A,
			ESC = B,
			PgUp = L,
			PgDn = R;
	}


	//! \brief 本机按键对象。
	class Key
	{
	public:
		typedef u32 InputType; //!< 本机按键输入类型。

	private:
		InputType _value;

	public:
		/*!
		\brief 构造：使用本机按键输入对象。
		*/
		Key(InputType = 0);

		/*!
		\brief 转换：本机按键输入对象。
		*/
		operator InputType() const;
	};

	inline
	Key::Key(Key::InputType i)
	: _value(i)
	{}

	inline
	Key::operator Key::InputType() const
	{
		return _value;
	}


	//! \brief 按键信息。
	typedef struct KeysInfo
	{
		Key Up, Down, Held;
	} KeysInfo;


	//! \brief 屏幕光标信息。
	typedef struct CursorInfo : public ::touchPosition
	{
		/*!
		\brief 取横坐标。
		*/
		SDst GetX() const;
		/*!
		\brief 取纵坐标。
		*/
		SDst GetY() const;
	} CursorInfo;

	inline SDst
	CursorInfo::GetX() const
	{
		return px;
	}
	inline SDst
	CursorInfo::GetY() const
	{
		return py;
	}


	/*!
	\brief 调试模式：设置状态。
	\note 当且仅当状态为 true 时，
		以下除 YDebugGetStatus 外的调试模式函数有效。
	*/
	void
	YDebugSetStatus(bool = true);

	/*!
	\brief 调试模式：取得状态。
	*/
	bool
	YDebugGetStatus();

	/*!
	\brief 调试模式：显示控制台（fc 为前景色，bc 为背景色）。
	*/
	void
	YDebugBegin(Color fc = ColorSpace::White, Color bc = ColorSpace::Blue);

	/*!
	\brief 调试模式：按键继续。
	*/
	void
	YDebug();
	/*!
	\brief 调试模式：显示控制台字符串，按键继续。
	*/
	void
	YDebug(const char*);

	/*!
	\brief 调试模式：显示控制台字（int 型数据），按键继续。
	*/
	void
	YDebugW(int);

	/*!
	\brief 调试模式 printf ：显示控制台格式化输出 ，按键继续。
	*/
	int
	yprintf(const char*, ...)
		_ATTRIBUTE ((format (printf, 1, 2)));

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


	//定长路径字符串类型。
	typedef char PATHSTR[MAXPATHLEN];


	//目录迭代器。
	class HDirectory
	{
	public:
		typedef ::DIR_ITER* IteratorType; //!< 本机迭代器类型。

		static PATHSTR Name; //!< 节点名称。
		static struct ::stat Stat; //!< 节点状态信息。
		static int LastError; //!< 上一次操作结果，0 为无错误。

	private:
		IteratorType dir;

	public:
		/*!
		\brief 构造：使用路径字符串。
		*/
		explicit
		HDirectory(CPATH = NULL);

	private:
		/*!
		\brief 构造：使用本机迭代器。
		*/
		HDirectory(IteratorType&);

	public:
		/*!
		\brief 复制构造。
		\note 浅复制。
		*/
		HDirectory(const HDirectory&);
		/*!
		\brief 析构。
		*/
		~HDirectory();

		/*!
		\brief 迭代：向后遍历。
		*/
		HDirectory&
		operator++();
		/*!
		\brief 迭代：向前遍历。
		*/
		HDirectory
		operator++(int);

		/*!
		\brief 判断文件系统节点有效性。
		*/
		bool
		IsValid() const; 
		/*!

		\brief 从节点状态信息判断是否为目录。
		*/
		static bool
		IsDirectory();

		/*!
		\brief 打开。
		*/
		void
		Open(CPATH);

		/*!
		\brief 关闭。
		*/
		void
		Close();

		/*!
		\brief 复位。
		*/
		void
		Reset();
	};

	inline
	HDirectory::HDirectory(CPATH path)
		: dir(NULL)
	{
		Open(path);
	}
	inline
	HDirectory::HDirectory(const HDirectory& h)
		: dir(h.dir)
	{}
	inline
	HDirectory::HDirectory(HDirectory::IteratorType& d)
		: dir(d)
	{}
	inline
	HDirectory::~HDirectory()
	{
		Close();
	}

	inline HDirectory
	HDirectory::operator++(int)
	{
		return ++HDirectory(*this);
	}

	inline bool
	HDirectory::IsValid() const
	{
		return dir;
	}


	/*!
	\brief 判断指定路径字符串是否表示一个绝对路径。
	*/
	bool
	IsAbsolute(CPATH);

	/*!
	\brief 取指定路径的文件系统根节点名称的长度。
	*/
	std::size_t
	GetRootNameLength(CPATH);

	/*!
	\brief 快速刷新缓存映像到显示屏缓冲区。
	\note 第一参数为显示屏缓冲区，第二参数为源缓冲区。
	*/
	void
	ScreenSynchronize(PixelType*, const PixelType*);

	/*!
	\brief 复位屏幕显示模式。
	*/
	void
	ResetVideo();

	/*!
	\brief 启动控制台。
	\note fc 为前景色，bc为背景色。
	*/
	void
	YConsoleInit(u8 dspIndex,
		Color fc = ColorSpace::White, Color bc = ColorSpace::Black);

	/*!
	\brief 输出控制台字（int 型数据）。
	*/
	inline void
	iputw(int n)
	{
		iprintf("%d\n", n);
	}


	/*!
	\brief 等待任意按键。
	*/
	void
	WaitForInput();

	/*!
	\brief 等待 mask 包含的按键。
	*/
	void
	WaitForKey(u32 mask);

	/*!
	\brief 等待任意按键（除触摸屏、翻盖外）。
	*/
	inline void
	WaitForKeypad()
	{
		WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y | KEY_L | KEY_R
			| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
			| KEY_START | KEY_SELECT);
	}

	/*!
	\brief 等待任意按键（除 L 、 R 和翻盖外）。
	*/
	inline void
	WaitForFrontKey()
	{
		WaitForKey(KEY_TOUCH | KEY_A | KEY_B | KEY_X | KEY_Y
			| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
			| KEY_START | KEY_SELECT);
	}

	/*!
	\brief 等待任意按键（除 L 、 R 、触摸屏和翻盖外）。
	*/
	inline void
	WaitForFrontKeypad()
	{
		WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y
			| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
			|KEY_START | KEY_SELECT);
	}

	/*!
	\brief 等待方向键。
	*/
	inline void
	WaitForArrowKey()
	{
		WaitForKey(KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN);
	}

	/*!
	\brief 等待按键 A 、 B 、 X 、 Y 键。
	*/
	inline void
	WaitForABXY()
	{
		WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y);
	}

	/*
	void trimString(char* string);
	void unescapeString(char* string);
	u32 versionStringToInt(char* string);
	void versionIntToString(char* out, u32 version);
	*/

	/*!
	\brief 写入当前按键信息。
	*/
	void
	WriteKeysInfo(KeysInfo&, CursorInfo&);


	/*!
	\brief 组合计时器寄存器的值。
	*/
	inline u32
	timers2msRaw(vu16 l, vu16 h)
	{
		return l | (h << 16);
	}
	/*!
	\brief 寄存器 - 毫秒转换。
	*/
	inline u32
	timers2ms(vu16 l, vu16 h)
	{
		return timers2msRaw(l, h) >> 5;
	}

	/*!
	\brief 初始化计时器。
	\note 目标寄存器为指定参数。
	*/
	void
	InitTimers(vu16&, vu16&);
	/*!
	\brief 初始化计时器。
	\note 会判断是否已经初始化。若已初始化则不重复进行初始化。
	*/
	void
	InitTimers();

	/*!
	\brief 清除计时器。
	\note 目标寄存器为指定参数。
	*/
	inline void
	ClearTimers(vu16& l = TIMER0_DATA, vu16& h = TIMER1_DATA)
	{
		l = 0;
		h = 0;
	}

	/*!
	\brief 初始化实时时钟。
	*/
	inline void
	InitRTC()
	{
		InitTimers(TIMER2_CR, TIMER3_CR);
	}

	/*!
	\brief 清除实时时钟。
	*/
	inline void
	ClearRTC()
	{
		ClearTimers(TIMER2_DATA, TIMER3_DATA);
	}

	/*!
	\brief 复位实时时钟。
	*/
	inline void
	ResetRTC()
	{
		InitRTC();
		ClearRTC();
	}

	/*!
	\brief 取实时时钟计数。
	*/
	inline u32
	GetRTC()
	{
		return timers2ms(TIMER2_DATA, TIMER3_DATA);
	}

	/*!
	\brief 开始 tick 计时。
	*/
	void
	StartTicks();

	/*!
	\brief 取 tick 数。
	*/
	u32
	GetTicks();

	/*!
	\brief 延时 ms 毫秒。
	\note ms 仅低 25 位有效。
	*/
	void
	Delay(u32 ms);

	/*void setupCapture(int bank);
	void waitForCapture();
	char* basename(char*);
	*/

	/*!
	\brief 初始化视频输出。
	*/
	bool
	InitVideo();
}

namespace platform_ex
{
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
}

#endif

