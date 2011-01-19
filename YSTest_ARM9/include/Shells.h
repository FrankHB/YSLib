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
\version 0.3066;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-06 21:38:16 + 08:00;
\par 修改时间:
	2011-01-16 08:02 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::Shells;
*/


#ifndef INCLUDED_SHELLS_H_
#define INCLUDED_SHELLS_H_

//#include <YSLib/Helper/shlds.h>
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

GHStrong<YImage>&
GetImage(int);

class ShlLoad : public ShlDS
{
private:
	struct TFrmLoadUp
		: public YForm
	{
		YLabel lblTitle, lblStatus;

		TFrmLoadUp();
	};
	struct TFrmLoadDown
		: public YForm
	{
		YLabel lblStatus;

		TFrmLoadDown();
	};

public:
	virtual int
	OnActivated(const Message&);
	//	InitializeComponents();
};


class ShlExplorer : public ShlDS
{
private:
	struct TFrmFileListMonitor
		: public YForm
	{
		YLabel lblTitle, lblPath;

		TFrmFileListMonitor();
	};
	struct TFrmFileListSelecter
		: public YForm
	{
		YFileBox fbMain;
		YButton btnTest, btnOK;
		YHorizontalScrollBar sbTestH;
	//	YHorizontalTrack tkTestH;
		YVerticalScrollBar sbTestV;
	//	YVerticalTrack tkTestV;

		TFrmFileListSelecter();

		void
		frm_KeyPress(KeyEventArgs&);

		void
		fb_Selected(IndexEventArgs&);

		void
		btnTest_Click(TouchEventArgs&);

		void
		btnOK_Click(TouchEventArgs&);
	};
	void LoadNextWindows();

public:
	virtual int
	OnActivated(const Message&);

	virtual int
	ShlProc(const Message&);

	static void
	fb_KeyPress(IVisualControl&, KeyEventArgs&);
	static void
	fb_Confirmed(IVisualControl&, IndexEventArgs&);
};


class ShlSetting : public ShlDS
{
public:
	typedef ShlDS ParentType;

	static const SPOS s_left = 5;
	static const SDST s_size = 22;

	static HWND hWndC;

	struct TFormA : public YForm
	{
		YLabel lblA;
		YLabel lblA2;

		TFormA();

		void ShowString(const String&);
	};
	struct TFormB : public YForm
	{
		YButton btnB, btnB2;

		TFormB();

		static void
		btnB_Enter(IVisualControl& sender, InputEventArgs&);
		static void
		btnB_Leave(IVisualControl& sender, InputEventArgs&);
	};

	struct TFormC : public YForm
	{
		YButton btnC;
		YButton btnD;
		YButton btnReturn;
		YButton btnExit;

		TFormC();

		void
		btnC_TouchUp(TouchEventArgs&);
		void
		btnC_TouchDown(TouchEventArgs&);
		void
		btnC_Click(TouchEventArgs&);

		static void
		btnC_KeyPress(IVisualControl& sender, KeyEventArgs& e);

		void
		btnD_Click(TouchEventArgs&);

		void
		btnReturn_Click(TouchEventArgs&);

		void
		btnExit_Click(TouchEventArgs&);
	};

	void ShowString(const String&);
	void ShowString(const char*);

	static void
	TFormC_TouchDown(IVisualControl&, TouchEventArgs&);

	virtual int
	OnActivated(const Message&);

	virtual int
	OnDeactivated(const Message&);

	virtual int
	ShlProc(const Message&);
};


class ShlReader : public Shells::ShlGUI
{
public:
	typedef ShlGUI ParentType;

	static string path;

	MDualScreenReader Reader;
	YTextFile* pTextFile;

	GHStrong<YImage> hUp, hDn;
	bool bgDirty;

	ShlReader();
	virtual DefEmptyDtor(ShlReader);

	virtual int
	OnActivated(const Message&);

	virtual int
	OnDeactivated(const Message&);

	virtual int
	ShlProc(const Message&);

	virtual void
	UpdateToScreen();

	void
	OnClick(TouchEventArgs&);

	void
	OnKeyPress(KeyEventArgs&);
};

YSL_END;

#endif

