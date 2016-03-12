/*
	© 2013-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file bitseg.hpp
\ingroup YStandardEx
\brief 位段数据结构和访问。
\version r389
\author FrankHB <frankhb1989@gmail.com>
\since build 507
\par 创建时间:
	2014-06-12 21:42:50 +0800
\par 修改时间:
	2016-03-12 23:54 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::BitSegment
*/


#ifndef YB_INC_ystdex_bitseg_hpp_
#define YB_INC_ystdex_bitseg_hpp_ 1

#include "iterator_op.hpp" // for size_t, std::iterator,
//	std::random_access_iterator_tag, byte, ptrdiff_t, iterator_operators_t,
//	std::iterator_traits, CHAR_BIT;

namespace ystdex
{

/*!
\ingroup iterators
\brief 位段迭代器。
\tparam _vN 段宽度。
\tparam _bEndian 位序， \c ture 时为小端（ LSB 0 ），否则为大端（ MSB 0 ）。
\warning 非虚析构。
\see http://en.wikipedia.org/wiki/Bit_numbering 。
\since build 549
\todo 支持 const byte* 实现的迭代器。

对字节分段提供的随机访问迭代器。
*/
template<size_t _vN, bool _bEndian = false>
class bitseg_iterator : public
	random_access_iteratable<bitseg_iterator<_vN, _bEndian>, ptrdiff_t, byte&>
{
	static_assert(_vN != 0, "A bit segment should contain at least one bit.");
	static_assert(_vN != CHAR_BIT, "A bit segment should not be a byte.");
	static_assert(CHAR_BIT % _vN == 0,
		"A byte should be divided by number of segments without padding.");

public:
	//! \since build 676
	using iterator_category = std::random_access_iterator_tag;
	//! \since build 676
	using value_type = byte;
	using difference_type = ptrdiff_t;
	using pointer = byte*;
	using reference = byte&;

	//! \since build 549
	static yconstexpr const bool lsb = _bEndian;
	static yconstexpr const unsigned char seg_n = CHAR_BIT / _vN;
	static yconstexpr const unsigned char seg_size = 1 << _vN;
	static yconstexpr const unsigned char seg_width = _vN;

protected:
	byte* base;
	//! \note CHAR_BIT <= UCHAR_MAX 恒成立，因此使用 unsigned char 存储。
	unsigned char shift;
	mutable byte value;

public:
	//! \since build 461
	//@{
	/*!
	\brief 构造：使用基指针和偏移位。
	\post 断言： <tt>shift < seg_n</tt> 。
	\note value 具有未决定值。
	\since build 549
	*/
	bitseg_iterator(byte* p = {}, size_t n = 0) ynothrow
		: base(p), shift(n)
	{
		yassume(shift < seg_n);
	}

	bitseg_iterator&
	operator+=(difference_type n) ynothrowv
	{
		yconstraint(base);
		yassume(shift < seg_n);

		const size_t new_shift(shift + size_t(n));

		yunseq(base += new_shift / seg_n, shift = new_shift % seg_n);
		return *this;
	}

	bitseg_iterator&
	operator-=(difference_type n) ynothrowv
	{
		base += -n;
		return *this;
	}

	reference
	operator*() const ynothrowv
	{
		yconstraint(base);
		return value = *base >> seg_width * (lsb ? seg_n - 1 - shift : shift)
			& ((1 << seg_width) - 1);
	}

	inline bitseg_iterator&
	operator++() ynothrowv
	{
		yconstraint(base);
		yassume(shift < seg_n);
		if(++shift == seg_n)
			yunseq(shift = 0, ++base);
		return *this;
	}

	inline bitseg_iterator&
	operator--() ynothrowv
	{
		yconstraint(base);
		yassume(shift < seg_n);
		if(shift == 0)
			yunseq(--base, shift = seg_n - 1);
		else
			--shift;
		return *this;
	}

	//! \since build 600
	//@{
	friend bool
	operator==(const bitseg_iterator& x, const bitseg_iterator& y) ynothrow
	{
		return x.base == y.base && x.shift == y.shift;
	}

	friend bool
	operator<(const bitseg_iterator& x, const bitseg_iterator& y) ynothrow
	{
		return x.base < y.base || (x.base == y.base && x.shift < y.shift);
	}
	//@}

	explicit yconstfn
	operator pointer() const ynothrow
	{
		return base;
	}

	yconstfn size_t
	get_shift() const ynothrow
	{
		return shift;
	}
	//@}
};

} // namespace ystdex;

#endif

