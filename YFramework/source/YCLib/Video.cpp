/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Video.cpp
\ingroup YCLib
\brief 平台相关的视频输出接口。
\version r1208;
\author FrankHB<frankhb1989@gmail.com>
\since build 312 。
\par 创建时间:
	2012-05-26 20:19:54 +0800;
\par 修改时间:
	2012-06-01 21:01 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::Video;
*/


#include "YCLib/Video.h"
#include "YCLib/NativeAPI.h"

namespace platform_ex
{

#ifdef YCL_DS
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
#ifdef YCL_DS
YConsoleInit(std::uint8_t dspIndex, Color fc, Color bc)
{
#define BITALPHA BIT(15) //!<  Alpha 位。
//	PrintConsole* p(dspIndex ? consoleMainInit() : consoleDemoInit());

	if(YB_LIKELY(dspIndex ? consoleMainInit() : consoleDemoInit()))
	{
		//使用 ANSI Escape 序列 CUrsor Position 指令设置光标位置为左上角。
		std::printf("\x1b[0;0H");

		PixelType* bg_palette = dspIndex ? BG_PALETTE : BG_PALETTE_SUB;

		bg_palette[0]	= bc | BITALPHA;
		bg_palette[255]	= fc | BITALPHA;
	}
#elif defined(YCL_MINGW32)
YConsoleInit(std::uint8_t, Color, Color)
{
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

