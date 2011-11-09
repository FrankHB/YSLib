/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HexBrowser.h
\ingroup YReader
\brief 十六进制浏览器。
\version r1262;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-10-14 18:13:04 +0800;
\par 修改时间:
	2011-11-05 10:50 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YReader::HexBrowser;
*/


#ifndef INCLUDED_HEXVIEWER_H_
#define INCLUDED_HEXVIEWER_H_

#include <YSLib/UI/scroll.h>
#include <YSLib/Service/yfile.h>
#include <YSLib/Service/ytext.h>
#include <YSLib/Helper/yglobal.h>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 十六进制模型：十六进制视图区域数据源。
\warning 非虚析构。
*/
class HexModel
{
protected:
	File Source; //!< 文件数据源。

public:
	DefGetter(const File&, Source, Source)
};


/*!
\brief 十六进制视图。
\warning 非虚析构。
*/
class HexView
{
public:
	static yconstexpr size_t ItemPerLine = 8; //!< 每行数据总数（字节）。

	typedef std::uintptr_t IndexType; //!< 索引类型。
	typedef vector<char> DataType; //!< 显示数据类型。

protected:
	Drawing::TextState TextState; //!< 文本状态。

private:
	IndexType item_num; //!< 行数。
	DataType data; //!< 当前显示的数据。

public:
	HexView(FontCache& = FetchGlobalInstance().GetFontCache());

protected:
	PDefHOperator(char&, [], DataType::size_type i)
		ImplRet(data[i])

public:
	/*!
	\brief 取项目行高。
	*/
	SDst
	GetItemHeight() const;
	DefGetter(IndexType, ItemNum, item_num)

protected:
	DefGetter(DataType::const_iterator, Begin, data.cbegin())
	DefMutableGetter(DataType::iterator, Begin, data.begin())
	DefGetter(DataType::const_iterator, End, data.cend())
	DefMutableGetter(DataType::iterator, End, data.end())

	PDefH(void, ClearData)
		ImplExpr(data.clear())

	PDefH(void, ResizeData, DataType::size_type s)
		ImplExpr(data.resize(s))

	PDefH(IndexType, UpdateItemNum, SDst h)
		ImplRet(item_num = FetchResizedLineN(TextState, h))
};

inline SDst
HexView::GetItemHeight() const
{
	return GetTextLineHeightExOf(TextState);
}


/*!
\brief 十六进制视图区域。
*/
class HexViewArea : public ScrollableContainer,
	protected HexModel, protected HexView
{
public:
	using HexView::ItemPerLine;
	using HexView::IndexType;
	using HexView::DataType;

	explicit
	HexViewArea(const Drawing::Rect& = Drawing::Rect::Empty,
		FontCache& = FetchGlobalInstance().GetFontCache());

	using HexView::GetItemHeight;
	using HexView::GetItemNum;
	/*!
	\brief 取包含指定点且被指定谓词过滤的顶端部件指针。
	*/
	virtual IWidget*
	GetTopWidgetPtr(const Point&, bool(&)(const IWidget&));
	using HexModel::GetSource;

	void
	Load(const_path_t);

	/*!
	\brief 定位视图顶端至指定竖直位置。
	*/
	void
	LocateViewPosition(SDst);

	virtual Rect
	Refresh(const PaintContext&);

	void
	Reset();

	void
	UpdateData(u32);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

