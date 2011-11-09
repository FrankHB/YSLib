/*
	Copyright (C) by Franksoft 2009 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ygdi.cpp
\ingroup Service
\brief 平台无关的图形设备接口。
\version r3525;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2009-12-14 18:29:46 +0800;
\par 修改时间:
	2011-11-05 11:24 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::YGDI;
*/


#include "YSLib/Service/ygdi.h"
#include "YSLib/Service/yblit.h"

using namespace ystdex;

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

Padding::Padding(SDst l, SDst r, SDst t, SDst b)
	: Left(l), Right(r), Top(t), Bottom(b)
{}
/*Padding::Padding(u64 m)
	: Left(m >> 48), Right((m >> 32) & 0xFFFF),
	Top((m >> 16) & 0xFFFF), Bottom(m & 0xFFFF)
{}*/

Padding&
Padding::operator+=(const Padding& m)
{
	Left += m.Left;
	Right += m.Right;
	Top += m.Top;
	Bottom += m.Bottom;
	return *this;
}


Padding
operator+(const Padding& a, const Padding& b)
{
	return Padding(a.Left + b.Left, a.Right + b.Right,
		a.Top + b.Top, a.Bottom + b.Bottom);
}
Rect
operator+(const Rect& r, const Padding& m)
{
	return Rect(r.X + m.Left, r.Y + m.Top,
		vmax<int>(0, r.Width - GetHorizontalOf(m)),
		vmax<int>(0, r.Height - GetVerticalOf(m)));
}


u64
GetAllOf(Padding& m)
{
	u64 r = (m.Left << 16) | m.Right;

	r = (r << 32) | (m.Top << 16) | m.Bottom;
	return r;
}

void
SetAllOf(Padding& m, SDst left, SDst right, SDst top, SDst bottom)
{
	m.Left = left;
	m.Right = right;
	m.Top = top;
	m.Bottom = bottom;
}


Padding
FetchMargin(const Rect& r, const Size& s)
{
	return Padding(vmax<int>(0, r.X), vmax<int>(0, s.Width - r.X - r.Width),
		vmax<int>(0, r.Y), vmax<int>(0, s.Height - r.Y - r.Height));
}


BitmapBuffer::BitmapBuffer(ConstBitmapPtr i, SDst w, SDst h)
	: Graphics()
	//不能提前初始化 size ，否则指针非空和面积非零状态不一致。
{
	SetSize(w, h);
	if(i)
		mmbcpy(pBuffer, i, GetSizeOfBuffer());
}
BitmapBuffer::BitmapBuffer(const BitmapBuffer& buf)
	: Graphics()
{
	SetSize(buf.GetWidth(), buf.GetHeight());
	if(auto p = buf.GetBufferPtr())
		mmbcpy(pBuffer, p, GetSizeOfBuffer());
}

void
BitmapBuffer::SetSize(SDst w, SDst h)
{
	u32 s(w * h);

	if(s == 0)
	{
		ydelete_array(pBuffer);
		pBuffer = nullptr;
	}
	else if(GetAreaOf(GetSize()) < s)
		try
		{
			BitmapPtr pBufferNew(ynew PixelType[s]);

			std::swap(pBuffer, pBufferNew);
			ydelete_array(pBufferNew);
		}
		catch(std::bad_alloc&)
		{
			throw LoggedEvent("Allocation failed @ BitmapBuffer::SetSize;", 1);
		}

	YAssert(!((pBuffer != nullptr) ^ (s != 0)), "Buffer corruptied"
		" @ BitmapBuffer::SetSize;");

	size.Width = w;
	size.Height = h;
	ClearImage();
}
void
BitmapBuffer::SetSizeSwap()
{
	std::swap(size.Width, size.Height);
	ClearImage();
}

void
BitmapBuffer::ClearImage() const
{
	Drawing::ClearImage(*this);
}

void
BitmapBuffer::BeFilledWith(Color c) const
{
	Fill(*this, c);
}


