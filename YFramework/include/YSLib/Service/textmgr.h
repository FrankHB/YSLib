/*
	© 2010-2013 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textmgr.h
\ingroup Service
\brief 文本管理服务。
\version r3822
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-01-05 17:48:09 +0800
\par 修改时间:
	2013-12-11 08:59 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Service::TextManager
*/


#ifndef YSL_INC_Service_textmgr_h_
#define YSL_INC_Service_textmgr_h_ 1

#include "../Core/ystring.h"
#include "yftext.h"
#include "../Adaptor/ycont.h"

namespace YSLib
{

namespace Text
{

/*
\brief 文本文件块缓冲区。
\since build 145
*/
class YF_API TextFileBuffer
{
public:
	/*!
	\brief 缓冲区块类型。

	保存转换后的文本区块和初始转换偏移状态。
	\since build 273
	*/
	using BlockType = pair<vector<ucs2_t>, size_t>;
	/*!
	\brief 缓冲映射类型。

	区块号到指定缓冲区快的映射。
	\since build 273
	*/
	using MapType = map<size_t, BlockType>;
	/*!
	\brief 目标编码迭代器类型。
	\since build 460
	*/
	//@{
	class YF_API iterator : public std::iterator<
		std::bidirectional_iterator_tag, ucs2_t, ptrdiff_t, const ucs2_t*,
		const ucs2_t&>
	{
		friend class TextFileBuffer;

	private:
		//! \since build 460
		TextFileBuffer* p_buffer;
		//文本读取位置。
		size_t block;
		size_t index;

	public:
		/*!
		\brief 构造：指定文本读取位置。
		*/
		iterator(TextFileBuffer* = {}, size_t = 0, size_t = 0) ynothrow;

		/*!
		\brief 迭代：循环向后遍历。
		*/
		iterator&
		operator++() ynothrow;

		/*!
		\brief 迭代：循环向前遍历。
		*/
		iterator&
		operator--() ynothrow;

		//! \since build 460
		reference
		operator*() const ynothrow;

		/*!
		\brief 比较：相等关系。
		*/
		YF_API friend bool
		operator==(const iterator&, const iterator&) ynothrow;

		DefGetter(const ynothrow, TextFileBuffer*, BufferPtr, p_buffer)
		DefGetter(const ynothrow, size_t, BlockN, block)
		DefGetter(const ynothrow, size_t, IndexN, index)
	};
	using const_iterator = iterator;
	//@}

	/*!
	\brief 默认文本区块大小。
	\since build 273
	*/
	static yconstexpr size_t BlockSize = 2048U;

protected:
	/*!
	\brief 文本文件引用。
	\since build 273
	*/
	TextFile& File;
	/*!
	\brief 文本大小。
	\since build 273
	*/
	size_t nTextSize;
	/*!
	\brief 区块数。
	\since build 273
	*/
	size_t nBlock;
	/*!
	\brief 缓冲映射。
	\since build 273
	*/
	MapType Map;

private:
	/*!
	\brief 固定编码每字符的字节数。
	\note 变长编码为 1 。
	\since build 273;
	*/
	size_t fixed_width;
	/*!
	\brief 最大编码每字符的字节数。
	\since build 273;
	*/
	size_t max_width;

public:
	/*!
	\brief 构造：使用文本文件。
	*/
	explicit
	TextFileBuffer(TextFile&);
	/*!
	\brief 析构：空实现。
	*/
	virtual DefDeDtor(TextFileBuffer)

	/*!
	\brief 块随机访问。
	\since build 273
	*/
	BlockType&
	operator[](size_t);

	/*!
	\brief 取缓冲区块数。
	\since build 273
	*/
	DefGetter(const ynothrow, size_t, BlockN, nBlock)
	DefGetter(const ynothrow, Encoding, Encoding, File.Encoding)
	DefGetterMem(const ynothrow, size_t, Size, File)
	DefGetter(const ynothrow, size_t, TextSize, nTextSize)

	/*!
	\brief 取文本字节位置对应的迭代器。
	\since build 273
	*/
	iterator
	GetIterator(size_t);
	/*!
	\brief 取迭代器对应的文本字节位置。
	\since build 273
	*/
	size_t
	GetPosition(iterator);

	//! \since build 460
	//@{
	/*!
	\brief 取文本缓冲区起始迭代器。
	\note 指向起始字符。
	*/
	iterator
	begin() ynothrow;

	/*!
	\brief 取文本缓冲区终止迭代器。
	\note 指向终止字符后一位置。
	*/
	iterator
	end() ynothrow;
	//@}
};

/*
\relates TextFileBuffer::iterator
\since build 460
*/
inline bool
operator!=(const TextFileBuffer::iterator& x, const TextFileBuffer::iterator y)
	ynothrow
{
	return !(x == y);
}


/*!
\brief 从文本文件缓冲区复制指定字节位置起始的字符串。
\return 不越界时，第三参数为零则为空串，否则为尽可能长但不大于指定长度的缓冲区内容。
\throw std::out_of_range 起始位置越界无法访问。
\note 仅抛出以上异常。
\note 可能刷新缓冲。
\since build 397
*/
YF_API string
CopySliceFrom(TextFileBuffer&, size_t, size_t) ythrow(std::out_of_range);

} // namespace Text;

} // namespace YSLib;

#endif

