/*
	© 2009-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdi.h
\ingroup Service
\brief 平台无关的图形设备接口。
\version r3694
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-14 18:29:46 +0800
\par 修改时间:
	2014-04-06 17:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YGDI
*/


#ifndef YSL_INC_Service_ygdi_h_
#define YSL_INC_Service_ygdi_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YGDIBase

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
	/*
	\brief 空白距离：左、右、上、下。
	\since build 365
	*/
	SPos Left, Right, Top, Bottom;

	/*!
	\brief 默认构造：使用零边距。
	\since build 365
	*/
	yconstfn
	Padding()
		: Padding(0, 0, 0, 0)
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
	Padding&
	operator+=(const Padding& m)
	{
		yunseq(Left += m.Left, Right += m.Right, Top += m.Top,
			Bottom += m.Bottom);
		return *this;
	}
};

//! \relates Padding
//@{
/*!
\brief 加法：对应分量调用 operator+ 。
*/
yconstfn Padding
operator+(const Padding& x, const Padding& y)
{
	return Padding(x.Left + y.Left, x.Right + y.Right, x.Top + y.Top,
		x.Bottom + y.Bottom);
}
/*!
\brief 加法：缩小屏幕标准矩形，相对位置由指定边距决定。
\note 若边距过大，则矩形的宽或高可能为 0 。
*/
YF_API Rect
operator+(const Rect&, const Padding&);
//@}


/*!
\brief 取水平边距和。
*/
inline SDst
GetHorizontalOf(const Padding& m)
{
	return max<SPos>(0, m.Left) + max<SPos>(0, m.Right);
}

/*!
\brief 取竖直边距和。
*/
inline SDst
GetVerticalOf(const Padding& m)
{
	return max<SPos>(0, m.Top) + max<SPos>(0, m.Bottom);
}


/*!
\brief 取内边界相对于外边界的边距。
*/
YF_API Padding
FetchMargin(const Rect&, const Size&);


/*!
\brief 剪切操作：取标准矩形交集并判断是否严格非空。
\since build 372
*/
inline bool
Clip(Rect& x, const Rect& y)
{
	x &= y;
	return !x.IsUnstrictlyEmpty();
}

/*!
\brief 根据指定源的边界优化绘制上下文的剪切区域。
\return 若边距决定不足以被渲染则为 Point() ，否则为源的起始偏移位置。
\note 当不需要绘制时，不修改偏移坐标。
\since build 372

检查边距限制下需要保留绘制的区域，结果保存至绘制上下文的除渲染目标外的其它成员。
*/
YF_API Point
ClipBounds(Rect&, const Rect&);

/*!
\brief 根据指定边距和源的大小优化绘制上下文的剪切区域。
\return 若边距决定不足以被渲染则为 Point() ，否则为源的起始偏移位置。
\note 当不需要绘制时，不修改偏移坐标。
\since build 369

检查边距限制下需要保留绘制的区域，结果保存至绘制上下文的除渲染目标外的其它成员。
*/
YF_API Point
ClipMargin(PaintContext&, const Padding&, const Size&);


/*!
\brief 使用 Graphics 定义的基本图像。
\since build 406
*/
class YF_API BasicImage : protected Graphics,
	implements IImage
{
public:
	DefDeCtor(BasicImage)
	BasicImage(const Graphics& g)
		: Graphics(g)
	{}

	using Graphics::operator!;

	using Graphics::operator bool;

	using Graphics::GetBufferPtr;
	using Graphics::GetHeight;
	using Graphics::GetWidth;
	using Graphics::GetSize;
	ImplI(IImage) DefGetter(const ynothrow override, const Graphics&, Context,
		*this)

	//! \since build 409
	ImplI(IImage) DefClone(const override, BasicImage)

	ImplI(IImage) DefSetter(const Size&, Size, sGraphics)
};


/*!
\brief 标准矩形像素图缓冲区。
\note 满足 <tt>std::is_nothrow_move_constructible<T>::value &&
	std::is_nothrow_move_assignable<T>::value</tt> 。
\note 保证像素数据连续。
\since build 418
*/
class YF_API CompactPixmap : public BasicImage
{
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
	CompactPixmap(unique_ptr<PixelType[]>, const Size&) ynothrow;
	CompactPixmap(const CompactPixmap&);
	/*!
	\brief 转移构造：转移资源。
	*/
	CompactPixmap(CompactPixmap&&) ynothrow;
	/*!
	\brief 析构：释放资源。
	*/
	~CompactPixmap() override
	{
		delete[] pBuffer;
	}

	/*
	\brief 统一赋值：使用值参数和交换函数进行复制或转移赋值。
	\since build 476
	*/
	PDefHOp(CompactPixmap&, =, CompactPixmap msg) ynothrow
		ImplRet(msg.swap(*this), *this)

