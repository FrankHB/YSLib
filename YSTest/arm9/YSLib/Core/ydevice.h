// YSLib::Core::YDevice by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-28 16:39:39 + 08:00;
// UTime = 2010-10-24 17:12 + 08:00;
// Version = 0.2734;


#ifndef INCLUDED_YOUTPUT_H_
#define INCLUDED_YOUTPUT_H_

// YDevice ：平台无关的设备抽象层。

#include "../Helper/yglobal.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Device)

//图形设备。
class YGraphicDevice : public YObject, protected Drawing::Size
{
public:
	typedef YObject ParentType;

private:
	Drawing::BitmapPtr ptr;

public:
	//********************************
	//名称:		YGraphicDevice
	//全名:		YSLib::Device::YGraphicDevice::YGraphicDevice
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	SDST
	//形式参数:	SDST
	//形式参数:	Drawing::BitmapPtr
	//功能概要:	构造：指定宽度和高度，从指定缓冲区指针。
	//备注:		
	//********************************
	YGraphicDevice(SDST, SDST, Drawing::BitmapPtr = NULL);

	DefGetter(const Size&, Size, *this)
	DefGetterBase(u32, Area, Size)
	virtual DefGetter(Drawing::BitmapPtr, Ptr, ptr)

	virtual DefSetter(Drawing::BitmapPtr, Ptr, ptr)
};

inline
YGraphicDevice::YGraphicDevice(SDST w, SDST h, Drawing::BitmapPtr p)
	: YObject(), Size(w, h),
	ptr(p)
{}


//屏幕。
class YScreen : public YGraphicDevice
{
	friend class YSLib::Def;

public:
	typedef YGraphicDevice ParentType;
	typedef int BGType;

private:
	static bool S_InitScr;

	//********************************
	//名称:		InitScreen
	//全名:		YSLib::Device::YScreen::InitScreen
	//可访问性:	private static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	静态初始化。
	//备注:		
	//********************************
	static void InitScreen();
	//********************************
	//名称:		CheckInit
	//全名:		YSLib::Device::YScreen::CheckInit
	//可访问性:	private static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	状态检查。
	//备注:		
	//********************************
	static void CheckInit();

	BGType bg;

public:
	//********************************
	//名称:		YScreen
	//全名:		YSLib::Device::YScreen::YScreen
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	SDST
	//形式参数:	SDST
	//形式参数:	Drawing::BitmapPtr
	//功能概要:	构造：指定宽度和高度，从指定缓冲区指针。
	//备注:		
	//********************************
	YScreen(SDST, SDST, Drawing::BitmapPtr = NULL);

	//********************************
	//名称:		Reset
	//全名:		YSLib::Device::YScreen::Reset
	//可访问性:	public static 
	//返回类型:	void
	//修饰符:	
	//功能概要:	复位。
	//备注:		无条件初始化。
	//********************************
	static void
	Reset();

	//********************************
	//名称:		GetPtr
	//全名:		YSLib::Device::YScreen::GetPtr
	//可访问性:	virtual public 
	//返回类型:	Drawing::BitmapPtr
	//修饰符:	const ythrow()
	//功能概要:	取指针。
	//备注:		无异常抛出。
	//			进行状态检查。
	//********************************
	virtual Drawing::BitmapPtr
	GetPtr() const ythrow();
	DefGetter(const BGType&, BgID, bg)

	DefSetter(const BGType&, BgID, bg)

	//********************************
	//名称:		Update
	//全名:		YSLib::Device::YScreen::Update
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	Drawing::BitmapPtr
	//功能概要:	更新。
	//备注:		复制到屏幕。
	//********************************
	void
	Update(Drawing::BitmapPtr);
	//********************************
	//名称:		Update
	//全名:		YSLib::Device::YScreen::Update
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	Drawing::Color
	//功能概要:	更新。
	//备注:		以纯色填充屏幕。
	//********************************
	void
	Update(Drawing::Color = 0);
};

inline void
YScreen::InitScreen()
{
	S_InitScr = !Def::InitVideo();
}

inline Drawing::BitmapPtr
YScreen::GetPtr() const ythrow()
{
	CheckInit();
	return ParentType::GetPtr();
}

YSL_END_NAMESPACE(Device)

YSL_END

#endif

