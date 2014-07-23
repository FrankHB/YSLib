/*
	© 2013-2014 FrankHB.

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
\version r523
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2013-07-10 11:29:04 +0800
\par 修改时间:
	2014-07-20 11:40 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(MinGW32)::Win32GUI
*/


#ifndef YCL_MinGW32_INC_Win32GUI_h_
#define YCL_MinGW32_INC_Win32GUI_h_ 1

#include "YCLib/YModules.h"
#include YFM_MinGW32_YCLib_MinGW32
#include YFM_YSLib_Core_YGDIBase
#include YFM_YSLib_Core_YEvent
#include <mutex> // for std::mutex;
#include <atomic>

namespace platform_ex
{

//! \since build 389
using NativeWindowHandle = ::HWND;

inline namespace Windows
{

/*!
\brief 窗口消息转发事件映射。
\since build 514
\todo 处理返回值。
*/
using MessageMap = std::map<unsigned, YSLib::GEvent<void(::WPARAM, ::LPARAM)>>;

/*!
\brief 添加使用指定优先级调用 ::DefWindowProcW 处理 Windows 消息的处理器。
\relates MessageMap
\since build 518
\todo 处理返回值。
*/
YF_API void
BindDefaultWindowProc(NativeWindowHandle, MessageMap&, unsigned,
	YSLib::EventPriority = 0);


/*!
\brief 本机窗口引用。
\note 不具有所有权。
\warning 非虚析构。
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
	//! \since build 492
	DefDeCopyCtor(WindowReference)
	//! \since build 492
	WindowReference(WindowReference&& r) ynothrow
		: hWindow(r.hWindow)
	{
		r.hWindow = {};
	}

	//! \since build 445
	//@{
	YSLib::Drawing::Rect
	GetClientBounds() const;
	YSLib::Drawing::Point
	GetClientLocation() const;
	YSLib::Drawing::Size
	GetClientSize() const;
	//@}
	//! \since build 518
	YSLib::Drawing::Point
	GetCursorLocation() const;
	YSLib::Drawing::Point
	GetLocation() const;
	DefGetter(const ynothrow, NativeWindowHandle, NativeHandle, hWindow)
	/*!
	\brief 取不透明度。
	\pre 窗口启用 WS_EX_LAYERED 样式。
	\pre 之前必须在此窗口上调用过 SetOpacity 或 ::SetLayeredWindowAttributes 。
	\since build 430
	*/
	YSLib::Drawing::AlphaType
	GetOpacity() const;
	YSLib::Drawing::Size
	GetSize() const;

	//! \since build 445
	void
	SetClientBounds(const YSLib::Drawing::Rect&);
	/*!
	\brief 设置不透明度。
	\pre 窗口启用 WS_EX_LAYERED 样式。
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


//! \warning 非虚析构。
//@{
/*!
\brief 虚拟屏幕缓存。
\note 像素格式和 platform::PixelType 兼容。
\since build 445
*/
class YF_API ScreenBuffer
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

	//! \since build 445
	ScreenBuffer&
	operator=(ScreenBuffer&&);

	//! \since build 386
	//@{
	DefGetter(const ynothrow, YSLib::Drawing::BitmapPtr, BufferPtr, pBuffer)
	DefGetter(const ynothrow, ::HBITMAP, NativeHandle, hBitmap)
	DefGetter(const ynothrow, const YSLib::Drawing::Size&, Size, size)

	/*!
	\brief 从缓冲区更新并按 Alpha 预乘。
	\post ::HBITMAP 的 rgbReserved 为 0 。
	\warning 直接复制，没有边界和大小检查。实际存储必须和 32 位 ::HBITMAP 兼容。
	\since build 435
	*/
	void
	Premultiply(YSLib::Drawing::BitmapPtr) ynothrow;

	/*!
	\brief 重新设置大小。
	\note 当大小一致时无操作，否则重新分配，可导致 pBuffer 和 hBitmap 值改变。
	\since build 445
	*/
	void
	Resize(const YSLib::Drawing::Size&);

	/*!
	\brief 从缓冲区更新。
	\pre 断言：参数非空。
	\post ::HBITMAP 的 rgbReserved 为 0 。
	\warning 直接复制，没有边界和大小检查。实际存储必须和 32 位 ::HBITMAP 兼容。
	*/
	void
	UpdateFrom(YSLib::Drawing::BitmapPtr) ynothrow;
	//@}

	/*
	\brief 交换。
	\since build 430
	*/
	void
	swap(ScreenBuffer&) ynothrow;
};

/*!
\relates ScreenBuffer
\since build 445
*/
inline DefSwap(ynothrow, ScreenBuffer)


/*!
\brief 虚拟屏幕区域缓存。
\note 像素格式和 platform::PixelType 兼容。
\since build 435
*/
class YF_API ScreenRegionBuffer : private ScreenBuffer
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

