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
\version 0.3152;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-06 21:38:16 +0800;
\par 修改时间:
	2011-04-09 08:04 +0800;
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

GHStrong<YImage>&
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

	void
	frm_KeyUp(KeyEventArgs&);

	void
	frm_KeyDown(KeyEventArgs&);

	void
	frm_KeyPress(KeyEventArgs&);

	void
	btnTest_Click(TouchEventArgs&);

	void
	btnOK_Click(TouchEventArgs&);

	void
	fb_ViewChanged(EventArgs&);

	static void
	fb_KeyPress(IControl&, KeyEventArgs&);

	static void
	fb_Confirmed(IControl&, IndexEventArgs&);
};


class ShlSetting : public ShlDS
{
public:
	typedef ShlDS ParentType;

	static const SPos s_left = 5;
	static const SDst s_size = 22;

private:
	HWND hWndUp, hWndDown;

public:
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
		btnB_Enter(IControl& sender, InputEventArgs&);

		static void
		btnB_Leave(IControl& sender, InputEventArgs&);

		void
		btnB2_Click(TouchEventArgs&);
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
		btnC_KeyPress(IControl& sender, KeyEventArgs& e);

		void
		btnD_Click(TouchEventArgs&);

		void
		btnReturn_Click(TouchEventArgs&);

		void
		btnExit_Click(TouchEventArgs&);
	};

	virtual int
	ShlProc(const Message&);

	virtual int
	OnActivated(const Message&);

	virtual int
	OnDeactivated(const Message&);

	void ShowString(const String&);
	void ShowString(const char*);

	static void
	TFormC_TouchDown(IControl&, TouchEventArgs&);
};


class ShlReader : public ShlDS
{
public:
	typedef ShlDS ParentType;

	static string path;

	MDualScreenReader Reader;
	YTextFile* pTextFile;

	GHStrong<YImage> hUp, hDn;
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
	OnKeyPress(KeyEventArgs&);
};

YSL_END;

#endif

