/*
	© 2011-2016, 2018-2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cstdio.h
\ingroup YStandardEx
\brief ISO C 标准输入/输出扩展。
\version r938
\author FrankHB <frankhb1989@gmail.com>
\since build 245
\par 创建时间:
	2011-09-21 08:30:08 +0800
\par 修改时间:
	2023-03-24 23:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::CStandardIO
*/


#ifndef YB_INC_ystdex_cstdio_h_
#define YB_INC_ystdex_cstdio_h_ 1

#include "cassert.h" // for <cstdio>, yconstraint;
#include <cstdarg> // for std::va_list;
#include <memory> // for std::unique_ptr;
#include <array> // for std::array;
#include <ios> // for std::ios_base::openmode;
#include "cstring.h" // for ystdex::is_null;
#include "operators.hpp" // for input_iteratable, byte;

namespace ystdex
{

/*!
\brief 计算指定格式字符串输出的占用的字节数。
\pre 断言：第一参数非空。
\return 成功为字节数，否则为 size_t(-1) 。
\note 分别使用 std::vsnprintf 和 std::vswprintf 实现。
\since build 564
*/
//!@{
YB_ATTR_nodiscard YB_API YB_NONNULL(1) YB_PURE size_t
vfmtlen(const char*, std::va_list) ynothrowv;
YB_ATTR_nodiscard YB_API YB_NONNULL(1) YB_PURE size_t
vfmtlen(const wchar_t*, std::va_list) ynothrowv;
//!@}


/*!
\brief 关闭流缓冲。
\pre 参数非空。
\return 是否成功。
\since build 863
*/
inline bool
setnbuf(std::FILE* stream) ynothrowv
{
	yconstraint(stream);
	return std::setvbuf(stream, {}, _IONBF, 0) == 0;
}


/*!
\brief 独占所有权的 C 标准库流指针或实现提供的等价类型。
\note POSIX 管道流可使用函数实现的 \c ::pclose 代替 std::fclose 。
\since build 566
*/
using unique_file_ptr = std::unique_ptr<std::FILE, decltype(std::fclose)&>;


/*!
\brief 使用指定缓冲区和缓冲区处理函数从流中顺序读取所有内容。
\pre 断言：指针参数非空。
\note 假定文件可读。
\note 读取前设置关闭源缓冲。
\sa ystdex::setnbuf
\since build 839
*/
template<typename _func>
static void
read_all_with_buffer(std::FILE* fp, char* p_buf, size_t len, _func append)
{
	yconstraint(fp);
	yconstraint(p_buf);
	yconstraint(len > 0);

	ystdex::setnbuf(fp);
	for(size_t n; (n = std::fread(p_buf, 1, len, fp)) != 0; )
		append(p_buf, n);
}


/*!
\brief 固定模式字符串。
\tparam _tChar 字符类型。
\invariant 内容为 NTCTS 。
\since build 970
*/
template<typename _tChar = char>
using fixed_openmode = std::array<_tChar, yimpl(4)>;


/*!
\brief ISO C/C++ 标准输入输出接口打开模式转换。
\since build 923
*/
//!@{
namespace details
{

template<typename _tChar>
struct modes_holder final
{
	static yconstexpr const fixed_openmode<_tChar> modes[] = {
		{_tChar('w'), _tChar('b'), _tChar()},
		{_tChar('w'), _tChar()},
		{_tChar('a'), _tChar('b'), _tChar()},
		{_tChar('a'), _tChar()},
		{_tChar('r'), _tChar('b'), _tChar()},
		{_tChar('r'), _tChar()},
		{_tChar('w'), _tChar('+'), _tChar('b'), _tChar()},
		{_tChar('w'), _tChar('+'), _tChar()},
		{_tChar('a'), _tChar('+'), _tChar('b'), _tChar()},
		{_tChar('a'), _tChar('+'), _tChar()},
		{_tChar('r'), _tChar('+'), _tChar('b'), _tChar()},
		{_tChar('r'), _tChar('+'), _tChar()},
		{}
	};
};

#if __cpp_inline_variables < 201606L
template<typename _tChar>
yconstexpr const fixed_openmode<_tChar> modes_holder<_tChar>::modes[];
#endif

} // namespace details;

/*!
\see ISO C++11 Table 132 。
\note 忽略 std::ios_base::ate 。
\see LWG 596 。
*/
//!@{
template<typename _tChar = char>
YB_ATTR_nodiscard YB_STATELESS yconstfn_relaxed const fixed_openmode<_tChar>&
fixed_openmode_conv(std::ios_base::openmode mode) ynothrow
{
	using namespace std;
	// NOTE: WG21 N2643 (in ISO C++2b) is not relied on.
#if __cpp_constexpr >= 201304L
	yconstexpr
#endif
		const auto& modes(details::modes_holder<_tChar>::modes);

	switch(unsigned((mode &= ~ios_base::ate) & ~ios_base::binary))
	{
	case ios_base::out:
	case ios_base::out | ios_base::trunc:
		return mode & ios_base::binary ? modes[0] : modes[1];
	case ios_base::out | ios_base::app:
	case ios_base::app:
		return mode & ios_base::binary ? modes[2] : modes[3];
	case ios_base::in:
		return mode & ios_base::binary ? modes[4] : modes[5];
	case ios_base::in | ios_base::out:
		return mode & ios_base::binary ? modes[6] : modes[7];
	case ios_base::in | ios_base::out | ios_base::trunc:
		return mode & ios_base::binary ? modes[8] : modes[9];
	case ios_base::in | ios_base::out | ios_base::app:
	case ios_base::in | ios_base::app:
		return mode & ios_base::binary ? modes[10] : modes[11];
	default:
		break;
	}
	return modes[12];
}

//! \note 返回值未指定，但内容确定且无副作用，因此可用 \c YB_STATELESS 。
template<typename _tChar = char>
YB_ATTR_nodiscard YB_STATELESS yconstfn_relaxed const _tChar*
openmode_conv(std::ios_base::openmode mode) ynothrow
{
	const auto& fmode(ystdex::fixed_openmode_conv<_tChar>(mode));

	return ystdex::is_null(fmode[0]) ? nullptr : &fmode[0];
}
//!@}
/*!
\brief ISO C/C++ 标准输入输出接口打开模式转换。
\return 若失败（包括空参数情形）为 std::ios_base::openmode() ，否则为对应的值。
\see ISO C11 7.21.5.3/3 。
\note 顺序严格限定。
\note 支持 x 转换。
*/
//!@{
template<typename _tChar = char>
YB_ATTR_nodiscard YB_PURE std::ios_base::openmode
openmode_conv(const _tChar* s) ynothrow
{
	using namespace std;

	if(s)
	{
		ios_base::openmode mode;

		switch(*s)
		{
		case _tChar('w'):
			mode = ios_base::out | ios_base::trunc;
			break;
		case _tChar('r'):
			mode = ios_base::in;
			break;
		case _tChar('a'):
			mode = ios_base::app;
			break;
		default:
			goto invalid;
		}
		if(!ystdex::is_null(s[1]))
		{
			auto l(char_traits<_tChar>::length(s));

			if(s[l - 1] == _tChar('x'))
			{
				if(mode & ios_base::out)
					mode &= ~ios_base::out;
				else
					goto invalid;
				--l;
			}

			bool b(s[1] == 'b'), p(s[1] == '+');

			switch(l)
			{
			case 2:
				if(b != p)
					// NOTE: The value of s[1] is either 'b' or '+'.
					break;
				// NOTE: Since the value of s[1] cannot be both 'b' and '+' at
				//	the same time, it has the value neither 'b' or '+'.
				goto invalid;
			case 3:
				yunseq(b = b != (s[2] == _tChar('b')),
					p = p != (s[2] == _tChar('+')));
				if(b && p)
					break;
			default:
				goto invalid;
			}
			if(p)
				mode |= *s == _tChar('r') ? ios::out : ios::in;
			if(b)
				mode |= ios::binary;
		}
		return mode;
	}
invalid:
	return ios_base::openmode();
}
//! \since build 970
template<typename _tChar = char>
YB_ATTR_nodiscard YB_PURE inline std::ios_base::openmode
openmode_conv(const fixed_openmode<_tChar>& fmode) ynothrow
{
	return ystdex::openmode_conv<_tChar>(&fmode[0]);
}
//!@}
//!@}


/*!
\brief 判断指定路径的文件是否存在。
\pre 断言：字符串参数非空。
\note 使用 \c std::fopen 实现，第二参数指定打开模式。
\return 以指定路径打开文件是否成功。
\since build 970
*/
//!@{
YB_ATTR_nodiscard YB_API YB_NONNULL(1, 2) bool
fexists(const char*, const char* = "rb") ynothrowv;
/*!
\warning 若模式转换无法得到支持 \c std::fopen 的模式，则行为未定义。
\sa ystdex::openmode_conv
*/
YB_ATTR_nodiscard YB_API YB_NONNULL(1) bool
fexists(const char*, std::ios_base::openmode) ynothrowv;
//!@}


/*!
\brief 基于 ISO C 标准库的流只读迭代器。
\since build 245
*/
class YB_API ifile_iterator
	: public input_iteratable<ifile_iterator, const byte&>
{
public:
	//! \since build 676
	//!@{
	using iterator_category = std::input_iterator_tag;
	using value_type = byte;
	using pointer = const byte*;
	using reference = const byte&;
	using difference_type = ptrdiff_t;
	//!@}
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
	\post \c !stream 。

	构造空流迭代器。
	*/
	yconstfn
	ifile_iterator()
		: value()
	{}
	/*!
	\brief 构造：使用流指针。
	\pre 断言：\c ptr 。
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
	//!@{
	//! \pre 断言：\c !stream 。
	int_type
	sputbackc(char_type c)
	{
		yconstraint(stream);
		// XXX: Assume the resule is always in the range of %char.
		return std::ungetc(char(c), stream);
	}
	/*!
	\pre 断言：<tt>!stream || steram == s</tt> 。
	\post <tt>stream == s</tt> 。
	*/
	int_type
	sputbackc(char_type c, std::FILE* s)
	{
		yconstraint(!stream || stream == s);
		stream = s;
		return sputbackc(c);
	}

	//! \pre 间接断言：\c !stream 。
	int_type
	sungetc()
	{
		return sputbackc(value);
	}
	//! \pre 间接断言：<tt>!stream || steram == s</tt> 。
	int_type
	sungetc(std::FILE* s)
	{
		return sputbackc(value, s);
	}
	//!@}
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

	YB_ATTR_nodiscard bool
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

	YB_ATTR_nodiscard YB_PURE byte*
	get() const ynothrow
	{
		return content.get();
	}

	YB_ATTR_nodiscard YB_PURE bool
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

