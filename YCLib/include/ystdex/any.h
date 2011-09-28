/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file any.h
\ingroup YCLib
\brief 函数对象、算法和实用程序。
\version r1166;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-09-26 07:55:44 +0800;
\par 修改时间:
	2011-09-28 08:59 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YStandardExtend::Any;
*/


#ifndef YCL_INC_YSTDEX_ANY_H_
#define YCL_INC_YSTDEX_ANY_H_

#include "../ydef.h"

namespace ystdex
{
	/*
	\brief 任意非可复制构造的非聚集类型。
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
		yconstexprf const void*
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
		yconstexprf const _type&
		access() const
		{
			return *static_cast<const _type*>(access());
		}
	};


	/*!
	\brief 任意对象引用类型。
	\warning 不检查 cv-qualifier 。
	*/
	class void_ref
	{
	private:
		const volatile void* ptr;

	public:
		template<typename _type>
		yconstexprf
		void_ref(_type& obj)
			: ptr(&obj)
		{}

		template<typename _type>
		yconstexprf operator _type&()
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

