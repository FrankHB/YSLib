/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ycast.hpp
\ingroup YCLib
\brief C++ 转换模板类。
\version 0.1542;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-12-15 08:13:18 + 08:00; 
\par 修改时间:
	2010-12-25 21:38 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YCLib::YCast;
*/


#ifndef INCLUDED_XWRAPPER_HPP_
#define INCLUDED_XWRAPPER_HPP_

#include "ystdex.h"
#include <memory>
#include <typeinfo>

namespace ystdex
{
	/*!
	\brief 自动指针转换：包装一个对象为一个整数。
	*/
	template<class _type, typename _tIntegral>
	_tIntegral
	auto_integral_cast(_type h)
	{
		return reinterpret_cast<_tIntegral>(
			std::auto_ptr<_type*>(new _type(h)));
	}

	/*!
	\brief 自动整数转换：转换整数为 std::auto 所有的对象。
	*/
	template<class _type, typename _tIntegral>
	_type
	integral_auto_cast(_tIntegral i)
	{
		_type* p(reinterpret_cast<std::auto_ptr<_type> >(i).release());

		assert(p);

		return *p;
	}


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

		enum
		{
			value = sizeof(C) < sizeof(A) + sizeof(B)
		};
	};


	template <class _tDst, class _tSrc>
	inline _tDst
	polymorphic_cast(_tSrc* x)
	{
		_tDst tmp(dynamic_cast<_tDst>(x));

		if(!tmp)
			throw std::bad_cast();
		return tmp;
	}

	template <class _tDst, class _tSrc>
	inline _tDst
	polymorphic_downcast(_tSrc* x)
	{
		assert(dynamic_cast<_tDst>(x) == x);

		return static_cast<_tDst>(x);
	}
	template <class _tDst, class _tSrc>
	inline _tDst&
	polymorphic_downcast(_tSrc& x)
	{
		return *polymorphic_downcast<typename remove_reference<_tDst>
			::type*>(&x);
	}


	template<typename _type>
	struct remove_rp
	{
		typedef typename remove_pointer<typename remove_reference<_type>
			::type>::type type;
	};


	namespace _impl
	{
		template<typename _tFrom, typename _tTo, bool _bNonVirtualDownCast>
		struct _general_polymorphic_cast_helper
		{
			inline static _tTo
			cast(_tFrom x)
			{
				return polymorphic_downcast<_tTo>(x);
			}
		};
		template<typename _tFrom, typename _tTo>
		struct _general_polymorphic_cast_helper<_tFrom, _tTo, false>
		{
			inline static _tTo
			cast(_tFrom x)
			{
				return dynamic_cast<_tTo>(x);
			}
		};

		template<typename _tFrom, typename _tTo, bool _bUseStaticCast>
		struct _general_cast_helper
		{
			inline static _tTo
			cast(_tFrom x)
			{
				return static_cast<_tTo>(x);
			}
		};
		template<typename _tFrom, typename _tTo>
		struct _general_cast_helper<_tFrom, _tTo, false>
		{
			inline static _tTo
			cast(_tFrom x)
			{
				return _general_polymorphic_cast_helper<_tFrom, _tTo,
					(is_base_of<_tFrom, _tTo>::value
					&& !has_common_nonempty_virtual_base<typename
					remove_rp<_tFrom>::type, typename remove_rp<_tTo>::type>::value)
				>::cast(x);
			}
		};
		template<typename _type>
		struct _general_cast_helper<_type, _type, true>
		{
			inline static _type
			cast(_type x)
			{
				return x;
			}
		};
		template<typename _type>
		struct _general_cast_helper<_type, _type, false>
		{
			inline static _type
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
	\defgroup GeneralCast
	\brief 一般类型转换。
	
	能确保安全隐式转换时使用 static_cast ；
	除此之外非虚基类向派生类转换使用 polymophic_downcast；
	否则使用 dynamic_cast。
	*/
	//@{
	template<typename _tDst, typename _tSrc>
	inline _tDst
	general_cast(_tSrc* x)
	{
		return _impl::_general_cast_helper<_tSrc*, _tDst,
			_impl::_general_cast_type_helper<_tSrc*, _tDst>::value>::cast(x);
	}
	template<typename _tDst, typename _tSrc>
	inline _tDst
	general_cast(_tSrc& x)
	{
		return _impl::_general_cast_helper<_tSrc&, _tDst,
			_impl::_general_cast_type_helper<_tSrc&, _tDst>::value>::cast(x);
	}
	template<typename _tDst, typename _tSrc>
	inline const _tDst
	general_cast(const _tSrc& x)
	{
		return _impl::_general_cast_helper<const _tSrc&, _tDst,
			_impl::_general_cast_type_helper<const _tSrc&, const _tDst>::value>
			::cast(x);
	}
	//@}
}

#endif

