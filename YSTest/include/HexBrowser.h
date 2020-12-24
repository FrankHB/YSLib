/*
	© 2011-2015, 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file HexBrowser.h
\ingroup YReader
\brief 十六进制浏览器。
\version r556
\author FrankHB <frankhb1989@gmail.com>
\since build 253
\par 创建时间:
	2011-10-14 18:13:04 +0800
\par 修改时间:
	2020-12-24 12:06 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::HexBrowser
*/


#ifndef INC_YReader_HexBrowser_h_
#define INC_YReader_HexBrowser_h_ 1

#include <YSLib/UI/YModules.h>
#include <Helper/YModules.h>
#include YFM_YSLib_UI_Scroll
#include YFM_YSLib_Service_TextLayout
#include YFM_Helper_Initialization

namespace YSLib
{

namespace UI
{

/*!
\ingroup DataModels
\brief 十六进制模型：十六进制视图区域数据源。
\since build 257
*/
class HexModel final
{
private:
	/*!
	\brief 文件流数据源。
	\since build 619
	*/
	mutable filebuf source{};
	//! \since build 597
	size_t size = 0;

public:
	DefDeCtor(HexModel)
	/*!
	\brief 构造：以二进制只读模式打开文件并初始化大小。
	\pre 间接断言：路径参数非空。
	\since build 412
	*/
	YB_NONNULL(2)
	HexModel(const char*);
	DefDeMoveCtor(HexModel)

	DefDeMoveAssignment(HexModel)

	DefPred(const ynothrow, Valid, source.is_open())

public:
	/*!
	\brief 取当前读取位置。
	\return 若成功为当前缓冲区读取位置相对于开始的字节数，否则为 size_t(-1) 。
	\since build 587
	*/
	DefGetter(const, size_t, Position,
		size_t(source.pubseekoff(0, std::ios_base::cur, std::ios_base::in)))
	DefGetter(const ynothrow, size_t, Size, size)

	//! \since build 620
	DefBoolNeg(explicit, source.is_open())

	/*!
	\pre 间接断言；参数非空。
	\since build 597
	*/
	PDefH(std::streamsize, Fill, char* dst, std::streamsize n) const
		ImplRet(source.sgetn(Nonnull(dst), n))

	//! \since build 632
	PDefH(void, Seek, std::streamoff off) const
		ImplExpr(source.pubseekoff(off, std::ios_base::beg, std::ios_base::in))
};


/*!
\brief 十六进制视图。
\warning 非虚析构。
\since build 257
*/
class HexView
{
public:
	static yconstexpr const size_t ItemPerLine = 8; //!< 每行数据总数（字节）。

	using IndexType = std::uintptr_t; //!< 索引类型。
	using DataType = vector<char>; //!< 显示数据类型。

protected:
	Drawing::TextState TextState; //!< 文本状态。

private:
	IndexType item_num; //!< 行数。

protected:
	/*!
	\brief 当前显示的数据。
	\since 356
	*/
	DataType datCurrent;

public:
	//! \since build 396
	HexView(FontCache& = FetchDefaultFontCache());

public:
	/*!
	\brief 取项目行高。
	*/
	DefGetter(const ynothrow, SDst, ItemHeight,
		GetTextLineHeightExOf(TextState))
	DefGetter(const ynothrow, IndexType, ItemNum, item_num)

protected:
	PDefH(IndexType, UpdateItemNum, SDst h)
		ImplRet(item_num = FetchResizedLineN(TextState, h))
};


/*!
\brief 十六进制视图区域。
\since build 253
*/
class HexViewArea : public ScrollableContainer, protected HexView
{
public:
	using HexView::ItemPerLine;
	using HexView::IndexType;
	using HexView::DataType;
	/*!
	\brief 视图参数类型。
	\note bool 参数表示主动定位视图。
	\since build 269
	*/
	using ViewArgs = GValueEventArgs<bool>;

private:
	/*!
	\brief 模型。
	*/
	HexModel model;

public:
	/*!
	\brief 视图变更事件。
	\since build 269
	*/
	GEvent<void(ViewArgs)> ViewChanged;

	//! \since build 396
	explicit
	HexViewArea(const Drawing::Rect& = {},
		FontCache& = FetchDefaultFontCache());

	//! \since build 620
	DefBoolNeg(explicit, bool(model))

	using HexView::GetItemHeight;
	using HexView::GetItemNum;
	DefGetter(const ynothrow, const HexModel&, Model, model)

	/*!
	\pre 间接断言：路径参数非空。
	\since build 412
	*/
	YB_NONNULL(2) void
	Load(const char*);

	/*!
	\brief 定位视图顶端至指定竖直位置（行数）。
	\since build 474
	*/
	void
	LocateViewPosition(size_t);

	/*!
	\brief 刷新：按指定参数绘制界面并更新状态。
	\since build 294
	*/
	void
	Refresh(PaintEventArgs&&) override;

	void
	Reset();

	void
	UpdateData(std::uint32_t);

	/*!
	\brief 更新视图。

	调用视图变更事件、调整视图长度后无效化自身。
	\note 参数表示是否确定仅变更视图位置。
	\since build 269
	*/
	void
	UpdateView(bool = {});
};

} // namespace UI;

} // namespace YSLib;

#endif

