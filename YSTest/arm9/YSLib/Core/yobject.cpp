// YSLib::Core::YObject by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-16 20:06:58;
// UTime = 2010-6-26 6:17;
// Version = 0.1408;


#include "yobject.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

const SPoint SPoint::Zero = SPoint();
const SPoint SPoint::FullScreen = SPoint(SCRW, SCRH);

const SVec SVec::Zero = SVec();
const SVec SVec::FullScreen = SVec(SCRW, SCRH);

const SSize SSize::Zero = SSize();
const SSize SSize::FullScreen = SSize(SCRW, SCRH);

const SRect SRect::Empty = SRect();
const SRect SRect::FullScreen = SRect(SPoint::Zero, SCRW, SCRH);

YSL_END_NAMESPACE(Drawing)

YSL_END

