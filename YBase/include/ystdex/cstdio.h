/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstdio.h
\ingroup YStandardEx
\brief ISO C 标准输入/输出扩展。
\version r1229;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-09-21 08:30:08 +0800;
\par 修改时间:
	2011-10-10 19:28 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YStandardEx::CStandardIO;
*/


#ifndef YCL_INC_YSTDEX_CSTDIO_H_
#define YCL_INC_YSTDEX_CSTDIO_H_

#include "../ydef.h"
#include <cstdio>
#include <iterator>
#include <utility>
#include "memory.hpp"

namespace ystdex
{
	/*!
	\brief 路径类型定义。
	*/
	//@{
	typedef char* path_t;
	typedef const char* const_path_t;
	//@}

	/*!
	\brief 判断指定路径的文件是否存在。
	\note 使用 ISO C 标准库实现。
	*/
	bool
	fexists(const_path_t);


	/*!
	\brief ISO C 标准流只读迭代器。
	*/
	class ifile_iterator : public std::iterator<std::input_iterator_tag,
		byte, ptrdiff_t, const byte*, const byte&>
	{
	protected:
		typedef std::iterator<std::input_iterator_tag,
			byte, ptrdiff_t, const byte*, const byte&> traits_type;

	public:
		typedef byte char_type;
		typedef std::FILE istream_type;

	private:
		istream_type* stream; //!< 流指针。
		char_type value;

	public:
		/*!
		\brief 无参数构造。

		构造空流迭代器。
		\post <tt>stream == nullptr</tt> 。
		*/
		yconstexprf
		ifile_iterator();
		/*!
		\brief 构造：使用流指针。
		\post <tt>stream == &s</tt> 。
		*/
		yconstexprf
		ifile_iterator(istream_type& s);
		/*!
		\brief 复制构造：默认实现。
		*/
		yconstexprf
		ifile_iterator(const ifile_iterator&) = default;
		~ifile_iterator() = default;
		
		yconstexprf reference
		operator*() const;
		yconstexprf pointer
		operator->() const;

		/*
		\brief 前置自增。
		\pre 断言检查： stream 。

		读入字符。
		\return 自身引用。
		*/
		ifile_iterator&
		operator++();
		/*
		\brief 后置自增。
		\pre 断言检查：同前置自增。

		读入字符。
		\return 迭代器副本。
		*/
		ifile_iterator
		operator++(int);

		friend yconstexprf bool
		operator==(const ifile_iterator&, const ifile_iterator&);

		yconstexprf istream_type*
		get_stream() const;
	};

	yconstexprf
	ifile_iterator::ifile_iterator()
		: stream(), value()
	{}
	yconstexprf
	ifile_iterator::ifile_iterator(istream_type& s)
		: stream(&s), value()
	{}

	yconstexprf ifile_iterator::reference
	ifile_iterator::operator*() const
	{
		return value;
	}

	yconstexprf ifile_iterator::pointer
	ifile_iterator::operator->() const
	{
		return &**this;
	}

	inline ifile_iterator&
	ifile_iterator::operator++()
	{
		assert(stream);

		value = static_cast<unsigned>(fgetc(stream));
		return *this;
	}

	inline ifile_iterator
	ifile_iterator::operator++(int)
	{
		const auto i(*this);

		++*this;
		return i;
	}

	yconstexprf ifile_iterator::istream_type*
	ifile_iterator::get_stream() const
	{
		return stream;
	}

	yconstexprf bool
	operator==(const ifile_iterator& x, const ifile_iterator& y)
	{
		return x.stream == y.stream;
	}
	yconstexprf bool
	operator!=(const ifile_iterator& x, const ifile_iterator& y)
	{
		return !(x == y);
	}

	/*!
	\ingroup is_dereferencable
	\brief 判断 ifile_iterator 实例是否确定为可解引用。
	*/
	inline bool
	is_dereferencable(const ifile_iterator& i)
	{
		return !std::feof(i.get_stream());
	}

	/*!
	\ingroup is_undereferencable
	\brief 判断 ifile_iterator 实例是否确定为不可解引用。
	*/
	inline bool
	is_undereferencable(const ifile_iterator& i)
	{
		return std::feof(i.get_stream());
	}
}

#endif

