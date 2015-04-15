/*
	© 2013-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HostedGUI.h
\ingroup YCLib
\ingroup YCLibLimitedPlatforms
\brief 宿主 GUI 接口。
\version r1122
\author FrankHB <frankhb1989@gmail.com>
\since build 560
\par 创建时间:
	2013-07-10 11:29:04 +0800
\par 修改时间:
	2015-04-12 16:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::HostedGUI
*/


#ifndef YCL_INC_HostedGUI_h_
#define YCL_INC_HostedGUI_h_ 1

#include "YCLib/YModules.h"
#include YFM_YCLib_Host
#include <ystdex/utility.hpp> // for ystdex::nptr;
#include YFM_YSLib_Core_YGraphics
#include YFM_YSLib_Core_YEvent
#include <atomic>
#if YCL_HostedUI_XCB
#	include YFM_YCLib_XCB
#elif YCL_Win32 || YCL_Android
#elif YF_Hosted
#	error "Unknown platform with XCB support found."
#endif

#if YF_Hosted && YCL_HostedUI
#	if YCL_Win32
//! \since build 564
//@{
struct HBITMAP__;
struct HBRUSH__;
struct HDC__;
struct HINSTANCE__;
struct HWND__;
using HBITMAP = ::HBITMAP__*;
using HBRUSH = ::HBRUSH__*;
using HDC = ::HDC__*;
using HINSTANCE = ::HINSTANCE__*;
using HWND = ::HWND__*;
#		if YCL_Win64
using LPARAM = long long;
#		else
using LPARAM = long;
#		endif
using LRESULT = ::LPARAM;
using WPARAM = std::uintptr_t;
using WNDPROC = ::LRESULT(__stdcall*)(::HWND, unsigned, ::WPARAM, ::LPARAM);
//@}
//! \since build 565
//@{
struct tagWNDCLASSW;
struct tagWNDCLASSEXW;
using WNDCLASSW = ::tagWNDCLASSW;
using WNDCLASSEXW = ::tagWNDCLASSEXW;
//@}
#	elif YCL_Android
struct ANativeWindow;
#	endif

namespace platform_ex
{

#	if YCL_HostedUI_XCB
//! \since build 562
using NativeWindowHandle = ystdex::nptr<XCB::WindowData*>;
#	elif YCL_Win32
//! \since build 389
using NativeWindowHandle = ::HWND;
#	elif YCL_Android
//! \since build 492
using NativeWindowHandle = ::ANativeWindow*;
#	endif

//! \since build 563
//@{
/*!
\typedef MessageID
\brief 用户界面消息类型。
*/
/*!
\typedef MessageHandler
\brief 用户界面消息响应函数类型。
\note 使用 XCB 的平台：实际参数类型同 <tt>::xcb_generic_event_t*</tt> 。
*/

#	if YCL_HostedUI_XCB
using MessageID = std::uint8_t;
using MessageHandler = void(void*);
#	elif YCL_Win32
using MessageID = unsigned;
using MessageHandler = void(::WPARAM, ::LPARAM, ::LRESULT&);
#	endif
//@}

#	if YCL_HostedUI_XCB || YCL_Win32
/*!
\brief 窗口消息转发事件映射。
\since build 514
*/
using MessageMap = std::map<MessageID, YSLib::GEvent<MessageHandler>>;
#	endif

#	if YCL_Win32
/*!
\brief 添加使用指定优先级调用 ::DefWindowProcW 处理 Windows 消息的处理器。
\relates MessageMap
\since build 518
\todo 处理返回值。
*/
YF_API void
BindDefaultWindowProc(NativeWindowHandle, MessageMap&, unsigned,
	YSLib::EventPriority = 0);
#	endif


/*!
\brief 本机窗口引用。
\note 不具有所有权。
\warning 非虚析构。
\since build 427
*/
class YF_API WindowReference : private ystdex::nptr<NativeWindowHandle>
{
public:
	//! \since build 563
	DefDeCtor(WindowReference)
	//! \since build 560
	using nptr::nptr;
	//! \since build 560
	DefDeCopyMoveCtorAssignment(WindowReference)

#	if YCL_Win32
	//! \since build 569
	bool
	IsVisible() const ynothrow;
#	endif

#	if YCL_HostedUI_XCB
	//! \since build 562
	//@{
	DefGetterMem(const, YSLib::Drawing::Rect, Bounds, Deref())
	DefGetterMem(const, YSLib::Drawing::Point, Location, Deref())
	DefGetterMem(const, YSLib::Drawing::Size, Size, Deref())
	//@}
#	elif YCL_Win32
	/*!
	\exception 异常中立：由 CheckScalar 抛出。
	\since build 543
	*/
	YSLib::Drawing::Rect
	GetBounds() const;
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
#	elif YCL_Android
	//! \since build 498
	YSLib::SDst
	GetHeight() const;
#	endif
	DefGetter(const ynothrow, NativeWindowHandle, NativeHandle, get())
#	if YCL_Win32
	/*!
	\brief 取不透明度。
	\pre 窗口启用 WS_EX_LAYERED 样式。
	\pre 之前必须在此窗口上调用过 SetOpacity 或 ::SetLayeredWindowAttributes 。
	\since build 430
	*/
	YSLib::Drawing::AlphaType
	GetOpacity() const;
	//! \since build 538
	WindowReference
	GetParent() const;
	//! \exception 异常中立：由 CheckScalar 抛出。
	YSLib::Drawing::Size
	GetSize() const;
#	elif YCL_Android
	DefGetter(const, YSLib::Drawing::Size, Size, {GetWidth(), GetHeight()})
	//! \since build 498
	YSLib::SDst
	GetWidth() const;
#	endif

#	if YCL_HostedUI_XCB
	//! \since build 562
	//@{
	DefSetterMem(const YSLib::Drawing::Rect&, Bounds, Deref())

