﻿/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Video.cpp
\ingroup YCLib
\brief 平台相关的视频输出接口。
\version r373
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-26 20:19:54 +0800
\par 修改时间:
	2015-01-02 09:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::Video
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Video
#include YFM_YCLib_NativeAPI

namespace platform_ex
{

#if YCL_DS
using ::lcdMainOnTop;
using ::lcdMainOnBottom;
using ::lcdSwap;
using ::videoSetMode;
using ::videoSetModeSub;

using ::touchRead;
#endif

} // namespace platform_ex;

namespace platform
{

namespace
{

//! \since build 441
static_assert(sizeof(Pixel) == sizeof(Pixel::Trait::IntegerType),
	"Wrong size of pixel type found.");
static_assert(yalignof(Pixel) == yalignof(Pixel::Trait::IntegerType),
	"Wrong alignment of pixel type found.");

#if YCL_DS

extern "C"
{
	extern const ::u8 default_font[];
}

::PrintConsole mainConsole{
	{
		reinterpret_cast<std::uint16_t*>(const_cast<::u8*>(default_font)), \
			// font graphics;
		nullptr, 0, 4, // font palette, font color count and bpp;
		0, // first ASCII character in the set;
		128, // number of characters in the set;
		true // convert single color;
	},
	nullptr, nullptr, // font background map and graphics;
	22, 3, // map and char base;
	0, -1, // background layer in use and background ID;
	0, 0, // cursor X and Y coordinates;
	0, 0, // prev-cursor X and Y coordinates;
	32, 24, // console width and height;
	0, 0, // window X and Y coordinates;
	32, 24, //window width and height;
	3, // tab size;
	0, // font character offset;
	0, // selected palette;
	nullptr, // print callback;
	false, // console initialized
	true, // load graphics;
};

PrintConsole*
consoleMainInit()
{
	::videoSetMode(MODE_0_2D);
	::vramSetBankA(VRAM_A_MAIN_BG);
	return ::consoleInit(nullptr, mainConsole.bgLayer,
		BgType_Text4bpp, BgSize_T_256x256,
		mainConsole.mapBase, mainConsole.gfxBase, true, true);
}
#endif

}


void
#if YCL_DS
YConsoleInit(std::uint8_t dspIndex, Color fc, Color bc)
{
#define BITALPHA BIT(15) //!<  Alpha 位。
//	PrintConsole* p(dspIndex ? consoleMainInit() : consoleDemoInit());

	if(YB_LIKELY(dspIndex ? consoleMainInit() : consoleDemoInit()))
	{
		//使用 ANSI Escape 序列 CUrsor Position 指令设置光标位置为左上角。
		std::printf("\x1b[0;0H");

		std::uint16_t* bg_palette = dspIndex ? BG_PALETTE : BG_PALETTE_SUB;

		bg_palette[0] = Pixel(bc).Integer | BITALPHA;
		bg_palette[255] = Pixel(fc).Integer | BITALPHA;
	}
#else
YConsoleInit(std::uint8_t, Color, Color)
{
// TODO: Implementation.
#endif
}


bool
InitVideo()
{
#if YCL_DS
	platform_ex::ResetVideo();
	platform_ex::lcdMainOnTop();
#endif
	return true;
}
}

namespace platform_ex
{

#if YCL_DS
void
ResetVideo()
{
	REG_BG0CNT = 0,
	REG_BG1CNT = 0,
	REG_BG2CNT = 0,
	REG_BG3CNT = 0,
	REG_BG0CNT_SUB = 0,
	REG_BG1CNT_SUB = 0,
	REG_BG2CNT_SUB = 0,
	REG_BG3CNT_SUB = 0;
	::vramDefault();
	::vramSetBanks_EFG(VRAM_E_LCD, VRAM_F_LCD, VRAM_G_LCD),
	::vramSetBankH(VRAM_H_LCD),
	::vramSetBankI(VRAM_I_LCD);
//	memset(OAM, 0, SPRITE_COUNT * sizeof(SpriteEntry));
//	memset(OAM_SUB, 0, SPRITE_COUNT * sizeof(SpriteEntry));
	::videoSetMode(MODE_5_2D);
	::videoSetModeSub(MODE_5_2D);
}


platform::Pixel*
InitScrUp(int& id)
{
	//初始化背景，并得到屏幕背景ID 。
	id = ::bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

	//获得屏幕背景所用的显存地址。
	return reinterpret_cast<platform::Pixel*>(::bgGetGfxPtr(id));
}

platform::Pixel*
InitScrDown(int& id)
{
	//初始化背景，并得到屏幕背景ID 。
	id = ::bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

	//获得屏幕背景所用的显存地址。
	return reinterpret_cast<platform::Pixel*>(::bgGetGfxPtr(id));
}

void
ScreenSynchronize(platform::Pixel* buf, const platform::Pixel* src)
	ynothrow
{
	using ScreenBufferType = platform::Pixel[SCREEN_WIDTH * SCREEN_HEIGHT];

	::DC_FlushRange(src, sizeof(ScreenBufferType));
	::dmaCopyWordsAsynch(3, src, buf, sizeof(ScreenBufferType));
}
#endif

#if YCL_DS || YF_Hosted
bool
DSVideoState::IsLCDMainOnTop() const
{
#	if YCL_DS
	return REG_POWERCNT & POWER_SWAP_LCDS;
#	else
	return LCD_main_on_top;
#endif
}

void
DSVideoState::SetLCDMainOnTop(bool b)
{
#	if YCL_DS
	b ? lcdMainOnTop() : lcdMainOnBottom();
#	else
	LCD_main_on_top = b;
#endif
}

void
DSVideoState::SwapLCD()
{
#	if YCL_DS
	lcdSwap();
#	else
	LCD_main_on_top = !LCD_main_on_top;
#endif
}
#endif

}

