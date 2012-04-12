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
\version r1223;
\author FrankHB<frankhb1989@gmail.com>
\since build 296 。
\par 创建时间:
	2012-03-25 12:49:27 +0800;
\par 修改时间:
	2012-04-11 08:23 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Helper::DSMain;
*/


#ifndef YSL_INC_HELPER_DSMAIN_H_
#define YSL_INC_HELPER_DSMAIN_H_

#include "../Core/yapp.h"
#include "../Core/ydevice.h"
#include "shlds.h"
#include "../UI/Label.h"
#include "../../YCLib/Input.h"

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
private:
	Drawing::FontCache* pFontCache; //!< 默认字体缓存。

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
	virtual
	~DSApplication();

	/*!
	\brief 取字体缓存引用。
	\throw LoggedEvent 记录异常事件。
	\note 仅抛出以上异常。
	*/
	Drawing::FontCache&
	GetFontCache() const ythrow(LoggedEvent);
	/*!
	\brief 取上屏幕。
	\pre 断言检查：句柄非空。
	\note 无异常抛出。
	\since build 297 。
	*/
	Devices::Screen&
	GetScreenUp() const ynothrow;
	/*!
	\brief 取下屏幕。
	\pre 断言检查：句柄非空。
	\note 无异常抛出。
	\since build 297 。
	*/
	Devices::Screen&
	GetScreenDown() const ynothrow;

	/*!
	\brief 处理当前消息。
	\note 优先级小于 UIResponseLimit 的消息时视为后台消息，否则为前台消息。
	\return 循环条件。
	\since build 297 。

	若主消息队列为空，调用后台消息处理程序，否则从主消息队列取出并分发消息。
	当取出的消息的标识为 SM_QUIT 时视为终止循环。
	对于后台消息，分发前调用后台消息处理程序。
	*/
	bool
	DealMessage();

	/*!
	\brief 复位默认字体缓存：使用指定路径。
	\throw LoggedEvent 记录异常事件。
	\note 仅抛出以上异常。
	*/
	void
	ResetFontCache(const_path_t) ythrow(LoggedEvent);
};


/*!
\brief 向指定桌面分发响应输入状态。
\since build 299 。

指定平台相关的用户界面输入处理。
*/
void
DispatchInput(Desktop&);

/*!
\brief 取平台相关的全局资源。
\note 无异常抛出。
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

