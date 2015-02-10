/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstdio.h
\ingroup YStandardEx
\brief ISO C 标准输入/输出扩展。
\version r458
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2011-09-21 08:30:08 +0800
\par 修改时间:
	2015-02-10 13:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CStandardIO
*/


#ifndef YB_INC_ystdex_cstdio_h_
#define YB_INC_ystdex_cstdio_h_ 1

#include "cassert.h" // for ../ydef.h, <cstdio> and yconstraint;
#include <cstdarg> // for std::va_list;
#include <memory> // for std::unique_ptr;
#include <ios> // for std::ios_base::openmode;
#include "deref_op.hpp" // for ystdex::is_undereferenceable;

namespace ystdex
{

/*!
\brief 计算指定格式字符串输出的占用的字节数。
\pre 断言：第一参数非空。
\return 成功为字节数，否则为 size_t(-1) 。
\note 分别使用 \c std::vsnprintf 和 \c std::vswprintf 实现。
\since build 564
*/
//@{
YB_API size_t
vfmtlen(const char*, std::va_list) ynothrow;
YB_API size_t
vfmtlen(const wchar_t*, std::va_list) ynothrow;
//@}


/*!
\brief 关闭流缓冲。
\pre 参数非空。
\since build 566
*/
inline int
setnbuf(std::FILE* stream) ynothrow
{
	yconstraint(stream);
	return std::setvbuf(stream, {}, _IONBF, 0);
}

/*!
\brief 判断指定路径的文件是否存在。
\note 使用 std::fopen 实现。
\pre 断言：参数非空。
\since build 326
*/
YB_API bool
fexists(const char*) ynothrow;


/*!
\brief 独占所有权的 C 标准库流指针或实现提供的等价类型。
\note POSIX 管道流可使用函数实现的 \c ::pclose 代替 \c std::fclose 。
\since build 566
*/
using unique_file_ptr = std::unique_ptr<std::FILE*, decltype(std::fclose)&>;


/*
\brief ISO C/C++ 标准输入输出接口打开模式转换。
\see ISO C++11 Table 132 。
\note 忽略 std::ios_base::ate 。
\since build 326
*/
YB_API const char*
openmode_conv(std::ios_base::openmode) ynothrow;
/*
\brief ISO C/C++ 标准输入输出接口打开模式转换。
\return 若失败（包括空参数情形）为 std::ios_base::openmode() ，否则为对应的值。
\see ISO C11 7.21.5.3/3 。
\note 顺序严格限定。
\note 支持 x 转换。
\since build 326
*/
YB_API std::ios_base::openmode
openmode_conv(const char*) ynothrow;


/*!
\brief 基于 ISO C 标准库的流只读迭代器。
\since build 245
*/
class YB_API ifile_iterator : public std::iterator<std::input_iterator_tag,
	byte, ptrdiff_t, const byte*, const byte&>
{
protected:
	using traits_type = std::iterator<std::input_iterator_tag, byte, ptrdiff_t,
		const byte*, const byte&>;

public:
	using char_type = byte;

private:
	/*!
	\brief 流指针。
	\since build 458
	*/
	std::FILE* stream;
	char_type value;

public:
	/*!
	\brief 无参数构造。
	\post <tt>!stream</tt> 。

	构造空流迭代器。
	*/
	yconstfn
	ifile_iterator()
		: stream(), value()
	{}
	/*!
	\brief 构造：使用流引用。
	\pre <tt>ptr</tt>。
	\post <tt>stream == ptr</tt> 。
	\since build 458
	*/
	explicit
	ifile_iterator(std::FILE* ptr)
		: stream(ptr)
	{
		yconstraint(ptr);
		++*this;
	}
	/*!
	\brief 复制构造：默认实现。
	*/
	yconstfn
	ifile_iterator(const ifile_iterator&) = default;
	~ifile_iterator() = default;

	//! \since build 461
	yconstfn reference
	operator*() const ynothrow
	{
		return value;
	}

	yconstfn pointer
	operator->() const
	{
		return &**this;
	}

	/*
	\brief 前置自增。
	\pre 断言：流指针非空。
	\return 自身引用。
	\note 当读到 EOF 时置流指针为空指针。

	使用 std::fgetc 读字符。
	*/
	ifile_iterator&
	operator++();
	/*
	\brief 后置自增。
	\pre 断言：同前置自增。
	\return 迭代器副本。

	读入字符。
	*/
	ifile_iterator
	operator++(int)
	{
		const auto i(*this);

		++*this;
		return i;
	}

	friend yconstfn bool
	operator==(const ifile_iterator& x, const ifile_iterator& y)
	{
		return x.stream == y.stream;
	}

	yconstfn std::FILE*
	get_stream() const
	{
		return stream;
	}
};

yconstfn bool
operator!=(const ifile_iterator& x, const ifile_iterator& y)
{
	return !(x == y);
}


/*!
\ingroup is_undereferenceable
\brief 判断 ifile_iterator 实例是否确定为不可解引用。
\since build 461
*/
inline bool
is_undereferenceable(const ifile_iterator& i) ynothrow
{
	return !i.get_stream();
}

} // namespace ystdex;

#endif

