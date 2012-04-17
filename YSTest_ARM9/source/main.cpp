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
\version r2666;
\author FrankHB<frankhb1989@gmail.com>
\since build 1 。
\par 创建时间:
	2009-11-12 21:26:30 +0800;
\par 修改时间:
	2012-04-17 07:38 +0800;
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

#define G_COMP_NAME u"Franksoft"
#define G_APP_NAME u"YReader"
#define G_APP_VER u"build 301"

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
	\todo 实现。
	*/
	Log&
	operator<<(char)
	{
		return *this;
	}
	/*!
	\brief 输出字符指针表示的字符串。
	\todo 实现。
	*/
	Log&
	operator<<(const char*)
	{
		return *this;
	}
	/*!
	\brief 输出字符串。
	*/
	Log&
	operator<<(const string& s)
	{
		return operator<<(s.c_str());
	}

	/*!
	\brief 提示错误。
	\todo 实现。
	*/
	void
	Error(const char*)
	{}
	/*!
	\brief 提示错误。
	*/
	void
	Error(const string& s)
	{
		Error(s.c_str());
	}
	/*!
	\brief 提示致命错误。
	\note 中止程序。
	*/
	void
	FatalError(const char* s)
	{
		ShowFatalError(s);
	}
	/*!
	\brief 提示致命错误。
	\note 中止程序。
	*/
	void
	FatalError(const string& s)
	{
		FatalError(s.c_str());
	}
};


#ifdef YSL_USE_MEMORY_DEBUG

void
OnExit_DebugMemory_continue()
{
	std::puts("Input to continue...");
	platform::WaitForInput();
}

/*!
\since build 301 。
*/
template<std::size_t _vN, class _tContainer>
void
OnExit_DebugMemory_print(_tContainer& con, const char* item_name)
{
	if(!con.empty())
	{
		std::fprintf(stderr, "%i %s(s) detected:\n", con.size(), item_name);

		typename _tContainer::size_type n(0);

		for(auto i(con.cbegin()); i != con.cend(); ++i)
		{
			if(n++ < _vN)
				GetDebugMemoryList().Print(i, stderr);
			else
			{
				n = 0;
				OnExit_DebugMemory_continue();
			}
		}
		OnExit_DebugMemory_continue();
	}
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
	OnExit_DebugMemory_print<4>(GetDebugMemoryList().Blocks, "memory leak");
	OnExit_DebugMemory_print<4>(GetDebugMemoryList().DuplicateDeletedBlocks,
		"duplicate memory deleting");
#if 0
	DebugMemory.PrintAll(stderr);
	DebugMemory.PrintAll(fp);
	DebugMemory.PrintAllDuplicate(stderr);
	DebugMemory.PrintAllDuplicate(fp);
#endif
//	std::fclose(fp);
	std::puts("Input to terminate...");
	WaitForInput();
}

#endif

/*!
\since build 301 。
*/
void
Repaint(Desktop& dsk)
{
	SetInvalidationOf(dsk);
	dsk.Validate();
	dsk.Update();
}

} // unnamed namespace;


//! 非 YSLib 声明的平台相关函数。
//@{
/*!
\brief Shell 对象释放函数。
*/
extern void
ReleaseShells();
//@}

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

			platform::YDebugSetStatus(true);
			{
				using namespace YSL_ Components;

				Desktop dsk_up(theApp.GetScreenUp()),
					dsk_dn(theApp.GetScreenDown());
				Label lblTitle(Rect(50, 20, 100, 22)),
					lblStatus(Rect(60, 80, 120, 22)),
					lblDetails(Rect(30, 20, 160, 22));
				ProgressBar pb(Rect(8, 168, 240, 16), 10);

				dsk_up += lblTitle;
				dsk_up += lblStatus;
				dsk_dn += lblDetails;
				dsk_up += pb;
				yunseq(
					dsk_up.Background = SolidBrush(Color(240, 216, 192)),
					dsk_dn.Background
						= SolidBrush(FetchGUIState().Colors[Styles::Desktop]),
					//Color(240, 216, 240),
					lblTitle.Text = G_APP_NAME,
					lblStatus.Text = u"Now loading...",
					lblDetails.Text = u"初始化中，请稍后……",
					lblDetails.ForeColor = Drawing::ColorSpace::White
				);
				lblDetails.SetTransparent(true);
				SetInvalidationOf(dsk_up);
				yunseq(dsk_up.Validate(), dsk_dn.Validate());
				dsk_up.Update(), dsk_dn.Update();
				//初始化所有图像资源。
				for(size_t i(0); i < 10; ++i)
				{
					pb.SetValue(i);
			//		Invalidate(pb);
					dsk_up.Background
						= SolidBrush(Color(255 - i * 255 / 10, 216, 192));
					Repaint(dsk_up);
					YReader::FetchImage(i);
				}
				pb.SetValue(10);
				Repaint(dsk_up);
			}
			if(YCL_UNLIKELY(!FetchAppInstance().Switch(
				make_shared<YReader::ShlExplorer>())))
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
		log.FatalError("Unhandled exception @ main function;");
	}
	// TODO: return exit code properly;
	return 0;
}