BitmapBufferEx::BitmapBufferEx(ConstBitmapPtr i, SDst w, SDst h)
	: BitmapBuffer(), pBufferAlpha()
{
	SetSize(w, h);
	if(i)
		mmbcpy(pBuffer, i, GetSizeOfBuffer());
}
BitmapBufferEx::BitmapBufferEx(const BitmapBufferEx& buf)
	: BitmapBuffer(), pBufferAlpha()
{
	SetSize(buf.GetWidth(), buf.GetHeight());
	if(auto p = buf.GetBufferPtr())
	{
		mmbcpy(pBuffer, p, GetSizeOfBuffer());
		mmbcpy(pBufferAlpha, buf.GetBufferAlphaPtr(), GetSizeOfBufferAlpha());
	}
}

void
BitmapBufferEx::SetSize(SDst w, SDst h)
{
	u32 s(w * h);

	if(s == 0)
	{
		ydelete_array(pBuffer);
		pBuffer = nullptr;
		ydelete_array(pBufferAlpha);
		pBufferAlpha = nullptr;
	}
	else if(GetAreaOf(size) < s)
	{
		BitmapPtr pBufferNew(nullptr);

		try
		{
			pBufferNew = ynew PixelType[s];

			u8* pBufferAlphaNew(ynew u8[s]);

			std::swap(pBuffer, pBufferNew);
			ydelete_array(pBufferNew);
			std::swap(pBufferAlpha, pBufferAlphaNew);
			ydelete_array(pBufferAlphaNew);
		}
		catch(std::bad_alloc&)
		{
			ydelete_array(pBufferNew);
			throw LoggedEvent("Allocation failed"
				" @ BitmapBufferEx::SetSize;", 1);
		}
	}

	YAssert(!((pBuffer != nullptr) ^ (s != 0)), "Buffer corruptied"
		" @ BitmapBufferEx::SetSize;");
	YAssert(!((pBufferAlpha != nullptr) ^ (s != 0)), "Buffer corruptied"
		" @ BitmapBufferEx::SetSize;");

	size.Width = w;
	size.Height = h;
	ClearImage();
}

void
BitmapBufferEx::ClearImage() const
{
	const u32 t = GetAreaOf(size);

	ClearPixel(pBuffer, t);
	ClearPixel(pBufferAlpha, t);
}

bool
CopyTo(BitmapPtr dst, const Graphics& g, const Size& ds,
	const Point& dp, const Point& sp, const Size& sc, Rotation rot)
{
	if(~rot & 1 && dst && g.IsValid())
	{
		(rot == RDeg0
			? Blit<BlitLoop, false, false, BitmapPtr, ConstBitmapPtr>
			: Blit<BlitLoop, true, true, BitmapPtr, ConstBitmapPtr>)(
			dst, ds, g.GetBufferPtr(), g.GetSize(), dp, sp, sc);
		return true;
	}
	return false;
}
bool
CopyTo(BitmapPtr dst, const BitmapBufferEx& buf, const Size& ds,
	const Point& dp, const Point& sp, const Size& sc, Rotation rot)
{
	if(~rot & 1 && dst && buf.IsValid())
	{
		(rot == RDeg0
			? Blit<BlitTransparentLoop, false, false, BitmapPtr, IteratorPair>
			: Blit<BlitTransparentLoop, true, true, BitmapPtr, IteratorPair>)(
			dst, ds, IteratorPair(buf.GetBufferPtr(), buf.GetBufferAlphaPtr()),
			buf.GetSize(), dp, sp, sc);
		return true;
	}
	return false;
}

bool
BlitTo(BitmapPtr dst, const BitmapBufferEx& buf, const Size& ds,
	const Point& dp, const Point& sp, const Size& sc, Rotation rot)
{
	if(~rot & 1 && dst && buf.IsValid())
	{
		(rot == RDeg0
			? Blit<BlitBlendLoop, false, false, BitmapPtr, IteratorPair>
			: Blit<BlitBlendLoop, true, true, BitmapPtr, IteratorPair>)(
			dst, ds, IteratorPair(buf.GetBufferPtr(), buf.GetBufferAlphaPtr()),
			buf.GetSize(), dp, sp, sc);
		return true;
	}
	return false;
}

YSL_END_NAMESPACE(Drawing)

YSL_END

