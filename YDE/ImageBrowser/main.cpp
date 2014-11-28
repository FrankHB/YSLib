/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file main.cpp
\ingroup ImageBrowser
\brief 主界面。
\version r147
\author FrankHB <frankhb1989@gmail.com>
\since build 424
\par 创建时间:
	2013-07-07 12:57:53 +0800
\par 修改时间:
	2014-11-27 12:26 +0800
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

	if(argc > 1)
		try
		{
			GUIApplication app;
			ImageCodec codec;
			ImagePanel pnl(Rect(0, 0, 640, 480));

			pnl.Load(platform_ex::MBCSToWCS(argv[1]));
			ShowTopLevelDraggable(pnl);
//			GetWindowPtrOf(ctl)->SetOpacity(0xC0);
//			auto h_wnd(wnd.GetNativeHandle());
//			wnd.SetOpacity(0xFF);
//			wnd.Opacity = 0xFF;
			GetWindowPtrOf(pnl)->UseOpacity = true;
			FetchEvent<Click>(pnl) += [](CursorEventArgs&& e){
				if(e.GetKeys()[VK_RBUTTON])
					YSLib::PostQuitMessage(0);
			};
			Execute(app);
		}
		catch(platform_ex::Win32Exception& e)
		{
			cerr << "Unhandled Win32Exception" << e.GetErrorCode() << "found: "
				<< endl << e.GetMessage() << endl << "From: " << e.what()
				<< endl;
		}
		catch(std::exception& e)
		{
			cerr << "Unhandled exception [" << typeid(e).name() << "]: " << endl
				<< e.what() << endl;
		}
		catch(...)
		{
			cerr << "Unhandled exception found main function." << endl;
		}
}

