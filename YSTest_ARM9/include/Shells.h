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
\version r3480;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-06 21:38:16 +0800;
\par 修改时间:
	2011-10-30 13:31 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::Shells;
*/


#ifndef INCLUDED_SHELLS_H_
#define INCLUDED_SHELLS_H_

#include <YSLib/Helper/shlds.h>
#include <YSLib/UI/progress.h>
#include "DSReader.h"
#include "HexBrowser.h"

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
		OnEnter_btnEnterTest(TouchEventArgs&&);

		static void
		OnLeave_btnEnterTest(TouchEventArgs&&);
	};

	struct TFormExtra : public Form
	{
		Button btnDragTest;
		Button btnTestEx;
		Button btnClose;
		Button btnExit;

		TFormExtra();

		static void
		OnKeyPress_btnDragTest(KeyEventArgs&&);
	};

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

	virtual int
	OnActivated(const Message&);

	virtual int
	OnDeactivated(const Message&);

	virtual void
	UpdateToScreen();

private:
	IWidget*
	GetBoundControlPtr(const KeyCode&);

	void
	ShowString(const String&);
	void
	ShowString(const char*);

	static void
	OnConfirmed_fbMain(IndexEventArgs&&);

	static void
	OnClick_ShowWindow(TouchEventArgs&&);
};


class ShlReader : public ShlDS
{
public:
	typedef ShlDS ParentType;

	class ReaderPanel : public AUIBoxControl
	{
	public:
		ShlReader& Shell;
		Button btnClose;
		HorizontalTrack trReader;
		Label lblProgress;

		ReaderPanel(const Rect&, ShlReader&);

		ImplI(AUIBoxControl) IWidget*
		GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));

		virtual Rect
		Refresh(const PaintContext&);
	};

	class FileInfoPanel : public AUIBoxControl
	{
	public:
		ShlReader& Shell;
		Button btnClose;
		Label lblInfo;

		FileInfoPanel(const Rect&, ShlReader&);

		ImplI(AUIBoxControl) IWidget*
		GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));

		virtual Rect
		Refresh(const PaintContext&);

		void
		UpdateData();
	};

	static string path;
	static bool is_text;

	DualScreenReader Reader;
	ReaderPanel pnlReader;
	FileInfoPanel pnlFileInfo;
	TextFile* pTextFile;
	shared_ptr<Image> hUp, hDn;
	MenuHost mhMain;
	HexViewArea HexArea;

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

