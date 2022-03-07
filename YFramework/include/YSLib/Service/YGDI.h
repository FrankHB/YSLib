/*
	© 2009-2017, 2019, 2021-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YGDI.h
\ingroup Service
\brief 平台无关的图形设备接口。
\version r4055
\author FrankHB <frankhb1989@gmail.com>
\since build 566
\par 创建时间:
	2009-12-14 18:29:46 +0800
\par 修改时间:
	2022-02-28 05:48 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YGDI
*/


#ifndef YSL_INC_Service_YGDI_h_
#define YSL_INC_Service_YGDI_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YGraphics

namespace YSLib
{

namespace Drawing
{

// GDI 逻辑对象。

/*!
\brief 空白样式。
\warning 非虚析构。
*/
struct YF_API Padding
{
	/*!
	\brief 空白距离：左、右、上、下。
	\since build 365
	*/
	SPos Left = 0, Right = 0, Top = 0, Bottom = 0;

	/*!
	\brief 无参数构造：使用零边距。
	\since build 365
	*/
	DefDeCtor(Padding)
	/*!
	\brief 构造：使用 4 个相同的边距。
	\since build 586
	*/
	explicit yconstfn
	Padding(SPos x)
		: Padding(x, x, x, x)
	{}
	/*!
	\brief 构造：使用 4 个边距。
	\since build 365
	*/
	yconstfn
	Padding(SPos l, SPos r, SPos t, SPos b)
		: Left(l), Right(r), Top(t), Bottom(b)
	{}

	/*!
	\brief 加法赋值：对应分量调用 operator+= 。
	*/
	PDefHOp(Padding&, +=, const Padding& m)
		ImplRet(yunseq(Left += m.Left, Right += m.Right, Top += m.Top,
			Bottom += m.Bottom), *this)

	/*!
	\brief 减法赋值：对应分量调用 operator-= 。
	\since build 572
	*/
	PDefHOp(Padding&, -=, const Padding& m)
		ImplRet(yunseq(Left -= m.Left, Right -= m.Right, Top -= m.Top,
			Bottom -= m.Bottom), *this)

