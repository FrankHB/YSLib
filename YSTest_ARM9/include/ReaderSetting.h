/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ReaderSetting.h
\ingroup YReader
\brief 阅读器设置数据。
\version r1164;
\author FrankHB<frankhb1989@gmail.com>
\since build 328 。
\par 创建时间:
	2012-07-24 22:13:41 +0800;
\par 修改时间:
	2012-07-24 23:05 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YReader::ReaderSetting;
*/


#ifndef INCLUDED_READERSETTING_H_
#define INCLUDED_READERSETTING_H_

#include "Shells.h"
#include "ColorPicker.h"

YSL_BEGIN_NAMESPACE(YReader)

/*!
\brief 取编码字符串。
\since build 290 。
*/
String
FetchEncodingString(MTextList::IndexType);


/*!
\brief 阅读器设置。
\since build 287 。
*/
struct ReaderSetting
{
	Color UpColor, DownColor, FontColor;
	Drawing::Font Font;
	/*!
	\brief 平滑滚屏。
	\since build 293 。
	*/
	bool SmoothScroll;
	/*!
	\brief 滚屏间隔。
	\since build 293 。
	*/
	std::chrono::milliseconds ScrollDuration;
	/*!
	\brief 平滑滚屏间隔。
	\since build 293 。
	*/
	std::chrono::milliseconds SmoothScrollDuration;
};


/*!
\brief 设置面板。
\since build 275 。
*/
class SettingPanel : public DialogPanel
{
	/*!
	\brief 友元类：共享设置状态。
	\since build 287 。
	*/
	friend class ShlTextReader;

protected:
	/*!
	\brief 标签：文本区域示例。
	\note 不加入 SettingPanel 容器，加入上屏桌面。
	\since build 280 。
	*/
	Label lblAreaUp, lblAreaDown;
	/*!
	\brief 按钮：字体大小递减/递增。
	\since build 280 。
	*/
	Button btnFontSizeDecrease, btnFontSizeIncrease;
	/*!
	\brief 按钮：设置。
	\since build 279 。
	*/
	Button btnSetUpBack, btnSetDownBack, btnTextColor;
	/*!
	\brief 下拉列表：字体设置。
	\since build 282 。
	*/
	DropDownList ddlFont;
	/*!
	\brief 下拉列表：编码设置。
	\since build 290 。
	*/
	DropDownList ddlEncoding;
	/*!
	\brief 复选框：选定自动光滑滚屏。
	\since build 292 。
	*/
	CheckBox chkSmoothScroll;
	/*!
	\brief 标签：选定自动光滑说明。
	\since build 292 。
	*/
	Label lblSmoothScroll;
	/*!
	\brief 下拉列表：自动滚屏时间设置。
	\since build 292 。
	*/
	DropDownList ddlScrollTiming;
	ColorBox boxColor;

private:
	/*!
	\brief 当前设置的颜色指针。
	\none 为空指针时忽略。
	\since build 277 。
	*/
	Color* pColor;
	/*!
	\brief 当前设置的编码。
	\since build 292 。
	*/
	Text::Encoding current_encoding;
	/*!
	\brief 滚屏间隔。
	\since build 292 。
	*/
	std::chrono::milliseconds scroll_duration;
	/*!
	\brief 平滑滚屏间隔。
	\since build 292 。
	*/
	std::chrono::milliseconds smooth_scroll_duration;

public:
	SettingPanel();

	/*!
	\brief 读取设置。
	\since build 293 。
	*/
	SettingPanel&
	operator<<(const ReaderSetting&);

	/*!
	\brief 保存设置。
	\since build 293 。
	*/
	SettingPanel&
	operator>>(ReaderSetting&);

	/*!
	\brief 更新示例文本。
	\since build 280 。
	*/
	void
	UpdateInfo();
};

YSL_END_NAMESPACE(YReader)

#endif

