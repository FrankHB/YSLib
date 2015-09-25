/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Main.cpp
\ingroup ImageBrowser
\brief 主界面。
\version r192
\author FrankHB <frankhb1989@gmail.com>
\since build 424
\par 创建时间:
	2013-07-07 12:57:53 +0800
\par 修改时间:
	2015-09-24 12:18 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	ImageBrowser::Main
*/


#include "ImageControl.h"
#include <iostream>
using namespace std;

int
main(int argc, char* argv[])
{
	using namespace ImageBrowser;
	using namespace Host;
	using namespace platform_ex;

	if(argc > 1)
		FilterExceptions([=]{
			GUIApplication app;
			ImageCodec codec;
			ImagePanel pnl({Point::Invalid, 640, 480});

			pnl.Load(DecodeArg(argv[1]));
			ShowTopLevelDraggable(pnl);
			pnl.SetRootMode(Deref(GetHostRendererPtrOf(pnl)).RootMode),
			pnl.SetupContextMenu();
			Execute(app);
		}, yfsig);
}

