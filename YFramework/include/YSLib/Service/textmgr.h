/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file textmgr.h
\ingroup Service
\brief 文本管理服务。
\version r4721;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-01-05 17:48:09 +0800;
\par 修改时间:
	2011-12-28 22:18 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::TextManager;
*/


#ifndef YSL_INC_SERVICE_TEXTMGR_H_
#define YSL_INC_SERVICE_TEXTMGR_H_

#include "../Core/ystring.h"
#include "yftext.h"
#include "../Adaptor/ycont.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

/*
\brief 文本文件块缓冲区。
\since build 145 。
*/
class TextFileBuffer
{
public:
	/*!
	\brief 缓冲区块类型。
	
	保存转换后的文本区块和初始转换偏移状态。
	\since build 273 。
	*/
	typedef pair<vector<ucs2_t>, size_t> BlockType;
	/*!
	\brief 缓冲映射类型。

	区块号到指定缓冲区快的映射。
	\since build 273 。
	*/
	typedef map<size_t, BlockType> MapType;
	/*!
	\brief 目标编码迭代器类型。
	\since build 273 。
	*/
	class Iterator : std::iterator<std::bidirectional_iterator_tag, ucs2_t,
		ptrdiff_t, const ucs2_t, const ucs2_t&>
	{
		friend class TextFileBuffer;

	private:
		TextFileBuffer* pBuffer;
		//文本读取位置。
		size_t block;
		size_t index;

	public:
		/*!
		\brief 构造：指定文本读取位置。
		\note 无异常抛出。
		*/
		Iterator(TextFileBuffer* = nullptr, size_t = 0, size_t = 0)
			ynothrow;

		/*!
		\brief 迭代：循环向后遍历。
		\note 无异常抛出。
		*/
		Iterator&
		operator++() ynothrow;

		/*!
		\brief 迭代：循环向前遍历。
		\note 无异常抛出。
		*/
		Iterator&
		operator--() ynothrow;

		ucs2_t
		operator*() const ynothrow;


		/*!
		\brief 比较：相等关系。
		*/
		friend bool
		operator==(const Iterator&, const Iterator&) ynothrow;

		DefGetter(const ynothrow, TextFileBuffer*, BufferPtr, pBuffer)
		DefGetter(const ynothrow, size_t, BlockN, block)
		DefGetter(const ynothrow, size_t, IndexN, index)
	};

	/*!
	\brief 默认文本区块大小。
	\since build 273 。
	*/
	static yconstexpr size_t nBlockSize = 2048U;

protected:
	/*!
	\brief 文本文件引用。
	\since build 273 。
	*/
	TextFile& File;
	/*!
	\brief 文本大小。
	\since build 273 。
	*/
	size_t nTextSize;
	/*!
	\brief 区块数。
	\since build 273 。
	*/
	size_t nBlock;
	/*!
	\brief 缓冲映射。
	\since build 273 。
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
	virtual DefEmptyDtor(TextFileBuffer)

	/*!
	\brief 块随机访问。
	\since build 273 。
	*/
	BlockType&
	operator[](size_t);

	/*!
	\brief 取缓冲区块数。
	\since build 273 。
	*/
	DefGetter(const ynothrow, size_t, BlockN, nBlock)
	DefGetterMem(const ynothrow, Encoding, Encoding, File)
	DefGetterMem(const ynothrow, size_t, Size, File)
	DefGetter(const ynothrow, size_t, TextSize, nTextSize)
	/*!
	\brief 取文本缓冲区起始迭代器。
	\note 指向起始字符。
	\since build 273 。
	*/
	Iterator
	GetBegin() ynothrow;
	/*!
	\brief 取文本缓冲区终止迭代器。
	\note 指向终止字符后一位置。
	\since build 273 。
	*/
	Iterator
	GetEnd() ynothrow;
	/*!
	\brief 取文本字节位置对应的迭代器。
	\since build 273 。
	*/
	Iterator
	GetIterator(size_t);
	/*!
	\brief 取迭代器对应的文本字节位置。
	\since build 273 。
	*/
	size_t
	GetPosition(Iterator);
};

inline bool
operator!=(const TextFileBuffer::Iterator& x, const TextFileBuffer::Iterator y)
	ynothrow
{
	return !(x == y);
}

YSL_END_NAMESPACE(Text)

YSL_END

#endif