	PDefH(void, Close, )
		ImplRet(Deref().Close())

	/*!
	\brief 检查引用值，若非空则返回引用。
	\throw std::runtime_error 引用为空。
	*/
	XCB::WindowData&
	Deref() const;

	//! \since build 563
	PDefH(void, Hide, )
		ImplRet(Deref().Hide())

	PDefH(void, Invalidate, )
		ImplRet(Deref().Invalidate())

	PDefH(void, Move, const YSLib::Drawing::Point& pt)
		ImplRet(Deref().Move(pt))

	PDefH(void, Resize, const YSLib::Drawing::Size& s)
		ImplRet(Deref().Resize(s))
	//@}

	//! \since build 563
	PDefH(void, Show, )
		ImplRet(Deref().Show())
#	elif YCL_Win32
	/*!
	\brief 按参数指定的客户区边界设置窗口边界。
	\note 线程安全。
	\since build 570
	*/
	void
	SetBounds(const YSLib::Drawing::Rect&);
	/*!
	\brief 按参数指定的客户区边界设置窗口边界。
	\exception 异常中立：由 CheckScalar 抛出。
	\since build 445
	*/
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
	\brief 按参数指定的客户区位置移动窗口。
	\since build 570
	*/
	void
	MoveClient(const YSLib::Drawing::Point&);

	/*!
	\brief 调整窗口大小。
	\note 线程安全。
	*/
	void
	Resize(const YSLib::Drawing::Size&);

	/*!
	\brief 按参数指定的客户区大小调整窗口大小。
	\exception 异常中立：由 CheckScalar 抛出。
	\note 线程安全。
	*/
	void
	ResizeClient(const YSLib::Drawing::Size&);

	/*!
	\brief 显示窗口。
	\note 使用 <tt>::ShowWindowAsync</tt> 实现，非阻塞调用，直接传递参数。
	\note 默认参数为 \c SW_SHOWNORMAL ，指定若窗口被最小化则恢复且激活窗口。
	\return 异步操作是否成功。
	\since build 548
	*/
	bool
	Show(int = 1) ynothrow;
#	endif

protected:
	//! \since build 560
	using nptr::get_ref;
};


#	if YCL_HostedUI_XCB || YCL_Android
/*!
\brief 更新指定图形接口上下文的至窗口。
\pre 间接断言：本机句柄非空。
\since build 559
*/
YF_API void
UpdateContentTo(NativeWindowHandle, const YSLib::Drawing::Rect&,
	const YSLib::Drawing::ConstGraphics&);
#	elif YCL_Win32
/*!
\brief 按指定窗口类名、客户区大小、标题文本、样式和附加样式创建本机顶级窗口。
\note 最后的默认参数分别为 \c WS_POPUP 和 \c WS_EX_LTRREADING 。
\exception 异常中立：由 CheckScalar 抛出。
\since build 564
*/
YF_API NativeWindowHandle
CreateNativeWindow(const wchar_t*, const YSLib::Drawing::Size&, const wchar_t*
	= L"", unsigned long = 0x80000000L, unsigned long = 0x00000000L);
#	endif


#	if YCL_HostedUI_XCB || YCL_Android
/*!
\brief 屏幕缓存数据。
\note 非公开实现。
\since build 498
*/
class ScreenBufferData;
#	endif


/*
\note 像素格式和 platform::Pixel 兼容。
\warning 非虚析构。
*/
//@{
/*!
\brief 虚拟屏幕缓存。
\note Android 平台：像素跨距等于实现的缓冲区的宽。
\since build 445
*/
class YF_API ScreenBuffer
{
private:
#	if YCL_HostedUI_XCB || YCL_Android
	/*!
	\invariant bool(p_impl) 。
	\since build 498
	*/
	std::unique_ptr<ScreenBufferData> p_impl;
	/*!
	\brief 宽：以像素数计量的缓冲区的实际宽度。
	\since build 498
	*/
	YSLib::SDst width;
#	elif YCL_Win32
	//! \since build 386
	YSLib::Drawing::Size size;

protected:
	YSLib::Drawing::BitmapPtr pBuffer;
	::HBITMAP hBitmap;
#	endif

public:
	//! \brief 构造：使用指定的缓冲区大小和等于缓冲区宽的像素跨距。
	ScreenBuffer(const YSLib::Drawing::Size&);
#	if YCL_HostedUI_XCB || YCL_Android
	/*!
	\brief 构造：使用指定的缓冲区大小和像素跨距。
	\throw Exception 像素跨距小于缓冲区大小。
	\since build 498
	*/
	ScreenBuffer(const YSLib::Drawing::Size&, YSLib::SDst);
#	endif
	//! \since build 386
	ScreenBuffer(ScreenBuffer&&) ynothrow;
	~ScreenBuffer();

