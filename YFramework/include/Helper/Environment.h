/*
	© 2013-2016, 2020, 2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Environment.h
\ingroup Helper
\brief 环境。
\version r1160
\author FrankHB <frankhb1989@gmail.com>
\since build 521
\par 创建时间:
	2013-02-08 01:28:03 +0800
\par 修改时间:
	2022-09-13 06:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::Environment
*/


#ifndef INC_Helper_Environment_h_
#define INC_Helper_Environment_h_ 1

#include "YModules.h"
#include YFM_Helper_YGlobal
#include YFM_YSLib_Core_ValueNode // for ValueNode;
#if YF_Hosted
#	if YCL_Win32 && defined(NDEBUG)
// XXX: Use Win32 subsystem.
#		define YF_Helper_Environment_UseStdout false
#	else
#		define YF_Helper_Environment_UseStdout true
#	endif
#	define YF_Helper_Environment_NPL_UseBacktrace true
#	define YF_Helper_Environment_NPL_UseSourceInfo true
#else
#	define YF_Helper_Environment_UseStdout false
#	define YF_Helper_Environment_NPL_UseBacktrace false
#	define YF_Helper_Environment_NPL_UseSourceInfo false
#endif
#include YFM_YSLib_Core_YShellDefinition // for ostringstream;
#include YFM_NPL_NPLA1 // for NPL::A1::GlobalState, NPL::TermNode,
//	NPL::ContextNode, NPL::A1::InvokeIn, NPL::Environment,
//	NPL::A1::EnvironmentGuard, NPL::ToReducer, NPL::A1::ReduceOnce;

namespace YSLib
{

#if YCL_Android
//! \since build 502
//@{
namespace Devices
{

class AndroidScreen;

} // namespace Devices;

namespace Android
{

class NativeHost;

} // namespace Android;
//@}
#endif


/*!
\brief 应用程序环境。
\warning 非线程安全。
\warning 初始化后不检查成员使用的分配器相等性。
\since build 378

应用程序实例使用的环境。
通常只支持一个实例，但没有检查。
*/
class YF_API Environment
{
public:
	/*!
	\brief 环境根节点。
	\since build 688
	*/
	ValueNode Root{};
	//! \since build 954
	//@{
	ostringstream DefaultOutputStream;
	//! \since build 955
	NPL::A1::GlobalState Global;
	//! \since build 955
	NPL::A1::ContextState Main{Global};
	NPL::TermNode Term{Global.Allocator};

private:
#	if YF_Helper_Environment_NPL_UseBacktrace
	NPL::ContextNode::ReducerSequence backtrace{Global.Allocator};
#	endif
	//@}

public:
	/*!
	\brief 构造：初始化环境。
	\note Win32 平台：尝试无参数调用 FixConsoleHandler ，若失败则跟踪警告。
	\sa Application::AddExit
	\since build 693
	*/
	Environment(Application&);
	~Environment();

	//! \since build 688
	//@{
	/*!
	\brief 取值类型根节点。
	\pre 断言：已初始化。
	*/
	ValueNode&
	GetRootRef() ynothrowv;
	//@}

	//! \since build 954
	//@{
	template<typename _func>
	void
	ExecuteNPLA1(_func f)
	{
		// NOTE: The ground environment is saved during the call to
		//	%NPL::A1::InvokeIn.
		NPL::A1::InvokeIn(Main, [&]{
			RewriteNPLA1(f);
		});
	}
	/*!
	\pre 断言：参数指针不等于当前环境。
	\pre 间接断言：参数指针非空。
	*/
	template<typename _func>
	void
	ExecuteNPLA1(_func f, shared_ptr<NPL::Environment> p_env)
	{
		YAssert(p_env != Main.GetRecordPtr(),
			"Invalid environment found.");

		NPL::A1::EnvironmentGuard gd(Main,
			Main.SwitchEnvironmentUnchecked(std::move(p_env)));

		RewriteNPLA1(f);
	}

private:
	void
	PrepareExecution(NPL::ContextNode&);

	template<typename _func>
	void
	RewriteNPLA1(_func f)
	{
		Main.Rewrite(NPL::ToReducer(Global.Allocator, trivial_swap,
			[&](NPL::ContextNode& ctx){
			PrepareExecution(ctx);
			f(Term);
			Global.Preprocess(Term);
			return NPL::A1::ReduceOnce(Term, ctx);
		}));
	}
	//@}
};

} // namespace YSLib;

#endif

