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
\version r472
\author FrankHB <frankhb1989@gmail.com>
\since build 492
\par 创建时间:
	2014-04-09 18:30:24 +0800
\par 修改时间:
	2014-12-05 17:04 +0800
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

//! \since build 499
struct AConfiguration;
//! \since build 502
struct AInputQueue;
//! \since build 499
struct ALooper;

namespace platform_ex
{

//! \since build 492
using NativeWindowHandle = ::ANativeWindow*;

/*!
\brief Android 平台扩展接口。
\since build 492
*/
inline namespace Android
{

/*!
\brief 本机窗口引用。
\note 不具有所有权。
\warning 非虚析构。
\since build 492
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

	DefGetter(const ynothrow, NativeWindowHandle, NativeHandle, hWindow)
	//! \since build 498
	YSLib::SDst
	GetHeight() const;
	DefGetter(const, YSLib::Drawing::Size, Size, {GetWidth(), GetHeight()})
	//! \since build 498
	YSLib::SDst
	GetWidth() const;
};


/*!
\brief 宿主窗口。
\note 保持引用计数。
\since build 492
*/
class YF_API HostWindow : private WindowReference, private YSLib::noncopyable
{
public:
	HostWindow(NativeWindowHandle);
	DefDelMoveCtor(HostWindow)
	virtual
	~HostWindow();

	//! \since build 498
	using WindowReference::GetHeight;
	using WindowReference::GetNativeHandle;
	using WindowReference::GetSize;
	//! \since build 498
	using WindowReference::GetWidth;
};


/*!
\brief 屏幕缓存数据。
\note 非公开实现。
*/
class ScreenBufferData;


/*!
\warning 非虚析构。
\note 像素格式和 platform::PixelType 兼容。
\since build 492
*/
//@{
/*!
\brief 虚拟屏幕缓存。
\note 像素跨距等于实现的缓冲区的宽。
*/
class YF_API ScreenBuffer
{
private:
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

public:
	//! \brief 构造：使用指定的缓冲区大小和等于缓冲区宽的像素跨距。
	ScreenBuffer(const YSLib::Drawing::Size&);
	/*!
	\brief 构造：使用指定的缓冲区大小和像素跨距。
	\throw Exception 像素跨距小于缓冲区大小。
	\since build 498
	*/
	ScreenBuffer(const YSLib::Drawing::Size&, YSLib::SDst);
	ScreenBuffer(ScreenBuffer&&) ynothrow;
	~ScreenBuffer();

	ScreenBuffer&
	operator=(ScreenBuffer&&);

	YSLib::Drawing::BitmapPtr
	GetBufferPtr() const ynothrow;
	//! \since build 499
	const YSLib::Drawing::Graphics&
	GetContext() const ynothrow;
	//! \since build 498
	YSLib::Drawing::Size
	GetSize() const ynothrow;
	//! \since build 498
	YSLib::SDst
	GetStride() const ynothrow;

	/*!
	\brief 重新设置大小。
	\note 当大小一致时无操作，否则重新分配，可导致 pBuffer 和 hBitmap 值改变。
	*/
	void
	Resize(const YSLib::Drawing::Size&);

	/*!
	\brief 从缓冲区更新。
	\pre 断言：参数非空。
	\pre 缓冲区大小和像素跨距完全一致。
	\warning 直接复制，没有边界和大小检查。实际存储必须和 32 位 RGBA8888 兼容。
	\since build 558
	*/
	void
	UpdateFrom(YSLib::Drawing::ConstBitmapPtr) ynothrow;

	//! \brief 交换。
	void
	swap(ScreenBuffer&) ynothrow;
};

/*!
\relates ScreenBuffer
\since build 492
*/
inline DefSwap(ynothrow, ScreenBuffer)


//! \brief 虚拟屏幕区域缓存。
class YF_API ScreenRegionBuffer : private ScreenBuffer
{
private:
	//! \since build 551
	YSLib::mutex mtx;

public:
	ScreenRegionBuffer(const YSLib::Drawing::Size&);
	/*!
	\brief 构造：使用指定的缓冲区大小和像素跨距。
	\exception Exception 像素跨距小于缓冲区大小。
	\since build 498
	*/
	ScreenRegionBuffer(const YSLib::Drawing::Size&, YSLib::SDst);

	using ScreenBuffer::GetBufferPtr;
	//! \since build 499
	using ScreenBuffer::GetContext;
//	using ScreenBuffer::GetNativeHandle;
	using ScreenBuffer::GetSize;
	//! \since build 499
	using ScreenBuffer::GetStride;
	DefGetter(ynothrow, ScreenBuffer&, ScreenBufferRef, *this)

	using ScreenBuffer::Resize;

	/*!
	\pre 间接断言：参数非空。
	\since build 558
	*/
	void
	UpdateFrom(YSLib::Drawing::ConstBitmapPtr) ynothrow;

	//! \pre 断言：本机句柄非空。
	void
	UpdateTo(NativeWindowHandle, const YSLib::Drawing::Point& = {}) ynothrow;
};
//@}


/*!
\brief 输入队列。
\note 按键的处理后转交给系统继续处理。
\warning 非虚析构。
\since build 502
\todo 添加定制处理后是否转交的接口。
*/
class YF_API InputQueue
{
private:
	std::reference_wrapper<::AInputQueue> queue_ref;

public:
	InputQueue(::ALooper&, ::AInputQueue&);
	~InputQueue();
};


/*!
\brief 取当前线程的本机消息循环。
\note 参数指定是否允许非回调。
\throw Exception 调用失败。
\since build 499
*/
YF_API ::ALooper&
FetchNativeLooper(bool) ythrow(Exception);

YF_API void
TraceConfiguration(::AConfiguration&,
	platform::Logger::Level = platform::Descriptions::Notice);

} // inline namespace Android;

} // namespace platform_ex;

#endif

