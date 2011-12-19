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
\version r1335;
\author FrankHB<frankhb1989@gmail.com>
\since build 253 。
\par 创建时间:
	2011-10-14 18:13:04 +0800;
\par 修改时间:
	2011-12-17 09:37 +0800;
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
\since build 257 。
*/
class HexModel
{
private:
	/*!
	\brief 文件数据源。
	\note 保证非空。
	\since build 270 。
	*/
	unique_ptr<File> pSource;

public:
	HexModel();
	HexModel(const_path_t);
	DefDelCopyCtor(HexModel)
	DefDeMoveCtor(HexModel)

	DefDeMoveAssignment(HexModel)
	HexModel&
	operator=(unique_ptr<File>&&);

	DefPredMem(const ynothrow, Valid, GetSource())

private:
	DefGetter(const ynothrow, File&, Source, *pSource)

public:
	DefGetterMem(const ynothrow, FILE*, Ptr, GetSource())

	DefGetterMem(const ynothrow, File::OffsetType, Position, GetSource())
	DefGetterMem(const ynothrow, File::SizeType, Size, GetSource())

	DefFwdTmpl(const, File::OffsetType, SetPosition,
		GetSource().SetPosition(args...))

	DefFwdFn(const, int, CheckEOF, GetSource().CheckEOF())
};

inline
HexModel::HexModel()
	: pSource(new File())
{}
inline
HexModel::HexModel(const_path_t path)
	: pSource(new File(path))
{}
inline HexModel&
HexModel::operator=(unique_ptr<File>&& pFile)
{
	pSource = std::move(pFile);
	return *this;
}


/*!
\brief 十六进制视图。
\warning 非虚析构。
\since build 257 。
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

public:
	/*!
	\brief 取项目行高。
	*/
	SDst
	GetItemHeight() const;
	DefGetter(const ynothrow, IndexType, ItemNum, item_num)

protected:
	DefGetter(const ynothrow, DataType::const_iterator, Begin, data.cbegin())
	DefGetter(ynothrow, DataType::iterator, Begin, data.begin())
	DefGetter(const ynothrow, DataType::const_iterator, End, data.cend())
	DefGetter(ynothrow, DataType::iterator, End, data.end())

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
\since build 253 。
*/
class HexViewArea : public ScrollableContainer,
	protected HexView
{
public:
	using HexView::ItemPerLine;
	using HexView::IndexType;
	using HexView::DataType;
	/*!
	\brief 视图参数类型。
	\note bool 参数表示主动定位视图。
	\since build 269 。
	*/
	typedef GValueEventArgs<bool> ViewArgs;
	/*!
	\brief 视图事件委托类型。
	\since build 269 。
	*/
	DeclDelegate(HViewEvent, ViewArgs)

private:
	/*!
	\brief 模型。
	*/
	HexModel model;

public:
	/*!
	\brief 视图变更事件。
	\since build 269 。
	*/
	DeclEvent(HViewEvent, ViewChanged)

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
	DefGetter(const ynothrow, const HexModel&, Model, model)

	void
	Load(const_path_t);

	/*!
	\brief 定位视图顶端至指定竖直位置（行数）。
	*/
	void
	LocateViewPosition(u32);

	virtual Rect
	Refresh(const PaintContext&);

	void
	Reset();

	void
	UpdateData(u32);

	/*!
	\brief 更新视图。

	调用视图变更事件、调整视图长度后无效化自身。
	\note 参数表示是否确定仅变更视图位置。
	\since build 269 。
	*/
	void
	UpdateView(bool = false);
};

YSL_END_NAMESPACE(Components)

YSL_END

#endif

