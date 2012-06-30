/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file utility.hpp
\ingroup YStandardEx
\brief 函数对象和实用程序。
\version r1986;
\author FrankHB<frankhb1989@gmail.com>
\since build 189 。
\par 创建时间:
	2010-05-23 06:10:59 +0800;
\par 修改时间:
	2012-06-28 11:09 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YStandardEx::Utilities;
*/


#ifndef YB_INC_YSTDEX_UTILITY_HPP_
#define YB_INC_YSTDEX_UTILITY_HPP_

#include "type_op.hpp" // for ../ydef.h, ystdex::variadic_sequence,
	// ystdex::make_natural_sequence;
#include <cstring>
#include <utility>
#include <functional>
#include <array> // for std::array;
#include <string> // for std::char_traits;
#include <algorithm> // for std::copy_n;

namespace ystdex
{

/*!
\brief 顺序递归调用。
\since build 303 。
*/
//@{
template<typename _fCallable>
inline void
seq_apply(_fCallable)
{}
template<typename _fCallable, typename _type, typename... _tParams>
inline void
seq_apply(_fCallable f, _type&& arg, _tParams&&... args)
{
	f(arg), seq_apply(f, yforward(args)...);
}
//@}


namespace details
{

template<class>
struct unseq_dispatcher;

template<size_t... _vSeq>
struct unseq_dispatcher<variadic_sequence<_vSeq...>>
{
	template<typename _fCallable, typename... _tParams>
	static inline void
	call(_fCallable f, _tParams&&... args)
	{
		yunseq((f(yforward(args)), 0)...);
	}
};

} // namespace details;

/*!
\brief 非顺序调用。
\since build 303 。
*/
template<typename _fCallable, typename... _tParams>
inline void
unseq_apply(_fCallable f, _tParams&&... args)
{
	details::unseq_dispatcher<typename make_natural_sequence<
		sizeof...(_tParams)>::type>::call(f, yforward(args)...);
}


/*!	\defgroup helper_functions Helper Functions
\brief 助手功能。
*/

/*
\brief 不可复制对象：禁止继承此类的对象调用复制构造函数和复制赋值操作符。
\warning 非虚析构。
*/
struct noncopyable
{
protected:
	/*!
	\brief \c protected 构造：空实现。
	\note 保护非多态类。
	*/
	yconstfn
	noncopyable()
	{}
	/*!
	\brief \c protected 析构：空实现。
	*/
	~noncopyable()
	{}

public:
	/*!
	\brief 禁止复制构造。
	*/
	yconstfn
	noncopyable(const noncopyable&) = delete;

