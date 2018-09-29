/*
	© 2015, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSVideo.cpp
\ingroup YCLib
\ingroup DS
\brief DS 视频输出接口。
\version r200
\author FrankHB <frankhb1989@gmail.com>
\since build 585
\par 创建时间:
	2015-03-17 12:46:32 +0800
\par 修改时间:
	2018-09-20 02:33 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(DS)::DSVideo
*/


#include "YCLib/YModules.h"
#include YFM_DS_YCLib_DSVideo // for ystdex::replace_cast;
#include YFM_YCLib_NativeAPI

namespace platform_ex
{

using namespace platform;

namespace
{

#if YCL_DS
extern "C" const ::u8 default_font[];

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
#endif

} // unnamed namespace;

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
//	std::fill_n(static_cast<void*>(OAM), SPRITE_COUNT * sizeof(SpriteEntry), 0),
//	std::fill_n(static_cast<void*>(OAM_SUB), SPRITE_COUNT * sizeof(SpriteEntry),
//		0);
	::videoSetMode(MODE_5_2D);
	::videoSetModeSub(MODE_5_2D);
}


platform::Pixel*
InitScrUp(int& id)
{
	id = ::bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
	return ystdex::replace_cast<platform::Pixel*>(::bgGetGfxPtr(id));
}

platform::Pixel*
InitScrDown(int& id)
{
	id = ::bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
	return ystdex::replace_cast<platform::Pixel*>(::bgGetGfxPtr(id));
}

void
ScreenSynchronize(platform::Pixel* buf, const platform::Pixel* src)
	ynothrow
{
	using ScreenBufferType = platform::Pixel[SCREEN_WIDTH * SCREEN_HEIGHT];

	::DC_FlushRange(src, sizeof(ScreenBufferType));
	::dmaCopyWordsAsynch(3, src, buf, sizeof(ScreenBufferType));
}

void
DSConsoleInit(bool use_customed, YSLib::Drawing::Color fc,
	YSLib::Drawing::Color bc) ynothrow
{
	if(YB_LIKELY(use_customed ? []{
		::videoSetMode(MODE_0_2D);
		::vramSetBankA(VRAM_A_MAIN_BG);
		return ::consoleInit({}, mainConsole.bgLayer,
			BgType_Text4bpp, BgSize_T_256x256,
			mainConsole.mapBase, mainConsole.gfxBase, true, true);
	}() : ::consoleDemoInit()))
	{
		// NOTE: Set cursor position to left top side.
		std::printf("\x1b[0;0H");

		const auto bg_palette(use_customed ? BG_PALETTE : BG_PALETTE_SUB);

		bg_palette[0] = Pixel(bc).Integer | BIT(15),
		bg_palette[255] = Pixel(fc).Integer | BIT(15);
	}
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
	b ? ::lcdMainOnTop() : ::lcdMainOnBottom();
#	else
	LCD_main_on_top = b;
#endif
}

void
DSVideoState::SwapLCD()
{
#	if YCL_DS
	::lcdSwap();
#	else
	LCD_main_on_top = !LCD_main_on_top;
#	endif
}
#endif

} // namespace platform_ex;

