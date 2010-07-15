// YSLib::Helper -> Shell_DS by Franksoft 2010
// CodePage = UTF-8
// CTime = 2010-3-13 14:17:14;
// UTime = 2010-7-14 11:53;
// Version = 0.1426;


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
	~ShlGUI();

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
	~ShlDS();

	DefGetter(HWND, UpWindowHandle, hWndUp)
	DefGetter(HWND, DownWindowHandle, hWndDown)

	virtual LRES
	ShlProc(const MMSG&);

	virtual LRES
	OnDeactivated(const MMSG&);
};


//平台相关输入处理。

inline void
OnTouchUp(const Runtime::YTouchEventArgs::InputType& pt)
{
	Runtime::ResponseTouchUp(*pDesktopDown, Runtime::YTouchEventArgs(pt));
}

inline void
OnTouchDown(const Runtime::YTouchEventArgs::InputType& pt)
{
	Runtime::ResponseTouchDown(*pDesktopDown, Runtime::YTouchEventArgs(pt));
}

inline void
OnTouchHeld(const Runtime::YTouchEventArgs::InputType& pt)
{
	Runtime::ResponseTouchHeld(*pDesktopDown, Runtime::YTouchEventArgs(pt));
}

inline void
OnKeyUp(const Runtime::YKeyEventArgs::InputType& key)
{
	Runtime::ResponseKeyUp(*pDesktopDown, Runtime::YKeyEventArgs(key));
}

inline void
OnKeyDown(const Runtime::YKeyEventArgs::InputType& key)
{
	Runtime::ResponseKeyDown(*pDesktopDown, Runtime::YKeyEventArgs(key));
}

inline void
OnKeyHeld(const Runtime::YKeyEventArgs::InputType& key)
{
	Runtime::ResponseKeyHeld(*pDesktopDown, Runtime::YKeyEventArgs(key));
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

