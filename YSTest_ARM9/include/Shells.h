/*
	Copyright (C) by Franksoft 2010 - 2011.

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
\version r3396;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-06 21:38:16 +0800;
\par 修改时间:
	2011-08-26 13:53 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::Shells;
*/


#ifndef INCLUDED_SHELLS_H_
#define INCLUDED_SHELLS_H_

#include <YSLib/Helper/shlds.h>
#include "DSReader.h"

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
using namespace Components::Widgets;
using namespace Components::Controls;
using namespace Components::Forms;
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

	DefGetter(Timers::TimeSpan, LastTick, last_tick)
	DefGetter(Timers::TimeSpan, NowTick, now_tick)

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

		static void
		OnEnter_btnEnterTest(IControl& sender, TouchEventArgs&&);

		static void
		OnLeave_btnEnterTest(IControl& sender, TouchEventArgs&&);

		void
		OnClick_btnMenuTest(TouchEventArgs&&);
	};

	struct TFormExtra : public Form
	{
		Button btnDragTest;
		Button btnTestEx;
		Button btnClose;
		Button btnExit;

		TFormExtra();

		void
		OnMove_btnDragTest(EventArgs&&);

		void
		OnTouchUp_btnDragTest(TouchEventArgs&&);

		void
		OnTouchDown_btnDragTest(TouchEventArgs&&);

		void
		OnClick_btnDragTest(TouchEventArgs&&);

		static void
		OnKeyPress_btnDragTest(IControl& sender, KeyEventArgs&& e);

		void
		OnClick_btnTestEx(TouchEventArgs&&);

		void
		OnClick_btnClose(TouchEventArgs&&);
	};

	Label lblTitle, lblPath;
	FileBox fbMain;
	Button btnTest, btnOK;
	CheckBox chkFPS;
	unique_ptr<TFormTest> pWndTest;
	unique_ptr<TFormExtra> pWndExtra;
	Label lblA, lblB;
	MenuHost mhMain;
	FPSCounter fpsCounter;

	ShlExplorer();

	virtual int
	OnActivated(const Message&);

	virtual int
	OnDeactivated(const Message&);

	virtual void
	UpdateToScreen();

private:
	IControl*
	GetBoundControlPtr(const KeyCode&);

	void
	ShowString(const String&);
	void
	ShowString(const char*);

	void
	OnClick_btnTest(TouchEventArgs&&);

	void
	OnClick_btnOK(TouchEventArgs&&);

	void
	OnViewChanged_fbMain(EventArgs&&);

	static void
	OnConfirmed_fbMain(IControl&, IndexEventArgs&&);

	static void
	OnClick_ShowWindow(IControl&, TouchEventArgs&&);

	static void
	OnTouchDown_FormExtra(IControl&, TouchEventArgs&&);
};


class ShlReader : public ShlDS
{
public:
	typedef ShlDS ParentType;

	class ReaderPanel : public AUIBoxControl
	{
	public:
		ShlReader& Shell;
		Button btnClose, btnUp, btnDown, btnLeft, btnRight;

		ReaderPanel(const Rect&, ShlReader&);

		ImplI1(AUIBoxControl) IControl*
		GetTopControlPtr(const Point&);

		virtual Rect
		Refresh(const Graphics&, const Point&, const Rect&);

		void
		UpdateEnablilty();
	};

	static string path;

	MDualScreenReader Reader;
	ReaderPanel Panel;
	TextFile* pTextFile;
	shared_ptr<Image> hUp, hDn;
	MenuHost mhMain;

	ShlReader();

	virtual int
	OnActivated(const Message&);

	virtual int
	OnDeactivated(const Message&);

private:
	void
	ExcuteReadingCommand(IndexEventArgs::IndexType);

	void
	ShowMenu(Menu::ID, const Point&);

	void
	OnClick(TouchEventArgs&&);

	void
	OnKeyDown(KeyEventArgs&&);
};

YSL_END_NAMESPACE(YReader)

#endif

