/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSMain.cpp
\ingroup Helper
\brief DS 平台框架。
\version r1459;
\author FrankHB<frankhb1989@gmail.com>
\since build 296 。
\par 创建时间:
	2012-03-25 12:48:49 +0800;
\par 修改时间:
	2012-04-01 08:46 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Helper::DSMain;
*/


#include "YSLib/Helper/DSMain.h"
#include "YSLib/Helper/yglobal.h"
#include "YSLib/Helper/Initialization.h"
#include "YSLib/UI/ydesktop.h"
#include "YSLib/Helper/shlds.h"
//#include <clocale>

YSL_BEGIN

namespace
{
	//! \brief 程序日志类。
	class Log : private noncopyable
	{
	public:
		/*!
		\brief 无参数构造：默认实现。
		*/
		yconstfn DefDeCtor(Log)
		/*!
		\brief 析构：空实现。
		*/
		virtual DefEmptyDtor(Log)

		/*!
		\brief 输出 char 字符。
		*/
		Log&
		operator<<(char);
		/*!
		\brief 输出字符指针表示的字符串。
		*/
		Log&
		operator<<(const char*);
		/*!
		\brief 输出字符串。
		*/
		Log&
		operator<<(const string&);

		/*!
		\brief 提示错误。
		*/
		void
		Error(const char*);
		/*!
		\brief 提示错误。
		*/
		void
		Error(const string&);
		/*!
		\brief 提示致命错误。
		\note 中止程序。
		*/
		void
		FatalError(const char*);
		/*!
		\brief 提示致命错误。
		\note 中止程序。
		*/
		void
		FatalError(const string&);
	};

	Log& Log::operator<<(char)
	{
		return *this;
	}
	Log& Log::operator<<(const char*)
	{
		return *this;
	}
	Log& Log::operator<<(const string& s)
	{
		return operator<<(s);
	}

	void
	Log::Error(const char*)
	{}
	void
	Log::Error(const string& s)
	{
		Error(s.c_str());
	}

	void
	Log::FatalError(const char* s)
	{
		ShowFatalError(s);
	}
	void
	Log::FatalError(const string& s)
	{
		FatalError(s.c_str());
	}
}

YSL_BEGIN_NAMESPACE(Devices)
class DSScreen;
YSL_END_NAMESPACE(Devices)

using Devices::DSScreen;
using namespace Drawing;

namespace
{
	/*!
	\brief 默认消息发生函数。
	*/
	void
	Idle()
	{
		//指示等待图形用户界面输入。
		PostMessage(FetchShellHandle(), SM_INPUT, 0x40);
	}

	//注册的应用程序指针。
	DSApplication* pApp;

	/*
	\brief 全局生存期屏幕。
	\since build 297 。
	*/
	//@{
	DSScreen* pScreenUp;
	DSScreen* pScreenDown;
	//@}
}

YSL_BEGIN_NAMESPACE(Devices)

/*!
\brief DS 屏幕。
\since 早于 build 218 。
*/
class DSScreen : public Screen
{
public:
	typedef int BGType;

private:
	BGType bg;

public:
	/*!
	\brief 构造：指定宽度和高度，从指定缓冲区指针。
	*/
	DSScreen(SDst, SDst, Drawing::BitmapPtr = nullptr);

	/*!
	\brief 复位。
	\note 无条件初始化。
	*/
	static void
	Reset();

	/*!
	\brief 取指针。
	\note 无异常抛出。
	\note 进行状态检查。
	*/
	virtual Drawing::BitmapPtr
	GetCheckedBufferPtr() const ynothrow;
	DefGetter(const ynothrow, const BGType&, BgID, bg)

	/*!
	\brief 更新。
	\note 复制到屏幕。
	*/
	void
	Update(Drawing::BitmapPtr);
	/*!
	\brief 更新。
	\note 以纯色填充屏幕。
	*/
	void
	Update(Drawing::Color = Drawing::Color());
};

DSScreen::DSScreen(SDst w, SDst h, BitmapPtr p)
	: Devices::Screen(w, h, p),
	bg(-1)
{}

BitmapPtr
DSScreen::GetCheckedBufferPtr() const ynothrow
{
	if(YCL_UNLIKELY(!GetBufferPtr()))
	{
		InitVideo();

		// NOTE: assert(YSL_ pScreenUp && YSL_ pScreenDown);
		yunseq(YSL_ pScreenUp->pBuffer = DS::InitScrUp(YSL_ pScreenUp->bg),
			YSL_ pScreenDown->pBuffer = DS::InitScrDown(YSL_ pScreenDown->bg));
	}
	return Devices::Screen::GetCheckedBufferPtr();
}

void
DSScreen::Update(BitmapPtr buf)
{
	DS::ScreenSynchronize(GetCheckedBufferPtr(), buf);
}
void
DSScreen::Update(Color c)
{
	FillPixel<PixelType>(GetCheckedBufferPtr(), GetAreaOf(GetSize()), c);
}

