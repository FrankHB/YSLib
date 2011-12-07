/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycommon.cpp
\ingroup YCLib
\brief 平台相关的公共组件无关函数与宏定义集合。
\version r2407;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2009-11-12 22:14:42 +0800;
\par 修改时间:
	2011-12-06 11:58 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YCommon;
*/


#include "YCLib/ycommon.h"
#include <cstdarg>
#include <cstring>
#include <cerrno>


namespace platform
{
	void*
	mmbset(void* d, int v, std::size_t t)
	{
		// NOTE: DMA fill to main RAM is maybe slower;
	//	return safe_dma_fill(d, v, t) != 0 ? std::memset(d, v, t) : d;
		return std::memset(d, v, t);
	}

	void*
	mmbcpy(void* d, const void* s, std::size_t t)
	{
		// NOTE: DMA copy to main RAM is slower;
		// TODO: ASM optimization, like using LDMIA instructions;
	//	return safe_dma_copy(d, s, t) != 0 ? std::memcpy(d, s, t) : d;
		return std::memcpy(d, s, t);
	}

	char*
	getcwd_n(char* buf, std::size_t t)
	{
		if(buf)
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
		for(char* slash(path); (slash = strchr(slash, '/')); ++slash)
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
		for(;;)
			swiWaitForVBlank();
	//	std::terminate();
	}

	
	static bool bDebugStatus(false);

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

