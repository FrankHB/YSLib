// YSLib::Helper -> YShellHelper by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2009-03-14 14:07:22 + 08:00;
// UTime = 2010-10-24 17:59 + 08:00;
// Version = 0.1844;


#ifndef INCLUDED_YSHELPER_H_
#define INCLUDED_YSHELPER_H_

// YShellHelper ：全局帮助模块。

#include "../ysbuild.h"

YSL_BEGIN

//定义并使用 dynamic_cast 初始化引用。
#define DefDynInitRef(_type, _name, _expr) \
	_type& _name(dynamic_cast<_type&>(_expr));

//句柄语法糖。

//********************************
//名称:		ReplaceHandle
//全名:		YSLib<_handle>::ReplaceHandle
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	_handle & h
//形式参数:	_handle hWnd
//功能概要:	句柄赋值。
//备注:		
//********************************
template<class _handle>
inline void
ReplaceHandle(_handle& h, _handle hWnd)
{
	YDelete(h);
	h = hWnd;
}

//********************************
//名称:		HandleCast
//全名:		YSLib<_type, _handle>::HandleCast
//可访问性:	public 
//返回类型:	_type*
//修饰符:	
//形式参数:	_handle h
//功能概要:	句柄转换：内建指针。
//备注:		
//********************************
template<class _type, class _handle>
inline _type*
HandleCast(_handle h)
{
	return dynamic_cast<_type*>(GetPointer(h));
}

//********************************
//名称:		HandleToReference
//全名:		YSLib<_type, _handle>::HandleToReference
//可访问性:	public 
//返回类型:	_type&
//修饰符:	ythrow(std::bad_cast)
//形式参数:	_handle h
//功能概要:	句柄转换：对象引用。
//备注:		
//********************************
template<class _type, class _handle>
inline _type&
HandleToReference(_handle h) ythrow(std::bad_cast)
{
	_type* _tmp(dynamic_cast<_type*>(GetPointer(h)));

	if(!_tmp)
		throw std::bad_cast();
	return *_tmp;
}

//********************************
//名称:		NewWindow
//全名:		YSLib<_type>::NewWindow
//可访问性:	public 
//返回类型:	YSLib::HWND
//修饰符:	
//形式参数:	HSHL hShl
//功能概要:	从指定 Shell 新建窗体。
//备注:		
//********************************
template<class _type>
HWND NewWindow(HSHL hShl)
{
	return HWND(new _type(HSHL(hShl)));
}


//全局函数。

//********************************
//名称:		NowShell
//全名:		YSLib::NowShell
//可访问性:	public 
//返回类型:	YSLib::HSHL
//修饰符:	
//功能概要:	取当前线程空间中运行的 Shell 句柄。
//备注:		
//********************************
inline HSHL
NowShell()
{
	return theApp.GetShellHandle();
}

//********************************
//名称:		IsNowShell
//全名:		YSLib::IsNowShell
//可访问性:	public 
//返回类型:	bool
//修饰符:	
//形式参数:	HSHL hShl
//功能概要:	判断 Shell 句柄是否为当前线程空间中运行的 Shell 句柄。
//备注:		
//********************************
inline bool
IsNowShell(HSHL hShl)
{
	return theApp.GetShellHandle() == hShl;
}

//********************************
//名称:		NowShellAdd
//全名:		YSLib::NowShellAdd
//可访问性:	public 
//返回类型:	YSLib::HSHL
//修饰符:	
//形式参数:	HSHL hShl
//功能概要:	添加 Shell 对象。
//备注:		
//********************************
inline HSHL
NowShellAdd(HSHL hShl)
{
	if(hShl)
		theApp += *hShl;
	return hShl;
}
//********************************
//名称:		NowShellAdd
//全名:		YSLib::NowShellAdd
//可访问性:	public 
//返回类型:	YSLib::HSHL
//修饰符:	
//形式参数:	YShell & shl
//功能概要:	添加 Shell 对象。
//备注:		
//********************************
inline HSHL
NowShellAdd(YShell& shl)
{
	theApp += shl;
	return HSHL(&shl);
}

//********************************
//名称:		NowShellTo
//全名:		YSLib::NowShellTo
//可访问性:	public 
//返回类型:	ERRNO
//修饰符:	
//形式参数:	HSHL hShl
//功能概要:	向指定 Shell 对象转移线程控制权。
//备注:		
//********************************
inline ERRNO
NowShellTo(HSHL hShl)
{
	return -!hShl->Activate();
}

//********************************
//名称:		NowShellToNew
//全名:		YSLib<_tShl>::NowShellToNew
//可访问性:	public 
//返回类型:	ERRNO
//修饰符:	
//功能概要:	向新建 Shell 对象转移控制权。
//备注:		
//********************************
template<class _tShl>
inline ERRNO
NowShellToNew()
{
	return NowShellTo(new _tShl());
}

