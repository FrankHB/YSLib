/*
	© 2009-2015, 2019-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YDevice.h
\ingroup Core
\brief 平台无关的设备抽象层。
\version r2097
\author FrankHB <frankhb1989@gmail.com>
\since build 586
\par 创建时间:
	2009-12-28 16:39:39 +0800
\par 修改时间:
	2020-01-25 16:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YDevice
*/


#ifndef YSL_INC_Core_YDevice_h_
#define YSL_INC_Core_YDevice_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YObject // for function, ystdex::unsupported;
#include YFM_YSLib_Core_YGraphics

namespace YSLib
{

namespace Devices
{

/*!
\brief 图形设备。
\since build 218
*/
class YF_API GraphicDevice : private noncopyable, protected Drawing::Graphics
{
public:
	//! \since build 559
	using Graphics = Drawing::Graphics;

	/*!
	\brief 构造：指定大小和缓冲区指针。
	\since build 319
	*/
	GraphicDevice(const Drawing::Size& s, Drawing::BitmapPtr p = {}) ynothrow
		: Graphics(p, s)
	{}
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 297
	*/
	virtual ~GraphicDevice();

	//! \since build 319
	//@{
	using Graphics::operator!;

	using Graphics::operator bool;
	//@}

	/*!
	\brief 取后备缓冲区。
	\since build 407
	*/
	virtual DefGetter(const, unique_ptr<Drawing::IImage>, BackBuffer,
		unique_ptr<Drawing::IImage>())
	//! \since build 296
	//@{
	using Graphics::GetHeight;
	using Graphics::GetWidth;
	using Graphics::GetSize;
	//@}

	//! \since build 558
	virtual DefSetter(, const Drawing::Size&, Size, sGraphics)
};


/*!
\brief 按键输入设备。
\since build 490
*/
class YF_API KeyInputDevice
{
public:
	//! \brief 输入测试器：判断是否在指定索引上激活输入状态。
	using Tester = function<bool(const KeyInput&, KeyIndex)>;

private:
	//! \brief 宽度：设备支持的按键编码上界（不含）。
	KeyIndex width;

public:
	explicit
	KeyInputDevice(KeyIndex w = KeyBitsetWidth)
		: width(w)
	{
		YAssert(w != 0, "Invalid key input device found.");
	}
	//! \since build 586
	DefDeCopyCtor(KeyInputDevice)
	//! \brief 虚析构：类定义外默认实现。
	virtual ~KeyInputDevice();

	DefGetter(const ynothrow, KeyIndex, Width, width)

	//! \brief 按键映射。
	virtual PDefH(KeyIndex, Map, KeyIndex code)
		ImplRet(code < width ? code : 0)

	//! \brief 默认输入测试。
	static PDefH(bool, DefaultTest, const KeyInput& keys, KeyIndex code)
		ImplRet(code < KeyBitsetWidth ? keys[code] : false)
};


/*!
\brief 屏幕。
\note 默认实现：假定缓冲区总是可访问且不失效。
\since build 218
*/
class YF_API Screen : public GraphicDevice
{
public:
	/*!
	\brief 构造：指定大小和缓冲区指针。
	\since build 427
	*/
	Screen(const Drawing::Size& s, Drawing::BitmapPtr p = {}) ynothrow
		: GraphicDevice(s, p)
	{}

	/*!
	\brief 更新：复制到屏幕。
	\pre 默认实现为断言：参数非空。
	\pre 参数指定的缓冲区和屏幕缓冲区兼容。
	\since build 558
	*/
	YB_NONNULL(2) virtual void
	Update(Drawing::ConstBitmapPtr) ynothrow;

	//! \since build 558
	PDefH(void, SetSize, const Drawing::Size&) override
		ImplThrow(
			ystdex::unsupported("The screen does not support alter the size."))
};

} // namespace Devices;

} // namespace YSLib;

#endif

