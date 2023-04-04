/*
	© 2013-2016, 2018-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Environment.cpp
\ingroup Helper
\brief 环境。
\version r2007
\author FrankHB <frankhb1989@gmail.com>
\since build 379
\par 创建时间:
	2013-02-08 01:27:29 +0800
\par 修改时间:
	2023-03-29 12:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Environment
*/


#include "Helper/YModules.h"
#include YFM_Helper_Environment // for make_any, string;
#include YFM_Helper_Initialization // for InitializeEnvironment,
//	ShowInitializedLog, PerformKeyAction, LoadComponents;
#if YCL_DS
#	include YFM_YCLib_NativeAPI // for ::powerOn, ::defaultExceptionHandler,
//	platform_ex::FileSystem;
#	include YFM_DS_YCLib_DSVideo // for platform_ex::DSConsoleInit;
#	include YFM_YSLib_Core_YGDIBase // for Drawing::ColorSpace;
#elif YCL_Win32
#	include YFM_Win32_YCLib_Consoles // for platform_ex::FixConsoleHandler;
#	include YFM_Win32_YCLib_MinGW32 // for platform_ex::Win32Exception;
#	include YFM_YSLib_Adaptor_YAdaptor // for FetchEnvironmentVariable;
#	include YFM_YCLib_Debug // for platform_ex::SendDebugString;
#endif
#include YFM_NPL_Dependency // for NPL, NPL::make_observer, A1,
//	NPL::A1::Forms::LoadStandardContext, TraceException, A1::TraceBacktrace,
//	NPLException;
#if YF_Helper_Environment_UseStdout
#	include <iostream> // for std::cout;
#endif
#if YF_Helper_Environment_NPL_UseBacktrace
#	include <ystdex/scope_guard.hpp> // for ystdex::make_guard;
#endif

namespace YSLib
{

using namespace platform_ex;
#if YF_Hosted
using namespace Host;
#endif

Environment::Environment(Application& app)
	: Root(app.get_allocator()), DefaultOutputStream(
	string(app.get_allocator())), Global(app.get_allocator())
{
#if !YF_Hosted
	// NOTE: This only effects freestanding implementations now, which may need
	//	different behavior than the default implementation. Hosted
	//	implemenations are expected to have termination handlers friendly to
	//	debugging, which can be overriden before the construction of
	//	%Environment if needed.
	std::set_terminate(terminate);
#endif
#if YCL_DS

	using namespace platform;
	namespace ColorSpace = Drawing::ColorSpace;

	::powerOn(POWER_ALL);
	::defaultExceptionHandler();
	DSConsoleInit(true, ColorSpace::Lime, ColorSpace::Black);
	FetchCommonLogger().SetSender([&] YB_LAMBDA_ANNOTATE(
		(Logger::Level lv, Logger&, const char* str), ynothrowv, nonnull(4)){
		if(ShowInitializedLog || lv <= Descriptions::Alert)
		{
			if(!ShowInitializedLog)
			{
				static const struct Init
				{
					Init()
					{
						DSConsoleInit({}, ColorSpace::White, ColorSpace::Blue);
					}
				} init;
			}
			// XXX: Error is ignored.
			std::fprintf(stderr, "%s\n", Nonnull(str));
			std::fflush(stderr);
		}
	});
	PerformKeyAction([&]{
		app.AddExit(make_any<FileSystem>());
	}, yfsig, "         LibFAT Failure         ",
		" An error is preventing the\n"
		" program from accessing\n"
		" external files.\n"
		"\n"
		" If you're using an emulator,\n"
		" make sure it supports DLDI\n"
		" and that it's activated.\n"
		"\n"
		" In case you're seeing this\n"
		" screen on a real DS, make sure\n"
		" you've applied the correct\n"
		" DLDI patch (most modern\n"
		" flashcards do this\n"
		" automatically).\n"
		"\n"
		" Note: Some cards only\n"
		" autopatch .nds files stored in\n"
		" the root folder of the card.\n");
#elif YCL_Win32
	TryExpr(FixConsoleHandler())
	CatchExpr(Win32Exception&,
		YTraceDe(Warning, "Console handler setup failed."))

	string env_str;

	// TODO: Extract as %YCoreUtilities functions?
	if(FetchEnvironmentVariable(env_str, "YF_DEBUG_OUTPUT"))
		FilterExceptions([&]{
			if(env_str == "1")
				FetchCommonLogger().SetSender(SendDebugString);
		});
#endif
#if false
	// TODO: Review locale APIs compatibility.
	static yconstexpr const char locale_str[]{"zh_CN.GBK"};

	if(!std::setlocale(LC_ALL, locale_str))
		throw GeneralEvent("Call of std::setlocale() with %s failed.\n",
			locale_str);
#endif
	// XXX: This should not fail.
	PerformKeyAction([&]{
#	if YF_Helper_Environment_NPL_UseSourceInfo
		Global.UseSourceLocation = true;
#	endif
		Global.OutputStreamPtr = NPL::make_observer(&
#	if YF_Helper_Environment_UseStdout
			std::cout
#	else
			DefaultOutputStream
#	endif
		);
		NPL::A1::Forms::LoadStandardContext(Main);
		Main.GetRecordRef().Freeze();
	}, yfsig, "         NPLA1 Failure         ",
		" An unexpected error occurs \n"
		" during the initializaiton.\n");
	// NOTE: Ensure the root node is initialized before lifetime of environment
	//	begins.
	YTraceDe(Notice, "Checking installation...");
	PerformKeyAction([&]{
		Root = LoadConfiguration(Root.get_allocator(), true);
		if(Root.GetName() == "YFramework")
			Root = PackNodes(string(), std::move(Root));
		LoadComponents(app, AccessNode(Root, "YFramework"));
		YTraceDe(Notice, "Check of installation succeeded.");
	}, yfsig, "      Invalid Installation      ",
		" Please make sure the data is\n stored in correct directory.\n");
	YF_Trace(Debug, "Environment lifetime began.");
}
Environment::~Environment()
{
	YF_Trace(Debug, "Environment lifetime ended.");
}

void
Environment::PrepareExecution(NPL::ContextNode& ctx)
{
	using namespace NPL;

	ctx.SaveExceptionHandler();
	// TODO: Blocked. Use C++14 lambda initializers to simplify the
	//	implementation.
	ctx.HandleException = ystdex::bind1([&](std::exception_ptr p,
		const ContextNode::ReducerSequence::const_iterator& i){
		ctx.TailAction = nullptr;
#	if YF_Helper_Environment_NPL_UseBacktrace
		ctx.Shift(backtrace, i);
#	else
		yunused(i);
#	endif
		YAssertNonnull(p);
		TryExpr(std::rethrow_exception(std::move(p)))
		catch(std::exception& e)
		{
#	if YF_Helper_Environment_NPL_UseBacktrace
			const auto gd(ystdex::make_guard([&]() ynothrowv{
				backtrace.clear();
			}));
#	endif
			auto& trace(ctx.Trace);

			TraceException(e, trace);
			trace.TraceFormat(Notice, "Location: %s.", Main.CurrentSource
				? Main.CurrentSource->c_str() : "<unknown>");
#	if YF_Helper_Environment_NPL_UseBacktrace
			A1::TraceBacktrace(backtrace, trace);
			// NOTE: As %Tools.SHBuild.Main.
			throw NPLException("Error detected in the execution"
				" (see the backtrace for details).");
#	else
			throw NPLException("Error detected in the execution.");
#	endif
		}
	}, ctx.GetCurrent().cbegin());
}

} // namespace YSLib;