	//! \since build 445
	ScreenBuffer&
	operator=(ScreenBuffer&&);

#	if YCL_HostedUI_XCB || YCL_Android
	//! \since build 492
	YSLib::Drawing::BitmapPtr
	GetBufferPtr() const ynothrow;
	//! \since build 566
	YSLib::Drawing::Graphics
	GetContext() const ynothrow;
	//! \since build 498
	YSLib::Drawing::Size
	GetSize() const ynothrow;
	//! \since build 498
	YSLib::SDst
	GetStride() const ynothrow;
#	elif YCL_Win32
	//! \since build 386
	//@{
	DefGetter(const ynothrow, YSLib::Drawing::BitmapPtr, BufferPtr, pBuffer)
	DefGetter(const ynothrow, ::HBITMAP, NativeHandle, hBitmap)
	DefGetter(const ynothrow, const YSLib::Drawing::Size&, Size, size)

	/*!
	\brief 从缓冲区更新并按 Alpha 预乘。
	\post ::HBITMAP 的 rgbReserved 为 0 。
	\warning 直接复制，没有边界和大小检查。实际存储必须和 32 位 ::HBITMAP 兼容。
	\since build 558
	*/
	void
	Premultiply(YSLib::Drawing::ConstBitmapPtr) ynothrow;
#	endif

	/*!
	\brief 重新设置大小。
	\note 当大小一致时无操作，否则重新分配，可导致 pBuffer 和 hBitmap 值改变。
	\since build 445
	*/
	void
	Resize(const YSLib::Drawing::Size&);

	/*!
	\brief 从缓冲区更新。
	\pre 间接断言：参数非空。
	\pre Android 平台：缓冲区大小和像素跨距完全一致。
	\post Win32 平台： \c ::HBITMAP 的 \c rgbReserved 为 0 。
	\warning 直接复制，没有边界和大小检查。
	\warning Win32 平台：实际存储必须和 32 位 ::HBITMAP 兼容。
	\warning Android 平台：实际存储必须和 32 位 RGBA8888 兼容。
	\since build 558
	*/
	void
	UpdateFrom(YSLib::Drawing::ConstBitmapPtr) ynothrow;
	//@}

#	if YCL_Win32
	/*!
	\brief 从缓冲区更新指定边界的区域。
	\pre 间接断言：参数非空。
	\post \c ::HBITMAP 的 \c rgbReserved 为 0 。
	\warning 直接复制，没有边界和大小检查。
	\warning 实际存储必须和 32 位 ::HBITMAP 兼容。
	\since build 591
	*/
	void
	UpdateFromBounds(YSLib::Drawing::ConstBitmapPtr,
		const YSLib::Drawing::Rect&) ynothrow;

	//! \since build 589
	void
	UpdatePremultipliedTo(NativeWindowHandle, YSLib::Drawing::AlphaType = 0xFF,
		const YSLib::Drawing::Point& = {});
#	endif

