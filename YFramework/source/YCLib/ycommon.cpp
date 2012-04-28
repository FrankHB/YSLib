/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycommon.cpp
\ingroup YCLib
\brief 平台相关的公共组件无关函数与宏定义集合。
\version r3088;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-12 22:14:42 +0800;
\par 修改时间:
	2012-04-28 15:36 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::YCommon;
*/


#include "YCLib/Debug.h"
#include <cstring>
#include <cerrno>
#include <CHRLib/chrproc.h>
#ifdef YCL_MINGW32
#include <mmsystem.h> // for multimedia timers;
#include <Shlwapi.h> // for ::PathIsRelative;
#endif

namespace platform
{

static_assert(std::is_same<CHRLib::ucs2_t, char16_t>::value,
	"Wrong character type!");
static_assert(std::is_same<CHRLib::ucs4_t, char32_t>::value,
	"Wrong character type!");
#ifdef YCL_MINGW32
// TODO: assert %alignof equality;
static_assert(sizeof(wchar_t) == sizeof(CHRLib::ucs2_t),
	"Wrong character type!");
#endif

void*
mmbset(void* d, int v, std::size_t t)
{
#ifdef YCL_DS
	// NOTE: DMA fill to main RAM is maybe slower;
//	return safe_dma_fill(d, v, t) != 0 ? std::memset(d, v, t) : d;
#endif
	return std::memset(d, v, t);
}

void*
mmbcpy(void* d, const void* s, std::size_t t)
{
#ifdef YCL_DS
	// NOTE: DMA copy to main RAM is even slower;
	// TODO: use ASM optimization, like using LDMIA instructions;
//	return safe_dma_copy(d, s, t) != 0 ? std::memcpy(d, s, t) : d;
#endif
	return std::memcpy(d, s, t);
}


std::FILE*
ufopen(const char* filename, const char* mode)
{
	yconstraint(filename),
	yconstraint(mode);
	yconstraint(*mode != '\0');

#ifdef YCL_DS
	return std::fopen(filename, mode);
#elif defined(YCL_MINGW32)
	using namespace CHRLib;

	const auto wname(reinterpret_cast<wchar_t*>(ucsdup(filename)));
	wchar_t tmp[5];
	auto wmode(&tmp[0]);

	if(YCL_UNLIKELY(std::strlen(mode) > 4))
		wmode = reinterpret_cast<wchar_t*>(ucsdup(mode));
	else
		CHRLib::MBCSToUCS2(reinterpret_cast<ucs2_t*>(wmode), mode);

	const auto fp(::_wfopen(wname, wmode));

	if(wmode != tmp)
		std::free(wmode);
	std::free(wname);
	return fp;
#else
#	error Unsupported platform found!
#endif
}

bool
ufexists(const char* path)
{
#ifdef YCL_DS
	return ystdex::fexists(path);
#elif defined(YCL_MINGW32)
	yconstraint(path);

	if(const auto file = ufopen(path, "rb"))
	{
		std::fclose(file);
		return true;
	}
	return false;
#else
#	error Unsupported platform found!
#endif
}

char*
getcwd_n(char* buf, std::size_t size)
{
	if(YCL_LIKELY(buf))
		return ::getcwd(buf, size);
	return nullptr;
}

char16_t*
u16getcwd_n(char16_t* buf, std::size_t size)
{
	if(size == 0)
	//	last_err = EINVAL;
		;
	else
	{
		using namespace std;
		using namespace CHRLib;

		if(YCL_LIKELY(buf))
#ifdef YCL_DS
		{
			const auto p(static_cast<ucs2_t*>(malloc((size + 1)
				* sizeof(ucs2_t))));

			if(YCL_LIKELY(p))
			{
				auto len(MBCSToUCS2(p,
					::getcwd(reinterpret_cast<char*>(buf), size)));

				if(size < len + 1)
				{
				//	last_err = ERANGE;
					return nullptr;
				}
				memcpy(buf, p, ++len * sizeof(ucs2_t));
				return buf;
			}
		//	else
			//	last_err = ENOMEM;
		}
#elif defined(YCL_MINGW32)
			return reinterpret_cast<ucs2_t*>(
				::_wgetcwd(reinterpret_cast<wchar_t*>(buf), size));
#else
#	error Unsupported platform found!
#endif
	}
	return nullptr;
}

bool
direxists(const_path_t path)
{
	auto dirState(::opendir(path));

	::closedir(dirState);
	return dirState;
}

bool
mkdirs(const_path_t cpath)
{
	PATHSTR path;

	std::strcpy(path, cpath);
	for(char* slash(path); (slash = strchr(slash, YCL_PATH_DELIMITER));
		++slash)
	{
		*slash = '\0';
		::mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
		*slash = '/';
	}
	//新建目录成功或目标路径已存在时返回 true 。
	return ::mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == 0
		|| errno == EEXIST;
}


void
terminate()
{
#ifdef YCL_DS
	for(;;)
		platform_ex::swiWaitForVBlank();
#else
	std::terminate();
#endif
}


#ifdef YCL_USE_YASSERT

void
yassert(bool exp, const char* expstr, const char* message,
	int line, const char* file)
{
	if(YCL_UNLIKELY(!exp))
	{
		yprintf("Assertion failed: \n"
			"%s\nMessage: \n%s\nAt line %i in file \"%s\".\n",
			expstr, message, line, file);
		std::abort();
	}
}

#endif


int HFileNode::LastError(0);

HFileNode&
HFileNode::operator++()
{
	LastError = 0;
	if(YCL_LIKELY(IsValid()))
	{
		if(!(p_dirent = ::readdir(dir)))
			LastError = 2;
	}
	else
		LastError = 1;
	return *this;
}

bool
HFileNode::IsDirectory() const
{
#if defined(YCL_DS) || defined(YCL_MINGW32)
	return p_dirent && platform_ex::IsDirectory(*p_dirent);
#else
#	error Unsupported platform found!
#endif
}

void
HFileNode::Open(const_path_t path)
{
	dir = path ? ::opendir(path) : nullptr;
}

void
HFileNode::Close()
{
	LastError = IsValid() ? ::closedir(dir) : 1;
}


bool
IsAbsolute(const_path_t path)
{
#ifdef YCL_DS
	return std::strchr(path, '/') == path
		|| std::strstr(path, "fat:/") == path
		|| std::strstr(path, "sd:/");
#elif defined(YCL_MINGW32)
	return !::PathIsRelativeA(path);
	return false;
#else
#	error Unsupported platform found!
#endif
}

std::size_t
GetRootNameLength(const_path_t path)
{
	const char* p(std::strchr(path, ':'));

	return !p ? 0 : p - path + 1;
}


namespace
{
#ifdef YCL_DS
	extern "C"
	{
		extern const u8 default_font[];
	}

