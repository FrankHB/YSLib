/*
	© 2012-2015, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file array.hpp
\ingroup YStandardEx
\brief 数组操作。
\version r166
\author FrankHB <frankhb1989@gmail.com>
\since build 532
\par 创建时间:
	2014-09-01 18:39:25 +0800
\par 修改时间:
	2018-11-26 14:01 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Array
*/


#ifndef YB_INC_ystdex_array_hpp_
#define YB_INC_ystdex_array_hpp_ 1

#include "utility.hpp" // for yforward, decay_copy;
#include "type_op.hpp" // for common_nonvoid_t;
#include <array> // for std::array;
#include <algorithm> // for std::copy_n;
#include <memory> // for std::addressof;
#include <iterator> // for std::make_move_iterator;

namespace ystdex
{

/*!
\note 为避免复杂性，对 std::reference_wrapper 的实例及类似类型透明。
\see WG21 N4315 。
\since build 562
*/
//@{
/*!
\ingroup metafunctions
\brief 取指定类型和参数类型推断的 std::array 实例。
\sa common_nonvoid_t
*/
template<typename _type = void, typename... _tParams>
using common_array_t = std::array<common_nonvoid_t<_type, decay_t<_tParams>...>,
	sizeof...(_tParams)>;


//! \brief 构造指定参数转换初始化的 std::array 对象。
template<typename _type = void, typename... _tParams>
yconstfn common_array_t<_type, _tParams...>
cast_array(_tParams&&... args)
{
	return {{decay_copy(common_nonvoid_t<_type, _tParams...>(args))...}};
}

//! \brief 转移指定参数至 std::array 对象。
template<typename _type = void, typename... _tParams>
yconstfn common_array_t<_type, _tParams...>
forward_as_array(_tParams&&... args)
{
	return {{yforward(args)...}};
}

/*!
\brief 构造指定参数初始化的 \c std::array 对象。
\see WG21 N4481 。
*/
template<typename _type = void, typename... _tParams>
yconstfn common_array_t<_type, _tParams...>
make_array(_tParams&&... args)
{
	return {{decay_copy(args)...}};
}
//@}

/*!
\brief 转换指定参数为 \c std::array 对象。
\since build 337
*/
//@{
template<typename _type, size_t _vN, typename _tSrc>
yconstfn std::array<_type, _vN>
to_array(const _tSrc& src)
{
	return std::array<_type, _vN>(src);
}
template<typename _type, size_t _vN>
yconstfn std::array<_type, _vN>
to_array(const std::array<_type, _vN>& src)
{
	return src;
}
template<typename _type, size_t _vN, typename _tSrcElement>
inline std::array<_type, _vN>
to_array(const _tSrcElement(&src)[_vN])
{
	std::array<_type, _vN> arr;

	std::copy_n(std::addressof(src[0]), _vN, std::addressof(arr[0]));
	return arr;
}
template<typename _type, size_t _vN, typename _tSrcElement>
inline std::array<_type, _vN>
to_array(_tSrcElement(&&src)[_vN])
{
	std::array<_type, _vN> arr;

	std::copy_n(std::make_move_iterator(std::addressof(src[0])), _vN,
		std::addressof(arr[0]));
	return arr;
}
//! \since build 636
template<typename _type, size_t _vN, typename _tSrcElement>
inline std::array<_type, _vN>
to_array(std::initializer_list<_tSrcElement> il)
{
	std::array<_type, il.size()> arr;

	std::copy_n(std::make_move_iterator(il.begin()),
		std::min(_vN, il.size()), std::addressof(arr[0]));
	return arr;
}
//@}

} // namespace ystdex;

#endif

