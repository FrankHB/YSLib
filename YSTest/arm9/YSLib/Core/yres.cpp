// YSLib::Core::YResource by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-22 17:28:28 + 08:00;
// UTime = 2010-10-28 13:58 + 08:00;
// Version = 0.1194;


#include "yres.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

YImage::YImage(ConstBitmapPtr s, SDST w, SDST h) : BitmapBuffer(s, w, h)
{}

void
YImage::SetImage(ConstBitmapPtr s, SDST w, SDST h)
{
	SetSize(w, h);
	if(img && s)
		std::memcpy(img, s, GetSizeOfBuffer());
}

YSL_END_NAMESPACE(Drawing)

YSL_END

