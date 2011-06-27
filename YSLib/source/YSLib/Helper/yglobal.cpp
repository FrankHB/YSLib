/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yglobal.cpp
\ingroup Helper
\brief 平台相关的全局对象和函数定义。
\version 0.3249;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-22 15:28:52 +0800;
\par 修改时间:
	2011-06-25 21:52 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Helper::YGlobal;
*/


#include "yglobal.h"
#include "../Core/yfilesys.h"
#include "../Core/yapp.h"
#include "../Core/yshell.h"
#include "../Adaptor/yfont.h"
#include "../Adaptor/ysinit.h"
#include "../UI/ydesktop.h"
#include "shlds.h"
//#include <clocale>

YSL_BEGIN

using Devices::DSScreen;
using namespace Drawing;

//全局常量。
extern const char* DEF_DIRECTORY; //!< 默认目录。
extern const char* G_COMP_NAME; //!< 制作组织名称。
extern const char* G_APP_NAME; //!< 产品名称。
extern const char* G_APP_VER; //!< 产品版本。
const IO::Path YApplication::CommonAppDataPath(DEF_DIRECTORY);
const String YApplication::CompanyName(G_COMP_NAME);
const String YApplication::ProductName(G_APP_NAME);
const String YApplication::ProductVersion(G_APP_VER);

//全局变量。
#ifdef YSL_USE_MEMORY_DEBUG
//MemoryList DebugMemory(nullptr);
#endif

/*!
\ingroup PublicObject
\brief 全局变量映射。
*/
//@{
//@}


YSL_BEGIN_NAMESPACE(Devices)

DSScreen::DSScreen(SDst w, SDst h, BitmapPtr p)
	: Devices::Screen(w, h, p),
	bg(-1)
{}

BitmapPtr
DSScreen::GetCheckedBufferPtr() const ynothrow
{
	if(!GetBufferPtr())
	{
		InitVideo();

		DSScreen& up_scr(FetchGlobalInstance().GetScreenUp());

		up_scr.pBuffer = DS::InitScrUp(up_scr.bg);

		DSScreen& down_scr(FetchGlobalInstance().GetScreenDown());

		down_scr.pBuffer = DS::InitScrDown(down_scr.bg);
	}

	return Devices::Screen::GetCheckedBufferPtr();
}

void
DSScreen::Update(BitmapPtr buf)
{
	platform::ScreenSynchronize(GetCheckedBufferPtr(), buf);
}
void
DSScreen::Update(Color c)
{
	FillPixel<PixelType>(GetCheckedBufferPtr(), GetAreaFrom(GetSize()), c);
}

YSL_END_NAMESPACE(Devices)


YDSApplication::YDSApplication()
	: pFontCache(), hScreenUp(), hScreenDown(), hDesktopUp(), hDesktopDown()
{}

FontCache&
YDSApplication::GetFontCache() const ythrow(LoggedEvent)
{
	if(!pFontCache)
		throw LoggedEvent("Null font cache pointer found"
			" @ YApplication::GetFontCache;");
	return *pFontCache;
}

void
YDSApplication::DestroyFontCache()
{
	ydelete(pFontCache);
	pFontCache = nullptr;
}

void
YDSApplication::ResetFontCache(const_path_t path) ythrow(LoggedEvent)
{
	try
	{
		ydelete(pFontCache);
		pFontCache = ynew FontCache(path);
	}
	catch(...)
	{
		throw LoggedEvent("Error occured @ YApplication::ResetFontCache;");
	}
}

void
YDSApplication::InitializeDevices() ynothrow
{
	//初始化显示设备。
	try
	{
		hScreenUp = share_raw(new DSScreen(MainScreenWidth, MainScreenHeight));
		hScreenDown = share_raw(new DSScreen(MainScreenWidth,
			MainScreenHeight));
	}
	catch(...)
	{
		throw LoggedEvent("Screen initialization failed.");
	}
	try
	{
		hDesktopUp = share_raw(new Desktop(*hScreenUp));
		hDesktopDown = share_raw(new Desktop(*hScreenDown));
	}
	catch(...)
	{
		throw LoggedEvent("Desktop initialization failed.");
	}
}

