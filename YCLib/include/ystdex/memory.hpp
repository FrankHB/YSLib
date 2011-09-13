/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file memory.hpp
\ingroup YCLib
\brief 存储和智能指针特性。
\version r1146;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-05-14 12:25:13 +0800;
\par 修改时间:
	2011-09-13 23:15 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YStandardExtend::Memory;
*/


#ifndef YCL_INC_YSTDEX_MEMORY_HPP_
#define YCL_INC_YSTDEX_MEMORY_HPP_

#include "../ystdex.h"
#include <memory>
#include "utility.hpp" //for std::nullptr_t;

namespace ystdex
{
	/*!
	\ingroup HelperFunction
	*/
	//@{

	/*!	\defgroup is_null Is Null Pointer
	\brief 判断指针实例是否为空指针。
	\tparam _tPointer 指针类型。
	\pre _tPointer 能转换为 bool 类型，当且仅当非空时为 true 。
	*/
	//@{
	template<typename _tPointer>
	inline bool
	is_null(const _tPointer& p)
	{
		return !static_cast<bool>(p);
	}
	template<typename _tPointer>
	inline bool
	is_null(_tPointer&& p)
	{
		return !static_cast<bool>(p);
	}
	//@}


	/*!	\defgroup is_not_null Is Not Null Pointer
	\brief 判断指针实例是否为空指针。
	\tparam _tPointer 指针类型。
	\pre _tPointer 能转换为 bool 类型，当且仅当空时为 true 。
	*/
	//@{
	template<typename _tPointer>
	inline bool
	is_not_null(const _tPointer& p)
	{
		return static_cast<bool>(p);
	}
	template<typename _tPointer>
	inline bool
	is_not_null(_tPointer&& p)
	{
		return static_cast<bool>(p);
	}
	//@}


	/*!	\defgroup raw Get Raw Pointers
	\brief 取内建指针。
	*/
	//@{
	template<typename _type>
	inline _type*
	raw(_type* const& p)
	{
		return p;
	}
	template<typename _type>
	inline _type*
	raw(const std::unique_ptr<_type>& p)
	{
		return p.get();
	}
	template<typename _type>
	inline _type*
	raw(const std::shared_ptr<_type>& p)
	{
		return p.get();
	}
	//@}

	/*!	\defgroup reset Reset Pointers
	\brief 安全删除指定引用的指针指向的对象。
	\post 指定引用指针的为空。
	*/
	//@{
	template<typename _type>
	inline bool
	reset(std::unique_ptr<_type>& p) ynothrow
	{
		if(p.get())
		{
			p.reset();
			return true;
		}
		return false;
	}
	template<typename _type>
	inline bool
	reset(std::shared_ptr<_type>& p) ynothrow
	{
		if(p.get())
		{
			p.reset();
			return true;
		}
		return false;
	}
	//@}


	/*!	\defgroup unique_raw Get Unique Pointer
	\brief 使用指定类型指针构造 std::unique_ptr 实例。
	\tparam _type 被指向类型。
	\note 不检查指针是否有效。
	*/
	//@{
	/*!
	\tparam _pSrc 指定指针类型。
	*/
	template<typename _type, typename _pSrc>
	inline std::unique_ptr<_type>
	unique_raw(const _pSrc& p)
	{
		return std::unique_ptr<_type>(p);
	}
	/*!
	\tparam _pSrc 指定指针类型。
	*/
	template<typename _type, typename _pSrc>
	inline std::unique_ptr<_type>
	unique_raw(_pSrc&& p)
	{
		return std::unique_ptr<_type>(p);
	}
	template<typename _type>
	inline std::unique_ptr<_type>
	unique_raw(_type* p)
	{
		return std::unique_ptr<_type>(p);
	}
	/*!
	\note 使用空指针构造空实例。
	*/
	template<typename _type>
	inline std::unique_ptr<_type>
	unique_raw(const nullptr_t& p)
	{
		return std::unique_ptr<_type>();
	}
	//@}


	/*!	\defgroup share_raw Get Shared Pointer
	\brief 使用指定类型指针构造 std::shared_ptr 实例。
	\tparam _type 被指向类型。
	\note 不检查指针是否有效。
	*/
	//@{
	/*!
	\tparam _pSrc 指定指针类型。
	*/
	template<typename _type, typename _pSrc>
	inline std::shared_ptr<_type>
	share_raw(const _pSrc& p)
	{
		return std::shared_ptr<_type>(p);
	}
	/*!
	\tparam _pSrc 指定指针类型。
	*/
	template<typename _type, typename _pSrc>
	inline std::shared_ptr<_type>
	share_raw(_pSrc&& p)
	{
		return std::shared_ptr<_type>(p);
	}
	template<typename _type>
	inline std::shared_ptr<_type>
	share_raw(_type* p)
	{
		return std::shared_ptr<_type>(p);
	}
	/*!
	\note 使用空指针构造空实例。
	*/
	template<typename _type>
	inline std::shared_ptr<_type>
	share_raw(const nullptr_t& p)
	{
		return std::shared_ptr<_type>();
	}
	//@}
}

#endif

