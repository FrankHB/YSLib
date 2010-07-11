// YSLib::Shell::YComponent by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-3-19 20:05:08;
// UTime = 2010-5-19 21:40;
// Version = 0.2188;


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

YSL_END_NAMESPACE(Components)

YSL_END

#endif

