// YSLib::Core::YDevice by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-28 16:39:39;
// UTime = 2010-8-2 15:42;
// Version = 0.2688;


#ifndef INCLUDED_YOUTPUT_H_
#define INCLUDED_YOUTPUT_H_

// YDevice：平台无关的设备抽象层。

#include "../Helper/yglobal.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Device)

//图形设备。
class YGraphicDevice : public YObject, protected Drawing::SSize
{
public:
	typedef YObject ParentType;

private:
	Drawing::BitmapPtr ptr;

public:
	YGraphicDevice(SDST, SDST, Drawing::BitmapPtr = NULL);

	DefGetter(const SSize&, Size, *this)
	DefGetterBase(u32, Area, SSize)
	virtual DefGetter(Drawing::BitmapPtr, Ptr, ptr)

	virtual DefSetter(Drawing::BitmapPtr, Ptr, ptr)
};

inline
YGraphicDevice::YGraphicDevice(SDST w, SDST h, Drawing::BitmapPtr p)
: SSize(w, h), ptr(p)
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

	static void InitScreen();
	static void CheckInit();

	BGType bg;

public:
	YScreen(SDST, SDST, Drawing::BitmapPtr = NULL);

	static void
	Reset();

	virtual Drawing::BitmapPtr
	GetPtr() const ythrow();
	DefGetter(const BGType&, BgID, bg)

	DefSetter(const BGType&, BgID, bg)

	void
	Update(Drawing::BitmapPtr);
	void
	Update(Drawing::PixelType = 0); //以纯色填充屏幕。
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

