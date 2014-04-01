/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ydevice.h
\ingroup Core
\brief 平台无关的设备抽象层。
\version r2038
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-28 16:39:39 +0800
\par 修改时间:
	2014-04-01 14:58 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YDevice
*/


#ifndef YSL_INC_Core_YDevice_h_
#define YSL_INC_Core_YDevice_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YObject
#include YFM_YSLib_Core_YGDIBase

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
	/*!
	\brief 构造：指定宽度、高度和缓冲区指针。
	\since build 319
	*/
	inline
	GraphicDevice(SDst w, SDst h, Drawing::BitmapPtr p = {}) ynothrow
		: Graphics(p, Drawing::Size(w, h))
	{}
	/*!
	\brief 析构：默认实现。
	\since build 297
	*/
	virtual DefDeDtor(GraphicDevice)

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
	using Graphics::GetBufferPtr;
	using Graphics::GetHeight;
	using Graphics::GetWidth;
	using Graphics::GetSize;
	using Graphics::GetSizeOfBuffer;
	DefGetter(const ynothrow, const Graphics&, Context, *this)
	//@}
};


/*!
\brief 按键输入设备。
\since build 490
*/
class YF_API KeyInputDevice
{
public:
	//! \brief 输入测试器：判断是否在指定索引上激活输入状态。
	using Tester = std::function<bool(const KeyInput&, KeyIndex)>;

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
	virtual DefDeDtor(KeyInputDevice)

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
\since build 218
*/
class YF_API Screen : public GraphicDevice
{
public:
	/*!
	\brief 构造：指定宽度、高度和缓冲区指针。
	\since build 319
	*/
	Screen(SDst w, SDst h, Drawing::BitmapPtr p = {}) ynothrow
		: GraphicDevice(w, h, p)
	{}
	/*!
	\brief 构造：指定大小和缓冲区指针。
	\since build 427
	*/
	Screen(const Drawing::Size& s, Drawing::BitmapPtr p = {}) ynothrow
		: Screen(s.Width, s.Height, p)
	{}

	/*!
	\brief 取指针。
	\note 进行状态检查。
	*/
	virtual Drawing::BitmapPtr
	GetCheckedBufferPtr() const ynothrow;

	/*!
	\brief 更新。
	\note 复制到屏幕。
	\since build 319
	*/
	virtual void
	Update(Drawing::BitmapPtr) ynothrow;
};

} // namespace Devices;

} // namespace YSLib;

#endif