	/*!
	\brief 设置内容。
	\note 预先设置缓冲区大小。
	\since build 319
	*/
	void
	SetContent(ConstBitmapPtr, SDst, SDst);
	PDefH(void, SetSize, SDst w, SDst h)
		ImplExpr(SetSize(Size(w, h)))
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

	virtual DefClone(const, CompactPixmap)

	/*
	\brief 交换。
	*/
	PDefH(void, swap, CompactPixmap& buf) ynothrow
		ImplExpr(std::swap<Graphics>(*this, buf))
};

/*!
\relates CompactPixmap
\since build 418
*/
inline DefSwap(ynothrow, CompactPixmap)


/*!
\brief 使用 8 位 Alpha 扩展的标准矩形像素图缓冲区。
\note 满足 <tt>std::is_nothrow_move_constructible<T>::value &&
	std::is_nothrow_move_assignable<T>::value</tt> 。
\note 保证像素数据和 Alpha 数据分别连续。
*/
class YF_API CompactPixmapEx : public CompactPixmap
{
protected:
	/*!
	\brief Alpha 缓冲区指针。
	\since build 417
	*/
	AlphaType* pBufferAlpha;

public:
	/*!
	\brief 无参数构造。
	\note 零初始化。
	*/
	CompactPixmapEx()
		: CompactPixmap(), pBufferAlpha()
	{}
	/*!
	\brief 构造：使用指定位图指针和大小。
	*/
	CompactPixmapEx(ConstBitmapPtr, SDst, SDst);
	CompactPixmapEx(const CompactPixmapEx&);
	/*!
	\brief 转移构造：转移资源。
	*/
	CompactPixmapEx(CompactPixmapEx&&) ynothrow;
	/*!
	\brief 析构：释放资源。
	*/
	~CompactPixmapEx() override
	{
		delete[] pBufferAlpha;
	}

	/*
	\brief 复制赋值：使用复制构造函数和交换函数。
	\since build 296
	*/
	CompactPixmapEx&
	operator=(const CompactPixmapEx& buf)
	{
		CompactPixmapEx(buf).swap(*this);
		return *this;
	}
	/*
	\brief 转移赋值：使用转移构造函数和交换函数。
	*/
	CompactPixmapEx&
	operator=(CompactPixmapEx&& buf) ynothrow
	{
		swap(buf);
		return *this;
	}

	/*!
	\brief 取 Alpha 缓冲区的指针。
	\since build 417
	*/
	DefGetter(const ynothrow, AlphaType*, BufferAlphaPtr, pBufferAlpha)
	/*!
	\brief 取 Alpha 缓冲区占用空间。
	\since build 407
	*/
	DefGetter(const ynothrow, size_t, SizeOfBufferAlpha,
		sizeof(AlphaType) * GetAreaOf(GetSize()))

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

	/*
	\brief 交换。
	*/
	void
	swap(CompactPixmapEx& buf) ynothrow
	{
		std::swap<CompactPixmap>(*this, buf),
		std::swap(pBufferAlpha, buf.pBufferAlpha);
	}
};

/*!
\relates CompactPixmapEx
\since build 418
*/
inline DefSwap(ynothrow, CompactPixmapEx)


/*!
\brief 图形接口上下文向指针指定的缓冲区复制。
\note 仅当指针和指向有效。自动裁剪适应大小。

以指定图形接口上下文作为源，向指定大小和点（相对左上角）的
	指定图形接口上下文以指定输出指向复制缓冲区内容。
*/
YF_API bool
CopyTo(BitmapPtr, const Graphics&, const Size&, const Point&, const Point&,
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
\since build 337

以第一个参数作为目标，以指定输出指向复制第二个参数的缓冲区内容
	至相对于目标缓冲区的点。
*/
inline bool
CopyTo(const Graphics& dst, const Graphics& src,
	const Point& dp = {}, const Point& sp = {}, Rotation rot = RDeg0)
{
	return CopyTo(dst.GetBufferPtr(), src, dst.GetSize(),
		dp, sp, src.GetSize(), rot);
}
/*!
\brief 位图缓冲区向图形接口上下文复制。
\note 仅当指针和指向有效。自动裁剪适应大小。
\since build 418

向指定大小和点（相对左上角）的指定图形接口上下文以指定输出指向复制缓冲区内容。
*/
inline bool
CopyTo(const Graphics& dst, const CompactPixmapEx& src,
	const Point& dp = {}, const Point& sp = {}, Rotation rot = RDeg0)
{
	return CopyTo(dst.GetBufferPtr(), src, dst.GetSize(),
		dp, sp, src.GetSize(), rot);
}

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
inline bool
BlitTo(const Graphics& dst, const CompactPixmapEx& src,
	const Point& dp = {}, const Point& sp = {}, Rotation rot = RDeg0)
{
	return BlitTo(dst.GetBufferPtr(), src, dst.GetSize(),
		dp, sp, src.GetSize(), rot);
}

} // namespace Drawing;

} // namespace YSLib;

#endif

