// YSLib::Shell::YForm by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-04-30 00:51:36 + 08:00;
// UTime = 2010-10-29 14:21 + 08:00;
// Version = 0.1397;


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

//	Widgets::YUIContainer Client;

protected:

public:
	//********************************
	//名称:		YForm
	//全名:		YSLib::Components::Forms::YForm::YForm
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const Rect &
	//形式参数:	const GHResource<YImage>
	//形式参数:	YDesktop *
	//形式参数:	HSHL
	//形式参数:	HWND
	//功能概要:	构造：使用指定边界、背景、桌面、 Shell 和父窗口。
	//备注:		
	//********************************
	explicit
	YForm(const Rect& = Rect::Empty, const GHResource<YImage> = new YImage(),
		YDesktop* = ::YSLib::pDefaultDesktop,
		HSHL = ::YSLib::theApp.GetShellHandle(), HWND = NULL);
	//********************************
	//名称:		~YForm
	//全名:		YSLib::Components::Forms::YForm::~YForm
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//********************************
	virtual
	~YForm() ythrow();

/*	virtual void
	SetSize(SDST, SDST);
	virtual void
	SetBounds(const Rect&);

protected:
	virtual void
	DrawBackground();

	virtual void
	DrawForeground();

public:
	virtual void
	Draw();
*/
};

YSL_END_NAMESPACE(Forms)

YSL_END_NAMESPACE(Components)

YSL_END

#endif

