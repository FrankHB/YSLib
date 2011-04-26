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
\brief Shell 声明。
\version 0.3192;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-06 21:38:16 +0800;
\par 修改时间:
	2011-04-23 14:21 +0800;
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

//全局常量。
//extern CPATH DEF_DIRECTORY;

using namespace Components;
using namespace Components::Widgets;
using namespace Components::Controls;
using namespace Components::Forms;
using namespace Drawing;
using namespace Runtime;

using namespace DS;
using namespace DS::Components;

GHandle<YImage>&
GetImage(int);

class ShlLoad : public ShlDS
{
public:
	typedef ShlDS ParentType;

	YLabel lblTitle, lblStatus, lblDetails;

	ShlLoad();

	virtual int
	OnActivated(const Message&);

	virtual int
	OnDeactivated(const Message&);
	//	InitializeComponents();
};


class ShlExplorer : public ShlDS
{
public:
	typedef ShlDS ParentType;

	YLabel lblTitle, lblPath;
	YFileBox fbMain;
	YButton btnTest, btnOK;
	YCheckBox chkTest;

	ShlExplorer();

	virtual int
	ShlProc(const Message&);

	virtual int
	OnActivated(const Message&);

	virtual int
	OnDeactivated(const Message&);

private:
	void
	OnKeyUp_frm(KeyEventArgs&);

	void
	OnKeyDown_frm(KeyEventArgs&);

	void
	OnKeyPress_frm(KeyEventArgs&);

	void
	OnClick_btnTest(TouchEventArgs&);

	void
	OnClick_btnOK(TouchEventArgs&);

	void
	OnViewChanged_fbMain(EventArgs&);

	static void
	OnKeyPress_fbMain(IControl&, KeyEventArgs&);

	static void
	OnConfirmed_fbMain(IControl&, IndexEventArgs&);
};


class ShlSetting : public ShlDS
{
public:
	typedef ShlDS ParentType;

	auto_ptr<IWindow> pWndTest;
	auto_ptr<IWindow> pWndExtra;
	YLabel lblA, lblB;

	struct TFormTest : public YForm
	{
		YButton btnEnterTest, btnShowWindow;

		TFormTest();

		static void
		OnEnter_btnEnterTest(IControl& sender, InputEventArgs&);

		static void
		OnLeave_btnEnterTest(IControl& sender, InputEventArgs&);

		void
		OnClick_btnShowWindow(TouchEventArgs&);
	};

	struct TFormExtra : public YForm
	{
		YButton btnDragTest;
		YButton btnTestEx;
		YButton btnReturn;
		YButton btnExit;

		TFormExtra();

		void
		OnMove_btnDragTest(EventArgs&);

		void
		OnTouchUp_btnDragTest(TouchEventArgs&);

		void
		OnTouchDown_btnDragTest(TouchEventArgs&);

		void
		OnClick_btnDragTest(TouchEventArgs&);

		static void
		OnKeyPress_btnDragTest(IControl& sender, KeyEventArgs& e);

		void
		OnClick_btnTestEx(TouchEventArgs&);

		void
		OnClick_btnReturn(TouchEventArgs&);

		void
		OnClick_btnExit(TouchEventArgs&);
	};

	ShlSetting();

	virtual int
	ShlProc(const Message&);

	virtual int
	OnActivated(const Message&);

	virtual int
	OnDeactivated(const Message&);

private:
	void ShowString(const String&);
	void ShowString(const char*);

	static void
	OnTouchDown_FormExtra(IControl&, TouchEventArgs&);
};


class ShlReader : public ShlDS
{
public:
	typedef ShlDS ParentType;

	static string path;

	MDualScreenReader Reader;
	YTextFile* pTextFile;

	GHandle<YImage> hUp, hDn;
	bool bgDirty;

	ShlReader();

	virtual int
	ShlProc(const Message&);

	virtual int
	OnActivated(const Message&);

	virtual int
	OnDeactivated(const Message&);

	virtual void
	UpdateToScreen();

private:
	void
	OnClick(TouchEventArgs&);

	void
	OnKeyDown(KeyEventArgs&);
};

YSL_END;

#endif

