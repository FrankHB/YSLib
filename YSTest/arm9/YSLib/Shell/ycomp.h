// YSLib::Shell::YComponent by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-19 20:05:08;
// UTime = 2010-9-18 10:41;
// Version = 0.2276;


#ifndef INCLUDED_YCOMPONENT_H_
#define INCLUDED_YCOMPONENT_H_

// YComponent ：平台无关的图形用户界面组件实现。

#include "../Core/yapp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

//基本组件接口。
DeclInterface(IComponent)
EndDecl


//基本组件。
class YComponent : public GMCounter<YComponent>, public YCountableObject,
	implements IComponent
{
public:
	typedef YCountableObject ParentType;
};


//控制台。
class YConsole : public YComponent
{
public:
	YScreen& Screen;

	explicit
	YConsole(YScreen& = *pDefaultScreen);
	virtual
	~YConsole();

	void
	Activate(Drawing::PixelType = Drawing::Color::White, Drawing::PixelType = Drawing::Color::Black);

	void
	Deactivate();
};

inline
YConsole::YConsole(YScreen& scr)
: Screen(scr)
{}
inline
YConsole::~YConsole()
{
	Deactivate();
}

inline void
YConsole::Activate(Drawing::PixelType fc, Drawing::PixelType bc)
{
	Def::InitConsole(Screen, fc, bc);
}

inline void
YConsole::Deactivate()
{
	Def::InitVideo();
}

YSL_END_NAMESPACE(Components)

YSL_END

#endif

