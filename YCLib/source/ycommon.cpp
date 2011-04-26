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
\version 0.2373;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-11-12 22:14:42 +0800;
\par 修改时间:
	2011-04-25 14:10 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YCommon;
*/


#include "ycommon.h"
#include <cstdarg>
#include <cstring>
#include <cerrno>
#include "ystdex/util.hpp" // for nullptr;


namespace platform
{
	namespace
	{
		//! \brief 检查 32 位地址是否在 DMA 操作有效范围之外。
		inline bool
		dma_out_of_range(u32 addr)
		{
			// 检查 TCM 和 BIOS (0x01000000, 0x0B000000, 0xFFFF0000) 。
			// NOTE: probably incomplete checks;
			return ((addr >> 24) == 0x01 || (addr >> 24) == 0x0B
				|| !~(addr|0xFFFF));
		}

		//! \brief 检查 32 位地址是否在主内存中。
		inline bool
		is_in_main_RAM(u32 addr)
		{
			return addr >> 24 == 0x02;
		}


		/*
		解释和代码参考：
		http://www.coranac.com/2009/05/dma-vs-arm9-fight/ ；
		http://www.coranac.com/2010/03/dma-vs-arm9-round-2/ 。
		*/

/*		const std::size_t ARM9_CACHE_LINE_SIZE(32);


		//! \brief 检查缓存区域端点。
		inline void
		dc_check(u32 addr)
		{
			if(addr % ARM9_CACHE_LINE_SIZE)
				DC_FlushRange(reinterpret_cast<void*>(addr), 1);
		}*/

		/*!
		\brief 检查缓存区域两端。
		\warning 如果之后无效化高速缓存，之间 CPU 对高速缓存的改动会被丢弃。
		*/
/*		inline void
		dc_check2(u32 addr, u32 size)
		{
			dc_check(addr); //检查缓存区域头部。
			dc_check(addr + size); //检查缓存区域尾部。
		}
*/

//		/*!
//		\brief 内存复制。
//		\note DMA 模式。
//		\warning 注意高速缓存刷新和无效化。
//		*/
/*		inline void
		dmacpy(void *dst, const void *src, std::size_t size)
		{
			dmaCopy(src, dst, size);
		}

		//! \brief 对已缓存区域的 DMA 复制。
		void
		dmacpy_cached(void *dst, const void *src, std::size_t size)
		{
			DC_FlushRange(src, size); //写回内存。

			u32 addr = reinterpret_cast<u32>(dst);
			dc_check2(addr, size);
			dmacpy(dst, src, size); //实际复制。
			DC_InvalidateRange(dst, size); //最终无效化。
		}
*/

		ystdex::errno_t
		safe_dma_fill(void *dst, int v, std::size_t size)
		{
			u32 d(reinterpret_cast<u32>(dst));

			if(dma_out_of_range(d))
				return 1;
			if(d & 1) //非对齐字节覆盖失败。
				return 2;
			while(DMA_CR(3) & DMA_BUSY)
				;
			v &= 0xFF;
			v |= v << 8;

			bool b(is_in_main_RAM(d)); //目标在主内存中。

			if(b)
			//	dc_check2(d, size);
				DC_FlushRange(dst, size);
			if((d | size) & 3)
				dmaFillHalfWords(v, dst, size);
			else
			{
				v |= v << 16;
				dmaFillWords(v, dst, size);
			}
			if(b) //设置目标范围内高速缓存污染状态。
				DC_InvalidateRange(dst, size);
			return 0;
		}

		ystdex::errno_t
		safe_dma_copy(void *dst, const void *src, std::size_t size)
		{
			const u32 s(reinterpret_cast<u32>(src)),
				d(reinterpret_cast<u32>(dst));

			// 检查 TCM 和 BIOS (0x01000000, 0x0B000000, 0xFFFF0000) 。
			// NOTE: probably incomplete checks;
			if(dma_out_of_range(s) || dma_out_of_range(d))
				return 1;
			if((s | d) & 1) //非对齐字节复制失败。
				return 2;
			while(DMA_CR(3) & DMA_BUSY)
				;
			if(is_in_main_RAM(s)) //需要写回内存。
				DC_FlushRange(src, size);

			bool b(is_in_main_RAM(d));

			if(b)
			//	dc_check2(d, size);
				DC_FlushRange(dst, size);
			if((s | d | size) & 3)
				dmaCopyHalfWords(3, src, dst, size);
			else
				dmaCopyWords(3, src, dst, size);
		//	if(b) //设置目标范围内高速缓存污染状态。
		//		DC_InvalidateRange(dst, size);
			return 0;
		}
	}

	void*
	mmbset(void* d, int v, std::size_t t)
	{
	//	return safe_dma_fill(d, v, t) != 0 ? std::memset(d, v, t) : d;
		return std::memset(d, v, t);
	}

	void*
	mmbcpy(void* d, const void* s, std::size_t t)
	{
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
	direxists(CPATH path)
	{
		DIR_ITER* dirState(::diropen(path));

		::dirclose(dirState);
		return dirState;
	}

	bool
	mkdirs(CPATH cpath)
	{
		PATHSTR path;

		std::strcpy(path, cpath);
		for(char* slash = path; (slash = strchr(slash, '/')); ++slash)
		{
			*slash = '\0';
			::mkdir(path, S_IRWXU|S_IRWXG|S_IRWXO);
			*slash = '/';
		}
		//新建目录成功或目标路径已存在时返回 true 。
		return ::mkdir(path, S_IRWXU|S_IRWXG|S_IRWXO) == 0 || errno == EEXIST;
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
		LastError = ::dirnext(dir, Name, &Stat);
		return *this;
	}

	bool
	HDirectory::IsDirectory()
	{
		return Stat.st_mode & S_IFDIR;
	}

	void
	HDirectory::Open(CPATH path)
	{
		dir = path ? ::diropen(path) : nullptr;
	}

	void
	HDirectory::Close()
	{
		if(IsValid())
			LastError = ::dirclose(dir);
	}

	void
	HDirectory::Reset()
	{
		LastError = ::dirreset(dir);
	}


	bool
	IsAbsolute(CPATH path)
	{
		return std::strchr(path, '/') == path
			|| std::strstr(path, "fat:/") == path;
	}

	std::size_t
	GetRootNameLength(CPATH path)
	{
		const char* p(std::strchr(path, ':'));

		return !p ? 0 : p - path + 1;
	}

	void
	ScreenSynchronize(PixelType* buf, const PixelType* src)
	{
	//	YAssert(safe_dma_copy(buf, src, sizeof(ScreenBufferType)) == 0,
	//		"Screen sychronize failure;");
		std::memcpy(buf, src, sizeof(ScreenBufferType));
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
		while(static_cast<u32>(timers2ms(TIMER0_DATA, TIMER1_DATA)) < ms)
			;
	*/
		ms <<= 5;
		while(static_cast<u32>(timers2msRaw(TIMER0_DATA, TIMER1_DATA)) < ms)
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

