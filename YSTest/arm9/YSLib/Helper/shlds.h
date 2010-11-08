// YSLib::Helper -> Shell_DS by Franksoft 2010
// CodePage = UTF-8
// CTime = 2010-03-13 14:17:14 + 08:00;
// UTime = 2010-11-08 18:10 + 08:00;
// Version = 0.1690;


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
	//********************************
	//名称:		ShlCLI
	//全名:		YSLib::Shells::ShlCLI::ShlCLI
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	无参数构造。
	//备注:		
	//********************************
	ShlCLI();
	//********************************
	//名称:		~ShlCLI
	//全名:		YSLib::Shells::ShlCLI::~ShlCLI
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	ythrow()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//********************************
	virtual
	~ShlCLI() ythrow();

	//********************************
	//名称:		OnActivated
	//全名:		YSLib::Shells::ShlCLI::OnActivated
	//可访问性:	virtual public 
	//返回类型:	LRES
	//修饰符:	
	//形式参数:	const Message &
	//功能概要:	处理激活消息。
	//备注:		
	//********************************
	virtual LRES
	OnActivated(const Message&);

	//********************************
	//名称:		OnDeactivated
	//全名:		YSLib::Shells::ShlCLI::OnDeactivated
	//可访问性:	virtual public 
	//返回类型:	LRES
	//修饰符:	
	//形式参数:	const Message &
	//功能概要:	处理停用消息。
	//备注:		
	//********************************
	virtual LRES
	OnDeactivated(const Message&);

	//********************************
	//名称:		ExecuteCommand
	//全名:		YSLib::Shells::ShlCLI::ExecuteCommand
	//可访问性:	public 
	//返回类型:	IRES
	//修饰符:	
	//形式参数:	const uchar_t *
	//功能概要:	执行命令行。
	//备注:		
	//********************************
	IRES
	ExecuteCommand(const uchar_t*);
	//********************************
	//名称:		ExecuteCommand
	//全名:		YSLib::Shells::ShlCLI::ExecuteCommand
	//可访问性:	public 
	//返回类型:	IRES
	//修饰符:	
	//形式参数:	const String &
	//功能概要:	执行命令行。
	//备注:		
	//********************************
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

	//********************************
	//名称:		OnDeactivated
	//全名:		YSLib::Shells::ShlGUI::OnDeactivated
	//可访问性:	virtual public 
	//返回类型:	LRES
	//修饰符:	
	//形式参数:	const Message &
	//功能概要:	处理停用消息。
	//备注:		
	//********************************
	virtual LRES
	OnDeactivated(const Message&);

	//********************************
	//名称:		SendDrawingMessage
	//全名:		YSLib::Shells::ShlGUI::SendDrawingMessage
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	发送绘制消息。
	//备注:		
	//********************************
	void
	SendDrawingMessage();

	//********************************
	//名称:		UpdateToScreen
	//全名:		YSLib::Shells::ShlGUI::UpdateToScreen
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	更新到屏幕。
	//备注:		
	//********************************
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
	//********************************
	//名称:		ShlDS
	//全名:		YSLib::DS::ShlDS::ShlDS
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	无参数构造。
	//备注:		
	//********************************
	ShlDS();
	DefEmptyDtor(ShlDS)

	DefGetter(HWND, UpWindowHandle, hWndUp)
	DefGetter(HWND, DownWindowHandle, hWndDown)

	//********************************
	//名称:		ShlProc
	//全名:		YSLib::DS::ShlDS::ShlProc
	//可访问性:	virtual public 
	//返回类型:	LRES
	//修饰符:	
	//形式参数:	const Message &
	//功能概要:	Shell 处理函数。
	//备注:		
	//********************************
	virtual LRES
	ShlProc(const Message&);

	//********************************
	//名称:		OnDeactivated
	//全名:		YSLib::DS::ShlDS::OnDeactivated
	//可访问性:	virtual public 
	//返回类型:	LRES
	//修饰符:	
	//形式参数:	const Message &
	//功能概要:	处理停用消息。
	//备注:		
	//********************************
	virtual LRES
	OnDeactivated(const Message&);
};


//平台相关输入处理。

