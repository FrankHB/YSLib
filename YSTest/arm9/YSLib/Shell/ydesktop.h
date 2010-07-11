// YSLib::Shell::YDesktop by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-5-2 12:00:08;
// UTime = 2010-7-4 3:10;
// Version = 0.1970;


#ifndef INCLUDED_YDESKTOP_H_
#define INCLUDED_YDESKTOP_H_

// YDesktop：平台无关的桌面抽象层。

#include "ywindow.h"
#include <list>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Device)

//桌面。
class YDesktop : public YFrameWindow
{
	friend class YSLib::Def;

public:
	typedef YComponent ParentType;
	typedef std::list<IVisualControl*> DOs; //桌面对象组类型。

private:
	YScreen& Screen; //屏幕对象。
	DOs sDOs; //桌面对象组（末尾表示顶端）。

public:
	Drawing::PixelType BackColor; //背景色。

	explicit
	YDesktop(YScreen&, Drawing::PixelType = 0, HResource<Drawing::YImage> = NULL);
	virtual
	~YDesktop();

	DefGetter(const YScreen&, Screen, Screen) //取屏幕对象。
	DefGetter(Drawing::BitmapPtr, BackgroundPtr, Screen.GetPtr()) //取背景指针。

	virtual PDefHead(IVisualControl*, GetTopVisualControlPtr, const SPoint& p) const
		ImplBodyBase(YDesktop, GetTopDesktopObjectPtr, p)

	virtual void
	operator+=(IVisualControl&); //向桌面对象组添加桌面对象。
	virtual bool
	operator-=(IVisualControl&); //从桌面对象组中移除指定桌面对象。
	DOs::size_type
	RemoveAll(IVisualControl&); //从桌面对象组中移除所有指定桌面对象，返回移除的对象数。
	bool
	MoveToTop(IVisualControl&); //从桌面对象组中查找指定桌面对象对象并重新插入至顶端。
	void
	RemoveTopDesktopObject(); //移除桌面对象组中首个桌面对象。
	void
	ClearDesktopObjects(); //清除桌面对象组。
	IVisualControl*
	GetFirstDesktopObjectPtr() const; //取得桌面对象组中首个桌面对象的句柄。
	IVisualControl*
	GetTopDesktopObjectPtr() const; //取得桌面对象组中顶端桌面对象的句柄。
	IVisualControl*
	GetTopDesktopObjectPtr(const SPoint&) const; //取得桌面对象组中包含指定点的顶端桌面对象的句柄。

	virtual void
	DrawBackground();
	void
	DrawDesktopObjects(); //依次绘制桌面对象组对象。
	virtual void
	Draw();

	virtual void
	Refresh();
	virtual void
	Update(); //更新缓冲区内容至屏幕。
};

YSL_END_NAMESPACE(Device)

YSL_END

#endif

