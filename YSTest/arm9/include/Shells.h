/*
	Copyright (C) by Franksoft 2010.

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
\version 0.2970;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-03-06 21:38:16 + 08:00;
\par 修改时间:
	2010-12-12 06:34 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::Shells;
*/


#ifndef INCLUDED_SHELLS_H_
#define INCLUDED_SHELLS_H_

//#include "../YSLib/Helper/shlds.h"
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

		TFrmLoadUp(HSHL hShl)
		: YForm(Rect::FullScreen, GetImage(1), pDesktopUp, hShl),
			lblTitle(Rect(50, 20, 100, 22), this),
			lblStatus(Rect(60, 80, 80, 22), this)
		{
			lblTitle.Text = YApplication::ProductName;
			lblStatus.Text = "Loading...";
		//	lblTitle.Transparent = true;
			Draw();
		}
	};
	struct TFrmLoadDown
		: public YForm
	{
		YLabel lblStatus;

		TFrmLoadDown(HSHL hShl)
		: YForm(Rect::FullScreen, GetImage(2), pDesktopDown, hShl),
			lblStatus(Rect(30, 20, 160, 22), this)
		{
			lblStatus.Text = FS("初始化中，请稍后……");
			lblStatus.SetTransparent(true);
			Draw();
		}
	};

