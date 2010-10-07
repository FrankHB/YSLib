// YSLib::Helper -> Shell_DS by Franksoft 2010
// CodePage = UTF-8
// CTime = 2010-03-13 14:17:14 + 08:00;
// UTime = 2010-10-05 15:27 + 08:00;
// Version = 0.1496;


#ifndef INCLUDED_SHLDS_H_
#define INCLUDED_SHLDS_H_

// Shell_DS ： Shell 类库 DS 版本。

#include "yshelper.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Shells)

//标准命令行界面 Shell 。
class ShlCLI : public YShellMain
{
public:
	ShlCLI();
	virtual
	~ShlCLI() ythrow();

	virtual LRES
	OnActivated(const Message&);

	virtual LRES
	OnDeactivated(const Message&);

	IRES
	ExecuteCommand(const uchar_t*);
	IRES
	ExecuteCommand(const String&);
};

inline
ShlCLI::ShlCLI()
	: YShellMain()
{}
inline
ShlCLI::~ShlCLI() ythrow()
{}

inline int
ShlCLI::ExecuteCommand(const String& s)
{
	return ExecuteCommand(s.c_str());
}


//标准图形用户界面窗口 Shell 。
class ShlGUI : public YShellMain
{
public:
	ShlGUI();
	virtual
	~ShlGUI() ythrow();

	virtual LRES
	OnDeactivated(const Message&);

	void
	SendDrawingMessage();

	virtual void
	UpdateToScreen();
};

inline
ShlGUI::ShlGUI()
	: YShellMain()
{}
inline
ShlGUI::~ShlGUI() ythrow()
{}

YSL_END_NAMESPACE(Shells)


YSL_BEGIN_NAMESPACE(DS)

//双屏全屏窗口 Shell 。
class ShlDS : public Shells::ShlGUI
{
protected:
	HWND hWndUp, hWndDown;

public:
	ShlDS();
	virtual
	~ShlDS() ythrow();

	DefGetter(HWND, UpWindowHandle, hWndUp)
	DefGetter(HWND, DownWindowHandle, hWndDown)

	virtual LRES
	ShlProc(const Message&);

	virtual LRES
	OnDeactivated(const Message&);
};


//平台相关输入处理。

inline void
OnTouchUp(const Runtime::MTouchEventArgs::InputType& pt)
{
	Runtime::ResponseTouchUp(*pDesktopDown, Runtime::MTouchEventArgs(pt));
}

inline void
OnTouchDown(const Runtime::MTouchEventArgs::InputType& pt)
{
	Runtime::ResponseTouchDown(*pDesktopDown, Runtime::MTouchEventArgs(pt));
}

inline void
OnTouchHeld(const Runtime::MTouchEventArgs::InputType& pt)
{
	Runtime::ResponseTouchHeld(*pDesktopDown, Runtime::MTouchEventArgs(pt));
}

inline void
OnKeyUp(const Runtime::MKeyEventArgs::InputType& key)
{
	Runtime::ResponseKeyUp(*pDesktopDown, Runtime::MKeyEventArgs(key));
}

inline void
OnKeyDown(const Runtime::MKeyEventArgs::InputType& key)
{
	Runtime::ResponseKeyDown(*pDesktopDown, Runtime::MKeyEventArgs(key));
}

inline void
OnKeyHeld(const Runtime::MKeyEventArgs::InputType& key)
{
	Runtime::ResponseKeyHeld(*pDesktopDown, Runtime::MKeyEventArgs(key));
}

void
ResponseInput(const Message&);


// Shell 快捷操作。

inline void
NowShellInsertDropMessage(Shells::MSGPRIORITY p = 0x80)
{
	InsertMessage(NULL, SM_DROP, p, handle_cast<WPARAM>(NowShell()));
}

inline void
ShlClearBothScreen(HSHL h = NowShell())
{
	h->ClearScreenWindows(*pDesktopUp);
	h->ClearScreenWindows(*pDesktopDown);
}

inline void
NowShellDrop(Shells::MSGPRIORITY p = 0x80)
{
	NowShellInsertDropMessage(p);
	ShlClearBothScreen();
}

YSL_END_NAMESPACE(DS)

YSL_END;

#endif