void
YDSApplication::ReleaseDevices() ynothrow
{
	reset(hDesktopUp);
	reset(hScreenUp);
	reset(hDesktopDown);
	reset(hScreenDown);
}


YDSApplication&
FetchGlobalInstance() ynothrow
{
	static YDSApplication theApp;

	return theApp;
}

YApplication&
FetchAppInstance()
{
	return FetchGlobalInstance();
}


namespace
{
	bool
	operator==(const KeysInfo& a, const KeysInfo& b)
	{
		return a.Up == b.Up && a.Down == b.Down && a.Held == b.Held;
	}

	inline bool
	operator!=(const KeysInfo& a, const KeysInfo& b)
	{
		return !(a == b);
	}
}

YSL_BEGIN_NAMESPACE(Messaging)

bool
InputContent::operator==(const InputContent& rhs) const
{
	return Keys == rhs.Keys && CursorLocation == rhs.CursorLocation;
}

YSL_END_NAMESPACE(Messaging)


namespace
{
	/*!
	\note 转换指针设备光标位置为屏幕点。
	*/
	inline Point
	ToSPoint(const CursorInfo& c)
	{
		return Point(c.GetX(), c.GetY());
	}

	//图形用户界面输入等待函数。
	void
	WaitForGUIInput()
	{
		using namespace Messaging;

		static KeysInfo keys;
		static CursorInfo TouchPos_Old, TouchPos;
		static shared_ptr<InputContent> pContent;

		if(keys.Held & KeySpace::Touch)
			TouchPos_Old = TouchPos;
		scanKeys();
		WriteKeysInfo(keys, TouchPos);

		const Point pt(ToSPoint(keys.Held & KeySpace::Touch
			? TouchPos : TouchPos_Old));

		if(!pContent || ((FetchAppInstance().GetDefaultMessageQueue().IsEmpty()
			|| keys != pContent->Keys || pt != pContent->CursorLocation)
			&& pt != Point::FullScreen))
		{
			pContent = share_raw(new InputContent(keys, pt));
			SendMessage<SM_INPUT>(FetchShellHandle(), 0x40, pContent);
		}
	}
}


void
Idle()
{
	WaitForGUIInput();
}

bool
InitConsole(Devices::Screen& scr, Drawing::PixelType fc, Drawing::PixelType bc)
{
	using namespace platform;

	if(raw(FetchGlobalInstance().GetScreenUpHandle()) == &scr)
		YConsoleInit(true, fc, bc);
	else if(raw(FetchGlobalInstance().GetScreenDownHandle()) == &scr)
		YConsoleInit(false, fc, bc);
	else
		return false;
	return true;
}

//非 yglobal.h 声明的平台相关函数。


/*!
\brief Shell 对象释放函数。
*/
extern void
ReleaseShells();


void
ShowFatalError(const char* s)
{
	using namespace platform;

	YDebugSetStatus();
	YDebugBegin();
	std::printf("Fatal Error:\n%s\n", s);
	terminate();
}

#ifdef YSL_USE_MEMORY_DEBUG

namespace
{
	void
	OnExit_DebugMemory_continue()
	{
		std::fflush(stderr);
		std::puts("Input to continue...");
		WaitForInput();
	}

