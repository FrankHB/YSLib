/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file main.cpp
\ingroup DS
\brief 主源文件。
\version r1801
\author FrankHB <frankhb1989@gmail.com>
\since build 1
\par 创建时间:
	2009-11-12 21:26:30 +0800
\par 修改时间:
	2014-07-14 09:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Main
*/


#include "ShlExplorer.h"

using namespace platform;
using std::puts;

namespace YSLib
{

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
	virtual DefDeDtor(Log)

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
\since build 301
*/
template<std::size_t _vN, class _tCon>
void
OnExit_DebugMemory_print(_tCon& con, const char* item_name)
{
	if(!con.empty())
	{
		std::fprintf(stderr, "%i %s(s) detected:\n", con.size(), item_name);

		typename _tCon::size_type n(0);

		for(const auto& item : con)
		{
			if(n++ < _vN)
				GetDebugMemoryList().Print(item, stderr);
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

	YConsoleInit(false, ColorSpace::White, ColorSpace::Blue);
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
\since build 301
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
//@}

} // namespace YSLib;

#if YCL_Android
//! \since build 502
extern "C" void
y_android_main()
#else
int
main()
#endif
{
	using namespace YSLib;

	Log log;

	try
	{
		{
			//应用程序实例。
			DSApplication app;

			YTraceDe(Debug, "Application initialized.");
			{
				using namespace YSLib::UI;
				using YReader::Image_N;

				Desktop dsk_m(app.GetScreenUp()), dsk_s(app.GetScreenDown());
				Label lblTitle({50, 20, 100, 22}), lblStatus({60, 80, 120, 22}),
					lblDetails({30, 20, 160, 22});
				ProgressBar pb({8, 168, 240, 16}, Image_N);

				AddWidgets(dsk_m, lblTitle, lblStatus, pb);
				dsk_s += lblDetails;
				yunseq(
				dsk_m.Background = SolidBrush({240, 216, 192}),
				dsk_s.Background
					= SolidBrush(FetchGUIState().Colors[Styles::Desktop]),
				//Color(240, 216, 240),
				lblTitle.Text = G_APP_NAME,
				lblStatus.Text = u"Now loading...",
				lblDetails.Text = u"初始化中，请稍后……",
				lblDetails.ForeColor = Drawing::ColorSpace::White
				);
				lblDetails.Background = nullptr;
				SetInvalidationOf(dsk_m);
				yunseq(dsk_m.Validate(), dsk_s.Validate());
				dsk_m.Update(), dsk_s.Update();
				YTraceDe(Debug, "Static screens updated.");
				//初始化所有图像资源。
				for(size_t i(0); i < Image_N; ++i)
				{
					pb.SetValue(i);
			//		Invalidate(pb);
					dsk_m.Background
						= SolidBrush(Color(255 - i * 255 / Image_N, 216, 192));
					Repaint(dsk_m);
					YReader::FetchImage(i);
				}
				pb.SetValue(Image_N);
				Repaint(dsk_m);
			}
			YTraceDe(Debug, "Splash completed.");
			Execute(app, make_shared<YReader::ShlExplorer>());
			//释放 Shell （必要，保证释放 Shell 且避免资源泄漏）。
			YReader::Cleanup();
		}

	#ifdef YSL_USE_MEMORY_DEBUG
		OnExit_DebugMemory();
	#endif
	}
	catch(FatalError& e)
	{
		HandleFatalError(e);
	}
	catch(LoggedEvent& e)
	{
		log.FatalError("Unhandled logged event with level = "
			+ to_string(e.GetLevel()) + " : " + e.what());
	}
	catch(std::exception& e)
	{
		// TODO: Distinguish errors from %std::runtime_error and YSLib-defined
		//	exceptions.
		log.FatalError(string("Unhandled std::exception: ") + e.what());
	}
	catch(...)
	{
		log.FatalError("Unhandled exception @ main function;");
	}
	// TODO: Return exit code properly.
}