	//! \since build 435
	using ScreenBuffer::Premultiply;
	//! \since build 445
	using ScreenBuffer::Resize;

	//! \pre 间接断言：参数非空。
	void
	UpdateFrom(YSLib::Drawing::BitmapPtr) ynothrow;

	//! \since build 435
	void
	UpdatePremultipliedTo(NativeWindowHandle, YSLib::Drawing::AlphaType = 0xFF,
		const YSLib::Drawing::Point& = {}) ynothrow;

	void
	UpdateTo(NativeWindowHandle, const YSLib::Drawing::Point& = {}) ynothrow;
};
//@}


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
	PDefH(void, Update, ScreenRegionBuffer& rbuf,
		const YSLib::Drawing::Point& pt = {}) ynothrow
		ImplExpr(Update(rbuf.GetScreenBufferRef(), pt))

	//! \since build 435
	void
	UpdatePremultiplied(ScreenBuffer&, NativeWindowHandle,
		YSLib::Drawing::AlphaType = 0xFF, const YSLib::Drawing::Point& = {})
		ynothrow;
	//! \since build 435
	void
	UpdatePremultiplied(ScreenRegionBuffer& rbuf, NativeWindowHandle h_wnd,
		YSLib::Drawing::AlphaType a = 0xFF,
		const YSLib::Drawing::Point& pt = {}) ynothrow
	{
		UpdatePremultiplied(rbuf.GetScreenBufferRef(), h_wnd, a, pt);
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
\warning 非虚析构。
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


/*!
\brief 窗口类。
\since build 432
*/
class YF_API WindowClass
{
private:
	::HINSTANCE h_instance;

public:
	//! \since build 512
	WindowClass(const wchar_t*, ::WNDPROC, unsigned = 0,
		::HBRUSH = ::HBRUSH(COLOR_MENU + 1));
	~WindowClass();
};


//! \since build 382
yconstexpr wchar_t WindowClassName[]{L"YFramework Window"};


/*!
\brief 宿主窗口。
\since build 429
*/
class YF_API HostWindow : private WindowReference, private YSLib::noncopyable
{
public:
	/*!
	\brief 窗口消息转发事件映射。
	\since build 512
	*/
	Windows::MessageMap MessageMap;

	//! \throw LoggedEvent 窗口类名不是 WindowClassName 。
	HostWindow(NativeWindowHandle);
	DefDelMoveCtor(HostWindow)
	virtual
	~HostWindow();

	//! \since build 445
	//@{
	using WindowReference::GetClientBounds;
	using WindowReference::GetClientLocation;
	using WindowReference::GetClientSize;
	//@}
	//! \since build 518
	using WindowReference::GetCursorLocation;
	//! \since build 427
	//@{
	using WindowReference::GetLocation;
	using WindowReference::GetNativeHandle;
	//! \since build 430
	using WindowReference::GetOpacity;
	using WindowReference::GetSize;

	//! \since build 445
	using WindowReference::SetClientBounds;
	//! \since build 430
	using WindowReference::SetOpacity;
	//! \since build 428
	using WindowReference::SetText;

	using WindowReference::Close;

	//! \since build 429
	using WindowReference::Invalidate;

	/*!
	\brief 取相对于窗口的可响应输入的点的位置。
	\note 默认输入边界为客户区，输入总是视为有效；实现为直接返回参数。
	\return 若参数表示的位置无效则 YSLib::Drawing::Point::Invalie ，
		否则为相对于窗口输入边界的当前点的坐标。
	\since build 518
	*/
	virtual YSLib::Drawing::Point
	MapPoint(const YSLib::Drawing::Point&) const;

	using WindowReference::Move;
	//@}

	//! \since build 427
	//@{
	using WindowReference::Resize;

	using WindowReference::ResizeClient;

	using WindowReference::Show;
	//@}
};

} // inline namespace Windows;

} // namespace platform_ex;

#endif

