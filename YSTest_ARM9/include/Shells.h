/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup YReader YReader
\brief YReader 阅读器。
*/

/*!	\file Shells.h
\ingroup YReader
\brief Shell 框架逻辑。
\version r3596;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-03-06 21:38:16 +0800;
\par 修改时间:
	2012-02-27 22:26 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YReader::Shells;
*/


#ifndef INCLUDED_SHELLS_H_
#define INCLUDED_SHELLS_H_

#include <YSLib/Helper/shlds.h>

YSL_BEGIN

typedef decltype(__LINE__) ResourceIndex;
typedef map<ResourceIndex, ValueObject> ResourceMap;

#define DeclResource(_name) const ResourceIndex _name(__LINE__);

YSL_END


YSL_BEGIN_NAMESPACE(YReader)

//全局常量。
//extern CPATH DEF_DIRECTORY;

using namespace YSLib;

using namespace Components;
using namespace Drawing;

using namespace DS;
using namespace DS::Components;

using platform::YDebugSetStatus;


shared_ptr<Image>&
FetchImage(size_t);


/*!
\brief FPS 计数器。
*/
class FPSCounter
{
private:
	Timers::TimeSpan last_tick;
	Timers::TimeSpan now_tick;

public:
	FPSCounter();

	DefGetter(const ynothrow, Timers::TimeSpan, LastTick, last_tick)
	DefGetter(const ynothrow, Timers::TimeSpan, NowTick, now_tick)

	/*!
	\brief 刷新计数器。
	\return 每秒毫帧数。
	*/
	u32
	Refresh();
};

inline
FPSCounter::FPSCounter()
	: last_tick(), now_tick()
{}


class ShlExplorer : public ShlDS
{
public:
	typedef ShlDS ParentType;

	struct TFormTest : public Form
	{
		Button btnEnterTest, btnMenuTest, btnShowWindow,
			btnPrevBackground, btnNextBackground;

		TFormTest();
	};

	struct TFormExtra : public Form
	{
		Button btnDragTest;
		Button btnTestEx;
		Button btnClose;
		Button btnExit;

		TFormExtra();
	};

public:
	Label lblTitle, lblPath;
	FileBox fbMain;
	Button btnTest, btnOK;
	CheckBox chkFPS, chkHex;
	unique_ptr<TFormTest> pWndTest;
	unique_ptr<TFormExtra> pWndExtra;
	Label lblA, lblB;
	MenuHost mhMain;
	FPSCounter fpsCounter;

	ShlExplorer();

	/*!
	\brief 处理激活消息。
	\since build 289 。
	*/
	virtual void
	OnActivated(const Message&);

	/*!
	\brief 处理停用消息。
	\since build 289 。
	*/
	virtual void
	OnDeactivated();

	/*!
	\brief 处理绘制消息：更新到屏幕并刷新 FPS 。
	\since build 289 。
	*/
	virtual void
	OnPaint();

private:
	IWidget*
	GetBoundControlPtr(const KeyCode&);

	static void
	OnConfirmed_fbMain(IndexEventArgs&&);

	static void
	OnClick_ShowWindow(TouchEventArgs&&);
};

YSL_END_NAMESPACE(YReader)

#endif

