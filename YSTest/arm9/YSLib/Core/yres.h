// YSLib::Core::YResource by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-28 16:46:40;
// UTime = 2010-7-26 6:27;
// Version = 0.1314;


#ifndef INCLUDED_YRES_H_
#define INCLUDED_YRES_H_

// YResource ：资源管理模块。

#include "yobject.h"
#include "../Shell/ygdi.h"

YSL_BEGIN

//全局默认共享资源生成函数（线程空间内共享；全局资源初始化之后可调用）。
template<class T>
GHResource<T>&
GetGlobalResource()
{
	static GHResource<T> p;

	if(p == NULL)
		p = new T;
	return p;
}

//资源定义。

YSL_BEGIN_NAMESPACE(Drawing)

class YImage : public GMCounter<YImage>, public YCountableObject, public MBitmapBuffer //图像资源。
{
public:
	typedef YCountableObject ParentType;

	explicit
	YImage(ConstBitmapPtr = NULL, SDST = 0, SDST = 0);

	BitmapPtr
	GetImagePtr() const;

	void
	SetImage(ConstBitmapPtr, SDST = SCRW, SDST = SCRH);
};

inline BitmapPtr
YImage::GetImagePtr() const
{
	return GetBufferPtr();
}

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

