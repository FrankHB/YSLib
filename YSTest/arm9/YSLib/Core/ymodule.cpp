// YSLib::Core::YModule by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-5-1 13:52:56;
// UTime = 2010-6-11 15:26;
// Version = 0.1202;


#include "ymodule.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Runtime)

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

YSL_END_NAMESPACE(Runtime)

YSL_END

