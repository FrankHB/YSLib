/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file type_op.hpp
\ingroup YCLib
\brief C++ 类型操作模板类。
\version r1141;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-04-14 08:54:25 +0800;
\par 修改时间:
	2011-09-13 23:15 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YStandardExtend::TypeOperation;
*/


#ifndef YCL_INC_YSTDEX_TYPEOP_HPP_
#define YCL_INC_YSTDEX_TYPEOP_HPP_

#include "../ystdex.h"

namespace ystdex
{
	template<class _type>
	struct has_nonempty_virtual_base
	{
		struct A
			: _type
		{
			~A() throw()
			{}
		};
		struct B
			: _type
		{
			~B() throw()
			{}
		};
		struct C
			: A, B
		{
			~C() throw()
			{}
		};

		enum
		{
			value = sizeof(C) < sizeof(A) + sizeof(B)
		};
	};

	template<class _type1, class _type2>
	struct has_common_nonempty_virtual_base
	{
		struct A
			: virtual _type1
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

		struct B
			: virtual _type2
		{
			~B() throw()
			{}
		};
		struct C
			: A, B
		{
			~C() throw()
			{}
		};

#if defined(YCL_IMPL_GNUCPP) && YCL_IMPL_GNUCPP >= 40600
//#	pragma GCC diagnostic warning "-Wextra"
#	pragma GCC diagnostic pop
#endif

		enum
		{
			value = sizeof(C) < sizeof(A) + sizeof(B)
		};
	};
}

#endif

