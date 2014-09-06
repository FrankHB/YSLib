/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file array.hpp
\ingroup YStandardEx
\brief 数组操作。
\version r103
\author FrankHB <frankhb1989@gmail.com>
\since build 532
\par 创建时间:
	2014-09-01 18:39:25 +0800
\par 修改时间:
	2014-09-01 19:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Array
*/


#ifndef YB_INC_ystdex_array_hpp_
#define YB_INC_ystdex_array_hpp_ 1

#include "type_op.hpp" // for yforward, ystdex::decay_copy;
#include <array> // for std::array;
#include <algorithm> // for std::copy_n;
#include <memory> // for std::addressof;

namespace ystdex
{

/*!
\brief 构造指定参数初始化的 std::array 对象。
\since build 337
*/
template<typename _type, typename... _tParams>
inline std::array<_type, sizeof...(_tParams)>
make_array(_tParams&&... args)
{
	// TODO: Use one pair of braces (depending on G++).
	return {{decay_copy(args)...}};
}

/*!
\brief 转移指定参数至 std::array 对象。
\since build 495
*/
template<typename _type, typename... _tParams>
inline std::array<_type, sizeof...(_tParams)>
forward_as_array(_tParams&&... args)
{
	// TODO: Use one pair of braces (depending on G++).
	return {{yforward(args)...}};
}

/*!
\brief 转换指定参数为 std::array 对象。
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
//@}

} // namespace ystdex;

#endif