//********************************
//名称:		OnTouchUp
//全名:		YSLib::DS::OnTouchUp
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	const Components::Controls::TouchEventArgs::InputType & pt
//功能概要:	处理屏幕接触结束事件。
//备注:		
//********************************
inline void
OnTouchUp(const Components::Controls::TouchEventArgs::InputType& pt)
{
	Components::Controls::TouchEventArgs e(pt);

	ResponseTouchUp(*pDesktopDown, e);
}

//********************************
//名称:		OnTouchDown
//全名:		YSLib::DS::OnTouchDown
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	const Components::Controls::TouchEventArgs::InputType & pt
//功能概要:	处理屏幕接触开始事件。
//备注:		
//********************************
inline void
OnTouchDown(const Components::Controls::TouchEventArgs::InputType& pt)
{
	Components::Controls::TouchEventArgs e(pt);

	ResponseTouchDown(*pDesktopDown, e);
}

//********************************
//名称:		OnTouchHeld
//全名:		YSLib::DS::OnTouchHeld
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	const Components::Controls::TouchEventArgs::InputType & pt
//功能概要:	处理屏幕接触保持事件。
//备注:		
//********************************
inline void
OnTouchHeld(const Components::Controls::TouchEventArgs::InputType& pt)
{
	Components::Controls::TouchEventArgs e(pt);

	ResponseTouchHeld(*pDesktopDown, e);
}

//********************************
//名称:		OnKeyUp
//全名:		YSLib::DS::OnKeyUp
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	const Components::Controls::KeyEventArgs::InputType & key
//功能概要:	处理按键接触结束事件。
//备注:		
//********************************
inline void
OnKeyUp(const Components::Controls::KeyEventArgs::InputType& key)
{
	Components::Controls::KeyEventArgs e(key);

	ResponseKeyUp(*pDesktopDown, e);
}

//********************************
//名称:		OnKeyDown
//全名:		YSLib::DS::OnKeyDown
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	const Components::Controls::KeyEventArgs::InputType & key
//功能概要:	处理按键接触开始事件。
//备注:		
//********************************
inline void
OnKeyDown(const Components::Controls::KeyEventArgs::InputType& key)
{
	Components::Controls::KeyEventArgs e(key);

	ResponseKeyDown(*pDesktopDown, e);
}

//********************************
//名称:		OnKeyHeld
//全名:		YSLib::DS::OnKeyHeld
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	const Components::Controls::KeyEventArgs::InputType & key
//功能概要:	处理按键接触保持事件。
//备注:		
//********************************
inline void
OnKeyHeld(const Components::Controls::KeyEventArgs::InputType& key)
{
	Components::Controls::KeyEventArgs e(key);

	ResponseKeyHeld(*pDesktopDown, e);
}

//********************************
//名称:		ResponseInput
//全名:		YSLib::DS::ResponseInput
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	const Message &
//功能概要:	响应输入。
//备注:		
//********************************
void
ResponseInput(const Message&);


// Shell 快捷操作。

//********************************
//名称:		NowShellInsertDropMessage
//全名:		YSLib::DS::NowShellInsertDropMessage
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	Shells::MSGPRIORITY p
//功能概要:	发送当前 Shell 预注销消息。
//备注:		
//********************************
inline void
NowShellInsertDropMessage(Shells::MSGPRIORITY p = 0x80)
{
	InsertMessage(NULL, SM_DROP, p, handle_cast<WPARAM>(NowShell()));
}

//********************************
//名称:		ShlClearBothScreen
//全名:		YSLib::DS::ShlClearBothScreen
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	HSHL h
//功能概要:	清除屏幕内容。
//备注:		
//********************************
inline void
ShlClearBothScreen(HSHL h = NowShell())
{
	h->ClearScreenWindows(*pDesktopUp);
	h->ClearScreenWindows(*pDesktopDown);
}

//********************************
//名称:		NowShellDrop
//全名:		YSLib::DS::NowShellDrop
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	Shells::MSGPRIORITY p
//功能概要:	当前 Shell 预注销。
//备注:		发送预注销消息后清除屏幕内容。
//********************************
inline void
NowShellDrop(Shells::MSGPRIORITY p = 0x80)
{
	NowShellInsertDropMessage(p);
	ShlClearBothScreen();
}

YSL_END_NAMESPACE(DS)

YSL_END;

#endif

