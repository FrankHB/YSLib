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
\version r1081;
\author FrankHB<frankhb1989@gmail.com>
\since build 296 。
\par 创建时间:
	2012-03-25 12:49:27 +0800;
\par 修改时间:
	2012-03-25 16:49 +0800;
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

YSL_BEGIN

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
	Update(Drawing::Color = 0);
};

YSL_END_NAMESPACE(Devices)


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

YSL_END_NAMESPACE(Shells)


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
	friend DSApplication&
	FetchGlobalInstance() ynothrow;

private:
	Drawing::FontCache* pFontCache; //!< 默认字体缓存。
	shared_ptr<Devices::DSScreen> hScreenUp; //!< DS 上屏幕句柄。
	shared_ptr<Devices::DSScreen> hScreenDown; //!< DS 上屏幕句柄。

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
	DefGetter(const ynothrow, const shared_ptr<Devices::DSScreen>&,
		ScreenUpHandle, hScreenUp)
	DefGetter(const ynothrow, const shared_ptr<Devices::DSScreen>&,
		ScreenDownHandle, hScreenDown)
	/*!
	\brief 取上屏幕。
	\note 断言检查：句柄非空。
	\note 无异常抛出。
	*/
	Devices::DSScreen&
	GetScreenUp() const ynothrow;
	/*!
	\brief 取下屏幕。
	\note 断言检查：句柄非空。
	\note 无异常抛出。
	*/
	Devices::DSScreen&
	GetScreenDown() const ynothrow;
	/*!
	\brief 取默认屏幕。
	\note 无异常抛出。
	*/
	PDefH(Devices::DSScreen&, GetDefaultScreen)
		ImplRet(GetScreenUp())

	/*!
	\brief 复位默认字体缓存：使用指定路径。
	\throw LoggedEvent 记录异常事件。
	\note 仅抛出以上异常。
	*/
	void
	ResetFontCache(const_path_t) ythrow(LoggedEvent);

	/*!
	\brief 运行主程序逻辑。
	\note 优先级小于 UIResponseLimit 的消息时视为后台消息，否则为前台消息。
	\since build 271 。

	若主消息队列为空，调用后台消息处理程序，否则从主消息队列取出并分发消息。
	当取出的消息的标识为 SM_QUIT 时退出消息循环。
	对于后台消息，分发前调用后台消息处理程序。
	*/
	int
	Run();
};

inline Devices::DSScreen&
DSApplication::GetScreenUp() const ynothrow
{
	YAssert(bool(hScreenUp), "Fatal error:"
		" null screen handle found @ DSApplication::GetScreenUp;");

	return *hScreenUp;
}
inline Devices::DSScreen&
DSApplication::GetScreenDown() const ynothrow
{
	YAssert(bool(hScreenDown), "Fatal error:"
		" null screen handle found @ DSApplication::GetScreenDown;");

	return *hScreenDown;
}


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
	return FetchGlobalInstance().GetDefaultScreen();
}

/*!
\brief 显示致命错误。
\since build 175 。
*/
void
ShowFatalError(const char*);

YSL_END

#endif