	/*!
	\brief 乘法赋值：对应分量调用 operator-= 。
	\since build 587
	*/
	PDefHOp(Padding&, *=, size_t n)
		// XXX: Conversion to 'SPos' might be implementation-defined.
		ImplRet(yunseq(Left *= SPos(n), Right *= SPos(n), Top *= SPos(n),
			Bottom *= SPos(n)), *this)
};

//! \since build 850
//@{
//! \relates Padding
//@{
//! \brief 加法逆元：对应分量调用一元 operator- 。
YB_ATTR_nodiscard YB_PURE yconstfn
	PDefHOp(Padding, -, const Padding& x) ynothrow
	ImplRet(Padding(-x.Left, -x.Right, -x.Top, -x.Bottom))

//! \brief 加法：对应分量调用 operator+ 。
YB_ATTR_nodiscard YB_PURE yconstfn
	PDefHOp(Padding, +, const Padding& x, const Padding& y) ynothrow
	ImplRet(Padding(x.Left + y.Left, x.Right + y.Right, x.Top + y.Top,
		x.Bottom + y.Bottom))

//! \brief 减法：对应分量调用 operator- 。
YB_ATTR_nodiscard YB_PURE yconstfn
	PDefHOp(Padding, -, const Padding& x, const Padding& y) ynothrow
	ImplRet(Padding(x.Left - y.Left, x.Right - y.Right, x.Top - y.Top,
		x.Bottom - y.Bottom))

//! \brief 乘法：对应分量调用 operator* 。
YB_ATTR_nodiscard YB_PURE yconstfn
	PDefHOp(Padding, *, const Padding& x, size_t n) ynothrow
	ImplRet(Padding(SPos(x.Left * ptrdiff_t(n)), SPos(x.Right * ptrdiff_t(n)),
		SPos(x.Top * ptrdiff_t(n)), SPos(x.Bottom * ptrdiff_t(n))))

//! \note 若边距过大，则矩形的宽或高可能为 0 。
//@{
//! \brief 加法：缩小屏幕标准矩形，相对位置由指定边距决定。
YB_ATTR_nodiscard YF_API YB_PURE Rect
operator+(const Rect&, const Padding&) ynothrow;

//! \brief 减法：放大屏幕标准矩形，相对位置由指定边距决定。
YB_ATTR_nodiscard YB_PURE inline
	PDefHOp(Rect, -, const Rect& r, const Padding& m) ynothrow
	ImplRet(r + -m)
//@}
//@}


//! \warning 不检查溢出。
//@{
//! \brief 取水平边距和。
YB_ATTR_nodiscard YB_PURE yconstfn
	PDefH(SDst, GetHorizontalOf, const Padding& m) ynothrowv
	ImplRet(SDst(ystdex::max<SPos>(0, m.Left) + ystdex::max<SPos>(0, m.Right)))

//! \brief 取竖直边距和。
YB_ATTR_nodiscard YB_PURE yconstfn
	PDefH(SDst, GetVerticalOf, const Padding& m) ynothrowv
	ImplRet(SDst(ystdex::max<SPos>(0, m.Top) + ystdex::max<SPos>(0, m.Bottom)))
//@}


//! \brief 取内边界相对外边界的边距。
YB_ATTR_nodiscard YF_API YB_PURE Padding
FetchMargin(const Rect&, const Size&) ynothrow;


//! \brief 剪切操作：取标准矩形交集并判断是否严格非空。
YB_ATTR_nodiscard inline
	PDefH(bool, Clip, Rect& x, const Rect& y) ynothrow
	ImplRet(x &= y, !x.IsUnstrictlyEmpty())

/*!
\brief 根据指定源的边界优化绘制上下文的剪切区域。
\return 若边距决定不足以被渲染则为 Point() ，否则为源的起始偏移位置。
\note 当不需要绘制时，不修改偏移坐标。

检查边距限制下需要保留绘制的区域，结果保存至绘制上下文的除渲染目标外的其它成员。
*/
YB_ATTR_nodiscard YF_API Point
ClipBounds(Rect&, const Rect&) ynothrow;

/*!
\brief 根据指定边距和源的大小优化绘制上下文的剪切区域。
\return 若边距决定不足以被渲染则为 Point() ，否则为源的起始偏移位置。
\note 当不需要绘制时，不修改偏移坐标。

检查边距限制下需要保留绘制的区域，结果保存至绘制上下文的除渲染目标外的其它成员。
*/
YB_ATTR_nodiscard YF_API Point
ClipMargin(PaintContext&, const Padding&, const Size&) ynothrow;
//@}


/*!
\brief 使用 Graphics 定义的基本图像。
\since build 406
*/
class YF_API BasicImage : protected Graphics, implements IImage
{
public:
	DefDeCtor(BasicImage)
	BasicImage(const Graphics& g)
		: Graphics(g)
	{}
	//! \since build 586
	DefDeCopyCtor(BasicImage)
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~BasicImage() override;

	using Graphics::operator!;

	using Graphics::operator bool;

	using Graphics::GetBufferPtr;
	using Graphics::GetHeight;
	using Graphics::GetWidth;
	using Graphics::GetSize;
	//! \since build 566
	DefGetter(const ynothrow ImplI(IImage), Graphics, Context, *this)

	//! \since build 409
	DefClone(const ImplI(IImage), BasicImage)

