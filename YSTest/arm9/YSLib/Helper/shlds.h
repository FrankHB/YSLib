// YSLib::Helper -> Shell_DS by Franksoft 2010
// CodePage = UTF-8
// CTime = 2010-3-13 14:17:14;
// UTime = 2010-7-11 15:58;
// Version = 0.1392;


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
	void
	DrawWindows();
	virtual void
	UpdateToScreen();
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

	HWND
	GetUpWindowHandle();
	HWND
	GetDownWindowHandle();
	virtual LRES
	ShlProc(const MMSG&);
};

inline HWND
ShlDS::GetUpWindowHandle()
{
	return hWndUp;
}
inline HWND
ShlDS::GetDownWindowHandle()
{
	return hWndDown;
}


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
OnTouchHeld(const Runtime::YTouchEventArgs::InputType& pt)
{
	Runtime::ResponseTouchHeld(*pDesktopDown, Runtime::YTouchEventArgs(pt));
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
NowShellClearBothScreen()
{
	NowShell()->ClearScreenWindows(*pDesktopUp);
	NowShell()->ClearScreenWindows(*pDesktopDown);
}

inline void
NowShellDrop(Shells::MSGPRIORITY p = 0x80)
{
	NowShellInsertDropMessage(p);
	NowShellClearBothScreen();
}

YSL_END_NAMESPACE(DS)

YSL_END;

#endif