	PrintConsole mainConsole =
	{
		//Font:
		{
			reinterpret_cast<u16*>(
				const_cast<u8*>(default_font)), //font gfx
			0, //font palette
			0, //font color count
			4, //bpp
			0, //first ascii character in the set
			128, //number of characters in the font set
			true //convert single color
		},
		0, //font background map
		0, //font background gfx
		22, //map base
		3, //char base
		0, //bg layer in use
		-1, //bg id
		0,0, //cursorX cursorY
		0,0, //prevcursorX prevcursorY
		32, //console width
		24, //console height
		0,  //window x
		0,  //window y
		32, //window width
		24, //window height
		3, //tab size
		0, //font character offset
		0, //selected palette
		0,  //print callback
		false, //console initialized
		true, //load graphics
	};

	PrintConsole*
	consoleMainInit()
	{
		videoSetMode(MODE_0_2D);
		vramSetBankA(VRAM_A_MAIN_BG);
		return consoleInit(nullptr, mainConsole.bgLayer,
			BgType_Text4bpp, BgSize_T_256x256,
			mainConsole.mapBase, mainConsole.gfxBase, true, true);
	}
#endif
}


void
YConsoleInit(u8 dspIndex, Color fc, Color bc)
{
#ifdef YCL_DS
#define BITALPHA BIT(15) //!<  Alpha 位。
//	PrintConsole* p(dspIndex ? consoleMainInit() : consoleDemoInit());

	if(YCL_LIKELY(dspIndex ? consoleMainInit() : consoleDemoInit()))
	{
		//使用 ANSI Escape 序列 CUrsor Position 指令设置光标位置为左上角。
		std::printf("\x1b[0;0H");

		PixelType* bg_palette = dspIndex ? BG_PALETTE : BG_PALETTE_SUB;

		bg_palette[0]	= bc | BITALPHA;
		bg_palette[255]	= fc | BITALPHA;
	}
#elif defined(YCL_MINGW32)
// TODO: impl;
#else
#	error Unsupported platform found!
#endif
}


namespace
{
	bool bUninitializedTimers(true);
#ifdef YCL_DS
	vu32 system_tick;

