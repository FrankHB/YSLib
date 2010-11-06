// YSLib::Shell::YFocus by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-05-01 13:52:56 + 08:00;
// UTime = 2010-11-04 11:45 + 08:00;
// Version = 0.1236;


#include "yfocus.h"

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
	return !(bFocused && IsFocusOfContainer(c))
		&& (bFocused = c.SetFocusingPtr(this));
}

bool
AFocusRequester::ReleaseFocus(GMFocusResponser<AFocusRequester>& c)
{
	return bFocused && IsFocusOfContainer(c)
		&& (bFocused = NULL, !(c.ClearFocusingPtr()));
}

YSL_END_NAMESPACE(Components)

YSL_END

