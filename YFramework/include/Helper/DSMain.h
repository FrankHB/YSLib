/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSMain.h
\ingroup Helper
\brief DS 平台框架。
\version r1497;
\author FrankHB<frankhb1989@gmail.com>
\since build 296 。
\par 创建时间:
	2012-03-25 12:49:27 +0800;
\par 修改时间:
	2012-08-09 03:16 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Helper::DSMain;
*/


#ifndef INC_HELPER_DSMAIN_H_
#define INC_HELPER_DSMAIN_H_

#include <YSLib/Core/yapp.h>
#include <YSLib/Core/ydevice.h>
#include <YCLib/Input.h>
#if YCL_MULTITHREAD == 1
#include <thread>
#include <mutex>
#include <condition_variable>
#endif

YSL_BEGIN

#ifndef SCREEN_WIDTH
#	define SCREEN_WIDTH 256
#endif
#ifndef SCREEN_HEIGHT
#	define SCREEN_HEIGHT 192
#endif

/*!	\defgroup CustomGlobalConstants Custom Global Constants
\ingroup GlobalObjects
\brief 平台相关的全局常量。
\since build 173 。
*/
//@{
/*!
\brief 屏幕大小。
\since build 215 。
*/
const SDst MainScreenWidth(SCREEN_WIDTH), MainScreenHeight(SCREEN_HEIGHT);
//@}


//前向声明。
YSL_BEGIN_NAMESPACE(Drawing)
class FontCache;
YSL_END_NAMESPACE(Drawing)


/*!
\brief 平台相关的应用程序类。
\note 含默认接口。
\since build 215 。
*/
class DSApplication : public Application
{
#if YCL_HOSTED && YCL_MULTITHREAD == 1
private:
	//! \brief 宿主背景线程。
	std::thread thread;

#if YCL_MINGW32
protected:
	//! \brief 本机主窗口句柄。
	::HWND hHost;
#endif

private:
	//! \brief 宿主环境互斥量。
	std::mutex mtx;
	//! \brief 宿主环境就绪条件。
	std::condition_variable init;
	/*!
	\brief 初始化条件。
	\since build 325 。
	*/
	std::condition_variable full_init;
#endif

protected:
	/*!
	\brief 默认字体缓存。
	\since build 325 。
	*/
	unique_ptr<Drawing::FontCache> pFontCache;
	/*!
	\brief 屏幕。
	\since build 325 。
	*/
	//@{
	unique_ptr<Devices::Screen> pScreenUp;
	unique_ptr<Devices::Screen> pScreenDown;
	//@}

public:
	/*!
	\brief 用户界面输入响应阈值。
	\note 默认值 0x40 。
	\see DSApplication::Run 。
	\since build 288 。

	用于主消息队列的消息循环中控制后台消息生成策略的全局消息优先级。
	*/
	Messaging::Priority UIResponseLimit;

	/*!
	\brief \c private 构造函数：非内联。
	\pre 断言检查：进程唯一性。
	*/
	DSApplication();

	/*!
	\brief 析构：释放资源。
	\since build 269 。
	*/
	~DSApplication() override;

	/*!
	\brief 取字体缓存引用。
	\pre 断言检查：指针非空。
	\since build 325 。
	*/
	Drawing::FontCache&
	GetFontCache() const ynothrow;
	/*!
	\brief 取上屏幕。
	\pre 断言检查：内部指针非空。
	\since build 297 。
	*/
	Devices::Screen&
	GetScreenUp() const ynothrow;
	/*!
	\brief 取下屏幕。
	\pre 断言检查：内部指针非空。
	\since build 297 。
	*/
	Devices::Screen&
	GetScreenDown() const ynothrow;

	/*!
	\brief 处理当前消息。
	\note 优先级小于 UIResponseLimit 的消息时视为后台消息，否则为前台消息。
	\return 循环条件。
	\since build 297 。

	若主消息队列为空，处理空闲消息，否则从主消息队列取出并分发消息。
	当取出的消息的标识为 SM_QUIT 时视为终止循环。
	对于后台消息，分发前调用后台消息处理程序：分发空闲消息并可进行时序控制。
	*/
	bool
	DealMessage();

#if YCL_HOSTED
	//! \since build 325 。
	::HWND
	GetWindowHandle()
	{
		return hHost;
	}

private:
	/*!
	\brief 初始化宿主资源和本机消息循环线程。
	\since build 325 。
	*/
	void
	HostTask();
#endif
};


/*!
\brief 取平台相关的全局资源。
\pre 内部断言检查存在应用程序实例。
\note 生存期未确定。需要手动初始化并注册应用程序实例后才能使用。
\since build 211 。
*/
DSApplication&
FetchGlobalInstance() ynothrow;

/*!
\ingroup HelperFunctions
\brief 取默认字体缓存。
\since build 219 。
*/
inline Drawing::FontCache&
FetchDefaultFontCache()
{
	return FetchGlobalInstance().GetFontCache();
}

/*!
\ingroup HelperFunctions
\brief 取默认屏幕。
\since build 219 。
*/
inline Devices::Screen&
FetchDefaultScreen()
{
	return FetchGlobalInstance().GetScreenUp();
}

/*!
\brief 以指定前景色和背景色初始化指定屏幕的控制台。
\since build 148 。
*/
bool
InitConsole(Devices::Screen&, Drawing::PixelType, Drawing::PixelType);

/*!
\brief 显示致命错误。
\since build 175 。
*/
void
ShowFatalError(const char*);

//! \brief 运行时平台。
namespace DS
{
	using namespace platform_ex;
}

YSL_END

#endif