	/*!
	\pre 间接断言：本机句柄非空。
	\since build 589
	*/
	void
	UpdateTo(NativeWindowHandle, const YSLib::Drawing::Point& = {}) ynothrow;

#	if YCL_Win32
	/*!
	\pre 间接断言：本机句柄非空。
	\since build 591
	*/
	void
	UpdateToBounds(NativeWindowHandle, const YSLib::Drawing::Rect&,
		const YSLib::Drawing::Point& = {}) ynothrow;
#	endif

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
\since build 435
*/
class YF_API ScreenRegionBuffer : private ScreenBuffer
{
private:
	//! \since build 551
	YSLib::mutex mtx{};

public:
#	if YCL_HostedUI_XCB || YCL_Android
	ScreenRegionBuffer(const YSLib::Drawing::Size&);
	/*!
	\brief 构造：使用指定的缓冲区大小和像素跨距。
	\exception Exception 像素跨距小于缓冲区大小。
	\since build 498
	*/
	ScreenRegionBuffer(const YSLib::Drawing::Size&, YSLib::SDst);
#	elif YCL_Win32
	ScreenRegionBuffer(const YSLib::Drawing::Size& s)
		: ScreenBuffer(s)
	{}
#	endif

	DefGetter(ynothrow, ScreenBuffer&, ScreenBufferRef, *this)

	//! \since build 589
	YSLib::locked_ptr<ScreenBuffer>
	Lock();
};
//@}


#	if YCL_Win32
//! \since build 428
//@{
/*!
\brief 窗口内存表面：储存窗口上的二维图形绘制状态。
\note 仅对内存上下文有所有权。
*/
class YF_API WindowMemorySurface
{
private:
	::HDC h_owner_dc, h_mem_dc;

public:
	WindowMemorySurface(::HDC);
	~WindowMemorySurface();

	DefGetter(const ynothrow, ::HDC, OwnerHandle, h_owner_dc)
	DefGetter(const ynothrow, ::HDC, NativeHandle, h_mem_dc)

	//! \since build 387
	void
	Update(ScreenBuffer&, const YSLib::Drawing::Point& = {}) ynothrow;

	//! \since build 591
	void
	UpdateBounds(ScreenBuffer&, const YSLib::Drawing::Rect&,
		const YSLib::Drawing::Point& = {}) ynothrow;

	//! \since build 591
	void
	UpdatePremultiplied(ScreenBuffer&, NativeWindowHandle,
		YSLib::Drawing::AlphaType = 0xFF, const YSLib::Drawing::Point& = {})
		ynothrow;
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
\note 仅对设备上下文有所有权。
*/
class YF_API WindowDeviceContext : public WindowDeviceContextBase
{
protected:
	WindowDeviceContext(NativeWindowHandle);
	~WindowDeviceContext();
};


/*!
\brief 窗口区域设备上下文。
\note 仅对设备上下文有所有权。
*/
class YF_API WindowRegionDeviceContext : public WindowDeviceContextBase
{
private:
	/*!
	\note 保持和 \c ::PAINTSTRUCT 二进制兼容。
	\since build 564
	*/
	ystdex::byte ps[64];

protected:
	WindowRegionDeviceContext(NativeWindowHandle);
	~WindowRegionDeviceContext();

public:
	/*!
	\exception 异常中立：由 CheckScalar 抛出。
	\since build 591
	*/
	YSLib::Drawing::Rect
	GetInvalidatedArea() const;
};
//@}


/*!
\brief 显示区域表面：储存显示区域上的二维图形绘制状态。
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
class YF_API WindowClass final : private YSLib::noncopyable
{
private:
	//! \since build 565
	std::wstring name;
	//! \since build 565
	unsigned short atom;
	::HINSTANCE h_instance;

public:
	/*!
	\since build 556
	*/
	//@{
	//! \throw Win32Exception 窗口类注册失败。
	//@{
	/*
	\pre 间接断言：指针参数非空。
	\note 应用程序实例句柄参数为空则使用 <tt>::GetModuleHandleW()</tt> 。
	\note 默认画刷参数等于 <tt>::HBRUSH(COLOR_MENU + 1)</tt> 。
	*/
	WindowClass(const wchar_t*, ::WNDPROC, unsigned = 0,
		::HBRUSH = ::HBRUSH(4 + 1), ::HINSTANCE = {});
	WindowClass(const ::WNDCLASSW&);
	WindowClass(const ::WNDCLASSEXW&);
	//@}
	/*!
	\pre 原子表示已注册的窗口类。
	\pre 实例句柄和注册时使用的值相等。
	\throw std::invalid_argument 原子值等于 \c 0 。
	\note 使用指定名称和原子并取得窗口类的所有权。名称不影响原子。
	*/
	WindowClass(const std::wstring&, unsigned short, ::HINSTANCE);
	//@}
	~WindowClass();

