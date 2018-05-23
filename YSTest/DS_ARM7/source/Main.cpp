/*
	© 2009-2013, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Main.cpp
\ingroup DS
\brief ARM7 主源文件。
\version r113
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-18 12:27:40 +0800
\par 修改时间:
	2018-05-23 17:23 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::Main_ARM7
*/


#include <nds.h>
//	#include <dswifi7.h>
//	#include <maxmod7.h>

int
main()
{
	::irqInit();
	::fifoInit();
	::touchInit();

	//从固件中读取用户设置。
	::readUserSettings();

	// 开始使用 RTC 跟踪 IRQ 。
	::initClockIRQ();

	::SetYtrigger(80);

	//installWifiFIFO();
	::installSoundFIFO();

	//::mmInstall(FIFO_MAXMOD);

	::installSystemFIFO();

	::irqSet(IRQ_VCOUNT, ::inputGetAndSend);
//	::irqSet(IRQ_VBLANK, Wifi_Update);

	::irqEnable(IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

	//保持 ARM7 空闲状态。
	while(true)
		::swiWaitForVBlank();
}