YSL_END_NAMESPACE(Devices)


DSApplication::DSApplication()
	: pFontCache(), UIResponseLimit(0x40)
{
	YAssert(!YSL_ pApp, "Duplicate instance found"
		" @ DSApplication::DSApplication;");

	//注册全局应用程序实例。
	YSL_ pApp = this;

	//全局初始化。

	//设置默认异常终止函数。
	std::set_terminate(terminate);

	//启用设备。
	::powerOn(POWER_ALL);

	//启用 LibNDS 默认异常处理。
	::defaultExceptionHandler();

	//初始化主控制台。
	platform::YConsoleInit(true, ColorSpace::Lime);

#if 0
	// TODO: review locale APIs compatibility;
	if(!setlocale(LC_ALL, "zh_CN.GBK"))
	{
		EpicFail();
		platform::yprintf("setlocale() with %s failed.\n", "zh_CN.GBK");
		terminate();
	}
#endif

	//初始化文件系统。
	//初始化 EFSLib 和 LibFAT 。
	//当 .nds 文件大于32MB时， EFS 行为异常。
#ifdef USE_EFS
	if(!::EFS_Init(EFS_AND_FAT | EFS_DEFAULT_DEVICE, nullptr))
	{
		//如果初始化 EFS 失败则初始化 FAT 。
#endif
		if(!fatInitDefault())
			LibfatFail();
		IO::ChangeDirectory(Application::CommonAppDataPath
			.GetNativeString());
#ifdef USE_EFS
	}
#endif

	//检查程序是否被正确安装。
	CheckInstall();

	//初始化系统字体资源。
	InitializeSystemFontCache();

	//初始化系统设备。
	//初始化显示设备。
	try
	{
		pScreenUp = new DSScreen(MainScreenWidth, MainScreenHeight);
		pScreenDown = new DSScreen(MainScreenWidth, MainScreenHeight);
	}
	catch(...)
	{
		throw LoggedEvent("Screen initialization failed.");
	}
	/*
	需要保证主 Shell 句柄在应用程序实例初始化之后初始化，
	因为 MainShell 的基类 Shell 的构造函数
	调用了 Application 的非静态成员函数。
	*/
	if(YCL_UNLIKELY(!FetchAppInstance().Switch(
		make_shared<Shells::MainShell>())))
		throw LoggedEvent("Failed launching the main shell;");
}

DSApplication::~DSApplication()
{
	//释放全局非静态资源。

	//释放默认字体资源。
	ydelete(pFontCache);
	pFontCache = nullptr;

	//释放设备。
	delete pScreenUp,
	delete pScreenDown;
}

FontCache&
DSApplication::GetFontCache() const ythrow(LoggedEvent)
{
	if(YCL_UNLIKELY(!pFontCache))
		throw LoggedEvent("Null font cache pointer found"
			" @ Application::GetFontCache;");
	return *pFontCache;
}

Devices::Screen&
DSApplication::GetScreenUp() const ynothrow
{
	YAssert(bool(pScreenUp), "Fatal error:"
		" null screen pointer found @ DSApplication::GetScreenUp;");

	return *pScreenUp;
}
Devices::Screen&
DSApplication::GetScreenDown() const ynothrow
{
	YAssert(bool(pScreenDown), "Fatal error:"
		" null screen pointer found @ DSApplication::GetScreenDown;");

	return *pScreenDown;
}

bool
DSApplication::DealMessage()
{
	using namespace Shells;

	if(Queue.IsEmpty())
		Idle();
	else
	{
		if(Queue.Peek(msg, hShell, true) == SM_QUIT)
			return false;
		if(msg.GetPriority() < UIResponseLimit)
			Idle();
		Dispatch(msg);
	}
	return true;
}

void
DSApplication::ResetFontCache(const_path_t path) ythrow(LoggedEvent)
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


DSApplication&
FetchGlobalInstance() ynothrow
{
	YAssert(pApp, "Null pointer found @ FetchGlobalInstance;");

	return *pApp;
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

	Log log;

	try
	{
		{
			//应用程序实例。
			DSApplication theApp;

			//主体：消息循环。
			while(theApp.DealMessage())
				;

			//清理消息队列（当应用程序实例为静态存储期对象时需要）。
		//	theApp.GetDefaultMessageQueue().Clear();
		//	theApp.GetBackupMessageQueue().Clear();

			//释放 Shell 。
			YSL_ ReleaseShells();
			//当主 Shell 句柄为静态存储期对象时需要通过 reset 释放。
		}

	#ifdef YSL_USE_MEMORY_DEBUG
		OnExit_DebugMemory();
	#endif
	}
	catch(std::exception& e)
	{
		log.FatalError(e.what());
	}
	catch(...)
	{
		log.FatalError("Unhandled exception @ int main(int, char*[]);");
	}
	// TODO: return exit code properly;
	return 0;
}