	//! \since build 565
	//@{
	DefGetter(const ynothrow, unsigned short, Atom, atom)
	DefGetter(const ynothrow, ::HINSTANCE, InstanceHandle, h_instance)
	DefGetter(const ynothrow, const std::wstring&, Name, name)
	//@}
};


//! \since build 382
yconstexpr wchar_t WindowClassName[]{L"YFramework Window"};
#	endif


/*!
\brief 宿主窗口。
\note Android 平台：保持引用计数。
\since build 429
*/
class YF_API HostWindow : private WindowReference, private YSLib::noncopyable
{
public:
#	if YCL_HostedUI_XCB
	//! \since build 563
	const XCB::Atom::NativeType WM_PROTOCOLS, WM_DELETE_WINDOW;
#	endif
#	if YCL_HostedUI_XCB || YCL_Win32
	/*!
	\brief 窗口消息转发事件映射。
	\since build 512
	*/
	platform_ex::MessageMap MessageMap;
#	endif

	/*!
	\brief 使用指定宿主句柄初始化宿主窗口。
	\pre 使用 XCB 的平台：间接断言：句柄非空。
	\pre 使用 XCB 的平台：句柄通过 <tt>new XCB::WindowData</tt> 得到。
	\pre Win32 平台：断言：句柄有效。
	\pre Win32 平台：断言：句柄表示的窗口在本线程上创建。
	\pre Win32 平台：断言： \c GWLP_USERDATA 数据等于 \c 0 。
	\throw GeneralEvent 使用 XCB 的平台：窗口从属的 XCB 连接发生错误。
	\throw GeneralEvent Win32 平台：窗口类名不是 WindowClassName 。

	检查句柄，初始化宿主窗口并取得所有权。对 Win32 平台初始化 HID 输入消息并注册
	\c WM_DESTROY 消息响应为调用 <tt>::PostQuitMessage(0)</tt> 。
	*/
	HostWindow(NativeWindowHandle);
	DefDelMoveCtor(HostWindow)
	virtual
	~HostWindow();

#	if YCL_Win32
	//! \since build 569
	using WindowReference::IsVisible;
#	endif

#	if YCL_HostedUI_XCB
	//! \since build 563
	//@{
	using WindowReference::GetBounds;
	using WindowReference::GetLocation;

	using WindowReference::SetBounds;
	//@}
#	elif YCL_Win32
	//! \since build 543
	using WindowReference::GetBounds;
	//! \since build 445
	//@{
	using WindowReference::GetClientBounds;
	using WindowReference::GetClientLocation;
	using WindowReference::GetClientSize;
	//@}
	//! \since build 518
	using WindowReference::GetCursorLocation;
	//! \since build 427
	using WindowReference::GetLocation;
#	elif YCL_Android
	//! \since build 498
	using WindowReference::GetHeight;
#	endif
	//! \since build 427
	//@{
	using WindowReference::GetNativeHandle;
#	if YCL_Win32
	//! \since build 430
	using WindowReference::GetOpacity;
	//! \since build 538
	using WindowReference::GetParent;
#	endif
	using WindowReference::GetSize;
#	if YCL_Android
	//! \since build 498
	using WindowReference::GetWidth;
#	endif

#	if YCL_Win32
	//! \since build 570
	using WindowReference::SetBounds;
	//! \since build 445
	using WindowReference::SetClientBounds;
	//! \since build 430
	using WindowReference::SetOpacity;
	//! \since build 428
	using WindowReference::SetText;
#	endif

#	if YCL_HostedUI_XCB || YCL_Win32
	using WindowReference::Close;
	//@}

	//! \since build 429
	using WindowReference::Invalidate;

#		if YCL_Win32
	/*!
	\brief 取相对窗口的可响应输入的点的位置。
	\note 默认输入边界为客户区，输入总是视为有效；实现为直接返回参数。
	\return 若参数表示的位置无效则 YSLib::Drawing::Point::Invalie ，
		否则为相对窗口输入边界的当前点的坐标。
	\since build 518
	*/
	virtual YSLib::Drawing::Point
	MapPoint(const YSLib::Drawing::Point&) const;
#		endif

	//! \since build 427
	//@{
	using WindowReference::Move;

#		if YCL_Win32
	//! \since build 570
	using WindowReference::MoveClient;

	using WindowReference::Resize;

	using WindowReference::ResizeClient;
#		endif

	using WindowReference::Show;
	//@}
#	endif
};

} // namespace platform_ex;

#endif

#endif

