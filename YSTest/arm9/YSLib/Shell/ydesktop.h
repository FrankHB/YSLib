// YSLib::Shell::YDesktop by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-05-02 12:00:08 + 08:00;
// UTime = 2010-10-29 23:14 + 08:00;
// Version = 0.2160;


#ifndef INCLUDED_YDESKTOP_H_
#define INCLUDED_YDESKTOP_H_

// YDesktop：平台无关的桌面抽象层。

#include "ywindow.h"
//#include <list>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//桌面。
class YDesktop : public YFrameWindow
{
	friend class YSLib::Def;

public:
	typedef YComponent ParentType;
	typedef list<IVisualControl*> DOs; //桌面对象组类型。

private:
	YScreen& Screen; //屏幕对象。
	DOs sDOs; //桌面对象组（末尾表示顶端）。

public:
	//********************************
	//名称:		YDesktop
	//全名:		YSLib::Components::YDesktop::YDesktop
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	YScreen &
	//形式参数:	Color
	//形式参数:	GHResource<Drawing::YImage>
	//功能概要:	
	//备注:		
	//********************************
	explicit
	YDesktop(YScreen&, Color = 0, GHResource<Drawing::YImage> = NULL);
	virtual DefEmptyDtor(YDesktop)

	DefGetter(const YScreen&, Screen, Screen) //取屏幕对象。
	DefGetter(BitmapPtr, BackgroundPtr, Screen.GetPtr()) //取背景指针。

	virtual PDefH(IVisualControl*, GetTopVisualControlPtr, const Point& p)
		ImplBodyBase(YDesktop, GetTopDesktopObjectPtr, p)

	//********************************
	//名称:		operator+=
	//全名:		YSLib::Components::YDesktop::operator+=
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	IVisualControl &
	//功能概要:	向桌面对象组添加桌面对象。
	//备注:		
	//********************************
	virtual void
	operator+=(IVisualControl&);
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Components::YDesktop::operator-=
	//可访问性:	virtual public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	IVisualControl &
	//功能概要:	从桌面对象组中移除指定桌面对象。
	//备注:		
	//********************************
	virtual bool
	operator-=(IVisualControl&);
	//********************************
	//名称:		RemoveAll
	//全名:		YSLib::Components::YDesktop::RemoveAll
	//可访问性:	public 
	//返回类型:	DOs::size_type
	//修饰符:	
	//形式参数:	IVisualControl &
	//功能概要:	从桌面对象组中移除所有指定桌面对象，返回移除的对象数。
	//备注:		
	//********************************
	DOs::size_type
	RemoveAll(IVisualControl&);

	//从桌面对象组中查找指定桌面对象对象并重新插入至顶端。
	//********************************
	//名称:		MoveToTop
	//全名:		YSLib::Components::YDesktop::MoveToTop
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	IVisualControl &
	//功能概要:	请求提升至容器顶端。
	//备注:		
	//********************************
	bool
	MoveToTop(IVisualControl&);

	//移除桌面对象组中首个桌面对象。
	//********************************
	//名称:		RemoveTopDesktopObject
	//全名:		YSLib::Components::YDesktop::RemoveTopDesktopObject
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	移除桌面对象组中顶端桌面对象。
	//备注:		
	//********************************
	void
	RemoveTopDesktopObject();

	//********************************
	//名称:		ClearDesktopObjects
	//全名:		YSLib::Components::YDesktop::ClearDesktopObjects
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清除桌面对象组。
	//备注:		
	//********************************
	void
	ClearDesktopObjects();

	//********************************
	//名称:		GetFirstDesktopObjectPtr
	//全名:		YSLib::Components::YDesktop::GetFirstDesktopObjectPtr
	//可访问性:	public 
	//返回类型:	IVisualControl*
	//修饰符:	const
	//功能概要:	取得桌面对象组中首个桌面对象的句柄。
	//备注:		
	//********************************
	IVisualControl*
	GetFirstDesktopObjectPtr() const;
	//********************************
	//名称:		GetTopDesktopObjectPtr
	//全名:		YSLib::Components::YDesktop::GetTopDesktopObjectPtr
	//可访问性:	public 
	//返回类型:	IVisualControl*
	//修饰符:	const
	//功能概要:	取得桌面对象组中顶端桌面对象的句柄。
	//备注:		
	//********************************
	IVisualControl*
	GetTopDesktopObjectPtr() const;
	//********************************
	//名称:		GetTopDesktopObjectPtr
	//全名:		YSLib::Components::YDesktop::GetTopDesktopObjectPtr
	//可访问性:	public 
	//返回类型:	IVisualControl*
	//修饰符:	const
	//形式参数:	const Point &
	//功能概要:	取得桌面对象组中包含指定点的顶端桌面对象的句柄。
	//备注:		
	//********************************
	IVisualControl*
	GetTopDesktopObjectPtr(const Point&) const;

	//********************************
	//名称:		DrawBackground
	//全名:		YSLib::Components::YDesktop::DrawBackground
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制背景。
	//备注:		
	//********************************
	virtual void
	DrawBackground();

protected:
	//依次绘制桌面对象组对象。
	//********************************
	//名称:		DrawDesktopObjects
	//全名:		YSLib::Components::YDesktop::DrawDesktopObjects
	//可访问性:	protected 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘制桌面对象。
	//备注:		
	//********************************
	void
	DrawDesktopObjects();

public:
	//********************************
	//名称:		Draw
	//全名:		YSLib::Components::YDesktop::Draw
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	绘图。
	//备注:		
	//********************************
	virtual void
	Draw();

	//********************************
	//名称:		Refresh
	//全名:		YSLib::Components::YDesktop::Refresh
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	刷新至窗口缓冲区。
	//备注:		
	//********************************
	virtual void
	Refresh();

	//********************************
	//名称:		Update
	//全名:		YSLib::Components::YDesktop::Update
	//可访问性:	virtual public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	更新缓冲区至屏幕。
	//备注:		
	//********************************
	virtual void
	Update();
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