//********************************
//名称:		NowShellToStored
//全名:		YSLib<_tShl>::NowShellToStored
//可访问性:	public 
//返回类型:	ERRNO
//修饰符:	
//功能概要:	向全局 Shell 管理器的对象转移控制权。
//备注:		
//********************************
template<class _tShl>
inline ERRNO
NowShellToStored()
{
	return NowShellTo(GStaticCache<_tShl>::GetPointer());
}

//********************************
//名称:		SetShellTo
//全名:		YSLib::SetShellTo
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	HSHL hShl
//形式参数:	Shells::MSGPRIORITY p
//功能概要:	通过主消息队列向指定 Shell 对象转移控制权。
//备注:		
//********************************
inline void
SetShellTo(HSHL hShl, Shells::MSGPRIORITY p = 0x80)
{
	InsertMessage(NULL, SM_SET, p, handle_cast<WPARAM>(hShl));
}

//********************************
//名称:		SetShellToNew
//全名:		YSLib<_tShl>::SetShellToNew
//可访问性:	public 
//返回类型:	void
//修饰符:	
//功能概要:	通过主消息队列向新建 Shell 对象转移控制权。
//备注:		
//********************************
template<class _tShl>
inline void
SetShellToNew()
{
	SetShellTo(new _tShl());
}

//********************************
//名称:		SetShellToStored
//全名:		YSLib<_tShl>::SetShellToStored
//可访问性:	public 
//返回类型:	void
//修饰符:	
//功能概要:	通过主消息队列向全局 Shell 管理器内的对象转移控制权。
//备注:		
//********************************
template<class _tShl>
inline void
SetShellToStored()
{
	SetShellTo(GStaticCache<_tShl>::GetPointer());
}

//********************************
//名称:		CallStoredAtOnce
//全名:		YSLib<_tShl>::CallStoredAtOnce
//可访问性:	public 
//返回类型:	void
//修饰符:	
//功能概要:	封装向全局 Shell 管理器内的对象转移控制权。
//备注:		
//********************************
template<class _tShl>
inline void
CallStoredAtOnce()
{
	NowShellToStored<_tShl>();
}

//********************************
//名称:		CallStored
//全名:		YSLib<_tShl>::CallStored
//可访问性:	public 
//返回类型:	void
//修饰符:	
//功能概要:	封装通过消息队列向全局 Shell 管理器内的对象转移控制权。
//备注:		
//********************************
template<class _tShl>
inline void
CallStored()
{
	SetShellToStored<_tShl>();
}


//资源相关定义和函数。

YSL_BEGIN_NAMESPACE(Drawing)

typedef void (*PPDRAW)(BitmapPtr, SDST, SDST); //简单屏幕绘图函数指针类型。

//********************************
//名称:		ScrDraw
//全名:		YSLib::Drawing::ScrDraw
//可访问性:	public 
//返回类型:	void
//修饰符:	
//形式参数:	BitmapPtr buf
//形式参数:	PPDRAW f
//功能概要:	全屏幕描点。
//备注:		颜色由坐标决定。
//********************************
inline void
ScrDraw(BitmapPtr buf, PPDRAW f)
{
	for(SDST y = 0; y < SCRH; ++y)
		for(SDST x = 0; x < SCRW; ++x)
			f(buf, x, y);
}

//********************************
//名称:		NewScrImage
//全名:		YSLib::Drawing::NewScrImage
//可访问性:	public 
//返回类型:	YImage*
//修饰符:	
//形式参数:	ConstBitmapPtr p
//功能概要:	新建屏幕图像。
//备注:		
//********************************
inline YImage*
NewScrImage(ConstBitmapPtr p)
{
	return new YImage(p, SCRW, SCRH);
}

//********************************
//名称:		NewScrImage
//全名:		YSLib::Drawing::NewScrImage
//可访问性:	public 
//返回类型:	YImage*
//修饰符:	
//形式参数:	PPDRAW f
//形式参数:	BitmapPtr gbuf
//功能概要:	新建屏幕图像并指定绘图函数填充。
//备注:		
//********************************
YImage*
NewScrImage(PPDRAW f, BitmapPtr gbuf = NULL);

//********************************
//名称:		NewBitmapRaw
//全名:		YSLib::Drawing<_tPixel>::NewBitmapRaw
//可访问性:	public 
//返回类型:	_tPixel*
//修饰符:	
//形式参数:	const _tPixel * s
//形式参数:	std::size_t n
//功能概要:	使用 new 分配空间并复制无压缩位图。
//备注:		
//********************************
template<typename _tPixel>
_tPixel*
NewBitmapRaw(const _tPixel* s, std::size_t n)
{
	if(s && n)
	{
		std::size_t size(sizeof(_tPixel) * n);
		_tPixel* d(new _tPixel[size]);

		if(d != NULL)
			std::memcpy(d, s, size);
		return d;
	}
	return NULL;
}

YSL_END_NAMESPACE(Drawing)

YSL_END;

#endif

