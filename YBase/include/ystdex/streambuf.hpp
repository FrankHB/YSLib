/*
	© 2015-2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file streambuf.hpp
\ingroup YStandardEx
\brief ISO C++ 标准库标准流缓冲扩展。
\version r242
\author FrankHB <frankhb1989@gmail.com>
\since build 636
\par 创建时间:
	2015-09-22 11:19:27 +0800
\par 修改时间:
	2020-10-26 17:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::StreamBuffer
*/


#ifndef YB_INC_ystdex_streambuf_hpp_
#define YB_INC_ystdex_streambuf_hpp_ 1

#include "algorithm.hpp" // for ystdex::equal;
#include <streambuf> // for std::basic_streambuf;
#include <iterator> // for std::istreambuf_iterator;

namespace ystdex
{

/*!
\brief 比较指定流缓冲区的内容相等。
\note 流读取失败视为截止。
\since build 636
*/
template<typename _tChar, class _tTraits>
bool
streambuf_equal(std::basic_streambuf<_tChar, _tTraits>& a,
	std::basic_streambuf<_tChar, _tTraits>& b)
{
	using iter_type = std::istreambuf_iterator<_tChar, _tTraits>;

	return
		ystdex::equal(iter_type(&a), iter_type(), iter_type(&b), iter_type());
}


/*!
\brief 刷新输入缓冲区到字符串。
\since build 902
*/
template<typename _tChar, class _tTraits, class _tAlloc>
std::ios_base::iostate
flush_input(std::basic_streambuf<_tChar, _tTraits>& buf,
	std::basic_string<_tChar, _tTraits, _tAlloc>& str)
{
	std::streamsize cnt(buf.in_avail());

	if(cnt == 0)
		return std::ios_base::goodbit;
	if(cnt > 0)
	{
		const auto l(str.length());
		using _tSize = decltype(l);
		const auto m(std::streamsize(str.max_size() - l));

		if(cnt < m)
		{
			str.resize(l + _tSize(cnt));
			buf.sgetn(&str[l], cnt);
			return std::ios_base::goodbit;
		}
		else
		{
			str.resize(str.max_size());
			buf.sgetn(&str[l], m);
		}
	}
	return std::ios_base::failbit;
}


//! \since build 711
//@{
/*!
\brief 内存中储存不具有所有权的只读流缓冲。
\warning 流操作不写缓冲区，否则行为未定义。
\todo 提供 setbuf 、uflow 和 showmanyc 等覆盖实现。
*/
template<typename _tChar, class _tTraits = std::char_traits<_tChar>>
class basic_membuf : public std::basic_streambuf<_tChar, _tTraits>
{
public:
	using char_type = _tChar;
	using int_type = typename _tTraits::int_type;
	using pos_type = typename _tTraits::pos_type;
	using off_type = typename _tTraits::off_type;
	using traits_type = _tTraits;

	//! \pre 断言：参数指针非空或指定的大小为 0 。
	basic_membuf(const char_type* buf, size_t size)
	{
		yconstraint(buf || size == 0);
		if(buf)
		{
			// NOTE: This should be safe since get area is not used to be
			//	written.
			const auto p(const_cast<char_type*>(buf));

			this->setg(p, p, p + size);
		}
	}

protected:
	//! \since build 805
	pos_type
	seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode
		which = std::ios_base::in | std::ios_base::out) override
	{
		if(bool(which & std::ios_base::in) && !bool(which & std::ios_base::out))
		{
			if(const auto beg = this->eback())
			{
				switch(way)
				{
				case std::ios_base::beg:
					break;
				case std::ios_base::cur:
					off += this->gptr() - beg;
					break;
				case std::ios_base::end:
					off += this->egptr() - beg;
					break;
				default:
					yassume(false);
				}
				if(off >= off_type())
				{
					const auto gend(this->egptr());

					if(gend - beg >= off)
					{
						this->setg(beg, beg + off, gend);
						return pos_type(off);
					}
				}
			}
			// NOTE: See LWG 453.
			else if(off == off_type())
				return pos_type(off_type(0));
		}
		return pos_type(off_type(-1));
	}

	//! \since build 805
	pos_type
	seekpos(pos_type sp, std::ios_base::openmode which
		= std::ios_base::in | std::ios_base::out) override
	{
		return basic_membuf::seekoff(off_type(sp), std::ios::beg, which);
	}
};

using membuf = basic_membuf<char>;
using wmembuf = basic_membuf<wchar_t>;
//@}

} // namespace ystdex;

#endif

