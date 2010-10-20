// YSLib::Helper -> YShellHelper by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2009-3-14 14:07:22 + 08:00;
// UTime = 2010-10-18 11:44 + 08:00;
// Version = 0.1626;


#ifndef INCLUDED_YSHELPER_H_
#define INCLUDED_YSHELPER_H_

// YShellHelper ：全局帮助模块。

#include "../ysbuild.h"

YSL_BEGIN

//定义并使用 dynamic_cast 初始化引用。
#define DefDynInitRef(_type, _name, _expr) _type& _name(dynamic_cast<_type&>(_expr));

//句柄语法糖。

template<class _handle>
inline void
ReplaceHandle(_handle& h, _handle hWnd)
{
	YDelete(h);
	h = hWnd;
}

template<class _type, class _handle>
inline _type*
HandleCast(_handle h)
{
	return dynamic_cast<_type*>(GetPointer(h));
}

template<class _type, class _handle>
inline _type&
HandleToReference(_handle h) ythrow(std::bad_cast)
{
	_type* _tmp(dynamic_cast<_type*>(GetPointer(h)));

	if(!_tmp)
		throw std::bad_cast();
	return *_tmp;
}

template<class _type>
HWND NewWindow(HSHL hShl)
{
	return HWND(new _type(HSHL(hShl)));
}


//全局函数。

//取当前线程空间中运行的 Shell 句柄。
inline HSHL
NowShell()
{
	return theApp.GetShellHandle();
}

//判断 Shell 句柄是否为当前线程空间中运行的 Shell 句柄。
inline bool
IsNowShell(HSHL hShl)
{
	return theApp.GetShellHandle() == hShl;
}

//添加 Shell 对象。
inline HSHL
NowShellAdd(HSHL hShl)
{
	if(hShl)
		theApp += *hShl;
	return hShl;
}
inline HSHL
NowShellAdd(YShell& shl)
{
	theApp += shl;
	return HSHL(&shl);
}

//向指定 Shell 对象转移线程控制权。
inline ERRNO
NowShellTo(HSHL hShl)
{
	return -!hShl->Activate();
}

//向新建 Shell 对象转移控制权。
template<class _tShl>
inline ERRNO
NowShellToNew()
{
	return NowShellTo(new _tShl());
}

//向全局 Shell 管理器的对象转移控制权。
template<class _tShl>
inline ERRNO
NowShellToStored()
{
	return NowShellTo(GStaticCache<_tShl>::GetPointer());
}

//通过消息队列向指定 Shell 对象转移控制权。
inline void
SetShellTo(HSHL hShl, Shells::MSGPRIORITY p = 0x80)
{
	InsertMessage(NULL, SM_SET, p, handle_cast<WPARAM>(hShl));
}

//通过消息队列向新建 Shell 对象转移控制权。
template<class _tShl>
inline void
SetShellToNew()
{
	SetShellTo(new _tShl());
}

//通过消息队列向全局 Shell 管理器的对象转移控制权。
template<class _tShl>
inline void
SetShellToStored()
{
	SetShellTo(GStaticCache<_tShl>::GetPointer());
}

//封装向全局 Shell 管理器的对象转移控制权。
template<class _tShl>
inline void
CallStoredAtOnce()
{
	NowShellToStored<_tShl>();
}

//封装通过消息队列向全局 Shell 管理器的对象转移控制权。
template<class _tShl>
inline void
CallStored()
{
	SetShellToStored<_tShl>();
}


//资源相关定义和函数。

YSL_BEGIN_NAMESPACE(Drawing)

typedef void (*PPDRAW)(BitmapPtr, SDST, SDST);

//全屏幕描点（颜色由坐标决定）。
inline void
ScrDraw(BitmapPtr buf, PPDRAW f)
{
	for(SDST y = 0; y < SCRH; ++y)
		for(SDST x = 0; x < SCRW; ++x)
			f(buf, x, y);
}

inline YImage*
NewScrImage(ConstBitmapPtr p)
{
	return new YImage(p, SCRW, SCRH);
}

YImage*
NewScrImage(PPDRAW f, BitmapPtr gbuf = NULL);

//使用 new 分配空间并复制无压缩位图。
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

