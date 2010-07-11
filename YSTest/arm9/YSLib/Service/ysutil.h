// YSLib::Service::YSystemUtilities by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:07:20;
// UTime = 2010-5-19 21:40;
// Version = 0.2323;


#ifndef INCLUDED_YSUTIL_H_
#define INCLUDED_YSUTIL_H_

// YSystemUtilities ：系统实用模块。

#include "../Core/ysdef.h"
#include "../Adapter/yfont.h"

YSL_BEGIN

//默认字体缓存。
extern YFontCache*& pDefaultFontCache;

inline void
DestroyDefFontCache()
{
	Drawing::DestroyFontCache(pDefaultFontCache);
	Drawing::YFont::ReleaseDefault();
}
inline void
CreateDefFontCache(CPATH p)
{
	Drawing::CreateFontCache(pDefaultFontCache, p);
}

YSL_END

#endif

