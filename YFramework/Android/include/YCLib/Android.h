/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Android.h
\ingroup YCLib
\ingroup Android
\brief YCLib Android 平台公共扩展。
\version r276
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 18:30:24 +0800
\par 修改时间:
	2014-04-10 01:40 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Android)::Android
*/


#ifndef YCL_Android_INC_Android_h_
#define YCL_Android_INC_Android_h_ 1

#include "YCLib/YModules.h"
#include YFM_YCLib_Host
#if !YCL_Android
#	error "This file is only for Android."
#endif
#include <android/native_window.h>
#include YFM_YSLib_Core_YGDIBase
#include <mutex>

namespace platform_ex
{

//! \since build 492
using NativeWindowHandle = ::ANativeWindow*;

/*!
\brief Windows 平台扩展接口。
\since build 492
*/
inline namespace Android
{

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
	DefDeCopyCtor(WindowReference)
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
	YSLib::Drawing::Point
	GetLocation() const;
	DefGetter(const ynothrow, NativeWindowHandle, NativeHandle, hWindow)
	YSLib::Drawing::Size
	GetSize() const;

	void
	SetClientBounds(const YSLib::Drawing::Rect&);
	//! \brief 设置标题栏文字。
	void
	SetText(const wchar_t*);

	//! \note 线程安全。
	void
	Close();

	//! \brief 无效化窗口客户区。
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
\brief 宿主窗口。
\since build 492
*/
class YF_API HostWindow : private WindowReference, private YSLib::noncopyable
{
public:
	/*!
	\brief 窗口消息转发事件映射。
	\since build 488
	*/
//	std::map< ::UINT, YSLib::GEvent<void(::WPARAM, ::LPARAM)>> MessageMap;

	HostWindow(NativeWindowHandle);
	DefDelMoveCtor(HostWindow)
	virtual
	~HostWindow();

	using WindowReference::GetClientBounds;
	using WindowReference::GetClientLocation;
	using WindowReference::GetClientSize;
	using WindowReference::GetLocation;
	using WindowReference::GetNativeHandle;
	using WindowReference::GetSize;

	using WindowReference::SetClientBounds;
	using WindowReference::SetText;

	using WindowReference::Close;

	using WindowReference::Invalidate;

	using WindowReference::Move;

	using WindowReference::Resize;

	using WindowReference::ResizeClient;

	using WindowReference::Show;
};


/*!
\warning 非虚析构。
\note 像素格式和 platform::PixelType 兼容。
\since build 492
*/
//@{
//! \brief 虚拟屏幕缓存。
class YF_API ScreenBuffer
{
private:
	YSLib::Drawing::Size size;

protected:
	YSLib::Drawing::BitmapPtr pBuffer;
//	::HBITMAP hBitmap;

public:
	ScreenBuffer(const YSLib::Drawing::Size&);
	ScreenBuffer(ScreenBuffer&&) ynothrow;
	~ScreenBuffer();

	ScreenBuffer&
	operator=(ScreenBuffer&&);

	DefGetter(const ynothrow, YSLib::Drawing::BitmapPtr, BufferPtr, pBuffer)
//	DefGetter(const ynothrow, ::HBITMAP, NativeHandle, hBitmap)
	DefGetter(const ynothrow, const YSLib::Drawing::Size&, Size, size)

	/*!
	\brief 从缓冲区更新并按 Alpha 预乘。
	\post ::HBITMAP 的 rgbReserved 为 0 。
	\warning 直接复制，没有边界和大小检查。实际存储必须和 32 位 ::HBITMAP 兼容。
	*/
	void
	Premultiply(YSLib::Drawing::BitmapPtr) ynothrow;

	/*!
	\brief 重新设置大小。
	\note 当大小一致时无操作，否则重新分配，可导致 pBuffer 和 hBitmap 值改变。
	*/
	void
	Resize(const YSLib::Drawing::Size&);

	/*!
	\brief 从缓冲区更新。
	\post ::HBITMAP 的 rgbReserved 为 0 。
	\warning 直接复制，没有边界和大小检查。实际存储必须和 32 位 ::HBITMAP 兼容。
	*/
	void
	UpdateFrom(YSLib::Drawing::BitmapPtr) ynothrow;
	//@}

	//! \brief 交换。
	void
	swap(ScreenBuffer&) ynothrow;
};

//! \since build 445
inline DefSwap(ynothrow, ScreenBuffer)


//! \brief 虚拟屏幕区域缓存。
class YF_API ScreenRegionBuffer : private ScreenBuffer
{
private:
	std::mutex mtx;

public:
	ScreenRegionBuffer(const YSLib::Drawing::Size& s)
		: ScreenBuffer(s), mtx()
	{}

	using ScreenBuffer::GetBufferPtr;
//	using ScreenBuffer::GetNativeHandle;
	using ScreenBuffer::GetSize;
	DefGetter(ynothrow, ScreenBuffer&, ScreenBufferRef, *this)

	//! \since build 435
	using ScreenBuffer::Premultiply;
	//! \since build 445
	using ScreenBuffer::Resize;

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

} // namespace Android;

} // namespace platform_ex;

#endif

