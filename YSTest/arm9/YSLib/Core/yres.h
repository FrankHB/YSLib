// YSLib::Core::YResource by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-28 16:46:40 + 08:00;
// UTime = 2010-10-28 13:58 + 08:00;
// Version = 0.1394;


#ifndef INCLUDED_YRES_H_
#define INCLUDED_YRES_H_

// YResource ：资源管理模块。

#include "yobject.h"
#include "../Shell/ygdi.h"

YSL_BEGIN

//********************************
//名称:		GetGlobalResource
//全名:		YSLib<T>::GetGlobalResource
//可访问性:	public 
//返回类型:	std::&
//修饰符:	
//功能概要:	全局默认共享资源生成函数。
//备注:		线程空间内共享；全局资源初始化之后可调用。
//********************************
template<class T>
GHResource<T>&
GetGlobalResource()
{
	static GHResource<T> p(new T);

	return p;
}

//资源定义。

YSL_BEGIN_NAMESPACE(Drawing)

//图像资源。
class YImage
	: public GMCounter<YImage>, public YCountableObject, public BitmapBuffer
{
public:
	typedef YCountableObject ParentType;

	//********************************
	//名称:		YImage
	//全名:		YSLib::Drawing::YImage::YImage
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	ConstBitmapPtr
	//形式参数:	SDST
	//形式参数:	SDST
	//功能概要:	构造：使用指定位图指针、长和宽。
	//备注:		
	//********************************
	explicit
	YImage(ConstBitmapPtr = NULL, SDST = 0, SDST = 0);

	DefGetter(BitmapPtr, ImagePtr, GetBufferPtr())

	void
	SetImage(ConstBitmapPtr, SDST = SCRW, SDST = SCRH);
};

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

