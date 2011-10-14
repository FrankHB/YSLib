/*
	Copyright (C) by Franksoft 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file memory.hpp
\ingroup YStandardEx
\brief 存储和智能指针特性。
\version r1277;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2011-05-14 12:25:13 +0800;
\par 修改时间:
	2011-10-12 18:16 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YStandardEx::Memory;
*/


#ifndef YCL_INC_YSTDEX_MEMORY_HPP_
#define YCL_INC_YSTDEX_MEMORY_HPP_

#include "../ydef.h"
#include <memory>
#include <cstring> // for std::memcpy and std::memmove;

namespace ystdex
{
	/*!
	\ingroup HelperFunctions
	*/
	//@{

	/*!	\defgroup is_null Is Null Pointer
	\brief 判断指针实例是否为空指针。
	\tparam _tPointer 指针类型。
	\pre _tPointer 能转换为 bool 类型，当且仅当非空时为 true 。
	*/
	//@{
	template<typename _tPointer>
	yconstexprf bool
	is_null(const _tPointer& p)
	{
		return !static_cast<bool>(p);
	}
	template<typename _tPointer>
	yconstexprf bool
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
	yconstexprf bool
	is_not_null(const _tPointer& p)
	{
		return static_cast<bool>(p);
	}
	template<typename _tPointer>
	yconstexprf bool
	is_not_null(_tPointer&& p)
	{
		return static_cast<bool>(p);
	}
	//@}



	/*!	\defgroup is_dereferencable Is Dereferencable Iterator
	\brief 判断迭代器实例是否确定可解引用。
	\tparam _tIterator 迭代器类型。
	\note 注意返回 \c false 不表示参数实际不可解引用。
	\note 默认实现对指针使用 \c is_not_null 。
	*/
	//@{
	template<typename _tIterator>
	yconstexprf bool
	is_dereferencable(const _tIterator&)
	{
		return false;
	}
	template<typename _type>
	yconstexprf bool
	is_dereferencable(_type* p)
	{
		return is_not_null(p);
	}
	//@}


	/*!	\defgroup is_undereferencable Is Undereferencable Iterator
	\brief 判断迭代器实例是否为可解引用。
	\tparam _tIterator 迭代器类型。
	\note 注意返回 \c false 不表示参数实际可解引用。
	\note 默认实现对指针使用 \c is_null 。
	*/
	//@{
	template<typename _tIterator>
	yconstexprf bool
	is_undereferencable(const _tIterator&)
	{
		return false;
	}
	template<typename _type>
	yconstexprf bool
	is_undereferencable(_type* p)
	{
		return is_null(p);
	}
	//@}


	/*!	\defgroup raw Get Raw Pointers
	\brief 取内建指针。
	*/
	//@{
	template<typename _type>
	yconstexprf _type*
	raw(_type* const& p)
	{
		return p;
	}
	template<typename _type>
	yconstexprf _type*
	raw(const std::unique_ptr<_type>& p)
	{
		return p.get();
	}
	template<typename _type>
	yconstexprf _type*
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
	yconstexprf std::unique_ptr<_type>
	unique_raw(const _pSrc& p)
	{
		return std::unique_ptr<_type>(p);
	}
	/*!
	\tparam _pSrc 指定指针类型。
	*/
	template<typename _type, typename _pSrc>
	yconstexprf std::unique_ptr<_type>
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
	yconstexprf std::unique_ptr<_type>
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
	yconstexprf std::shared_ptr<_type>
	share_raw(const _pSrc& p)
	{
		return std::shared_ptr<_type>(p);
	}
	/*!
	\tparam _pSrc 指定指针类型。
	*/
	template<typename _type, typename _pSrc>
	yconstexprf std::shared_ptr<_type>
	share_raw(_pSrc&& p)
	{
		return std::shared_ptr<_type>(p);
	}
	template<typename _type>
	yconstexprf std::shared_ptr<_type>
	share_raw(_type* p)
	{
		return std::shared_ptr<_type>(p);
	}
	/*!
	\note 使用空指针构造空实例。
	*/
	template<typename _type>
	yconstexprf std::shared_ptr<_type>
	share_raw(const nullptr_t& p)
	{
		return std::shared_ptr<_type>();
	}
	//@}


	/*!	\defgroup pod_operations POD Type Operations
	\brief POD 类型操作。
	\tparam _type 指定对象类型。
	\pre 静态断言： <tt>std::is_pod<typename std::remove_reference<_type>::type>
		::value</tt> 。
	\note 不检查指针是否有效。
	*/
	//@{
	template <class _type>
	inline _type*
	pod_fill(_type* first, _type* last, _type&& value)
	{
		static_assert(std::is_pod<typename std::remove_reference<_type>::type>
			::value, "Non-POD type found @ pod_fill;");

		switch((last - first) & 7)
		{
		case 0:
			while(first != last)
			{
				*first = value; ++first;
		case 7: *first = value; ++first;
		case 6: *first = value; ++first;
		case 5: *first = value; ++first;
		case 4: *first = value; ++first;
		case 3: *first = value; ++first;
		case 2: *first = value; ++first;
		case 1: *first = value; ++first;
			}
		}
		return last;
	}

	template <class _type>
	inline _type*
	pod_copy_n(const _type* first, std::size_t n, _type* result)
	{
		static_assert(std::is_pod<typename std::remove_reference<_type>::type>
			::value, "Non-POD type found @ pod_copy_n;");

		std::memcpy(result, first, sizeof(*first) * n);
		return result + n;
	}

	template <class _type>
	inline _type*
	pod_copy(const _type* first, const _type* last, _type* result)
	{
		return pod_copy_n(first, last - first, result);
	}

	template <class _type>
	inline _type*
	pod_move_n(const _type* first, std::size_t n, _type* result)
	{
		static_assert(std::is_pod<typename std::remove_reference<_type>::type>
			::value, "Non-POD type found @ pod_move_n;");

		std::memmove(result, first, sizeof(*first) * n);
		return result + n;
	}

	template <class _type>
	inline _type*
	pod_move(const _type* first, const _type* last, _type* result)
	{
		return pod_move_n(first, last - first, result);
	}

	//@}

}

#endif

