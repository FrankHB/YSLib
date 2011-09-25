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
\version r4494;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-01-05 17:48:09 +0800;
\par 修改时间:
	2011-09-25 12:19 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Service::TextManager;
*/


#ifndef YSL_INC_SERVICE_TEXTMGR_H_
#define YSL_INC_SERVICE_TEXTMGR_H_

#include "../Core/ystring.h"
#include "yftext.h"
#include "../Adaptor/cont.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

//文本缓冲区块。
class TextBlock : protected vector<ucs2_t>
{
public:
	typedef vector<ucs2_t>::const_iterator const_iterator;
	typedef vector<ucs2_t>::iterator iterator;
	typedef u16 BlockSizeType;

	BlockSizeType Index;

	/*!
	\brief 构造：使用区块的大小和总长度。
	*/
	TextBlock(BlockSizeType, SizeType);
	/*!
	\brief 析构：空实现。
	*/
	virtual DefEmptyDtor(TextBlock)

	using vector<ucs2_t>::operator[];

	using vector<ucs2_t>::size;
	using vector<ucs2_t>::capacity;

	using vector<ucs2_t>::at;

	using vector<ucs2_t>::clear;

	using vector<ucs2_t>::resize;

	using vector<ucs2_t>::begin;
	using vector<ucs2_t>::end;
	using vector<ucs2_t>::cbegin;
	using vector<ucs2_t>::cend;
};

inline bool
operator==(const TextBlock& x, const TextBlock& y)
{
	return x.Index == y.Index;
}

inline bool
operator<(const TextBlock& x, const TextBlock& y)
{
	return x.Index < y.Index;
}


/*!
\brief 从文本文件 f 中读取连续 n 个字节到文本缓冲区块 b 中，并返回成功读取的字符数。
\note 超过最大长度则放弃读取。
*/
SizeType
LoadText(TextBlock& b, TextFile& f, SizeType n);


//文本缓冲区块映射。
class TextMap : public OwnershipTag<TextBlock>
{
public:
	typedef TextBlock::BlockSizeType BlockSizeType;
	typedef map<BlockSizeType, unique_ptr<TextBlock>> MapType;

protected:
	MapType Map;

public:
	/*!
	\brief 析构。
	\note 清理文本区块和映射。
	*/
	virtual DefEmptyDtor(TextMap)

	/*!
	\brief 取指定区块号的文本区块。
	*/
	unique_ptr<TextBlock>&
	operator[](const BlockSizeType&);
	/*!
	\brief 添加文本区块。
	*/
	void
	operator+=(TextBlock&);
	/*!
	\brief 移除指定区块号的文本区块。
	*/
	bool
	operator-=(const BlockSizeType&);

	/*!
	\brief 清理文本区块和映射。
	\note 释放所有的 TextBlock 对象。
	*/
	PDefH0(void, Clear)
		ImplRet(static_cast<void>(Map.clear()))
};

inline unique_ptr<TextBlock>&
TextMap::operator[](const BlockSizeType& id)
{
	return Map[id];
}

inline void
TextMap::operator+=(TextBlock& item)
{
	Map.insert(make_pair(item.Index, &item));
}
inline bool
TextMap::operator-=(const BlockSizeType& idx)
{
	return Map.erase(idx) != 0;
}


//文本文件块缓冲区。
class TextFileBuffer : public TextMap
{
public:
	typedef TextMap::BlockSizeType BlockSizeType; //!< 区块索引类型。

	static yconstexpr SizeType nBlockSize = 0x2000; //!< 文本区块容量。

	//只读文本循环迭代器类。
	class Iterator
	{
		friend class TextFileBuffer;

	public:
		typedef TextFileBuffer ContainerType;
		typedef ContainerType::BlockSizeType BlockSizeType;

	private:
		TextFileBuffer* pBuffer;
		//文本读取位置。
		BlockSizeType block;
		SizeType index;

	public:
		/*!
		\brief 构造：指定文本读取位置。
		\note 无异常抛出。
		*/
		Iterator(TextFileBuffer* = nullptr, BlockSizeType = 0, SizeType = 0)
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
		operator*() ynothrow;

