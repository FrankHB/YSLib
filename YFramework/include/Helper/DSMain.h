/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file DSMain.h
\ingroup Helper
\brief DS 平台框架。
\version r653
\author FrankHB <frankhb1989@gmail.com>
\since build 296
\par 创建时间:
	2012-03-25 12:49:27 +0800
\par 修改时间:
	2013-02-13 13:45 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::DSMain
*/


#ifndef INC_Helper_DSMain_h_
#define INC_Helper_DSMain_h_ 1

#include "Helper/yglobal.h"
#include "YSLib/Core/yapp.h"
#include "YCLib/Input.h"
#include "NPL/Configuration.h"

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
\since build 173
*/
//@{
/*!
\brief 屏幕大小。
\since build 215
*/
const SDst MainScreenWidth(SCREEN_WIDTH), MainScreenHeight(SCREEN_HEIGHT);
//@}


//前向声明。
YSL_BEGIN_NAMESPACE(Devices)
class DSScreen;
YSL_END_NAMESPACE(Devices)
YSL_BEGIN_NAMESPACE(Drawing)
class FontCache;
YSL_END_NAMESPACE(Drawing)
#if YCL_HOSTED
YSL_BEGIN_NAMESPACE(Host)
class Window;
class Environment;
YSL_END_NAMESPACE(Host)
#endif


/*!
\brief 平台相关的应用程序类。
\note 含默认接口。
\since build 215
*/
class YF_API DSApplication : public Application
{
#if YCL_HOSTED
	//! \since build 380
	friend class Host::Environment;
	//! \since build 380
	friend class Host::Window;

private:
	/*!
	\brief 宿主状态。
	\since build 377
	*/
	unique_ptr<Host::Environment> p_hosted;
#endif

protected:
	/*!
	\brief 默认字体缓存。
	\since build 325
	*/
	unique_ptr<Drawing::FontCache> pFontCache;

private:
	/*!
	\brief 屏幕组。
	\since build 380
	*/
	array<unique_ptr<Devices::DSScreen>, 2> scrs;

public:
	/*!
	\brief 用户界面输入响应阈值。
	\note 默认值 0x40 。
	\see DSApplication::Run 。
	\since build 288

	用于主消息队列的消息循环中控制后台消息生成策略的全局消息优先级。
	*/
	Messaging::Priority UIResponseLimit;
	/*!
	\brief 值类型根节点。
	\since build 340
	*/
	ValueNode Root;

	/*!
	\brief \c private 构造函数：非内联。
	\pre 断言：进程唯一性。
	*/
	DSApplication();

	/*!
	\brief 析构：释放资源。
	\since build 269
	*/
	~DSApplication() override;

	//! \since build 377
	DefPred(const ynothrow, ScreenReady, bool(scrs[0]) && bool(scrs[1]))

	/*!
	\brief 取字体缓存引用。
	\pre 断言：指针非空。
	\since build 325
	*/
	Drawing::FontCache&
	GetFontCache() const ynothrow;
#if YCL_HOSTED
	//! \since build 378
	Host::Environment&
	GetHost();
#endif
	/*!
	\brief 取上屏幕。
	\pre 断言：内部指针非空。
	\since build 297
	*/
	Devices::Screen&
	GetScreenUp() const ynothrow;
	/*!
	\brief 取下屏幕。
	\pre 断言：内部指针非空。
	\since build 297
	*/
	Devices::Screen&
	GetScreenDown() const ynothrow;

	/*!
	\brief 处理当前消息。
	\note 优先级小于 UIResponseLimit 的消息时视为后台消息，否则为前台消息。
	\return 循环条件。
	\since build 297

	若主消息队列为空，处理空闲消息，否则从主消息队列取出并分发消息。
	当取出的消息的标识为 SM_QUIT 时视为终止循环。
	对于后台消息，分发前调用后台消息处理程序：分发空闲消息并可进行时序控制。
	*/
	bool
	DealMessage();
};


/*!
\brief 取平台相关的全局资源。
\pre 内部断言检查存在应用程序实例。
\note 生存期未确定。需要手动初始化并注册应用程序实例后才能使用。
\since build 211
*/
YF_API DSApplication&
FetchGlobalInstance() ynothrow;

/*!
\ingroup helper_functions
\brief 取默认字体缓存。
\since build 219
*/
inline Drawing::FontCache&
FetchDefaultFontCache()
{
	return FetchGlobalInstance().GetFontCache();
}

/*!
\brief 以指定前景色和背景色初始化指定屏幕的控制台。
\since build 148
*/
YF_API bool
InitConsole(Devices::Screen&, Drawing::PixelType, Drawing::PixelType);

/*!
\brief 显示致命错误。
\since build 175
*/
YF_API void
ShowFatalError(const char*);

//! \brief 运行时平台。
namespace DS
{
	using namespace platform_ex;
}

YSL_END

#endif

