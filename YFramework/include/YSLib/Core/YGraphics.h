/*
	© 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YGraphics.h
\ingroup Core
\brief 平台无关的基础图形接口。
\version r212
\author FrankHB <frankhb1989@gmail.com>
\since build 585
\par 创建时间:
	2015-03-17 18:03:31 +0800
\par 修改时间:
	2015-04-10 01:31 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YGraphics
*/


#ifndef YSL_INC_Core_YGraphics_h_
#define YSL_INC_Core_YGraphics_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YGDIBase
#include <ystdex/utility.hpp> // for ystdex::cloneable;

namespace YSLib
{

namespace Drawing
{

/*!
\brief 二维图形接口上下文模板。
\warning 非虚析构。
\since build 559
*/
template<typename _tPointer, class _tSize = Size>
class GGraphics
{
	static_assert(std::is_nothrow_move_constructible<_tPointer>(),
		"Invalid pointer type found.");
	static_assert(std::is_nothrow_copy_constructible<_tSize>(),
		"Invalid size type found.");

public:
	using PointerType = _tPointer;
	using SizeType = _tSize;
	using PixelType = ystdex::decay_t<decltype(PointerType()[0])>;

protected:
	/*!
	\brief 显示缓冲区指针。
	\warning 除非 PointerType 自身为具有所有权的智能指针，不应视为具有所有权。
	*/
	PointerType pBuffer{};
	//! \brief 图形区域大小。
	SizeType sGraphics{};

public:
	//! \brief 默认构造：使用空指针和大小。
	DefDeCtor(GGraphics)
	//! \brief 构造：使用指定位图指针和大小。
	explicit yconstfn
	GGraphics(PointerType p_buf, const SizeType& s = {}) ynothrow
		: pBuffer(std::move(p_buf)), sGraphics(s)
	{}
	//! \brief 构造：使用其它类型的指定位图指针和大小。
	template<typename _tPointer2, class _tSize2>
	explicit yconstfn
	GGraphics(_tPointer2 p_buf, const _tSize2& s = {}) ynothrow
		: GGraphics(static_cast<PointerType>(std::move(p_buf)),
		static_cast<SizeType>(s))
	{}
	//! \brief 构造：使用其它类型的指定位图指针和大小类型的二维图形接口上下文。
	template<typename _tPointer2, class _tSize2>
	yconstfn
	GGraphics(const GGraphics<_tPointer2, _tSize2>& g) ynothrow
		: GGraphics(std::move(g.GetBufferPtr()), g.GetSize())
	{}
	//! \brief 复制构造：浅复制。
	DefDeCopyCtor(GGraphics)
	DefDeMoveCtor(GGraphics)

	DefDeCopyAssignment(GGraphics)
	DefDeMoveAssignment(GGraphics)

	/*!
	\brief 判断无效或有效性。
	\since build 319
	*/
	DefBoolNeg(explicit,
		bool(pBuffer) && sGraphics.Width != 0 && sGraphics.Height != 0)

	/*!
	\brief 取指定行首元素指针。
	\pre 断言：参数不越界。
	\pre 间接断言：缓冲区指针非空。
	*/
	PointerType
	operator[](size_t r) const ynothrow
	{
		YAssert(r < sGraphics.Height, "Access out of range.");
		return Nonnull(pBuffer) + r * sGraphics.Width;
	}

	//! \since build 566
	DefGetter(const ynothrow, const PointerType&, BufferPtr, pBuffer)
	DefGetter(const ynothrow, const SizeType&, Size, sGraphics)
	//! \since build 196
	DefGetter(const ynothrow, SDst, Width, sGraphics.Width)
	//! \since build 196
	DefGetter(const ynothrow, SDst, Height, sGraphics.Height)
	//! \since build 177
	DefGetter(const ynothrow, size_t, SizeOfBuffer,
		sizeof(PixelType) * size_t(GetAreaOf(sGraphics))) //!< 取缓冲区占用空间。

	/*!
	\brief 取指定行首元素指针。
	\throw GeneralEvent 缓冲区指针为空。
	\throw std::out_of_range 参数越界。
	\since build 563
	*/
	PointerType
	at(size_t r) const
	{
		if(YB_UNLIKELY(!pBuffer))
			throw GeneralEvent("Null pointer found.");
		if(YB_UNLIKELY(r >= sGraphics.Height))
			throw std::out_of_range("Access out of range.");
		return pBuffer + r * sGraphics.Width;
	}
};


using BitmapPtr = Pixel*;
using ConstBitmapPtr = const Pixel*;

//! \since build 559
using ConstGraphics = GGraphics<ConstBitmapPtr>;

//! \since build 559
using Graphics = GGraphics<BitmapPtr>;


/*!
\brief 图像接口。
\since build 405
*/
DeclDerivedI(YF_API, IImage, ystdex::cloneable)
	//! \since build 566
	DeclIEntry(Graphics GetContext() const ynothrow)
	DeclIEntry(void SetSize(const Size&))

	//! \since build 409
	DeclIEntry(IImage* clone() const ImplI(ystdex::cloneable))
EndDecl


/*
\brief 绘制上下文。
\warning 非虚析构。
\since build 255
*/
struct YF_API PaintContext
{
	Graphics Target; //!< 渲染目标：图形接口上下文。
	/*!
	\brief 参考位置。

	指定渲染目标关联的参考点的位置的偏移坐标。
	除非另行约定，选取渲染目标左上角为原点的屏幕坐标系。
	*/
	Point Location;
	/*!
	\brief 剪切区域。

	相对图形接口上下文的标准矩形，指定需要保证被刷新的边界区域。
	除非另行约定，剪切区域的位置坐标选取渲染目标左上角为原点的屏幕坐标系。
	*/
	Rect ClipArea;
};

/*!
\brief 根据目标的边界更新剪切区域。
\relates PaintContext
\since build 453
*/
//@{
inline PDefH(void, UpdateClipArea, PaintContext& pc, const Rect& r)
	ImplExpr(pc.ClipArea = r & Rect(pc.Target.GetSize()))

inline PDefH(void, UpdateClipSize, PaintContext& pc, const Size& s)
	ImplExpr(UpdateClipArea(pc, {pc.Location, s}))
//@}

} // namespace Drawing;

} // namespace YSLib;

#endif

