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
\version r1263;
\author FrankHB<frankhb1989@gmail.com>
\since build 245 。
\par 创建时间:
	2011-09-21 08:30:08 +0800;
\par 修改时间:
	2011-12-21 17:33 +0800;
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
	\since build 216 。
	*/
	//@{
	typedef char* path_t;
	typedef const char* const_path_t;
	//@}

	/*!
	\brief 判断指定路径的文件是否存在。
	\note 使用 ISO C 标准库实现。
	\since 早于 build 132 。
	*/
	bool
	fexists(const_path_t);


	/*!
	\brief 基于 ISO C 标准库的流只读迭代器。
	\since build 245 。
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
		yconstfn
		ifile_iterator();
		/*!
		\brief 构造：使用流指针。
		\post <tt>stream == &s</tt> 。
		*/
		ifile_iterator(istream_type& s);
		/*!
		\brief 复制构造：默认实现。
		*/
		yconstfn
		ifile_iterator(const ifile_iterator&) = default;
		~ifile_iterator() = default;
		
		yconstfn reference
		operator*() const;
		yconstfn pointer
		operator->() const;

		/*
		\brief 前置自增。

		使用 std::fgetc 读字符。
		\pre 断言检查：流指针非空。
		\note 当读到 EOF 时置流指针为空指针。
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

		friend yconstfn bool
		operator==(const ifile_iterator&, const ifile_iterator&);

		yconstfn istream_type*
		get_stream() const;
	};

	yconstfn
	ifile_iterator::ifile_iterator()
		: stream(), value()
	{}
	inline
	ifile_iterator::ifile_iterator(istream_type& s)
		: stream(&s)
	{
		++*this;
	}

	yconstfn ifile_iterator::reference
	ifile_iterator::operator*() const
	{
		return value;
	}

	yconstfn ifile_iterator::pointer
	ifile_iterator::operator->() const
	{
		return &**this;
	}

	inline ifile_iterator
	ifile_iterator::operator++(int)
	{
		const auto i(*this);

		++*this;
		return i;
	}

	yconstfn ifile_iterator::istream_type*
	ifile_iterator::get_stream() const
	{
		return stream;
	}

	yconstfn bool
	operator==(const ifile_iterator& x, const ifile_iterator& y)
	{
		return x.stream == y.stream;
	}
	yconstfn bool
	operator!=(const ifile_iterator& x, const ifile_iterator& y)
	{
		return !(x == y);
	}

	/*!
	\ingroup is_dereferencable
	\brief 判断 ifile_iterator 实例是否确定为可解引用。
	\since build 249 。
	*/
	inline bool
	is_dereferencable(const ifile_iterator& i)
	{
		return i.get_stream();
	}

	/*!
	\ingroup is_undereferencable
	\brief 判断 ifile_iterator 实例是否确定为不可解引用。
	\since build 250 。
	*/
	inline bool
	is_undereferencable(const ifile_iterator& i)
	{
		return !i.get_stream();
	}
}

#endif

