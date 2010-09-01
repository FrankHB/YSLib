// YSLib::Core::YResource by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-22 17:28:28;
// UTime = 2010-9-1 14:41;
// Version = 0.1186;


#include "yres.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

YImage::YImage(ConstBitmapPtr s, SDST w, SDST h) : MBitmapBuffer(s, w, h)
{}

void
YImage::SetImage(ConstBitmapPtr s, SDST w, SDST h)
{
	SetSize(w, h);
	if(img != NULL && s != NULL)
		std::memcpy(img, s, GetSizeOfBuffer());
}

YSL_END_NAMESPACE(Drawing)

YSL_END