	DefSetter(ImplI(IImage), const Size&, Size, sGraphics)
};


/*!
\brief 标准矩形像素图缓冲区。
\post 满足 \c ystdex::is_nothrow_moveable<CompactPixmap>()</tt> 。
\note 保证像素数据连续。
\since build 418
*/
class YF_API CompactPixmap
	: private GGraphics<unique_ptr<Pixel[]>>, implements IImage
{
private:
	//! \since build 566
	//@{
	using BaseType = GGraphics<unique_ptr<Pixel[]>>;

protected:
	using BaseType::pBuffer;
	using BaseType::sGraphics;
	//@}

public:
	/*!
	\brief 无参数构造：默认实现。
	\note 零初始化。
	*/
	DefDeCtor(CompactPixmap)
	/*!
	\brief 构造：使用指定位图指针和大小。
	*/
	CompactPixmap(ConstBitmapPtr, SDst, SDst);
	/*!
	\brief 构造：使用指定位图指针和大小。
	\note 取得所有权。
	*/
	CompactPixmap(unique_ptr<Pixel[]>, const Size&) ynothrow;
	CompactPixmap(const CompactPixmap&);
	DefDeMoveCtor(CompactPixmap)

	//! \since build 768
	//@{
	//! \brief 复制赋值：使用复制和交换。
	PDefHOp(CompactPixmap&, =, const CompactPixmap& buf)
		ImplRet(ystdex::copy_and_swap(*this, buf))
	DefDeMoveAssignment(CompactPixmap)
	//@}

	//! \since build 566
	//@{
	using BaseType::operator!;

	using BaseType::operator bool;

	DefGetter(const ynothrow, BitmapPtr, BufferPtr,
		BaseType::GetBufferPtr().get())
	using BaseType::GetHeight;
	using BaseType::GetWidth;
	using BaseType::GetSize;
	DefGetter(const ynothrow ImplI(IImage), Graphics, Context,
		Graphics(GetBufferPtr(), GetSize()))
	//@}

	/*!
	\brief 设置内容。
	\note 预先设置缓冲区大小。
	\since build 319
	*/
	void
	SetContent(ConstBitmapPtr, SDst, SDst);
	PDefH(void, SetSize, SDst w, SDst h)
		ImplExpr(SetSize({w, h}))
	/*!
	\brief 重新设置缓冲区大小。
	\note 若有一边为零则删除缓冲区；若大于缓冲区空间则重新分配；
		设置后清除缓冲区。
	\since build 405
	*/
	void
	SetSize(const Size&) override;
	/*!
	\brief 交换宽和高；同时清除缓冲区。
	*/
	void
	SetSizeSwap();

	/*!
	\brief 清除缓冲区。
	*/
	virtual void
	ClearImage() const;

	DefClone(const ImplI(IImage), CompactPixmap)

	/*!
	\brief 交换。
	\since build 710
	*/
	friend PDefH(void, swap, CompactPixmap& x, CompactPixmap& y) ynothrow
		ImplExpr(ystdex::swap_dependent<BaseType>(x, y))
};

/*!
\relates CompactPixmap
\since build 630
*/
static_assert(ystdex::is_nothrow_moveable<CompactPixmap>(),
	"Postcondition failed.");


/*!
\brief 使用 8 位 Alpha 扩展的标准矩形像素图缓冲区。
\post 满足 \c ystdex::is_nothrow_moveable<CompactPixmapEx>()</tt> 。
\note 保证像素数据和 Alpha 数据分别连续。
*/
class YF_API CompactPixmapEx : public CompactPixmap
{
protected:
	/*!
	\brief Alpha 缓冲区指针。
	\since build 417
	*/
	unique_ptr<AlphaType[]> pBufferAlpha;

public:
	DefDeCtor(CompactPixmapEx)
	/*!
	\brief 构造：使用指定位图指针和大小。
	*/
	CompactPixmapEx(ConstBitmapPtr, SDst, SDst);
	CompactPixmapEx(const CompactPixmapEx&);
	DefDeMoveCtor(CompactPixmapEx)

	//! \since build 768
	//@{
	//! \brief 复制赋值：使用复制和交换。
	PDefHOp(CompactPixmapEx&, =, const CompactPixmapEx& buf)
		ImplRet(ystdex::copy_and_swap(*this, buf))
	DefDeMoveAssignment(CompactPixmapEx)
	//@}

	/*!
	\brief 取 Alpha 缓冲区的指针。
	\since build 417
	*/
	DefGetter(const ynothrow, AlphaType*, BufferAlphaPtr, pBufferAlpha.get())
	/*!
	\brief 取 Alpha 缓冲区占用空间。
	\since build 407
	*/
	DefGetter(const ynothrow, size_t, SizeOfBufferAlpha,
		sizeof(AlphaType) * size_t(GetAreaOf(GetSize())))