	void
	YDebugW(int n)
	{
		if(bDebugStatus)
		{
			YDebugBegin();
			iputw(n);
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

			t = viprintf(str, list);

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
		if(!exp)
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
		if(IsValid())
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
		return std::strchr(path, '/') == path
			|| std::strstr(path, "fat:/") == path;
	}

	std::size_t
	GetRootNameLength(const_path_t path)
	{
		const char* p(std::strchr(path, ':'));

		return !p ? 0 : p - path + 1;
	}

	void
	ScreenSynchronize(PixelType* buf, const PixelType* src)
	{
	//	YAssert(safe_dma_copy(buf, src, sizeof(ScreenBufferType)) == 0,
	//		"Screen sychronize failure;");
		DC_FlushRange(src, sizeof(ScreenBufferType));
		dmaCopy(src, buf, sizeof(ScreenBufferType));
	//	std::memcpy(buf, src, sizeof(ScreenBufferType));
	}

	void
	ResetVideo()
	{
		REG_BG0CNT = REG_BG1CNT = REG_BG2CNT = REG_BG3CNT = 0;
		REG_BG0CNT_SUB = REG_BG1CNT_SUB = REG_BG2CNT_SUB = REG_BG3CNT_SUB = 0;
		vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
		vramSetBankE(VRAM_E_LCD);
		vramSetBankF(VRAM_F_LCD);
		vramSetBankG(VRAM_G_LCD);
		vramSetBankH(VRAM_H_LCD);
		vramSetBankI(VRAM_I_LCD);
		std::memset(VRAM_A, 0, 0x90000);
	//	memset(OAM, 0, SPRITE_COUNT * sizeof(SpriteEntry));
	//	memset(OAM_SUB, 0, SPRITE_COUNT * sizeof(SpriteEntry));
		videoSetMode(MODE_5_2D);
		videoSetModeSub(MODE_5_2D);
	}


	namespace
	{
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
	}


	void
	YConsoleInit(u8 dspIndex, Color fc, Color bc)
	{
	//	PrintConsole* p(dspIndex ? consoleMainInit() : consoleDemoInit());

		if((dspIndex ? consoleMainInit() : consoleDemoInit()))
		{
			//使用 ANSI Escape 序列 CUrsor Position 指令设置光标位置为左上角。
			iprintf("\x1b[0;0H");

			PixelType* bg_palette = dspIndex ? BG_PALETTE : BG_PALETTE_SUB;

			bg_palette[0]	= bc | BITALPHA;
			bg_palette[255]	= fc | BITALPHA;
		}
	}


	void
	WaitForInput()
	{
		while(true)
		{
			scanKeys();
			if(keysDown())
				break;
			swiWaitForVBlank();
		}
	}

	void
	WaitForKey(u32 mask)
	{
		while(true)
		{
			scanKeys();
			if(keysDown() & mask)
				break;
			swiWaitForVBlank();
		}
	}


	void
	WriteKeysInfo(KeysInfo& key, CursorInfo& tp)
	{
		touchRead(&tp);
		//修正触摸位置。
		if(tp.px != 0 && tp.py != 0)
		{
			--tp.px;
			--tp.py;
		}
		else
		{
			tp.px = SCREEN_WIDTH;
			tp.py = SCREEN_HEIGHT;
		}
		//记录按键状态。
		key.Up = keysUp();
		key.Down = keysDown();
		key.Held = keysHeld();
	}


	/*void
	trimString(char* string) {
		int a = 0;
		int b = strlen(string);

		while(isspace(string[a]) && a < b) a++;
		while(isspace(string[b-1]) && b > a) b--;

		if(b-a <= 0) {
			//string length is zero
			string[0] = '\0';
			return;
		}
		memmove(string, string+a, b-a);
		string[b-a] = '\0';
	}
	void
	unescapeString(char* str) {
		char* s = str;
		while(*s != '\0')
		{
			if(*s == '\\')
			{
				switch (*++s) {
					case '\\': *str = '\\'; break;
					case '\'': *str = '\''; break;
					case '\"': *str = '\"'; break;
					case 'n':  *str = '\n'; break;
					case 'r':  *str = '\r'; break;
					case 't':  *str = '\t'; break;
					case 'f':  *str = '\f'; break;
					default: //Error
						break;
				}
			} else {
				*str = *s;
			}
			str++;
			s++;
		}
		*str = '\0';
	}

	//Accepts version strings in a(.b(.c)?)? format
	u32
	versionStringToInt(char* string) {
		u32 version = 0;

		char* firstDot = strchr(string, '.');
		char* secondDot = nullptr;
		if(firstDot) {
			*firstDot = '\0';
			secondDot = strchr(firstDot+1, '.');
			if(secondDot) {
				*secondDot = '\0';
			}
		}

		//Example result: 1.12.37 = 11237
		version = atoi(string)*10000;
		if(firstDot) {
			version += atoi(firstDot+1)*100;
			*firstDot = '.';
		}
		if(secondDot) {
			version += atoi(secondDot+1);
			*secondDot = '.';
		}
		return version;
	}
	void versionIntToString(char* out, u32 version)
	{
		sprintf(out, "%d.%d.%d", (int)(version/10000),
			(int)((version/100)%100), (int)(version%100));
	}*/


	static bool bUninitializedTimers(true);

	void
	InitTimers(vu16& l, vu16& h)
	{
		l = TIMER_ENABLE | TIMER_DIV_1024;
		h = TIMER_ENABLE | TIMER_CASCADE;
	}
	void
	InitTimers()
	{
		if(bUninitializedTimers)
		{
			TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1024;
			TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE;
			bUninitializedTimers = false;
		}
	}

	void
	StartTicks()
	{
		InitTimers();
		ClearTimers();
	}

	u32
	GetTicks()
	{
		InitTimers();
		return timers2ms(TIMER0_DATA, TIMER1_DATA);
	}

	void
	Delay(u32 ms)
	{
		bUninitializedTimers = true;
		StartTicks();
	/*
		ms &= 0x01FFFFFF;
		while(u32(timers2ms(TIMER0_DATA, TIMER1_DATA)) < ms)
			;
	*/
		ms <<= 5;
		while(u32(timers2msRaw(TIMER0_DATA, TIMER1_DATA)) < ms)
			;
	}


	/*void
	setupCapture(int bank) {
		REG_DISPCAPCNT = DCAP_ENABLE | DCAP_MODE(0)
			| DCAP_DST(0) | DCAP_SRC(0) | DCAP_SIZE(3)
			| DCAP_OFFSET(0) | DCAP_BANK(bank) | DCAP_B(15) | DCAP_A(0);
	}
	void
	waitForCapture() {
		while(REG_DISPCAPCNT & DCAP_ENABLE) {
			swiWaitForVBlank();
		}
	}

	char*
	basename(char* path)
	{
		int len = strlen(path);
		int sindex;
		for(int i = 0; i < len; ++i) {
			if(path[i] == '/')
				sindex = i;
		}
		return path+sindex+1;
	}*/

	bool
	InitVideo()
	{
		ResetVideo();
		//设置显示模式。
		vramSetBankA(VRAM_A_MAIN_BG);
		vramSetBankC(VRAM_C_SUB_BG);
		//设置主显示引擎渲染上屏。
		lcdMainOnTop();
	//	lcdMainOnBottom();
	//	lcdSwap();
		return true;
	}
}

namespace platform_ex
{
	platform::BitmapPtr
	InitScrUp(int& id)
	{
		//初始化背景，并得到屏幕背景ID 。
		id = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

		//获得屏幕背景所用的显存地址。
		return bgGetGfxPtr(id);
	}
	platform::BitmapPtr
	InitScrDown(int& id)
	{
		//初始化背景，并得到屏幕背景ID 。
		id = bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

		//获得屏幕背景所用的显存地址。
		return bgGetGfxPtr(id);
	}
}

