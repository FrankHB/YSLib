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
\version r498
\author FrankHB <frankhb1989@gmail.com>
\since build 507
\par 创建时间:
	2014-06-12 21:42:50 +0800
\par 修改时间:
	2016-09-19 17:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::BitSegment
*/


#ifndef YB_INC_ystdex_bitseg_hpp_
#define YB_INC_ystdex_bitseg_hpp_ 1

#include "iterator_op.hpp" // for size_t, index_sequence, fseq::fold,
//	fseq::plus, std::integral_constant, CHAR_BIT, byte, vseq::at_t,
//	vseq::split_n_t, fseq::vfold, std::random_access_iterator_tag, ptrdiff_t,
//	yconstraint, yassume;
#include "functor.hpp" // for bit_or;
#include "cstdint.hpp" // for ystdex::make_width_int;

namespace ystdex
{

/*!
\ingroup type_traits_operations
\since build 728
*/
//@{
//! \brief 位段特征。
template<size_t... _vSize>
struct bitseg_trait
{
	using sequence = index_sequence<_vSize...>;

	static yconstexpr size_t bits_n = fseq::fold(fseq::plus(),
		std::integral_constant<size_t, 0>(), sequence());
	static yconstexpr size_t bytes_n = (bits_n + CHAR_BIT - 1) / CHAR_BIT;

	using array = byte[bytes_n];
	using integer
		= typename ystdex::make_width_int<bits_n>::unsigned_least_type;

	template<size_t _vIdx>
	using component_width = vseq::at_t<sequence, _vIdx>;

	template<size_t _vIdx>
	using component_t = typename ystdex::make_width_int<
		component_width<_vIdx>::value>::unsigned_least_type;

	template<size_t _vIdx>
	using shift = decltype(fseq::fold(fseq::plus(), std::integral_constant<
		size_t, 0>(), vseq::split_n_t<_vIdx, sequence>()));

	template<size_t _vIdx>
	using mask = std::integral_constant<integer,
		((1ULL << component_width<_vIdx>::value) - 1) << shift<_vIdx>::value>;

	template<size_t _vIdx>
	static yconstfn component_t<_vIdx>
	extract(integer i) ynothrow
	{
		return
			component_t<_vIdx>((i & mask<_vIdx>::value) >> shift<_vIdx>::value);
	}

	template<size_t _vIdx>
	static yconstfn integer
	lift(component_t<_vIdx> val) ynothrow
	{
		return integer(integer(val) << shift<_vIdx>::value);
	}

	template<typename... _types>
	static yconstfn integer
	pack(_types... vals) ynothrow
	{
		return project(make_index_sequence<sizeof...(vals)>(), vals...);
	}

	template<size_t... _vIdxSeq>
	static yconstfn integer
	project(index_sequence<_vIdxSeq...>, component_t<_vIdxSeq>... vals) ynothrow
	{
		return fseq::vfold(bit_or<>(), integer(), lift<_vIdxSeq>(vals)...);
	}
};


/*!
\pre 第一模板参数指定的类型具有和 bitset_trait 的实例兼容的成员。
\brief 参数顺序重映射的位段特征。
*/
template<class _tTrait, size_t... _vSeq>
struct mapped_bitseg_trait : _tTrait
{
	using base = _tTrait;
	using typename base::integer;
	using mapping = index_sequence<_vSeq...>;
	template<size_t _vIdx>
	using map = vseq::at_t<mapping, _vIdx>;
	template<size_t _vIdx>
	using component_t
		= typename base::template component_t<map<_vIdx>::value>;
	template<size_t _vIdx>
	using component_width
		= typename base::template component_width<map<_vIdx>::value>;
	template<size_t _vIdx>
	using shift = typename base::template shift<map<_vIdx>::value>;
	template<size_t _vIdx>
	using mask = typename base::template mask<map<_vIdx>::value>;

	template<size_t _vIdx>
	static yconstfn component_t<_vIdx>
	extract(integer i) ynothrow
	{
		return base::template extract<map<_vIdx>::value>(i);
	}

	template<size_t _vIdx>
	static yconstfn component_t<_vIdx>
	lift(component_t<map<_vIdx>::value> val) ynothrow
	{
		return base::template lift<map<_vIdx>::value>(val);
	}

	template<typename... _types>
	static yconstfn integer
	pack(_types... vals) ynothrow
	{
		return project(make_index_sequence<sizeof...(vals)>(), vals...);
	}

	template<size_t... _vIdxSeq>
	static yconstfn integer
	project(index_sequence<_vIdxSeq...>,
		component_t<map<_vIdxSeq>::value>... vals) ynothrow
	{
		return base::template project(index_sequence<map<_vIdxSeq>::value...>(),
			vals...);
	}
};
//@}


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

