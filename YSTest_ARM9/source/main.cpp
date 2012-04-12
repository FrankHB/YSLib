/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file main.cpp
\ingroup DS
\brief ARM9 主源文件。
\version r2454;
\author FrankHB<frankhb1989@gmail.com>
\since build 1 。
\par 创建时间:
	2009-11-12 21:26:30 +0800;
\par 修改时间:
	2012-04-11 08:40 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Main_ARM9;
*/


//标识主源文件。
#ifdef YSL_ARM9_MAIN_CPP_
#	error The main source file has already compiled. Try to make the macro \
		YSL_ARM9_MAIN_CPP_ be defined iff. once in the project.
#endif

#define YSL_ARM9_MAIN_CPP_ YSL_MAIN_CPP_

#ifdef YSL_MAIN_CPP_
#	error The main source file has already compiled. Try to make the macro \
		YSL_MAIN_CPP_ be defined iff. once in the project.
#endif
#define YSL_MAIN_CPP_

//定义使用 EFSLib 。
//#define USE_EFS

//包含文件。
#include "Shells.h"

using namespace platform;
using std::puts;

YSL_BEGIN

#if 0
const char* G_COMP_NAME = "Franksoft";
const char* G_APP_NAME = "YShell Test";
const char* G_APP_VER = "0.2600";
#endif

//调试功能。
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

void
OnExit_DebugMemory_continue()
{
	std::fflush(stderr);
	std::puts("Input to continue...");
	platform::WaitForInput();
}

#ifdef YSL_USE_MEMORY_DEBUG

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

#endif

} // unnamed namespace;


//! 非 YSLib 声明的平台相关函数。
//@{
/*!
\brief Shell 对象释放函数。
*/
extern void
ReleaseShells();
//@}


using namespace Components;
using namespace Drawing;

YSL_BEGIN_NAMESPACE(Shells)

/*!
\brief 主 Shell 。
\since 早于 build 132 。
*/
class MainShell : public DS::ShlDS
{
public:
	typedef ShlDS ParentType;

	Components::Label lblTitle, lblStatus, lblDetails;

	/*!
	\brief 无参数构造。
	*/
	MainShell();
};

MainShell::MainShell()
	: ShlDS(),
	lblTitle(Rect(50, 20, 100, 22)),
	lblStatus(Rect(60, 80, 80, 22)),
	lblDetails(Rect(30, 20, 160, 22))
{
	auto& dsk_up(GetDesktopUp());
	auto& dsk_dn(GetDesktopDown());

	platform::YDebugSetStatus(true);

	dsk_up += lblTitle;
	dsk_up += lblStatus;
	dsk_dn += lblDetails;
	yunseq(
	//	dsk_up.Background = FetchImage(1),
		dsk_up.Background = SolidBrush(Color(240, 216, 192)),
	//	dsk_dn.Background = Color(240, 216, 240),
		dsk_dn.Background
			= SolidBrush(FetchGUIState().Colors[Styles::Desktop]),
		lblTitle.Text = "YReader",
		lblStatus.Text = "Loading...",
		lblDetails.Text = u"初始化中，请稍后……",
		lblDetails.ForeColor = ColorSpace::White
	);
	lblDetails.SetTransparent(true);
	SetInvalidationOf(dsk_up);
	yunseq(dsk_up.Validate(), dsk_dn.Validate());
	dsk_up.Update(),
	dsk_dn.Update();
	//初始化所有图像资源。

	auto& pb(*(ynew ProgressBar(Rect(8, 168, 240, 16), 10)));

	dsk_up += pb;
	for(size_t i(0); i < 10; ++i)
	{
		pb.SetValue(i);
//		Invalidate(pb);
		dsk_up.Background
			= SolidBrush(Color(255 - i * 255 / 10, 216, 192));
		SetInvalidationOf(dsk_up);
		Invalidate(dsk_up);
		dsk_up.Validate();
		dsk_up.Update();
		YReader::FetchImage(i);
	}
	pb.SetValue(10);
	Invalidate(dsk_up);
	dsk_up.Validate();
	dsk_up.Update();
	dsk_up -= pb;
	ydelete(&pb);
//	yunseq(GetDesktopUp().Background = nullptr,
//		GetDesktopDown().Background = nullptr);
	YReader::SetShellToNew<YReader::ShlExplorer>();
}

YSL_END_NAMESPACE(Shells)

YSL_END

int
#ifdef YCL_MINGW32
WINAPI
WinMain(HINSTANCE hThis, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmd)
#else
main(int argc, char* argv[])
#endif
{
	using namespace YSL;

	Log log;

	try
	{
		{
			//应用程序实例。
			DSApplication theApp;

			/*
			需要保证主 Shell 句柄在应用程序实例初始化之后初始化，
			因为 MainShell 的基类 Shell 的构造函数
			调用了 Application 的非静态成员函数。
			*/
			if(YCL_UNLIKELY(!FetchAppInstance().Switch(
				make_shared<Shells::MainShell>())))
				throw LoggedEvent("Failed launching the main shell;");
			//主体：消息循环。
			while(theApp.DealMessage())
				;
			//释放 Shell （必要，保证释放 Shell 且避免资源泄漏）。
			YSL_ ReleaseShells();
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

