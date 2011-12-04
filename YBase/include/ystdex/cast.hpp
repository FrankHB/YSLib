/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file cast.hpp
\ingroup YStandardEx
\brief C++ 转换模板类。
\version r1698;
\author FrankHB<frankhb1989@gmail.com>
\since build 175 。
\par 创建时间:
	2010-12-15 08:13:18 +0800;
\par 修改时间:
	2011-12-04 11:07 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YStandardEx::Cast;
*/


#ifndef YCL_INC_YSTDEX_CAST_HPP_
#define YCL_INC_YSTDEX_CAST_HPP_

#include "type_op.hpp"
#include <memory>
#include <typeinfo>

namespace ystdex
{
	/*!	\defgroup cast Cast
	\brief 显式类型转换。
	\since build 243 。
	*/


	/*!
	\ingroup cast
	\brief 多态类指针类型转换。
	\throw std::bad_cast dynamic_cast 失败。
	\since build 175 。
	*/
	template <class _tDst, class _tSrc>
	inline _tDst
	polymorphic_cast(_tSrc* x)
	{
		auto tmp(dynamic_cast<_tDst>(x));

		if(!tmp)
			throw std::bad_cast();
		return tmp;
	}

	/*!
	\ingroup cast
	\brief 多态类指针向派生类指针转换。
	\pre 断言： dynamic_cast 成功。
	\since build 175 。
	*/
	template <class _tDst, class _tSrc>
	inline _tDst
	polymorphic_downcast(_tSrc* x)
	{
		assert(dynamic_cast<_tDst>(x) == x);

		return _tDst(x);
	}
	/*!
	\ingroup cast
	\brief 多态类引用向派生类引用转换。
	\since build 175 。
	*/
	template <class _tDst, class _tSrc>
	yconstfn _tDst&
	polymorphic_downcast(_tSrc& x)
	{
		return *polymorphic_downcast<typename remove_reference<_tDst>
			::type*>(&x);
	}

	/*!
	\ingroup cast
	\brief 多态类指针交叉转换。
	\pre 断言： dynamic_cast 成功。
	\since build 179 。
	*/
	template <class _tDst, class _tSrc>
	inline _tDst
	polymorphic_crosscast(_tSrc* x)
	{
		auto p(dynamic_cast<_tDst>(x));

		assert(p);
		return p;
	}
	/*!
	\ingroup cast
	\brief 多态类引用交叉转换。
	\throw std::bad_cast dynamic_cast 失败。
	\since build 179 。
	*/
	template <class _tDst, class _tSrc>
	yconstfn _tDst&
	polymorphic_crosscast(_tSrc& x)
	{
		return *polymorphic_crosscast<typename remove_reference<_tDst>
			::type*>(&x);
	}


	namespace _impl
	{
		template<typename _tFrom, typename _tTo, bool _bNonVirtualDownCast>
		struct _general_polymorphic_cast_helper
		{
			static yconstfn _tTo
			cast(_tFrom x)
			{
				return polymorphic_downcast<_tTo>(x);
			}
		};
		template<typename _tFrom, typename _tTo>
		struct _general_polymorphic_cast_helper<_tFrom, _tTo, false>
		{
			static yconstfn _tTo
			cast(_tFrom x)
			{
				return dynamic_cast<_tTo>(x);
			}
		};

		template<typename _tFrom, typename _tTo, bool _bUseStaticCast>
		struct _general_cast_helper
		{
			static yconstfn _tTo
			cast(_tFrom x)
			{
				return _tTo(x);
			}
		};
		template<typename _tFrom, typename _tTo>
		struct _general_cast_helper<_tFrom, _tTo, false>
		{
			static yconstfn _tTo
			cast(_tFrom x)
			{
				return _general_polymorphic_cast_helper<_tFrom, _tTo,
					(is_base_of<_tFrom, _tTo>::value
					&& !has_common_nonempty_virtual_base<typename remove_rp<
					_tFrom>::type, typename remove_rp<_tTo>::type>::value)
				>::cast(x);
			}
		};
		template<typename _type>
		struct _general_cast_helper<_type, _type, true>
		{
			static inline _type
			cast(_type x)
			{
				return x;
			}
		};
		template<typename _type>
		struct _general_cast_helper<_type, _type, false>
		{
			static yconstfn _type
			cast(_type x)
			{
				return x;
			}
		};

		template<typename _tFrom, typename _tTo>
		struct _general_cast_type_helper
			: public integral_constant<bool, is_convertible<_tFrom, _tTo>
				::value>
		{};
	}

	/*!
	\ingroup cast
	\brief 一般类型转换。
	
	能确保安全隐式转换时使用 static_cast ；
	除此之外非虚基类向派生类转换使用 polymophic_downcast；
	否则使用 dynamic_cast。
	\since build 175 。
	*/
	//@{
	template<typename _tDst, typename _tSrc>
	yconstfn _tDst
	general_cast(_tSrc* x)
	{
		return _impl::_general_cast_helper<_tSrc*, _tDst,
			_impl::_general_cast_type_helper<_tSrc*, _tDst>::value>::cast(x);
	}
	template<typename _tDst, typename _tSrc>
	yconstfn _tDst
	general_cast(_tSrc& x)
	{
		return _impl::_general_cast_helper<_tSrc&, _tDst,
			_impl::_general_cast_type_helper<_tSrc&, _tDst>::value>::cast(x);
	}
	template<typename _tDst, typename _tSrc>
	yconstfn const _tDst
	general_cast(const _tSrc& x)
	{
		return _impl::_general_cast_helper<const _tSrc&, _tDst,
			_impl::_general_cast_type_helper<const _tSrc&, const _tDst>::value>
			::cast(x);
	}
	//@}
}

#endif

