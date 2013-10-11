/*
	© 2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShlExplorer.h
\ingroup YReader
\brief 文件浏览器。
\version r139
\author FrankHB <frankhb1989@gmail.com>
\since build 390
\par 创建时间:
	2013-03-20 21:10:49 +0800
\par 修改时间:
	2013-10-06 22:40 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ShlExplorer
*/


#ifndef INC_YReader_ShlExplorer_h_
#define INC_YReader_ShlExplorer_h_ 1

#include "About.h"

namespace YReader
{

//! \since build 429
class SwitchScreensButton : public Button
{
private:
	std::reference_wrapper<ShlDS> shell;

public:
	SwitchScreensButton(ShlDS&, const Point&);
};


class ShlExplorer : public ShlDS
{
private:
	//! \since build 437
	CheckButton* p_ChkFPS{};
	//! \since build 444
	unique_ptr<BorderResizer> p_border{};

protected:
	//! \since build 436
	DynamicWidget dynWgts_Main, dynWgts_Sub;
	//! \since build 363
	//@{
	unique_ptr<FrmAbout> pFrmAbout;
	MenuHost mhMain;
	FPSCounter fpsCounter;
	//@}
	//! \since build 429
	SwitchScreensButton btnSwitchMain, btnSwitchSub;
	/*!
	\brief 切换屏幕的按键掩码。
	\note 清空时无效。
	\since build 429
	*/
	KeyInput SwapMask = KeyInput().set(YCL_KEY(L)).set(YCL_KEY(R));

public:
	/*!
	\brief 构造：使用指定路径和上下桌面。
	\pre 第一参数为合法路径。
	\sa ShlDS::ShlDS
	\since build 395
	*/
	ShlExplorer(const IO::Path& = IO::GetNowDirectory(),
		const shared_ptr<Desktop>& = {}, const shared_ptr<Desktop>& = {});

	/*!
	\brief 处理绘制消息：更新到屏幕并刷新 FPS 。
	\since build 289
	*/
	void
	OnPaint() override;
};

} // namespace YReader;

#endif

