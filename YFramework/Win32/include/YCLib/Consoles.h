/*
	© 2013-2016, 2019-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Consoles.h
\ingroup YCLib
\ingroup Win32
\brief 控制台。
\version r418
\author FrankHB <frankhb1989@gmail.com>
\since build 520
\par 创建时间:
	2013-05-09 11:01:12 +0800
\par 修改时间:
	2023-03-26 12:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Win32)::Consoles
*/


#ifndef YCL_Win32_INC_Consoles_h_
#define YCL_Win32_INC_Consoles_h_

#include "YCLib/YModules.h"
#include YFM_Win32_YCLib_MinGW32 // for std::uint8_t, ::HANDLE;
#include YFM_YCLib_Container // for array;
#include YFM_YCLib_Video
#include <wincon.h> // for ::WORD, ::COORD;

namespace platform_ex
{

inline namespace Windows
{

/*!
\brief 安装控制台处理器。
\throw Win32Exception 设置失败。
\warning 默认不应在 \c std::at_quick_exit 注册依赖静态或线程生存期对象状态的回调。
\see https://msdn.microsoft.com/library/windows/desktop/ms682658.aspx 。
\see https://msdn.microsoft.com/library/windows/desktop/ms686016.aspx 。
\see $2015-01 @ %Documentation::Workflow 。
\since build 565

若第一参数为空，则使用具有以下行为的处理器：
对 \c CTRL_C_EVENT \c CTRL_CLOSE_EVENT 、 \c CTRL_BREAK_EVENT 、
\c CTRL_LOGOFF_EVENT 、和 \c CTRL_SHUTDOWN_EVENT ，调用
\c std::_Exit(STATUS_CONTROL_C_EXIT) 。
第二参数指定添加或移除。
若不设置控制台处理器，Win32 的默认行为使用 \c ::ExitProcess ，
	可能造成 C/C++ 运行时无法正常清理。
*/
YF_API void
FixConsoleHandler(int(WINAPI*)(unsigned long) = {}, bool = true);


/*!
\brief Windows 控制台对象。
\since build 304
*/
class YF_API WConsole
{
public:
	/*!
	\brief Windows 控制台文本样式。
	\since build 519
	*/
	enum Style : std::uint8_t
	{
		//! brief 暗淡文本，暗淡背景。
		Normal = 0,
		//! brief 明亮文本，暗淡背景。
		Bright = 0x08
	};

private:
	//! \since build 906
	::HANDLE h_out;
	//! \since build 519
	::WORD saved_attr;

public:
	/*!
	\brief 当前属性。
	\since build 519
	*/
	::WORD Attributes;

	//! \pre 断言：句柄非空有效。
	//!@{
	//! \since build 564
	WConsole(unsigned long h = STD_OUTPUT_HANDLE);
	/*!
	\brief 构造：使用已知控制台句柄。
	\since build 559
	*/
	WConsole(::HANDLE);
	//!@}
	//! \note 析构：重置属性。
	~WConsole();

	//! \since build 519
	//!@{
	//! \exception Win32Exception 操作失败。
	//!@{
	YB_ATTR_nodiscard DefGetter(const, ::COORD, CursorPosition,
		GetScreenBufferInfo().dwCursorPosition)
	::CONSOLE_SCREEN_BUFFER_INFO
	GetScreenBufferInfo() const;
	//!@}
	YB_ATTR_nodiscard
		DefGetter(const ynothrow, ::WORD, SavedAttributes, saved_attr)
	//!@}

	//! \exception Win32Exception 操作失败。
	//!@{
	//! \since build 645
	void
	SetBackColor(std::uint8_t) ynothrow;
	//! \since build 519
	void
	SetCursorPosition(::COORD);
	//! \since build 645
	void
	SetForeColor(std::uint8_t) ynothrow;
	//!@}

	/*!
	\brief 清除：更新属性并以空格填充输出缓冲区内容并移动光标到起始位置。
	\exception Win32Exception 操作失败。
	\since build 969
	*/
	PDefH(void, Clear, )
		ImplExpr(Update(), Erase())

	//! \since build 645
	YB_ATTR_nodiscard YB_STATELESS yconstfn static ::WORD
	ComposeAttributes(std::uint8_t fc, std::uint8_t bc) ynothrow
		ImplRet((fc & 15) | ((bc & 15) << 4))
	YB_ATTR_nodiscard YB_STATELESS yconstfn static ::WORD
	ComposeAttributes(::WORD attr, std::uint8_t fc, std::uint8_t bc)
		ynothrow
		ImplRet((attr & 0xFF00) | ComposeAttributes(fc, bc))

	//! \since build 519
	//!@{
	//! \exception Win32Exception 操作失败。
	void
	CursorUp(size_t);

	//! \since build 645
	YB_ATTR_nodiscard YB_STATELESS yconstfn static PDefH(array<std::uint8_t
		YPP_Comma 2>, DecomposeAttributes, ::WORD val) ynothrow
		ImplRet({{FetchForeColor(val), FetchBackColor(val)}})

	//! \exception Win32Exception 操作失败。
	//!@{
	/*!
	\brief 擦除：以参数填充输出缓冲区内容并移动光标到起始位置。
	\sa Fill
	\since build 969
	*/
	void
	Erase(wchar_t = L' ');

	//! \since build 645
	YB_ATTR_nodiscard YB_STATELESS yconstfn static
		PDefH(std::uint8_t, FetchBackColor, ::WORD attr) ynothrow
		ImplRet(std::uint8_t((attr >> 4) & 15))

	//! \since build 645
	YB_ATTR_nodiscard YB_STATELESS yconstfn static
		PDefH(std::uint8_t, FetchForeColor, ::WORD attr) ynothrow
		ImplRet(std::uint8_t(attr & 15))

	/*!
	\brief 填充：从指定位置开始输出指定字符和属性，然后移动光标到指定位置。
	\note 字符和属性更新到屏幕的顺序未指定。
	\since build 576
	*/
	void
	Fill(::COORD, unsigned long, wchar_t = L' ');

	/*!
	\brief 重置属性。
	\since build 645
	*/
	void
	RestoreAttributes();

	/*!
	\brief 更新当前属性。
	\sa Attributes
	*/
	void
	Update();
	//! \brief 更新参数指定的属性。
	void
	Update(::WORD); 

	//! \brief 更新前景色。
	void
	UpdateBackColor(std::uint8_t);

	//! \brief 更新背景色。
	void
	UpdateForeColor(std::uint8_t);
	//!@}

	/*!
	\brief 输出字符串。
	\pre 断言：第一参数的数据指针非空。
	\return 输出的字符数。
	\note 因为 Windows 控制台的限制，不保证可正确输出 Unicode BMP 外的代码点。
	\since build 645
	*/
	//!@{
	//! \note 使用 UTF-8 字符串。
	size_t
	WriteString(string_view);
	//! \note 第二参数为指定第一参数的编码的代码页。
	size_t
	WriteString(string_view, unsigned);
	//! \note 使用 UTF-16LE 字符串。
	//!@{
	//! \since build 970
	YB_NONNULL(2) size_t
	WriteString(const wchar_t*, size_t);
	PDefH(size_t, WriteString, wstring_view sv)
		ImplRet(WriteString(sv.data(), sv.length()))
	PDefH(size_t, WriteString, u16string_view sv)
		ImplRet(WriteString(wstring(sv.cbegin(), sv.cend())))
	//!@}
	//!@}
	//!@}
};

/*!
\ingroup helper_functions
\brief 创建控制台对象。
\return 指向新创建的控制台对象的指针，若构造失败则为空。
\note 不抛出 Win32Exception 。
\relates WConsole
\since build 593
*/
YF_API unique_ptr<WConsole>
MakeWConsole(unsigned long = STD_OUTPUT_HANDLE);

} // inline namespace Windows;

} // namespace platform_ex;

#endif

