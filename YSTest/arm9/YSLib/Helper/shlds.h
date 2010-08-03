// YSLib::Helper -> Shell_DS by Franksoft 2010
// CodePage = UTF-8
// CTime = 2010-3-13 14:17:14;
// UTime = 2010-8-2 15:44;
// Version = 0.1452;


#ifndef INCLUDED_SHLDS_H_
#define INCLUDED_SHLDS_H_

// Shell_DS ： Shell 类库 DS 版本。

#include "yshelper.h"

YSL_BEGIN

//标准图形用户界面窗口 Shell 。
class ShlGUI : public YShellMain
{
public:
	ShlGUI();
	virtual
	~ShlGUI() ythrow();

	void
	SendDrawingMessage();
	virtual void
	UpdateToScreen();

	virtual LRES
	OnDeactivated(const MMSG&);
};


YSL_BEGIN_NAMESPACE(DS)

//双屏全屏窗口 Shell 。
class ShlDS : public ShlGUI
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
	ShlProc(const MMSG&);

	virtual LRES
	OnDeactivated(const MMSG&);
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
ResponseInput(const MMSG&);


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

