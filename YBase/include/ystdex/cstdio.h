/*
	© 2011-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstdio.h
\ingroup YStandardEx
\brief ISO C 标准输入/输出扩展。
\version r674
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2011-09-21 08:30:08 +0800
\par 修改时间:
	2016-03-17 14:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CStandardIO
*/


#ifndef YB_INC_ystdex_cstdio_h_
#define YB_INC_ystdex_cstdio_h_ 1

#include "cassert.h" // for <cstdio> and yconstraint;
#include <cstdarg> // for std::va_list;
#include <memory> // for std::unique_ptr;
#include <ios> // for std::ios_base::openmode;
#include "operators.hpp" // for input_iteratable;

namespace ystdex
{

/*!
\brief 计算指定格式字符串输出的占用的字节数。
\pre 断言：第一参数非空。
\return 成功为字节数，否则为 size_t(-1) 。
\note 分别使用 std::vsnprintf 和 std::vswprintf 实现。
\since build 564
*/
//@{
YB_API YB_NONNULL(1) size_t
vfmtlen(const char*, std::va_list) ynothrow;
YB_API YB_NONNULL(1) size_t
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
\note POSIX 管道流可使用函数实现的 \c ::pclose 代替 std::fclose 。
\since build 566
*/
using unique_file_ptr = std::unique_ptr<std::FILE, decltype(std::fclose)&>;


/*!
\brief ISO C/C++ 标准输入输出接口打开模式转换。
\since build 326
*/
//@{
/*!
\see ISO C++11 Table 132 。
\note 忽略 std::ios_base::ate 。
\see http://wg21.cmeerw.net/lwg/issue596 。
*/
YB_API YB_PURE const char*
openmode_conv(std::ios_base::openmode) ynothrow;
/*!
\brief ISO C/C++ 标准输入输出接口打开模式转换。
\return 若失败（包括空参数情形）为 std::ios_base::openmode() ，否则为对应的值。
\see ISO C11 7.21.5.3/3 。
\note 顺序严格限定。
\note 支持 x 转换。
\since build 326
*/
YB_API YB_PURE std::ios_base::openmode
openmode_conv(const char*) ynothrow;
//@}


/*!
\brief 基于 ISO C 标准库的流只读迭代器。
\since build 245
*/
class YB_API ifile_iterator
	: private input_iteratable<ifile_iterator, const byte&>
{
protected:
	using traits_type = std::iterator<std::input_iterator_tag, byte, ptrdiff_t,
		const byte*, const byte&>;

public:
	//! \since build 676
	//@{
	using iterator_category = std::input_iterator_tag;
	using value_type = byte;
	using difference_type = ptrdiff_t;
	using pointer = const byte*;
	using reference = const byte&;
	//@}
	using char_type = byte;
	//! \since build 607
	using int_type = int;

private:
	/*!
	\brief 流指针。
	\since build 458
	*/
	std::FILE* stream{};
	char_type value;

public:
	/*!
	\brief 无参数构造。
	\post <tt>!stream</tt> 。

	构造空流迭代器。
	*/
	yconstfn
	ifile_iterator()
		: value()
	{}
	/*!
	\brief 构造：使用流指针。
	\pre 断言： <tt>ptr</tt> 。
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

	//! \since build 600
	ifile_iterator&
	operator=(const ifile_iterator&) = default;

	//! \since build 461
	yconstfn reference
	operator*() const ynothrow
	{
		return value;
	}

	/*!
	\brief 前置自增。
	\pre 断言：流指针非空。
	\return 自身引用。
	\note 当读到 EOF 时置流指针为空指针。

	使用 std::fgetc 读字符。
	*/
	ifile_iterator&
	operator++();

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

	/*!
	\ingroup is_undereferenceable
	\since build 676
	*/
	friend bool
	is_undereferenceable(const ifile_iterator& i) ynothrow
	{
		return !i.get_stream();
	}

	/*!
	\brief 向流中写回字符。
	\since build 607
	*/
	//@{
	//! \pre 断言： <tt>!stream</tt> 。
	int_type
	sputbackc(char_type c)
	{
		yconstraint(stream);
		return std::ungetc(c, stream);
	}
	/*!
	\pre 断言： <tt>!stream || steram == s</tt> 。
	\post <tt>stream == s</tt> 。
	*/
	int_type
	sputbackc(char_type c, std::FILE* s)
	{
		yconstraint(!stream || stream == s);
		stream = s;
		return sputbackc(c);
	}

	//! \pre 间接断言： <tt>!stream</tt> 。
	int_type
	sungetc()
	{
		return sputbackc(value);
	}
	//! \pre 间接断言： <tt>!stream || steram == s</tt> 。
	int_type
	sungetc(std::FILE* s)
	{
		return sputbackc(value, s);
	}
	//@}
};


/*!
\brief 块缓冲。
\warning 非虚析构。
\since build 604
*/
class YB_API block_buffer
{
private:
	bool need_flush = {};
	std::unique_ptr<byte[]> content;

public:
	block_buffer(size_t n)
		: content(new byte[n]())
	{}
	block_buffer(std::unique_ptr<byte[]> p)
		: content(std::move(p))
	{}
	block_buffer(block_buffer&&) = default;

	block_buffer&
	operator=(block_buffer&&) = default;

	void
	clear() ynothrow
	{
		flush();
		content.reset();
	}

	bool
	empty() ynothrow
	{
		return !content;
	}

	/*!
	\brief 填充字节到缓冲区。
	\param offset 起始偏移字节数。
	\param n 填充的字节数。
	\param value 填充的字节值。
	\pre 断言：指针参数和 \c get() 结果非空。
	\since build 606
	*/
	void
	fill(size_t offset, size_t n = 1, byte value = {}) ynothrowv;

	void
	flush() ynothrow
	{
		need_flush = {};
	}

	byte*
	get() const ynothrow
	{
		return content.get();
	}

	bool
	modified() const ynothrow
	{
		return need_flush;
	}

	/*!
	\brief 读取缓冲区内容到外部存储。
	\param dst 外部存储的字节序列。
	\param offset 起始偏移字节数。
	\param n 复制的字节数。
	\pre dst 指向的存储至少 n 个字节可写。
	\pre 断言：指针参数和 \c get() 结果非空。
	*/
	void
	read(void* dst, size_t offset, size_t n) const ynothrowv;

	void
	set(std::unique_ptr<byte[]> p) ynothrow
	{
		content = std::move(p);
	}

	friend void
	swap(block_buffer& x, block_buffer& y) ynothrow
	{
		std::swap(x.need_flush, y.need_flush),
		x.content.swap(y.content);
	}

	/*!
	\brief 写入外部字节序列的内容到缓冲区。
	\param offset 起始偏移字节数。
	\param src 外部存储的字节序列。
	\param n 复制的字节数。
	\pre src 指向的存储至少 n 个字节可读。
	\pre 断言：指针参数和 \c get() 结果非空。
	*/
	void
	write(size_t offset, const void* src, size_t n) ynothrowv;
};

} // namespace ystdex;

#endif

