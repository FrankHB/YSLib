/*
	© 2010-2015, 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YDesktop.h
\ingroup UI
\brief 平台无关的桌面抽象层。
\version r1444
\author FrankHB <frankhb1989@gmail.com>
\since build 586
\par 创建时间:
	2010-05-02 12:00:08 +0800
\par 修改时间:
	2020-01-25 16:25 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::UI::YDesktop
*/


#ifndef YSL_INC_UI_YDesktop_h_
#define YSL_INC_UI_YDesktop_h_ 1

#include "YModules.h"
#include YFM_YSLib_UI_YWindow
#include YFM_YSLib_Core_YDevice

namespace YSLib
{

namespace UI
{

/*!
\brief 桌面。
\warning 使用的渲染器必须是 BufferedRenderer 或其 public 派生类的对象。
\since 早于 build 132
*/
class YF_API Desktop : public Window
{
private:
	Devices::Screen& screen; //!< 屏幕对象。

public:
	/*!
	\brief 构造：使用指定屏幕对象引用、渲染器指针和背景。
	\note 渲染器指针为空时从按屏幕对象获取的缓冲区指针构造 BufferedRenderer 。
	\note 背景图像指针空指针时使用背景色。
	\since build 406
	*/
	explicit
	Desktop(Devices::Screen&, Color = Drawing::ColorSpace::Black,
		const shared_ptr<Drawing::Image>& = {},
		unique_ptr<BufferedRenderer> = {});
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~Desktop() override;

	DefGetter(const ynothrow, const Devices::Screen&, Screen, screen) \
		//!< 取屏幕对象。

	DefGetter(ynothrow, Rect, InvalidatedArea,
		GetRenderer().CommitInvalidation({}))

	/*!
	\brief 更新缓冲区至屏幕。
	*/
	void
	Update();

	/*!
	\brief 验证：绘制缓冲区使之有效。
	\since build 267
	\todo 渲染器类型安全。
	*/
	Rect
	Validate();
};

} // namespace UI;

} // namespace YSLib;

#endif

