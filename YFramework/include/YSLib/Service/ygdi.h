/*
	Copyright (C) by Franksoft 2009 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdi.h
\ingroup Service
\brief 平台无关的图形设备接口。
\version r3432
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2009-12-14 18:29:46 +0800
\par 修改时间:
	2012-12-27 02:32 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::YGDI
*/


#ifndef YSL_INC_SERVICE_YGDI_H_
#define YSL_INC_SERVICE_YGDI_H_ 1

#include "../Core/ygdibase.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

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
\brief 加法：缩小屏幕正则矩形，相对位置由指定边距决定。
\note 若边距过大，则矩形的宽或高可能为 0 。
*/
YF_API Rect
operator+(const Rect&, const Padding&);


/*!
\brief 取水平边距和。
*/
inline SDst
GetHorizontalOf(const Padding& m)
{
	return max<SPos>(m.Left, 0) + max<SPos>(m.Right, 0);
}

/*!
\brief 取竖直边距和。
*/
inline SDst
GetVerticalOf(const Padding& m)
{
	return max<SPos>(m.Top, 0) + max<SPos>(m.Bottom, 0);
}


/*!
\brief 取内边界相对于外边界的边距。
*/
YF_API Padding
FetchMargin(const Rect&, const Size&);


/*!
\brief 根据指定边距和源优化优化绘制上下文的剪切区域。
\since build 367

检查边距限制下需要保留绘制的区域，结果保存至绘制上下文的除渲染目标外的其它成员。
当不需要绘制时，不修改偏移坐标。
*/
YF_API void
Clip(PaintContext&, const Padding&, const Size&);


/*!
\brief 正则矩形位图缓冲区。
\note 满足 <tt>std::is_nothrow_move_constructible<T>::value &&
	std::is_nothrow_move_assignable<T>::value</tt> 。
*/
class YF_API BitmapBuffer : protected Graphics
{
public:
	/*!
	\brief 无参数构造：默认实现。
	\note 零初始化。
	*/
	DefDeCtor(BitmapBuffer)
	/*!
	\brief 构造：使用指定位图指针和大小。
	*/
	BitmapBuffer(ConstBitmapPtr, SDst, SDst);
	BitmapBuffer(const BitmapBuffer&);
	/*!
	\brief 转移构造：转移资源。
	\since build 296
	*/
	BitmapBuffer(BitmapBuffer&&) ynothrow;
	virtual DefClone(BitmapBuffer, Clone)
	/*!
	\brief 析构：释放资源。
	*/
	virtual
	~BitmapBuffer()
	{
		ydelete_array(pBuffer);
	}

	/*
	\brief 复制赋值：使用复制构造函数和交换函数。
	\since build 296
	*/
	BitmapBuffer&
	operator=(const BitmapBuffer& buf)
	{
		BitmapBuffer(buf).Swap(*this);
		return *this;
	}
	/*
	\brief 转移赋值：使用转移构造函数和交换函数。
	\since build 296
	*/
	BitmapBuffer&
	operator=(BitmapBuffer&& buf) ynothrow
	{
		Swap(buf);
		return *this;
	}

	//! \since build 319
	//@{
	using Graphics::operator!;

	using Graphics::operator bool;
	//@}

	//! \since build 296
	//@{
	using Graphics::GetBufferPtr;
	using Graphics::GetHeight;
	using Graphics::GetWidth;
	using Graphics::GetSize;
	using Graphics::GetSizeOfBuffer;
	DefGetter(const ynothrow, const Graphics&, Context, *this)
	//@}

	/*!
	\brief 设置内容。
	\note 预先设置缓冲区大小。
	\since build 319
	*/
	void
	SetContent(ConstBitmapPtr, SDst, SDst);
	/*!
	\brief 重新设置缓冲区大小。
	\note 若有一边为零则删除缓冲区；若大于缓冲区空间则重新分配；
		设置后清除缓冲区。
	*/
	virtual void
	SetSize(SDst, SDst);
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

	/*
	\brief 交换。
	\since build 296
	*/
	void
	Swap(BitmapBuffer& buf) ynothrow
	{
		std::swap<Graphics>(*this, buf);
	}
};


