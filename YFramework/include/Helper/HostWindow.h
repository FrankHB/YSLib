/*
	© 2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostWindow.h
\ingroup Helper
\brief 宿主环境窗口。
\version r292
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-18 18:16:53 +0800
\par 修改时间:
	2013-10-11 01:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostWindow
*/


#ifndef INC_Helper_HostWindow_h_
#define INC_Helper_HostWindow_h_ 1

#include "yglobal.h"
#if YCL_MinGW32
#	include <YCLib/Win32GUI.h>
#endif

namespace YSLib
{

#if YCL_HOSTED
namespace Host
{

#	if YCL_MinGW32
//! \since build 427
using namespace platform_ex;
#	endif

/*!
\brief 宿主环境支持的窗口。
\since build 379
*/
class YF_API Window : public MinGW32::HostWindow
{
private:
	//! \since build 380
	std::reference_wrapper<Environment> env;

public:
	/*!
	\brief 标记是否使用不透明性成员。
	\note 使用 Windows 层叠窗口实现，但和 WindowReference 实现不同：使用
		::UpdateLayeredWindows 而非 WM_PAINT 更新窗口。
	\warning 使用不透明性成员时在此窗口上调用 ::SetLayeredWindowAttributes 、
		GetOpacity 或 SetOpacity 可能出错。
	\since build 435
	*/
	bool UseOpacity{false};
	/*!
	\brief 不透明性。
	\note 仅当窗口启用 WS_EX_LAYERED 样式且 UseOpacity 设置为 true 时有效。
	\since build 435
	*/
	YSLib::Drawing::AlphaType Opacity{0xFF};

	/*!
	\exception LoggedEvent 异常中立：窗口类名不是 WindowClassName 。
	\since build 429
	*/
	//@{
	Window(NativeWindowHandle);
	Window(NativeWindowHandle, Environment&);
	//@}
	virtual
	~Window();

	DefGetter(const ynothrow, Environment&, Host, env)
	/*!
	\brief 取预定的指针设备输入响应有效区域的左上角和右下角坐标。
	\note 坐标相对于客户区。
	\since build 388
	*/
	virtual pair<YSLib::Drawing::Point, YSLib::Drawing::Point>
	GetInputBounds() const ynothrow;

	void
	OnLostFocus() override;

	/*!
	\brief 刷新：保持渲染状态同步。
	\since build 407
	*/
	virtual PDefH(void, Refresh, )
		ImplExpr(void())

	/*!
	\brief 更新：同步缓冲区。
	\note 根据 UseOpacity 选择更新操作。
	\since build 435
	*/
	void
	UpdateFrom(YSLib::Drawing::BitmapPtr, ScreenRegionBuffer&);
};

} // namespace Host;
#endif

} // namespace YSLib;

#endif