	/*!
	\brief 禁止赋值复制。
	*/
	noncopyable&
	operator=(const noncopyable&) = delete;
};


/*!
\ingroup helper_functions
\brief 计算指定数组类型对象的长度。
\since build 291 。
*/
//@{
template<typename _type, size_t _vN>
yconstfn size_t
arrlen(_type(&)[_vN])
{
	return _vN;
}
template<typename _type, size_t _vN>
yconstfn size_t
arrlen(_type(&&)[_vN])
{
	return _vN;
}
//@}


/*!
\brief 取指定参数初始化的 std::array 对象。
\since build 304 。
*/
//@{
template<typename _type, size_t _vN, typename _tSrc>
yconstfn std::array<_type, _vN>
make_array(const _tSrc& src)
{
	return std::array<_type, _vN>(src);
}
template<typename _type, size_t _vN>
yconstfn std::array<_type, _vN>
make_array(const std::array<_type, _vN>& src)
{
	return src;
}
template<typename _type, size_t _vN, typename _tSrcElement>
inline std::array<_type, _vN>
make_array(const _tSrcElement(&src)[_vN])
{
	using namespace std;

	array<_type, _vN> arr;

	copy_n(addressof(src[0]), _vN, addressof(arr[0]));
	return std::move(arr);
}
template<typename _type, size_t _vN, typename _tSrcElement>
inline std::array<_type, _vN>
make_array(_tSrcElement(&&src)[_vN])
{
	using namespace std;

	array<_type, _vN> arr;

	copy_n(make_move_iterator(addressof(src[0])), _vN, addressof(arr[0]));
	return std::move(arr);
}
//@}


/*!
\brief 按标识调用函数，保证调用一次。
\note 类似 std::call_once ，但多线程环境下失效。
\since build 301 。
	
当标识为 true 时候无作用，否则调用函数。
*/
template<typename _fCallable, typename... _tParams>
void
call_once(bool& b, _fCallable f, _tParams&&... args)
{
	if(!b)
	{
		f(yforward(args)...);
		b = true;
	}
}


/*!
\brief 类型参数化静态对象。
\warning 不可重入。
\warning 非线程安全。
\since build 303 。
*/
template<typename _type, typename, typename...>
inline _type&
parameterize_static_object()
{
	static _type obj;

	return obj;
}
/*!
\brief 非类型参数化静态对象。
\warning 不可重入。
\warning 非线程安全。
\since build 301 。
*/
template<typename _type, size_t...>
inline _type&
parameterize_static_object()
{
	static _type obj;

	return obj;
}


/*!
\brief 取类型标识和初始化调用指定的对象。
\tparam _tKey 起始类型参数化标识。
\tparam _tKeys 非起始类型参数化标识。
\tparam _fInit 初始化调用类型。
\tparam _tParams 初始化参数类型。
\return 初始化后的对象的左值引用。
\since build 303 。
*/
template<typename _tKey, typename... _tKeys, typename _fInit,
	typename... _tParams>
inline auto
get_init(_fInit f, _tParams&&... args) -> decltype(f(yforward(args)...))&
{
	typedef decltype(f(yforward(args)...)) obj_type;

	auto& p(parameterize_static_object<obj_type*, _tKey, _tKeys...>());

	if(!p)
		p = new obj_type(f(yforward(args)...));
	return *p;
}
/*!
\brief 取非类型标识和初始化调用指定的对象。
\tparam _vKeys 非类型参数化标识。
\tparam _fInit 初始化调用类型。
\tparam _tParams 初始化参数类型。
\return 初始化后的对象的左值引用。
\since build 301 。
*/
template<size_t... _vKeys, typename _fInit, typename... _tParams>
inline auto
get_init(_fInit f, _tParams&&... args) -> decltype(f(yforward(args)...))&
{
	typedef decltype(f(yforward(args)...)) obj_type;

	auto& p(parameterize_static_object<obj_type*, _vKeys...>());

	if(!p)
		p = new obj_type(f(yforward(args)...));
	return *p;
}


/*!	\defgroup functors General Functors
\brief 仿函数。
\note 函数对象包含函数指针和仿函数。
\since build 243 。
*/

/*!
\ingroup functors
\brief 引用相等关系仿函数。
*/
template<typename _type>
struct ref_eq
{
	yconstfn bool
	operator()(const _type& _x, const _type& _y) const
	{
		return &_x == &_y;
	}
};

/*!
\ingroup functors
\brief 编译期选择自增/自减运算仿函数。
*/
//@{
template<bool, typename _tScalar>
struct xcrease_t
{
	inline _tScalar&
	operator()(_tScalar& _x)
	{
		return ++_x;
	}
};
template<typename _tScalar>
struct xcrease_t<false, _tScalar>
{
	inline _tScalar&
	operator()(_tScalar& _x)
	{
		return --_x;
	}
};
//@}

/*!
\ingroup functors
\brief 编译期选择加法/减法复合赋值运算仿函数。
\since build 284 。
*/
//@{
template<bool, typename _tScalar1, typename _tScalar2>
struct delta_assignment
{
	yconstfn _tScalar1&
	operator()(_tScalar1& x, _tScalar2 y)
	{
		return x += y;
	}
};
template<typename _tScalar1, typename _tScalar2>
struct delta_assignment<false, _tScalar1, _tScalar2>
{
	yconstfn _tScalar1&
	operator()(_tScalar1& x, _tScalar2 y)
	{
		return x -= y;
	}
};
//@}

/*!
\ingroup helper_functions
\brief 编译期选择自增/自减运算。
*/
template<bool _bIsPositive, typename _tScalar>
yconstfn _tScalar&
xcrease(_tScalar& _x)
{
	return xcrease_t<_bIsPositive, _tScalar>()(_x);
}

/*!
\ingroup HelperFunctions
\brief 编译期选择加法/减法复合赋值运算。
\since build 284 。
*/
template<bool _bIsPositive, typename _tScalar1, typename _tScalar2>
yconstfn _tScalar1&
delta_assign(_tScalar1& _x, _tScalar2& _y)
{
	return delta_assignment<_bIsPositive, _tScalar1, _tScalar2>()(_x, _y);
}


/*!
\ingroup Functors
\brief 引用仿函数。
*/
template<typename _type>
struct deref_op
{
	/*!
	\brief 对指定对象使用 operator& 并返回结果。
	*/
	yconstfn _type*
	operator()(_type& _x) const
	{
		return &_x;
	}
};


/*!
\ingroup Functors
\brief const 引用仿函数。
*/
template<typename _type>
struct const_deref_op
{
	/*!
	\brief 对指定 const 对象使用 operator& 并返回结果。
	*/
	inline const _type*
	operator()(const _type& _x) const
	{
		return &_x;
	}
};


/*!
\ingroup Functors
\brief 间接访问比较仿函数。
\warning 非虚析构。
\since build 315 。
*/
template<typename _type, typename _tPointer = _type*,
	class _fCompare = std::less<_type>>
struct deref_comp
{
	/*
	\brief 比较指针指向的对象。
	\return 若参数有空指针则 false ，否则判断是否满足 _fCompare()(*_x, *_y) 。
	*/
	bool
	operator()(const _tPointer& _x, const _tPointer& _y) const
	{
		return _x && _y && _fCompare()(*_x, *_y);
	}
};


/*!
\ingroup Functors
\brief 间接访问字符串比较仿函数。
\warning 非虚析构。
\since build 315 。
*/
template<typename _tChar, class _fCompare = std::less<_tChar>>
struct deref_str_comp
{
	/*!
	\brief 比较指定字符串首字符的指针。
	\return 若参数有空指针则 false ，否则判断指定字符串是否满足字典序严格偏序关系。
	*/
	bool
	operator()(const _tChar* x, const _tChar* y) const
	{
		typedef std::char_traits<_tChar> traits_type;

		return x && y && std::lexicographical_compare(x, x + traits_type
			::length(x), y, y + traits_type::length(y), _fCompare());
	}
};

} // namespace ystdex;

#ifndef YB_HAS_BUILTIN_NULLPTR
using ystdex::nullptr;
#endif

#endif

