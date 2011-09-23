/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstdio.h
\ingroup YCLib
\brief ISO C 标准输入/输出扩展。
\version r1187;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-09-21 08:30:08 +0800;
\par 修改时间:
	2011-09-23 10:25 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YStandardExtend::CStandardIO;
*/


#ifndef YCL_INC_YSTDEX_CSTDIO_H_
#define YCL_INC_YSTDEX_CSTDIO_H_

#include "../ydef.h"
#include <cstdio>
#include <iterator>
#include <utility>

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
		int, ptrdiff_t, const int*, const int&>
	{
	protected:
		typedef std::iterator<std::input_iterator_tag,
			int, ptrdiff_t, const int*, const int&> traits_type;

	public:
		typedef int char_type;
		typedef std::FILE istream_type;

	private:
		istream_type* stream; //!< 流指针。
		int value;

	public:
		/*!
		\brief 无参数构造。

		构造空流迭代器。
		\post <tt>stream == nullptr</tt> 。
		*/
		ifile_iterator();
		/*!
		\brief 构造：使用流指针。
		\post <tt>stream == &s</tt> 。
		*/
		ifile_iterator(istream_type& s);
		/*!
		\brief 复制构造：默认实现。
		*/
		ifile_iterator(const ifile_iterator&) = default;
		ifile_iterator(istream_type*);
		~ifile_iterator() = default;
		
		reference
		operator*() const;
		pointer
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

		friend bool
		operator==(const ifile_iterator&, const ifile_iterator&);
	};

	inline
	ifile_iterator::ifile_iterator()
		: stream(), value()
	{}
	inline
	ifile_iterator::ifile_iterator(istream_type& s)
		: stream(&s), value()
	{}

	inline ifile_iterator::reference
	ifile_iterator::operator*() const
	{
		return value;
	}

	inline ifile_iterator::pointer
	ifile_iterator::operator->() const
	{
		return &**this;
	}

	inline ifile_iterator&
	ifile_iterator::operator++()
	{
		assert(stream);

		value = fgetc(stream);
		return *this;
	}

	inline ifile_iterator
	ifile_iterator::operator++(int)
	{
		const auto i(*this);

		++*this;
		return i;
	}

	inline bool
	operator==(const ifile_iterator& x, const ifile_iterator& y)
	{
		return x.stream == y.stream;
	}
	inline bool
	operator!=(const ifile_iterator& x, const ifile_iterator& y)
	{
		return !(x == y);
	}
}

#endif