	/*!
	\brief 内存调试退出函数。
	*/
	void
	OnExit_DebugMemory()
	{
		using namespace platform;

		YDebugSetStatus();
		YDebugBegin();
		std::puts("Normal exit;");

	//	std::FILE* fp(std::freopen("memdbg.log", "w", stderr));
		MemoryList& debug_memory_list(GetDebugMemoryList());
		const typename MemoryList::MapType& Map(debug_memory_list.Blocks);
	//	MemoryList::MapType::size_type s(DebugMemory.GetSize());

		if(!Map.empty())
		{
			std::fprintf(stderr, "%i memory leak(s) detected:\n", Map.size());

			MemoryList::MapType::size_type n(0);

			for(auto i(Map.cbegin()); i != Map.cend(); ++i)
			{
				if(n++ < 4)
					debug_memory_list.Print(i, stderr);
				else
				{
					n = 0;
					OnExit_DebugMemory_continue();
				}
			}
		//	DebugMemory.PrintAll(stderr);
		//	DebugMemory.PrintAll(fp);
			OnExit_DebugMemory_continue();
		}

		const typename MemoryList::ListType&
			List(debug_memory_list.DuplicateDeletedBlocks);

		if(!List.empty())
		{
			std::fprintf(stderr, "%i duplicate memory deleting(s) detected:\n",
				List.size());

			MemoryList::ListType::size_type n(0);

			for(auto i(List.cbegin()); i != List.cend(); ++i)
			{
				if(n++ < 4)
					debug_memory_list.Print(i, stderr);
				else
				{
					n = 0;
					OnExit_DebugMemory_continue();
				}
			}
		//	DebugMemory.PrintAllDuplicate(stderr);
		//	DebugMemory.PrintAllDuplicate(fp);
		}
	//	std::fclose(fp);
		std::puts("Input to terminate...");
		WaitForInput();
	}
}

#endif

YSL_END

int
main(int argc, char* argv[])
{
	using namespace YSL;

	try
	{
		//全局初始化。

		//设置默认异常终止函数。
		std::set_terminate(terminate);

		//启用设备。
		powerOn(POWER_ALL);

		//启用 LibNDS 默认异常处理。
		defaultExceptionHandler();

		//初始化主控制台。
		InitYSConsole();

	/*	if(!setlocale(LC_ALL, "zh_CN.GBK"))
		{
			EpicFail();
			platform::yprintf("setlocale() with %s failed.\n", "zh_CN.GBK");
			terminate();
		}*/

		//初始化文件系统。
		//初始化 EFSLib 和 LibFAT 。
		//当 .nds 文件大于32MB时， EFS 行为异常。
	#ifdef USE_EFS
		if(!EFS_Init(EFS_AND_FAT | EFS_DEFAULT_DEVICE, nullptr))
		{
			//如果初始化 EFS 失败则初始化 FAT 。
	#endif
			if(!fatInitDefault())
				LibfatFail();
			IO::ChangeDirectory(Text::StringToMBCS(
				YApplication::CommonAppDataPath));
	#ifdef USE_EFS
		}
	#endif

		//检查程序是否被正确安装。
		CheckInstall();

		//初始化系统字体资源。
		InitializeSystemFontCache();

		//初始化系统设备。
		FetchGlobalInstance().InitializeDevices();

		//注册全局应用程序对象。
		FetchAppInstance();

		/*
		需要保证主 Shell 句柄在应用程序实例初始化之后初始化，
		因为 YMainShell 的基类 YShell 的构造函数
		调用了 YApplication 的非静态成员函数。
		*/
		static shared_ptr<YShell> hMainShell(new Shells::YMainShell());

		if(!FetchAppInstance().SetShellHandle(hMainShell))
			FetchAppInstance().Log.FatalError("Failed launching the"
				" main shell @ main;");

		//主体。

		using namespace Shells;

		Message msg;

		//消息循环。
		while(FetchMessage(msg, 0) != SM_QUIT)
		{
			TranslateMessage(msg);
			DispatchMessage(msg);
		}

		const int r(0); // TODO: evaluate the main result properly;

		//释放 Shell 。

		YSL_ ReleaseShells();
		reset(hMainShell);

		//释放全局非静态资源。

		//释放默认字体资源。
		FetchGlobalInstance().DestroyFontCache();

		//释放设备。
		FetchGlobalInstance().ReleaseDevices();

	#ifdef YSL_USE_MEMORY_DEBUG
		OnExit_DebugMemory();
	#endif
		return r;
	}
	catch(std::exception& e)
	{
		YSL_ FetchAppInstance().Log.FatalError(e.what());
	}
	catch(...)
	{
		YSL_ FetchAppInstance().Log.FatalError("Unhandled exception"
			" @ int main(int, char*[]);");
	}
}

