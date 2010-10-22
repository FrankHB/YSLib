// YSLib::Shell::YComponent by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-03-19 20:05:18 + 08:00;
// UTime = 2010-10-22 13:27 + 08:00;
// Version = 0.1828;


#include "ycomp.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

bool
AFocusRequester::CheckRemoval(GMFocusResponser<AFocusRequester>& c) const
{
	if(IsFocusOfContainer(c))
	{
		c.ClearFocusingPtr();
		return true;
	}
	return false;
}

bool
AFocusRequester::RequestFocus(GMFocusResponser<AFocusRequester>& c)
{
	return !(bFocused && IsFocusOfContainer(c)) && (bFocused = c.SetFocusingPtr(this));
}

bool
AFocusRequester::ReleaseFocus(GMFocusResponser<AFocusRequester>& c)
{
	return bFocused && IsFocusOfContainer(c) && (bFocused = NULL, !(c.ClearFocusingPtr()));
}

YSL_BEGIN_NAMESPACE(Widgets)


YSL_END_NAMESPACE(Widgets)

YSL_END_NAMESPACE(Components)

YSL_END

