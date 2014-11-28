/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file GUIApplication.h
\ingroup Helper
\brief GUI 应用程序。
\version r364
\author FrankHB <frankhb1989@gmail.com>
\since build 398
\par 创建时间:
	2013-04-11 10:02:53 +0800
\par 修改时间:
	2014-11-27 15:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::GUIApplication
*/


#ifndef INC_Helper_GUIApplication_h_
#define INC_Helper_GUIApplication_h_ 1

#include "YModules.h"
#include YFM_Helper_YGlobal
#include YFM_YCLib_NativeAPI
#include YFM_YSLib_Core_YApplication
#include <ystdex/cast.hpp>
#include YFM_Helper_GUIShell
#if YCL_Win32
#	include YFM_MinGW32_YCLib_Win32GUI
#elif YCL_Android
#	include YFM_Android_YCLib_Android
#endif

namespace YSLib
{

/*!
\brief 平台相关的应用程序类。
\note 含默认接口。
\since build 398
*/
class YF_API GUIApplication : public Application
{
private:
	/*!
	\brief 环境状态。
	\since build 502
	*/
	unique_ptr<Environment> p_env;

public:
	/*!
	\brief 用户界面输入响应阈值。
	\sa DSApplication::Run

	用于主消息队列的消息循环中控制后台消息生成策略的全局消息优先级。
	*/
	Messaging::Priority UIResponseLimit = 0x40;

	/*!
	\brief \c private 构造函数：非内联。
	\pre 断言：进程唯一性。
	*/
	GUIApplication();
	/*!
	\brief 析构：释放资源。
	*/
	~GUIApplication() override;

	//! \since build 521
	Environment&
	GetEnvironment() ynothrow;

	/*!
	\brief 处理当前消息。
	\return 循环条件。
	\note 线程安全：全局消息队列互斥访问。
	\note 优先级小于 UIResponseLimit 的消息时视为后台消息，否则为前台消息。

	若主消息队列为空，处理空闲消息，否则从主消息队列取出并分发消息。
	当取出的消息的标识为 SM_Quit 时视为终止循环。
	对后台消息，分发前调用后台消息处理程序：分发空闲消息并可进行时序控制。
	*/
	bool
	DealMessage();
};


/*!
\brief 取全局应用程序实例。
\throw GeneralEvent 不存在初始化完成的应用程序实例。
\warning 调用线程安全，但不保证调用结束后实例仍在生存期内。
\since build 550
*/
//@{
YF_API yimpl(GUIApplication&)
FetchGlobalInstance();
template<class _tApp>
inline _tApp&
FetchGlobalInstance()
{
	return ystdex::polymorphic_downcast<_tApp&>(FetchGlobalInstance());
}
//@}

/*!
\brief 锁定实例。
\note 当返回结果为空时实例生存期未开始或已结束。
\since build 551
*/
YF_API locked_ptr<yimpl(GUIApplication), recursive_mutex>
LockInstance();


/*!
\brief 取单一环境对象引用。
\since build 398
*/
inline PDefH(Environment&, FetchEnvironment, )
	ImplRet(FetchGlobalInstance().GetEnvironment())


/*!
\brief 执行程序主消息循环。
\throw GeneralEvent 激活主 shell 失败。
\note 对宿主实现，设置退出所有窗口时向 YSLib 发送退出消息。
\since build 399
*/
YF_API void
Execute(GUIApplication&, shared_ptr<Shell> = make_shared<Shells::GUIShell>());

} // namespace YSLib;

#endif