	void
	timer_callback()
	{
		++system_tick;
	}
#elif defined(YCL_MINGW32)
	u32(*p_tick_getter)(); //!< 计时器指针。
	u64(*p_tick_getter_nano)(); //!< 高精度计时器指针。
	union
	{
		::DWORD start;
		struct
		{
			::LARGE_INTEGER start;
			::LARGE_INTEGER tps; //!< 每秒 tick 数。
		} hi; //!< 高精度计时器状态。
	} g_ticks; //!< 计时器状态。

	u32
	get_ticks_hi_res()
	{
		::LARGE_INTEGER now;

		::QueryPerformanceCounter(&now);
		now.QuadPart -= g_ticks.hi.start.QuadPart;
		now.QuadPart *= 1000;
		return ::DWORD(now.QuadPart / g_ticks.hi.tps.QuadPart);
	}

	u64
	get_ticks_hi_res_nano()
	{
		::LARGE_INTEGER now;

		::QueryPerformanceCounter(&now);
		now.QuadPart -= g_ticks.hi.start.QuadPart;
		now.QuadPart *= 1000000000;
		return now.QuadPart / g_ticks.hi.tps.QuadPart;
	}

	u32
	get_ticks_mm()
	{
		::DWORD now(::timeGetTime());

		return now < g_ticks.start ? (~::DWORD(0) - g_ticks.start) + now
			: now - g_ticks.start;
	}

