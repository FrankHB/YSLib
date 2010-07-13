// YSLib::Shell::YForm by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-4-30 0:51:36;
// UTime = 2010-7-13 1:53;
// Version = 0.1364;


#ifndef INCLUDED_YFORM_H_
#define INCLUDED_YFORM_H_

// YForm ：平台无关的图形用户界面窗体实现。

#include "ywindow.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

YSL_BEGIN_NAMESPACE(Forms)

//窗体。
class YForm : public GMCounter<YForm>, public YFrameWindow
{
public:
	typedef YFrameWindow ParentType;

//	Widgets::YWidgetContainer Client;

protected:

public:
	explicit
	YForm(const SRect& = SRect::Empty, const GHResource<YImage> = new YImage(),
		YDesktop* = ::YSLib::pDefaultDesktop, HSHL = ::YSLib::theApp.GetShellHandle(), HWND = NULL);
	virtual
	~YForm();

/*	virtual void
	SetSize(SDST, SDST);
	virtual void
	SetBounds(const SRect&);

	virtual void
	DrawBackground();
	virtual void
	DrawForeground();
	virtual void
	Draw();
*/
};

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