	using CompactPixmap::SetSize;
	/*!
	\brief 重新设置缓冲区大小。
	\note 若有一边为零则删除缓冲区；若大于缓冲区空间则重新分配；
		设置后清除缓冲区。
	\since build 405
	*/
	void
	SetSize(const Size&) override;

	/*!
	\brief 清除缓冲区。
	*/
	void
	ClearImage() const override;

	DefClone(const override, CompactPixmapEx)

	/*!
	\brief 交换。
	\since build 710
	*/
	PDefH(void, swap, CompactPixmapEx& x, CompactPixmapEx& y) ynothrow
		// XXX: See archived YSLib issues 38.
		ImplExpr(ystdex::swap_dependent<CompactPixmap>(x, y),
			std::swap(x.pBufferAlpha, y.pBufferAlpha))
};

/*!
\relates CompactPixmapEx
\since build 630
*/
static_assert(ystdex::is_nothrow_moveable<CompactPixmapEx>(),
	"Postcondition failed.");


/*!
\brief 图形接口上下文向指针指定的缓冲区复制。
\note 仅当指针和指向有效。自动裁剪适应大小。
\since build 559

以指定图形接口上下文作为源，向指定大小和点（相对左上角）的
	指定图形接口上下文以指定输出指向复制缓冲区内容。
*/
YF_API bool
CopyTo(BitmapPtr, const ConstGraphics&, const Size&, const Point&, const Point&,
	const Size&, Rotation = RDeg0);
/*!
\brief 位图缓冲区向指针指定的缓冲区复制。
\note 仅当指针和指向有效。自动裁剪适应大小。
\since build 418

向指定大小和点（相对左上角）的指定图形接口上下文以指定输出指向复制缓冲区内容。
*/
YF_API bool
CopyTo(BitmapPtr, const CompactPixmapEx&, const Size&,
	const Point&, const Point&, const Size&, Rotation = RDeg0);
/*!
\brief 图形接口上下文复制。
\note 仅当指针和指向有效。自动裁剪适应大小。
\since build 559

以第一参数作为目标，以指定输出指向复制第二参数的缓冲区内容
	至相对目标缓冲区的点。
*/
inline PDefH(bool, CopyTo, const Graphics& dst, const ConstGraphics& src,
	const Point& dp = {}, const Point& sp = {}, Rotation rot = RDeg0)
	ImplRet(CopyTo(dst.GetBufferPtr(), src, dst.GetSize(), dp, sp,
		src.GetSize(), rot))
/*!
\brief 位图缓冲区向图形接口上下文复制。
\note 仅当指针和指向有效。自动裁剪适应大小。
\since build 418

向指定大小和点（相对左上角）的指定图形接口上下文以指定输出指向复制缓冲区内容。
*/
inline PDefH(bool, CopyTo, const Graphics& dst, const CompactPixmapEx& src,
	const Point& dp = {}, const Point& sp = {}, Rotation rot = RDeg0)
	ImplRet(CopyTo(dst.GetBufferPtr(), src, dst.GetSize(), dp, sp,
		src.GetSize(), rot))

/*!
\brief 贴图：位图缓冲区向指针指定的缓冲区以贴图算法复制。
\note 仅当指针和指向有效。自动裁剪适应大小。
\since build 418

向指定大小和点（相对左上角）的指定图形接口上下文以指定输出指向以缓冲区内容贴图。
*/
YF_API bool
BlitTo(BitmapPtr, const CompactPixmapEx&, const Size&,
	const Point&, const Point&, const Size&, Rotation = RDeg0);
/*!
\brief 贴图：位图缓冲区向指针指定的缓冲区以贴图算法复制。
\note 仅当指针和指向有效。自动裁剪适应大小。
\since build 418

向指定大小和点（相对左上角）的指定图形接口上下文以指定输出指向以缓冲区内容贴图。
*/
inline PDefH(bool, BlitTo, const Graphics& dst, const CompactPixmapEx& src,
	const Point& dp = {}, const Point& sp = {}, Rotation rot = RDeg0)
	ImplRet(BlitTo(dst.GetBufferPtr(), src, dst.GetSize(), dp, sp,
		src.GetSize(), rot))

} // namespace Drawing;

} // namespace YSLib;

#endif

