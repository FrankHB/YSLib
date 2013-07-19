/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Win32GUI.h
\ingroup YCLib
\ingroup MinGW32
\brief Win32 GUI 接口。
\version r332
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 11:29:04 +0800
\par 修改时间:
	2013-07-18 21:06 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::Win32GUI
*/


#ifndef YCL_MinGW32_INC_Win32GUI_h_
#define YCL_MinGW32_INC_Win32GUI_h_ 1

#include "MinGW32.h"
#include <YSLib/Core/ygdibase.h>
#include <mutex> // for std::mutex;
#include <atomic>

namespace platform_ex
{

//! \since build 389
typedef ::HWND NativeWindowHandle;

inline namespace Windows
{

/*!
\brief 本机窗口引用。
\note 不具有所有权。
\since build 427
*/
class YF_API WindowReference
{
protected:
	NativeWindowHandle hWindow;

public:
	WindowReference(NativeWindowHandle h = {})
		: hWindow(h)
	{}
	DefDeMoveCtor(WindowReference)

	YSLib::Drawing::Point
	GetLocation() const;
	DefGetter(const ynothrow, NativeWindowHandle, NativeHandle, hWindow)
	/*!
	\brief 取不透明度。
	\pre 之前必须在此窗口上调用过 SetOpacity 或 ::SetLayeredWindowAttributes 。
	\note 当窗口未启用 WS_EX_LAYERED 样式时无效。
	\since build 430
	*/
	YSLib::Drawing::AlphaType
	GetOpacity() const;
	YSLib::Drawing::Size
	GetSize() const;

	/*!
	\brief 设置不透明度。
	\note 当窗口未启用 WS_EX_LAYERED 样式时无效。
	\since build 430
	*/
	void
	SetOpacity(YSLib::Drawing::AlphaType);
	/*!
	\brief 设置标题栏文字。
	\since build 428
	*/
	void
	SetText(const wchar_t*);

	//! \note 线程安全。
	void
	Close();

	/*!
	\brief 无效化窗口客户区。
	\since build 429
	*/
	void
	Invalidate();

	/*!
	\brief 移动窗口。
	\note 线程安全。
	*/
	void
	Move(const YSLib::Drawing::Point&);

	/*!
	\brief 调整窗口大小。
	\note 线程安全。
	*/
	void
	Resize(const YSLib::Drawing::Size&);

	/*!
	\brief 按客户区调整窗口大小。
	\note 线程安全。
	*/
	void
	ResizeClient(const YSLib::Drawing::Size&);

	/*!
	\brief 显示窗口。
	\return 异步操作是否成功。
	*/
	bool
	Show() ynothrow;
};


/*!
\brief 按指定窗口类名、客户区大小、标题文本、样式和附加样式创建本机顶层窗口。
\since build 430
*/
YF_API NativeWindowHandle
CreateNativeWindow(const wchar_t*, const YSLib::Drawing::Size&,
	const wchar_t* = L"", ::DWORD = WS_POPUP, ::DWORD = WS_EX_LTRREADING);


/*!
\brief 虚拟屏幕缓存。
\note 像素格式和 platform::PixelType 兼容。
\since build 379
*/
class ScreenBuffer
{
private:
	//! \since build 386
	YSLib::Drawing::Size size;

protected:
	YSLib::Drawing::BitmapPtr pBuffer;
	::HBITMAP hBitmap;

public:
	ScreenBuffer(const YSLib::Drawing::Size&);
	//! \since build 386
	ScreenBuffer(ScreenBuffer&&) ynothrow;
	~ScreenBuffer();

	//! \since build 386
	//@{
	DefGetter(const ynothrow, YSLib::Drawing::BitmapPtr, BufferPtr, pBuffer)
	DefGetter(const ynothrow, ::HBITMAP, NativeHandle, hBitmap)
	DefGetter(const ynothrow, const YSLib::Drawing::Size&, Size, size)

	/*!
	\brief 从缓冲区更新。
	\warning 直接复制，没有边界和大小检查。
	*/
	void
	UpdateFrom(YSLib::Drawing::BitmapPtr) ynothrow;
	//@}
};


//! \since build 387
class ScreenRegionBuffer : private ScreenBuffer
{
private:
	std::mutex mtx;

public:
	ScreenRegionBuffer(const YSLib::Drawing::Size& s)
		: ScreenBuffer(s), mtx()
	{}

	using ScreenBuffer::GetBufferPtr;
	using ScreenBuffer::GetNativeHandle;
	using ScreenBuffer::GetSize;
	DefGetter(ynothrow, ScreenBuffer&, ScreenBufferRef, *this)

	void
	UpdateFrom(YSLib::Drawing::BitmapPtr) ynothrow;

	void
	UpdateTo(NativeWindowHandle, const YSLib::Drawing::Point& = {}) ynothrow;
};


//! \since build 428
//@{
/*!
\brief 窗口内存表面：储存窗口上的二维图形绘制状态。
\note 仅对于内存上下文有所有权。
*/
class YF_API WindowMemorySurface
{
private:
	::HDC h_owner_dc, h_mem_dc;

public:
	WindowMemorySurface(::HDC h_dc)
		: h_owner_dc(h_dc), h_mem_dc(::CreateCompatibleDC(h_dc))
	{}
	~WindowMemorySurface()
	{
		::DeleteDC(h_mem_dc);
	}

	DefGetter(const ynothrow, ::HDC, OwnerHandle, h_owner_dc)
	DefGetter(const ynothrow, ::HDC, NativeHandle, h_mem_dc)

	//! \since build 387
	void
	Update(ScreenBuffer&, const YSLib::Drawing::Point& = {}) ynothrow;
	//! \since build 387
	void
	Update(ScreenRegionBuffer& rbuf, const YSLib::Drawing::Point& pt = {}) ynothrow
	{
		Update(rbuf.GetScreenBufferRef(), pt);
	}
};


class YF_API WindowDeviceContextBase
{
protected:
	NativeWindowHandle hWindow;
	::HDC hDC;

	WindowDeviceContextBase(NativeWindowHandle h_wnd, ::HDC h_dc)
		: hWindow(h_wnd), hDC(h_dc)
	{}
	DefDeDtor(WindowDeviceContextBase)

public:
	DefGetter(const ynothrow, ::HDC, DeviceContextHandle, hDC)
	DefGetter(const ynothrow, NativeWindowHandle, WindowHandle, hWindow)
};


/*!
\brief 窗口设备上下文。
\note 仅对于设备上下文有所有权。
*/
class YF_API WindowDeviceContext : public WindowDeviceContextBase
{
protected:
	WindowDeviceContext(NativeWindowHandle h_wnd)
		: WindowDeviceContextBase(h_wnd, ::GetDC(h_wnd))
	{}
	~WindowDeviceContext()
	{
		::ReleaseDC(hWindow, hDC);
	}
};


/*!
\brief 窗口区域设备上下文。
\note 仅对于设备上下文有所有权。
*/
class YF_API WindowRegionDeviceContext : public WindowDeviceContextBase
{
private:
	::PAINTSTRUCT ps;

protected:
	WindowRegionDeviceContext(NativeWindowHandle h_wnd)
		: WindowDeviceContextBase(h_wnd, ::BeginPaint(h_wnd, &ps))
	{}
	~WindowRegionDeviceContext()
	{
		::EndPaint(hWindow, &ps);
	}
};
//@}


/*!
\brief 显式区域表面：储存显式区域上的二维图形绘制状态。
\since build 387
*/
template<typename _type = WindowDeviceContext>
class GSurface : public _type, public WindowMemorySurface
{
public:
	//! \since build 428
	GSurface(NativeWindowHandle h_wnd)
		: _type(h_wnd), WindowMemorySurface(_type::GetDeviceContextHandle())
	{}
};


//! \since build 382
yconstexpr wchar_t WindowClassName[]{L"YFramework Window"};


/*!
\brief 宿主窗口。
\since build 429
*/
class YF_API HostWindow : private WindowReference
{
public:
	/*!
	\brief 限制指针设备响应在窗口边界内。
	\bug 必须支持 <tt>std::atomic</tt> 。
	\since build 427
	*/
	std::atomic<bool> BoundsLimited{false};

	//! \throw LoggedEvent 窗口类名不是 WindowClassName 。
	HostWindow(NativeWindowHandle);
	DefDelCopyCtor(HostWindow)
	DefDelMoveCtor(HostWindow)
	virtual
	~HostWindow();

	//! \since build 427
	//@{
	using WindowReference::GetLocation;
	using WindowReference::GetNativeHandle;
	//! \since build 430
	using WindowReference::GetOpacity;
	using WindowReference::GetSize;

	//! \since build 430
	using WindowReference::SetOpacity;
	//! \since build 428
	using WindowReference::SetText;

	using WindowReference::Close;

	//! \since build 429
	using WindowReference::Invalidate;

	using WindowReference::Move;
	//@}

	virtual void
	OnDestroy();

	virtual void
	OnLostFocus();

	virtual void
	OnPaint();

	//! \since build 427
	//@{
	using WindowReference::Resize;

	using WindowReference::ResizeClient;

	using WindowReference::Show;
	//@}
};

} // namespace Windows;

} // namespace platform_ex;

#endif

