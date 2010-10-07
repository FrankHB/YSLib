// YSLib::Shell::YComponent by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-03-19 20:05:08 + 08:00;
// UTime = 2010-10-06 14:00 + 08:00;
// Version = 0.2304;


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
	YConsole(YScreen& = *pDefaultScreen, bool = true, Drawing::Color = Drawing::Color::White, Drawing::Color = Drawing::Color::Black);
	virtual
	~YConsole();

	void
	Activate(Drawing::Color = Drawing::Color::White, Drawing::Color = Drawing::Color::Black);

	void
	Deactivate();
};

inline
YConsole::YConsole(YScreen& scr, bool a, Drawing::Color fc, Drawing::Color bc)
	: YComponent(),
	Screen(scr)
{
	if(a)
		Activate(fc, bc);
}
inline
YConsole::~YConsole()
{
	Deactivate();
}

inline void
YConsole::Activate(Drawing::Color fc, Drawing::Color bc)
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

