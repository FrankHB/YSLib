/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file any.h
\ingroup YStandardEx
\brief 函数对象、算法和实用程序。
\version r1176;
\author FrankHB<frankhb1989@gmail.com>
\since build 247 。
\par 创建时间:
	2011-09-26 07:55:44 +0800;
\par 修改时间:
	2011-12-04 11:09 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YStandardEx::Any;
*/


#ifndef YCL_INC_YSTDEX_ANY_H_
#define YCL_INC_YSTDEX_ANY_H_

#include "../ydef.h"

namespace ystdex
{
	/*
	\brief 任意非可复制构造的非聚集类型。
	\since build 207 。
	*/
	union no_copy_t
	{
		void* object_ptr;
		const void* const_object_ptr;
		void(*function_ptr)();
		void(no_copy_t::*member_function_pointer)();
	};


	/*
	\brief 任意 POD 类型。
	\note POD 含义参考 ISO C++ 2011 。
	\since build 207 。
	*/
	union any_pod_t
	{
		no_copy_t _unused;
		unsigned char plain_old_data[sizeof(no_copy_t)];

		void*
		access()
		{
			return &plain_old_data[0];
		}
		yconstfn const void*
		access() const
		{
			return &plain_old_data[0];
		}
		template<typename _type>
		_type&
		access()
		{
			return *static_cast<_type*>(access());
		}
		template<typename _type>
		yconstfn const _type&
		access() const
		{
			return *static_cast<const _type*>(access());
		}
	};


	/*!
	\brief 任意对象引用类型。
	\warning 不检查 cv-qualifier 。
	\since build 247 。
	*/
	class void_ref
	{
	private:
		const volatile void* ptr;

	public:
		template<typename _type>
		yconstfn
		void_ref(_type& obj)
			: ptr(&obj)
		{}

		template<typename _type>
		yconstfn operator _type&()
		{
			return *static_cast<_type*>(&*this);
		}

		void*
		operator&() const volatile
		{
			return const_cast<void*>(ptr);
		}
	};
}

#endif

