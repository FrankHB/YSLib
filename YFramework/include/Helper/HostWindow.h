/*
	© 2013-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostWindow.h
\ingroup Helper
\brief 宿主环境窗口。
\version r412
\author FrankHB <frankhb1989@gmail.com>
\since build 389
\par 创建时间:
	2013-03-18 18:16:53 +0800
\par 修改时间:
	2014-10-04 15:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::HostWindow
*/


#ifndef INC_Helper_HostWindow_h_
#define INC_Helper_HostWindow_h_ 1

#include "YModules.h"
#include YFM_Helper_YGlobal
#if YCL_Win32
#	include YFM_MinGW32_YCLib_Win32GUI
#	include YFM_YSLib_Core_YString // for YSLib::String;
#	include YFM_YSLib_UI_YWidget // for UI::IWidget;
#	include <atomic>
#	include <mutex>
#elif YCL_Android
#	include YFM_Android_YCLib_Android
#endif

namespace YSLib
{

#if YF_Hosted
namespace Host
{

//! \since build 427
using namespace platform_ex;

/*!
\brief 宿主环境支持的窗口。
\since build 379
*/
class YF_API Window : public platform_ex::HostWindow
{
private:
	//! \since build 380
	std::reference_wrapper<Environment> env;

#	if YCL_Win32
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
	Drawing::AlphaType Opacity{0xFF};
	//! \since build 511
	//@{
	//! \brief 鼠标键输入。
	std::atomic<short> RawMouseButton{0};

private:
	/*!
	\brief 标识宿主插入符。
	\since build 512
	\see https://src.chromium.org/viewvc/chrome/trunk/src/ui/base/ime/win/imm32_manager.cc
		IMM32Manager::CreateImeWindow 的注释。
	*/
	bool has_hosted_caret;
	//! \brief 输入组合字符串锁。
	std::recursive_mutex input_mutex{};
	//! \brief 输入法组合字符串。
	String comp_str{};
	//! \brief 相对窗口的宿主插入符位置缓存。
	Drawing::Point caret_location{Drawing::Point::Invalid};
	//@}
#	endif

public:
	/*!
	\exception LoggedEvent 异常中立：窗口类名不是 WindowClassName 。
	\since build 429
	*/
	//@{
	Window(NativeWindowHandle);
	Window(NativeWindowHandle, Environment&);
	//@}
	~Window() override;

	DefGetter(const ynothrow, Environment&, Host, env)

	/*!
	\brief 刷新：保持渲染状态同步。
	\since build 407
	*/
	virtual PDefH(void, Refresh, )
		ImplExpr(void())

#	if YCL_Win32
	/*!
	\brief 访问输入法状态。
	\note 线程安全：互斥访问。
	\since build 511
	*/
	template<typename _func>
	auto
	AccessInputString(_func f) -> decltype(f(comp_str))
	{
		std::lock_guard<std::recursive_mutex> lck(input_mutex);

		return f(comp_str);
	}

	/*!
	\brief 更新输入法编辑器候选窗口位置。
	\note 位置为相对窗口客户区的坐标。
	\note 若位置为 Drawing::Point::Invalid 则忽略。
	\sa caret_location
	\since build 512
	*/
	//@{
	//! \note 线程安全。
	//@{
	//! \note 取缓存的位置。
	void
	UpdateCandidateWindowLocation();
	//! \note 首先无条件更新缓存。
	void
	UpdateCandidateWindowLocation(const Drawing::Point&);
	//@}

	//! \note 无锁版本，仅供内部实现。
	void
	UpdateCandidateWindowLocationUnlocked();
	//@}

	/*!
	\brief 更新文本焦点：根据指定的部件和相对部件的位置调整状态。
	\since build 518
	*/
	virtual void
	UpdateTextInputFocus(UI::IWidget&, const Drawing::Point&);
#	endif

	/*!
	\brief 更新：同步缓冲区。
	\note 根据 UseOpacity 选择更新操作。
	\since build 435
	*/
	void
	UpdateFrom(Drawing::BitmapPtr, ScreenRegionBuffer&);
};

} // namespace Host;
#endif

} // namespace YSLib;

#endif

