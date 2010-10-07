// YSLib::Core::YObject by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58 + 08:00;
// UTime = 2010-10-05 18:02 + 08:00;
// Version = 0.1416;


#include "yobject.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

const Point Point::Zero = Point();
const Point Point::FullScreen = Point(SCRW, SCRH);

const Vec Vec::Zero = Vec();
const Vec Vec::FullScreen = Vec(SCRW, SCRH);

const Size Size::Zero = Size();
const Size Size::FullScreen = Size(SCRW, SCRH);

const Rect Rect::Empty = Rect();
const Rect Rect::FullScreen = Rect(Point::Zero, SCRW, SCRH);

YSL_END_NAMESPACE(Drawing)

YSL_END