public:
	virtual LRES
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

		TFrmFileListMonitor(HSHL hShl)
		: YForm(Rect::FullScreen, GetImage(3), pDesktopUp, hShl),
			lblTitle(Rect(16, 20, 220, 22), this),
			lblPath(Rect(12, 80, 240, 22), this)
		{
			lblTitle.Text = "文件列表：请选择一个文件。";
			lblPath.Text = "/";
		//	lblTitle.Transparent = true;
		//	lblPath.Transparent = true;
			Draw();
		}
	};
	struct TFrmFileListSelecter
		: public YForm
	{
		YFileBox fbMain;
		YButton btnTest, btnOK;
		YHorizontalScrollBar sbTestH;
	//	YHorizontalTrack tkTestH;
		YVerticalTrack tkTestV;

		TFrmFileListSelecter(HSHL hShl)
		: YForm(Rect::FullScreen, GetImage(4), pDesktopDown, hShl),
			fbMain(Rect(6, 10, 210, 150), this),
			btnTest(Rect(115, 165, 65, 22), this),
			btnOK(Rect(185, 165, 65, 22), this),
			sbTestH(Rect(10, 165, 95, 16), this),
		//	tkTestH(Rect(10, 165, 95, 16), this),
			tkTestV(Rect(230, 10, 16, 95), this)
		{
			btnTest.Text = FS(" 测试(X)");
			btnOK.Text = FS(" 确定(R)");
			FetchEvent<EControl::KeyPress>(*this)
				+= &TFrmFileListSelecter::frm_KeyPress;
		//	fbMain.TouchDown += YFileBox::OnClick;
		//	fbMain.Click += &YListBox::_m_OnClick;
			FetchEvent<EControl::KeyPress>(fbMain) += fb_KeyPress;
			fbMain.Selected.Add(*this, &TFrmFileListSelecter::fb_Selected);
			fbMain.Confirmed += fb_Confirmed;
			FetchEvent<EControl::Click>(btnTest).Add(*this,
				&TFrmFileListSelecter::btnTest_Click);
			FetchEvent<EControl::Click>(btnOK).Add(*this,
				&TFrmFileListSelecter::btnOK_Click);
			btnOK.SetTransparent(false);
			Draw();
		}

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
	virtual LRES
	OnActivated(const Message&);

	virtual LRES
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

		TFormA(HSHL hShl)
			: YForm(Rect::FullScreen, GetImage(5), pDesktopUp, hShl),
			lblA(Rect(s_left, 20, 200, s_size), this),
			lblA2(Rect(s_left, 80, 72, s_size), this)
		{
			lblA.Text = YApplication::ProductName;
			lblA2.Text = "程序测试";
			lblA2.SetTransparent(true);
		}

		void ShowString(const String& s)
		{
			lblA.Text = s;
			lblA.Refresh();
		}
	};
	struct TFormB : public YForm
	{
		YButton btnB, btnB2;

		TFormB(HSHL hShl)
			: YForm(Rect(10, 40, 228, 70), /*GetImage(6)*/NULL,
			pDesktopDown, hShl),
			btnB(Rect(2, 5, 224, s_size), this),
			btnB2(Rect(45, 35, 124, s_size), this)
		{
			btnB.Text = FS("测试程序");
			btnB2.Text = FS("测试程序2");
			BackColor = ARGB16(1, 31, 31, 15);
			FetchEvent<EControl::TouchMove>(*this) += OnDrag;
		//	btnB.TouchMove += &AVisualControl::OnTouchMove;
			FetchEvent<EControl::Enter>(btnB) += btnB_Enter;
			FetchEvent<EControl::Leave>(btnB) += btnB_Leave;
			FetchEvent<EControl::TouchMove>(btnB2) += OnDrag;
		//	btnB2.TouchDown += btnC_Click;
		//	btnB.Enabled = false;
		}

		static void
		btnB_Enter(IVisualControl& sender, InputEventArgs&);
		static void
		btnB_Leave(IVisualControl& sender, InputEventArgs&);
	};

	struct TFormC : public YForm
	{
		YButton btnC;
		YButton btnReturn;
		YButton btnExit;

		TFormC(HSHL hShl)
			: YForm(Rect(5, 60, 180, 120), /*GetImage(7)*/NULL,
			pDesktopDown, hShl),
			btnC(Rect(13, 45, 184, s_size), this),
			btnReturn(Rect(13, 82, 60, s_size), this),
			btnExit(Rect(83, 82, 60, s_size), this)
		{
			btnC.Text = FS("测试y");
			btnReturn.Text = FS("返回");
			btnExit.Text = FS("退出");
			BackColor = ARGB16(1, 31, 15, 15);
			FetchEvent<EControl::TouchDown>(*this) += TFormC_TouchDown;
			FetchEvent<EControl::TouchMove>(*this) += OnDrag;
			FetchEvent<EControl::TouchUp>(btnC).Add(*this,
				&TFormC::btnC_TouchUp);
			FetchEvent<EControl::TouchDown>(btnC).Add(*this,
				&TFormC::btnC_TouchDown);
			FetchEvent<EControl::TouchMove>(btnC) += OnDrag;
			FetchEvent<EControl::Click>(btnC).Add(*this, &TFormC::btnC_Click);
			FetchEvent<EControl::KeyPress>(btnC) += btnC_KeyPress;
		//	btnC.Enabled = false;
			btnReturn.BackColor = ARGB16(1, 22, 23, 24);
			FetchEvent<EControl::Click>(btnReturn).Add(*this,
				&TFormC::btnReturn_Click);
			FetchEvent<EControl::Click>(btnExit).Add(*this,
				&TFormC::btnExit_Click);
		}

		void
		btnC_TouchUp(TouchEventArgs&);

		void
		btnC_TouchDown(TouchEventArgs&);

		void
		btnC_Click(TouchEventArgs&);

		static void
		btnC_KeyPress(IVisualControl& sender, KeyEventArgs& e);

		void
		btnReturn_Click(TouchEventArgs&);

		void
		btnExit_Click(TouchEventArgs&);
	};

	void ShowString(const String& s)
	{
		GHHandle<TFormA>(dynamic_cast<TFormA*>(GetPointer(hWndUp)))
			->ShowString(s);
	}
	void ShowString(const char* s)
	{
		ShowString(String(s));
	}

	static void
	TFormC_TouchDown(IVisualControl& sender, TouchEventArgs& e)
	{
		try
		{
			TFormC& frm(dynamic_cast<TFormC&>(sender));

			frm.BackColor = ARGB16(1, std::rand(), std::rand(), std::rand());
			frm.Refresh();
		}
		catch(std::bad_cast&)
		{}
	}

	virtual LRES
	OnActivated(const Message&);

	virtual LRES
	OnDeactivated(const Message&);

	virtual LRES
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
	virtual
	~ShlReader() ythrow()
	{}

	virtual LRES
	OnActivated(const Message&);

	virtual LRES
	OnDeactivated(const Message&);

	virtual LRES
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

