/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Main.cpp
\ingroup ImageBrowser
\brief 主界面。
\version r171
\author FrankHB <frankhb1989@gmail.com>
\since build 424
\par 创建时间:
	2013-07-07 12:57:53 +0800
\par 修改时间:
	2014-12-28 23:47 +0800
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
		try
		{
			GUIApplication app;
			ImageCodec codec;
			ImagePanel pnl(Rect(0, 0, 640, 480));

			pnl.Load(DecodeArg(argv[1]));
			ShowTopLevelDraggable(pnl);
			FetchEvent<Click>(pnl) += [](CursorEventArgs&& e){
				if(e.GetKeys()[KeyCodes::Secondary])
					YSLib::PostQuitMessage(0);
			};
			Execute(app);
		}
#ifdef YCL_Win32
		CatchExpr(Win32Exception& e, cerr
			<< "Unhandled Win32Exception exception" << e.GetErrorCode()
			<< "found: " << endl << e.GetMessage() << endl << "From: "
			<< e.what() << endl)
#endif
		CatchExpr(exception& e, cerr << "Unhandled exception ["
			<< typeid(e).name() << "]: " << endl << e.what() << endl)
		CatchExpr(..., cerr << "Unhandled exception found main function."
			<< endl)
}

