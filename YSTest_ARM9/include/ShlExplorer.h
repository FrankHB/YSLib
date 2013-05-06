/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShlExplorer.h
\ingroup YReader
\brief 文件浏览器。
\version r90
\author FrankHB <frankhb1989@gmail.com>
\since build 390
\par 创建时间:
	2013-03-20 21:10:49 +0800
\par 修改时间:
	2013-05-05 13:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ShlExplorer
*/


#ifndef INC_YReader_ShlExplorer_h_
#define INC_YReader_ShlExplorer_h_ 1

#include "About.h"

YSL_BEGIN_NAMESPACE(YReader)

class ShlExplorer : public ShlDS
{
protected:
	//! \since build 363
	//@{
	Label lblTitle, lblPath;
	/*!
	\brief 信息标签：说明用户操作内容等。
	*/
	Label lblInfo;
	FileBox fbMain;
	Button btnTest, btnOK;
	Button btnMenu;
	DialogPanel pnlSetting;
	CheckButton cbHex, cbFPS;
	/*!
	\brief 复选按钮：指定开启切换背景时的预览。
	*/
	CheckButton cbPreview;
	Label lblDragTest;
	Button btnEnterTest, btnTestEx, btnPrevBackground, btnNextBackground;
	unique_ptr<FrmAbout> pFrmAbout;
	MenuHost mhMain;
	FPSCounter fpsCounter;
	//@}

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

private:
	IWidget*
	GetBoundControlPtr(const KeyInput&);
};

YSL_END_NAMESPACE(YReader)

#endif

