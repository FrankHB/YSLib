﻿/*
	© 2012-2013, 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ReaderSetting.h
\ingroup YReader
\brief 阅读器设置。
\version r346
\author FrankHB <frankhb1989@gmail.com>
\since build 328
\par 创建时间:
	2012-07-24 22:13:41 +0800
\par 修改时间:
	2023-03-31 12:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ReaderSetting
*/


#ifndef INC_YReader_ReaderSetting_h_
#define INC_YReader_ReaderSetting_h_ 1

#include "Shells.h" // for Color, Drawing::Font, std::chrono::milliseconds;
#include YFM_YSLib_Core_ValueNode // for ValueNode;

namespace YReader
{

/*!
\brief 阅读器设置。
\since build 287
*/
class ReaderSetting
{
public:
	Color UpColor, DownColor, FontColor;
	Drawing::Font Font;
	/*!
	\brief 平滑滚屏。
	\since build 293
	*/
	bool SmoothScroll;
	/*!
	\brief 滚屏间隔。
	\since build 293
	*/
	std::chrono::milliseconds ScrollDuration;
	/*!
	\brief 平滑滚屏间隔。
	\since build 293
	*/
	std::chrono::milliseconds SmoothScrollDuration;

	//! \since build 334
	//!@{
	ReaderSetting();
	//! \since build 399
	ReaderSetting(const ValueNode::Container&);
	DefDeCopyCtor(ReaderSetting)
	DefDeMoveCtor(ReaderSetting)

	DefDeCopyAssignment(ReaderSetting)
	DefDeMoveAssignment(ReaderSetting)

	YB_ATTR_nodiscard DefGetter(const ynothrow, std::chrono::milliseconds,
		TimerSetting, SmoothScroll ? SmoothScrollDuration : ScrollDuration)
	//!@}

	//! \since build 971
	YB_ATTR_nodiscard YB_PURE ValueNode::Container
	ToNodeContainer(ValueNode::allocator_type) const;
};

} // namespace YReader;

#endif

