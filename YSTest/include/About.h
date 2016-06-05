/*
	© 2012-2013, 2015-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file About.h
\ingroup YReader
\brief 关于界面。
\version r92
\author FrankHB <frankhb1989@gmail.com>
\since build 390
\par 创建时间:
	2013-03-20 21:07:32 +0800
\par 修改时间:
	2016-06-05 20:07 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::About
*/


#ifndef INC_YReader_About_h_
#define INC_YReader_About_h_ 1

#include "Shells.h"

namespace YReader
{

/*!
\brief 关于窗体。
\since build 363
*/
class AboutPanel : public DialogPanel
{
public:
	//! \since build 697
	enum class Command
	{
		SystemInfo
	};
	//! \since build 697
	GEvent<void(Command)> OnCommand{};

protected:
	//! \since build 434
	DynamicWidget dynWgts;

public:
	AboutPanel();
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 601
	*/
	~AboutPanel() override;
};


/*!
\brief 系统信息面板。
\since build 696
*/
class SystemInformationPanel : public DialogPanel
{
private:
	TreeView view;
	//! \since build 698
	Label lbl_title;

public:
	SystemInformationPanel();
	//! \brief 虚析构：类定义外默认实现。
	~SystemInformationPanel() override;

	//! \brief 更新内容。
	void
	UpdateContents();
};

} // namespace YReader;

#endif

