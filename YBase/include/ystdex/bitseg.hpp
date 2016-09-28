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
\version r553
\author FrankHB <frankhb1989@gmail.com>
\since build 507
\par 创建时间:
	2014-06-12 21:42:50 +0800
\par 修改时间:
	2016-09-27 21:07 +0800
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
\since build 729
*/
//@{
/*!
\brief 位段特征。

描述以整数位序压缩存储的若干个位的特性。
参数指定不同分量的位宽，其顺序隐含存储顺序。
*/
template<size_t... _vSize>
struct bitseg_traits
{
	//! \since build 728
	//@{
	using sequence = index_sequence<_vSize...>;

	static yconstexpr size_t bits_n = fseq::fold(fseq::plus(), size_t_<0>(),
		sequence());
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
	using shift = decltype(fseq::fold(fseq::plus(), size_t_<0>(),
		vseq::split_n_t<_vIdx, sequence>()));

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
	pack(_types... xs) ynothrow
	{
		return project(make_index_sequence<sizeof...(xs)>(), xs...);
	}

	template<size_t... _vIdxSeq>
	static yconstfn integer
	project(index_sequence<_vIdxSeq...>, component_t<_vIdxSeq>... xs) ynothrow
	{
		return fseq::vfold(bit_or<>(), integer(), lift<_vIdxSeq>(xs)...);
	}
	//@}
};


/*!
\brief 按索引有序映射的特征适配模板。
\pre 第一模板参数指定的类型具有和 bitset_traits 的实例兼容的成员。
\pre 静态断言：第一模板参数以外的参数数等于第一模板参数指定的分量数。
\todo 静态断言检查索引序列的其它性质。

允许位段中分量依次和特定 size_t 类型的存储索引关联。
参数指定的索引构成索引序列，取值应为一个排列，即 0 到分量数减 1 的不重复整数值。
在索引序列上使用表示元素位置的逻辑分量索引指定特定的存储索引。
被适配的原始存储位置可通过恒等映射为存储索引等效表示，其索引序列以 0 起始严格递增。
*/
template<class _tTraits, size_t... _vSeq>
struct ordered_bitseg_traits : _tTraits
{
	//! \since build 729
	static_assert(_tTraits::sequence::size() == sizeof...(_vSeq),
		"Invalid coponent index sequence found.");

	//! \since build 728
	//@{
	using base = _tTraits;
	using typename base::integer;
	//! \brief 指定映射关系的索引序列。
	using mapping = index_sequence<_vSeq...>;
	//! \brief 取指定的索引实现映射。
	template<size_t _vIdx>
	using map = vseq::at_t<mapping, _vIdx>;
	template<size_t _vIdx>
	using component_t = typename base::template component_t<map<_vIdx>::value>;
	template<size_t _vIdx>
	using component_width
		= typename base::template component_width<map<_vIdx>::value>;
	template<size_t _vIdx>
	using shift = typename base::template shift<map<_vIdx>::value>;
	template<size_t _vIdx>
	using mask = typename base::template mask<map<_vIdx>::value>;
	//! \since build 729
	//@{
	//! \brief 在索引序列查找指定的索引以实现逆映射。
	template<size_t _vIdx>
	using inversed_map = vseq::find_t<mapping, size_t_<_vIdx>>;
	template<size_t _vIdx>
	using inversed_mapped_component_t = component_t<inversed_map<_vIdx>::value>;
	//@}

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
	pack(_types... xs) ynothrow
	{
		return project(make_index_sequence<sizeof...(xs)>(), xs...);
	}

	template<size_t... _vIdxSeq>
	static yconstfn integer
	project(index_sequence<_vIdxSeq...>,
		component_t<map<_vIdxSeq>::value>... xs) ynothrow
	{
		return base::template project(index_sequence<map<_vIdxSeq>::value...>(),
			xs...);
	}
	//@}
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
		: base(p),
		shift((yconstraint(n < seg_n), static_cast<unsigned char>(n)))
	{}

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