/*!
\brief 扩展的正则矩形位图缓冲区。
\note 满足 <tt>std::is_nothrow_move_constructible<T>::value &&
	std::is_nothrow_move_assignable<T>::value</tt> 。
*/
class YF_API BitmapBufferEx : public BitmapBuffer
{
protected:
	u8* pBufferAlpha; //!<  Alpha 缓冲区指针。

public:
	/*!
	\brief 无参数构造。
	\note 零初始化。
	*/
	BitmapBufferEx()
		: BitmapBuffer(), pBufferAlpha()
	{}
	/*!
	\brief 构造：使用指定位图指针和大小。
	*/
	BitmapBufferEx(ConstBitmapPtr, SDst, SDst);
	BitmapBufferEx(const BitmapBufferEx&);
	/*!
	\brief 转移构造：转移资源。
	\since build 296
	*/
	BitmapBufferEx(BitmapBufferEx&&) ynothrow;
	virtual DefClone(BitmapBufferEx, Clone)
	/*!
	\brief 析构：释放资源。
	*/
	~BitmapBufferEx() override
	{
		ydelete_array(pBufferAlpha);
	}

	/*
	\brief 复制赋值：使用复制构造函数和交换函数。
	\since build 296
	*/
	BitmapBufferEx&
	operator=(const BitmapBufferEx& buf)
	{
		BitmapBufferEx(buf).Swap(*this);
		return *this;
	}
	/*
	\brief 转移赋值：使用转移构造函数和交换函数。
	\since build 296
	*/
	BitmapBufferEx&
	operator=(BitmapBufferEx&& buf) ynothrow
	{
		Swap(buf);
		return *this;
	}

	DefGetter(const ynothrow, u8*, BufferAlphaPtr, pBufferAlpha) \
		//!< 取 Alpha 缓冲区的指针。
	DefGetter(const ynothrow, size_t, SizeOfBufferAlpha, sizeof(u8)
		* GetAreaOf(GetSize())) //!< 取 Alpha 缓冲区占用空间。

	/*!
	\brief 重新设置缓冲区大小。
	\note 若有一边为零则删除缓冲区；若大于缓冲区空间则重新分配；
		设置后清除缓冲区。
	*/
	void
	SetSize(SDst, SDst) override;

	/*!
	\brief 清除缓冲区。
	*/
	void
	ClearImage() const override;

	/*
	\brief 交换。
	\since build 296
	*/
	void
	Swap(BitmapBufferEx& buf) ynothrow
	{
		std::swap<BitmapBuffer>(*this, buf),
		std::swap(pBufferAlpha, buf.pBufferAlpha);
	}
};


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
\brief 刷新：位图缓冲区向指针指定的缓冲区复制。
\note 仅当指针和指向有效。自动裁剪适应大小。

向指定大小和点（相对左上角）的指定图形接口上下文以指定输出指向复制缓冲区内容。
*/
YF_API bool
CopyTo(BitmapPtr, const BitmapBufferEx&, const Size&,
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
\brief 刷新：位图缓冲区向图形接口上下文复制。
\note 仅当指针和指向有效。自动裁剪适应大小。
\since build 337

向指定大小和点（相对左上角）的指定图形接口上下文以指定输出指向复制缓冲区内容。
*/
inline bool
CopyTo(const Graphics& dst, const BitmapBufferEx& src,
	const Point& dp = {}, const Point& sp = {}, Rotation rot = RDeg0)
{
	return CopyTo(dst.GetBufferPtr(), src, dst.GetSize(),
		dp, sp, src.GetSize(), rot);
}

/*!
\brief 贴图：位图缓冲区向指针指定的缓冲区以贴图算法复制。
\note 仅当指针和指向有效。自动裁剪适应大小。

向指定大小和点（相对左上角）的指定图形接口上下文以指定输出指向以缓冲区内容贴图。
*/
YF_API bool
BlitTo(BitmapPtr, const BitmapBufferEx&, const Size&,
	const Point&, const Point&, const Size&, Rotation = RDeg0);
/*!
\brief 贴图：位图缓冲区向指针指定的缓冲区以贴图算法复制。
\note 仅当指针和指向有效。自动裁剪适应大小。
\since build 337

向指定大小和点（相对左上角）的指定图形接口上下文以指定输出指向以缓冲区内容贴图。
*/
inline bool
BlitTo(const Graphics& dst, const BitmapBufferEx& src,
	const Point& dp = {}, const Point& sp = {}, Rotation rot = RDeg0)
{
	return BlitTo(dst.GetBufferPtr(), src, dst.GetSize(),
		dp, sp, src.GetSize(), rot);
}

YSL_END_NAMESPACE(Drawing)

YSL_END

#endif