		/*!
		\brief 迭代：重复循环向后遍历。
		\note 构造新迭代器进行迭代并返回。
		*/
		Iterator
		operator+(ptrdiff_t);

		/*!
		\brief 迭代：重复循环向前遍历。
		\note 构造新迭代器进行迭代并返回。
		*/
		Iterator
		operator-(ptrdiff_t);

		/*!
		\brief 比较：相等关系。
		*/
		friend bool
		operator==(const Iterator&, const Iterator&) ynothrow;

		/*!
		\brief 比较：不等关系。
		*/
		friend bool
		operator!=(const Iterator&, const Iterator&) ynothrow;

		/*!
		\brief 比较：严格偏序递增关系。
		*/
		friend bool
		operator<(const Iterator&, const Iterator&) ynothrow;

		/*!
		\brief 比较：严格偏序递减关系。
		*/
		friend bool
		operator>(const Iterator&, const Iterator&) ynothrow;

		/*!
		\brief 比较：非严格偏序递增关系。
		*/
		friend bool
		operator<=(const Iterator&, const Iterator&) ynothrow;

		/*!
		\brief 比较：非严格偏序递减关系。
		*/
		friend bool
		operator>=(const Iterator&, const Iterator&) ynothrow;

		/*!
		\brief 迭代：重复循环向后遍历。
		*/
		Iterator&
		operator+=(ptrdiff_t);

		/*!
		\brief 迭代：重复循环向前遍历。
		*/
		Iterator&
		operator-=(ptrdiff_t);

		DefGetter(TextFileBuffer*, BufferPtr, pBuffer)
		DefGetter(BlockSizeType, BlockN, block)
		/*!
		\brief 取文字缓冲区指针。
		\note 无异常抛出。
		*/
		DefGetter(SizeType, IndexN, index)
		const ucs2_t*
		GetTextPtr() const ynothrow;
		/*!
		\brief 取当前区块的长度。
		\note 无异常抛出。
		*/
		SizeType
		GetBlockLength() const ynothrow;
		/*!
		\brief 取指定区块号的区块的长度。
		\note 无异常抛出。
		*/
		SizeType
		GetBlockLength(BlockSizeType) const ynothrow;
	};

	TextFile& File; //!< 文本文件。

private:
	const SizeType nTextSize; //!< 文本区段长度。
	const BlockSizeType nBlock; //!< 文本区块数。

public:
	/*!
	\brief 构造：使用文本文件。
	*/
	explicit
	TextFileBuffer(TextFile&);

	DefGetter(SizeType, TextSize, nTextSize)

	/*!
	\brief 取指定区块号的区块。
	\throw std::out_of_range 指定的区块号对应下标超过文件大小。
	*/
	TextBlock&
	at(const BlockSizeType&);
	/*!
	\brief 取起始迭代器。
	\note 无异常抛出。
	*/
	Iterator
	begin() ynothrow;

	/*!
	\brief 取终止迭代器。
	\note 无异常抛出。
	*/
	Iterator
	end() ynothrow;
};

inline bool
operator!=(const TextFileBuffer::Iterator& lhs, const TextFileBuffer::Iterator& rhs)
	ynothrow
{
	return !(lhs == rhs);
}

inline bool
operator>(const TextFileBuffer::Iterator& lhs, const TextFileBuffer::Iterator& rhs)
	ynothrow
{
	return rhs < lhs;
}
inline bool
operator<=(const TextFileBuffer::Iterator& lhs, const TextFileBuffer::Iterator& rhs)
	ynothrow
{
	return !(rhs < lhs);
}

inline bool
operator>=(const TextFileBuffer::Iterator& x, const TextFileBuffer::Iterator& y)
	ynothrow
{
	return !(x < y);
}

inline TextFileBuffer::Iterator
TextFileBuffer::Iterator::operator-(ptrdiff_t o)
{
	return *this + -o;
}

inline TextFileBuffer::Iterator&
TextFileBuffer::Iterator::operator-=(ptrdiff_t o)
{
	return *this += -o;
}

inline SizeType
TextFileBuffer::Iterator::GetBlockLength() const ynothrow
{
	return GetBlockLength(block);
}

YSL_END_NAMESPACE(Text)

YSL_END

#endif

