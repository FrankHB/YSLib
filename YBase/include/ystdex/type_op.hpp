/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file type_op.hpp
\ingroup YStandardEx
\brief C++ 类型操作模板类。
\version r1272;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-14 08:54:25 +0800;
\par 修改时间:
	2011-11-13 15:36 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YStandardEx::TypeOperation;
*/


#ifndef YCL_INC_YSTDEX_TYPEOP_HPP_
#define YCL_INC_YSTDEX_TYPEOP_HPP_

#include "../ydef.h"

namespace ystdex
{
/*
	using std::tr1::add_reference;

	using std::tr1::has_nothrow_assign;
	using std::tr1::has_nothrow_constructor;
	using std::tr1::has_nothrow_copy;
	using std::tr1::has_trivial_assign;
	using std::tr1::has_trivial_constructor;
	using std::tr1::has_trivial_copy;
	using std::tr1::has_trivial_destructor;
*/

	using std::integral_constant;
	using std::true_type;
	using std::false_type;

	using std::is_void;
	using std::is_integral;
	using std::is_floating_point;
	using std::is_array;
	using std::is_pointer;
	using std::is_lvalue_reference;
	using std::is_rvalue_reference;
	using std::is_member_object_pointer;
	using std::is_member_function_pointer;
	using std::is_enum;
	using std::is_class;
	using std::is_union;
	using std::is_function;

	using std::is_reference;
	using std::is_arithmetic;
	using std::is_fundamental;
	using std::is_object;
	using std::is_scalar;
	using std::is_compound;
	using std::is_member_pointer;

	using std::is_const;
	using std::is_volatile;
	using std::is_trivial;
//	using std::is_trivially_copyable;
	using std::is_standard_layout;
	using std::is_pod;
	using std::is_literal_type;
	using std::is_empty;
	using std::is_polymorphic;
	using std::is_abstract;

	using std::is_signed;
	using std::is_unsigned;

	using std::is_constructible;
	/*
	using std::is_default_constructible;
	using std::is_copy_constructible;
	using std::is_move_constructible;

	using std::is_assignable;
	using std::is_copy_assignable;
	using std::is_move_assignable;

	using std::is_destructible;

	using std::is_trivially_constructible;
	using std::is_trivially_default_constructible;
	using std::is_trivially_copy_constructible;
	using std::is_trivially_move_constructible;

	using std::is_trivially_assignable;
	using std::is_trivially_copy_assignable;
	using std::is_trivially_move_assignable;
	using std::is_trivially_destructible;

	using std::is_nothrow_constructible;
	using std::is_nothrow_default_constructible;
	using std::is_nothrow_copy_constructible;
	using std::is_nothrow_move_constructible;

	using std::is_nothrow_assignable;
	using std::is_nothrow_copy_assignable;
	using std::is_nothrow_move_assignable;

	using std::is_nothrow_destructible;
	*/
	using std::has_virtual_destructor;

	using std::alignment_of;
	using std::rank;
	using std::extent;

	using std::is_same;
	using std::is_base_of;
	using std::is_convertible;

	using std::remove_const;
	using std::remove_volatile;
	using std::remove_cv;
	using std::add_const;
	using std::add_volatile;
	using std::add_cv;

	using std::remove_reference;
	using std::add_lvalue_reference;
	using std::add_rvalue_reference;

	using std::make_signed;
	using std::make_unsigned;

	using std::remove_extent;
	using std::remove_all_extents;

	using std::remove_pointer;
	using std::add_pointer;

	using std::aligned_storage;
	using std::decay;
	using std::enable_if;
	using std::conditional;
	using std::common_type;
//	using std::underlying_type;
	using std::result_of;


	template<class _type>
	struct has_nonempty_virtual_base
	{
	private:
		struct A : _type
		{
			~A() throw()
			{}
		};
		struct B : _type
		{
			~B() throw()
			{}
		};
		struct C : A, B
		{
			~C() throw()
			{}
		};

	public:
		static yconstexpr bool value = sizeof(C) < sizeof(A) + sizeof(B);
	};


	template<class _type1, class _type2>
	struct has_common_nonempty_virtual_base
	{
	private:
		struct A : virtual _type1
		{
			~A() throw()
			{}
		};

#ifdef __GNUC__
#	if YCL_IMPL_GNUCPP >= 40600
#		pragma GCC diagnostic push
#		pragma GCC diagnostic ignored "-Wextra"
#	else
#		pragma GCC system_header
//临时处理：关闭所有警告。
/*
关闭编译警告：(C++ only) Ambiguous virtual bases. ，
参见 http://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html 。
*/
#	endif
#endif

		struct B : virtual _type2
		{
			~B() throw()
			{}
		};
		struct C : A, B
		{
			~C() throw()
			{}
		};

#if defined(YCL_IMPL_GNUCPP) && YCL_IMPL_GNUCPP >= 40600
//#	pragma GCC diagnostic warning "-Wextra"
#	pragma GCC diagnostic pop
#endif

	public:
		static yconstexpr auto value = sizeof(C) < sizeof(A) + sizeof(B);
	};


	template<typename _tInt>
	struct integer_width
	{
		static yconstexpr auto value = sizeof(_tInt) * CHAR_BIT;
	};


	template<typename _type, bool>
	struct make_signed_c
	{
		typedef typename std::make_signed<_type>::type type;
	};

	template<typename _type>
	struct make_signed_c<_type, false>
	{
		typedef typename std::make_unsigned<_type>::type type;
	};


	template<size_t>
	struct make_fixed_width_int
	{
		typedef int type;
		typedef unsigned unsigned_type;
	};

	template<>
	struct make_fixed_width_int<8U>
	{
		typedef std::int8_t type;
		typedef std::uint8_t unsigned_type;
	};

	template<>
	struct make_fixed_width_int<16U>
	{
		typedef std::int16_t type;
		typedef std::uint16_t unsigned_type;
	};

	template<>
	struct make_fixed_width_int<32U>
	{
		typedef std::int32_t type;
		typedef std::uint32_t unsigned_type;
	};

	template<>
	struct make_fixed_width_int<64U>
	{
		typedef std::int64_t type;
		typedef std::uint64_t unsigned_type;
	};
}

#endif

