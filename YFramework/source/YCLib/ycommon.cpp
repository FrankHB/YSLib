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
\version r2702;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-12 22:14:42 +0800;
\par 修改时间:
	2012-03-31 22:03 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::YCommon;
*/


#include "YCLib/ycommon.h"
#include <cstdarg>
#include <cstring>
#include <cerrno>

namespace
{
#ifdef YCL_DS
	u32 keys(0), keys_old(0);

	inline void
	clear_keys()
	{
		yunseq(keys = 0, keys_old = 0);
	}

	inline u32
	keys_down()
	{
		return keys &~ keys_old;
	}

	inline u32
	keys_up()
	{
		return (keys ^ keys_old) & ~keys;
	}

	inline void
	update_keys()
	{
		keys_old = keys;
		keys = ::keysCurrent();
	}
#endif
}


namespace platform
{
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

	char*
	getcwd_n(char* buf, std::size_t t)
	{
		if(YCL_LIKELY(buf))
			return ::getcwd(buf, t);
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
		for(char* slash(path); (slash = strchr(slash, DEF_PATH_DELIMITER));
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

	namespace
	{
		static bool bDebugStatus(false);
	}

	void
	YDebugSetStatus(bool s)
	{
		bDebugStatus = s;
	}

	bool
	YDebugGetStatus()
	{
		return bDebugStatus;
	}

	void
	YDebugBegin(Color fc, Color bc)
	{
		if(bDebugStatus)
			YConsoleInit(false, fc, bc);
	}

	void
	YDebug()
	{
		if(bDebugStatus)
		{
			YDebugBegin();
			WaitForInput();
		}
	}
	void
	YDebug(const char* s)
	{
		if(bDebugStatus)
		{
			YDebugBegin();
			std::puts(s);
			WaitForInput();
		}
	}

	int
	yprintf(const char* str, ...)
	{
		int t = -1;

		if(bDebugStatus)
		{
			YDebugBegin();

			va_list list;

			va_start(list, str);

			t = std::vprintf(str, list);

			va_end(list);
			WaitForInput();
		}
		return t;
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


	PATHSTR HDirectory::Name;
	struct ::stat HDirectory::Stat;
	int HDirectory::LastError(0);

	HDirectory&
	HDirectory::operator++()
	{
		LastError = 0;
		if(YCL_LIKELY(IsValid()))
		{
			auto dp(::readdir(dir));

			if(dp)
				std::strcpy(Name, dp->d_name);
			else
				LastError = 2;
		}
		else
			LastError = 1;
		return *this;
	}

	bool
	HDirectory::IsDirectory()
	{
		return Stat.st_mode & S_IFDIR;
	}

	void
	HDirectory::Open(const_path_t path)
	{
		dir = path ? ::opendir(path) : nullptr;
	}

	void
	HDirectory::Close()
	{
		LastError = IsValid() ? ::closedir(dir) : 1;
	}

	void
	HDirectory::Reset()
	{
		::rewinddir(dir);
	}


	bool
	IsAbsolute(const_path_t path)
	{
#ifdef YCL_DS
		return std::strchr(path, '/') == path
			|| std::strstr(path, "fat:/") == path;
#elif defined(YCL_MINGW32)
		// TODO: impl;
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


	void
	WaitForInput()
	{
#ifdef YCL_DS
		while(true)
 		{
			update_keys();
			if(::keys_down())
				break;
 			swiWaitForVBlank();
		};
#else
		std::getchar();
#endif
	}


	void
	WriteKeys(KeysInfo& info)
	{
#ifdef YCL_DS
		update_keys();
		//记录按键状态。
		yunseq(info.Up = ::keys_up(), info.Down = ::keys_down(),
			info.Held = keys);
#elif defined(YCL_MINGW32)
	// TODO: impl;
#else
#	error Unsupported platform found!
#endif
	}

	void
	WriteCursor(CursorInfo& tp)
	{
#ifdef YCL_DS
		touchRead(&tp);
		//修正触摸位置。
		if(YCL_LIKELY(tp.px != 0 && tp.py != 0))
			yunseq(--tp.px, --tp.py);
		else
			// NOTE: Point::Invalid;
			yunseq(tp.px = SDst(-1), tp.py = SDst(-1));
#elif defined(YCL_MINGW32)
	// TODO: impl;
#else
#	error Unsupported platform found!
#endif
	}


	namespace
	{
#ifdef YCL_DS
		vu32 system_tick;
		bool bUninitializedTimers(true);

		void
		timer_callback()
		{
			++system_tick;
		}
#endif
	}

	void
	StartTicks()
	{
#ifdef YCL_DS
		if(bUninitializedTimers)
		{
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
			bUninitializedTimers = false;
		};
#elif defined(YCL_MINGW32)
	// TODO: impl;
#else
#	error Unsupported platform found!
#endif
	}

	u32
	GetTicks()
	{
		StartTicks();
#ifdef YCL_DS
		return system_tick;
#elif defined(YCL_MINGW32)
		return 0;
	// TODO: impl;
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
		return 0;
	// TODO: impl;
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
	WaitForKey(u32 mask)
	{
		while(true)
 		{
			update_keys();
			if(keys_down() & mask)
				break;
 			swiWaitForVBlank();
		};
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
	//		"Screen sychronize failure;");
		DC_FlushRange(src, sizeof(ScreenBufferType));
		dmaCopyWordsAsynch(3, src, buf, sizeof(ScreenBufferType));
	//	std::memcpy(buf, src, sizeof(ScreenBufferType));
	}
#endif
}

