/*
	© 2015-2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file streambuf.hpp
\ingroup YStandardEx
\brief ISO C++ 标准库标准流缓冲扩展。
\version r156
\author FrankHB <frankhb1989@gmail.com>
\since build 636
\par 创建时间:
	2015-09-22 11:19:27 +0800
\par 修改时间:
	2018-10-03 10:21 +0800
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


//! \since build 711
//@{
/*!
\brief 内存中储存不具有所有权的只读流缓冲。
\warning 流操作不写缓冲区，否则行为未定义。
\todo 提供 setbuf 。
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

	//! \since build 805
	//@{
protected:
	pos_type
	seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode
		which = std::ios_base::in | std::ios_base::out) override
	{
		if(bool(which & std::ios_base::in) && !bool(which & std::ios_base::out))
		{
			const char_type* beg(this->eback());

			if(beg || off == off_type())
			{
				update_egptr();
				if(way == std::ios_base::cur)
					off += this->gptr() - beg;
				else if(way == std::ios_base::end)
					off += this->egptr() - beg;
				if(off >= off_type() && this->egptr() - beg >= off)
				{
					set_input_area(off);
					return pos_type(off);
				}
			}
		}
		return pos_type(off_type(-1));
	}

	pos_type
	seekpos(pos_type sp, std::ios_base::openmode which
		= std::ios_base::in | std::ios_base::out) override
	{
		if(bool(which & std::ios_base::in) && !bool(which & std::ios_base::out))
		{
			const char_type* beg(this->eback());

			if(beg || off_type(sp) == off_type())
			{
				update_egptr();

				const auto pos(sp);

				if(pos_type() <= pos && pos <= pos_type(this->egptr() - beg))
				{
					set_input_area(off_type(pos));
					return sp;
				}
			}
		}
		return pos_type(off_type(-1));
	}

private:
	void
	set_input_area(off_type off)
	{
		this->setg(this->eback(), this->eback() + off, this->egptr());
	}

	void
	update_egptr()
	{
		if(this->pptr() && this->pptr() > this->egptr())
			this->setg(this->eback(), this->gptr(), this->pptr());
	}
	//@}
};

using membuf = basic_membuf<char>;
using wmembuf = basic_membuf<wchar_t>;
//@}

} // namespace ystdex;

#endif