	u64
	get_ticks_mm_nano()
	{
		return get_ticks_mm() * 1000000;
	}
#endif
}

void
StartTicks()
{
	if(bUninitializedTimers)
	{
#ifdef YCL_DS
		// f = 33.513982MHz;
		// BUS_CLOCK = 33513982 = 2*311*53881;
#if 0
		::irqSet(IRQ_TIMER(2), timer_callback);
		::irqEnable(IRQ_TIMER(2));
		TIMER2_DATA = 0;
		TIMER2_CR = TIMER_ENABLE | ::ClockDivider_1;
#endif
		::timerStart(2, ::ClockDivider_1, u16(TIMER_FREQ(1000)),
			timer_callback);
#elif defined(YCL_MINGW32)
		if(::QueryPerformanceFrequency(&g_ticks.hi.tps))
		{
			yunseq(p_tick_getter = get_ticks_hi_res,
				p_tick_getter_nano = get_ticks_hi_res_nano),
			::QueryPerformanceCounter(&g_ticks.hi.start);
		}
		else
		{
			yunseq(p_tick_getter = get_ticks_mm,
				p_tick_getter_nano = get_ticks_mm_nano),
			::timeBeginPeriod(1); //精度 1 毫秒。
			// FIXME: ::timeEndPeriod shall be used at exit;
			g_ticks.start = ::timeGetTime();
		}
#else
#	error Unsupported platform found!
#endif
		bUninitializedTimers = false;
	};
}

u32
GetTicks()
{
	StartTicks();
#ifdef YCL_DS
	return system_tick;
#elif defined(YCL_MINGW32)
	return p_tick_getter();
#else
#	error Unsupported platform found!
#endif
}

u64
GetHighResolutionTicks()
{
	StartTicks();
#ifdef YCL_DS
	return system_tick * 1000000ULL
		+ TIMER2_DATA * 1000000ULL / BUS_CLOCK;
#elif defined(YCL_MINGW32)
	return p_tick_getter_nano();
#else
#	error Unsupported platform found!
#endif
}

bool
InitVideo()
{
#ifdef YCL_DS
	platform_ex::ResetVideo();
	//设置显示模式。
	::vramSetBankA(VRAM_A_MAIN_BG);
	::vramSetBankC(VRAM_C_SUB_BG);
	//设置主显示引擎渲染上屏。
	platform_ex::lcdMainOnTop();
//	platform_ex::lcdMainOnBottom();
//	platform_ex::lcdSwap();
#endif
	return true;
}
}

namespace platform_ex
{
#ifdef YCL_DS
bool
AllowSleep(bool b)
{
	static bool bSleepEnabled(true); //与 libnds 默认的 ARM7 电源管理同步。
	const bool b_old(bSleepEnabled);

	if(b != bSleepEnabled)
	{
		bSleepEnabled = b;
		::fifoSendValue32(FIFO_PM,
			b ? PM_REQ_SLEEP_ENABLE : PM_REQ_SLEEP_DISABLE);
	}
	return b_old;
}


void
ResetVideo()
{
	REG_BG0CNT = REG_BG1CNT = REG_BG2CNT = REG_BG3CNT = 0;
	REG_BG0CNT_SUB = REG_BG1CNT_SUB = REG_BG2CNT_SUB = REG_BG3CNT_SUB = 0;
	::vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
	::vramSetBankE(VRAM_E_LCD);
	::vramSetBankF(VRAM_F_LCD);
	::vramSetBankG(VRAM_G_LCD);
	::vramSetBankH(VRAM_H_LCD);
	::vramSetBankI(VRAM_I_LCD);
	std::memset(VRAM_A, 0, 0x90000);
//	memset(OAM, 0, SPRITE_COUNT * sizeof(SpriteEntry));
//	memset(OAM_SUB, 0, SPRITE_COUNT * sizeof(SpriteEntry));
	videoSetMode(MODE_5_2D);
	videoSetModeSub(MODE_5_2D);
}


platform::BitmapPtr
InitScrUp(int& id)
{
	//初始化背景，并得到屏幕背景ID 。
	id = ::bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

	//获得屏幕背景所用的显存地址。
	return ::bgGetGfxPtr(id);
}

platform::BitmapPtr
InitScrDown(int& id)
{
	//初始化背景，并得到屏幕背景ID 。
	id = ::bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

	//获得屏幕背景所用的显存地址。
	return ::bgGetGfxPtr(id);
}

void
ScreenSynchronize(platform::PixelType* buf, const platform::PixelType* src)
{
	typedef platform::PixelType
		ScreenBufferType[SCREEN_WIDTH * SCREEN_HEIGHT]; //!< 主显示屏缓冲区。

//	YAssert(safe_dma_copy(buf, src, sizeof(ScreenBufferType)) == 0,
//		"Screen sychronize failure.");
	DC_FlushRange(src, sizeof(ScreenBufferType));
	dmaCopyWordsAsynch(3, src, buf, sizeof(ScreenBufferType));
//	std::memcpy(buf, src, sizeof(ScreenBufferType));
}
#endif
}

