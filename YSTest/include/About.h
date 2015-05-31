﻿/*
	© 2012-2013, 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file About.h
\ingroup YReader
\brief 关于界面。
\version r62
\author FrankHB <frankhb1989@gmail.com>
\since build 390
\par 创建时间:
	2013-03-20 21:07:32 +0800
\par 修改时间:
	2015-05-29 21:16 +0800
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
class FrmAbout : public Form
{
protected:
	//! \since build 434
	DynamicWidget dynWgts;

public:
	FrmAbout();
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 601
	*/
	~FrmAbout() override;
};

} // namespace YReader;

#endif

