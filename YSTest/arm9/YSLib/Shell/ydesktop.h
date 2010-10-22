// YSLib::Shell::YDesktop by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-05-02 12:00:08 + 08:00;
// UTime = 2010-10-22 13:36 + 08:00;
// Version = 0.2038;


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
	explicit
	YDesktop(YScreen&, Color = 0, GHResource<Drawing::YImage> = NULL);
	virtual DefEmptyDtor(YDesktop)

	DefGetter(const YScreen&, Screen, Screen) //取屏幕对象。
	DefGetter(BitmapPtr, BackgroundPtr, Screen.GetPtr()) //取背景指针。

	virtual PDefH(IVisualControl*, GetTopVisualControlPtr, const Point& p) const
		ImplBodyBase(YDesktop, GetTopDesktopObjectPtr, p)

	virtual void
	operator+=(IVisualControl&); //向桌面对象组添加桌面对象。
	virtual bool
	operator-=(IVisualControl&); //从桌面对象组中移除指定桌面对象。
	DOs::size_type
	RemoveAll(IVisualControl&); //从桌面对象组中移除所有指定桌面对象，返回移除的对象数。

	//从桌面对象组中查找指定桌面对象对象并重新插入至顶端。
	bool
	MoveToTop(IVisualControl&);

	//移除桌面对象组中首个桌面对象。
	void
	RemoveTopDesktopObject();

	//清除桌面对象组。
	void
	ClearDesktopObjects();

	IVisualControl*
	GetFirstDesktopObjectPtr() const; //取得桌面对象组中首个桌面对象的句柄。
	IVisualControl*
	GetTopDesktopObjectPtr() const; //取得桌面对象组中顶端桌面对象的句柄。
	IVisualControl*
	GetTopDesktopObjectPtr(const Point&) const; //取得桌面对象组中包含指定点的顶端桌面对象的句柄。

	virtual void
	DrawBackground();

	//依次绘制桌面对象组对象。
	void
	DrawDesktopObjects();

	virtual void
	Draw();

	virtual void
	Refresh();

	//更新缓冲区内容至屏幕。
	virtual void
	Update();
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

