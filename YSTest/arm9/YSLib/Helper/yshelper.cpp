// YSLib::Helper -> YShellHelper by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-4-4 13:42:15;
// UTime = 2010-6-22 3:06;
// Version = 0.1173;


#include "yshelper.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

YImage*
NewScrImage(PPDRAW f, BitmapPtr gbuf)
{
	bool s(!gbuf);

	if(s)
		gbuf = new SCRBUF;
	ScrDraw(gbuf, f);

	YImage* pi(NewScrImage(gbuf));

	if(s)
	{
		delete gbuf;
		gbuf = NULL;
	}
	return pi;
}

YSL_END_NAMESPACE(Drawing)

YSL_END;

