/*
	© 2011-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file memory.hpp
\ingroup YStandardEx
\brief 存储和智能指针特性。
\version r496
\author FrankHB <frankhb1989@gmail.com>
\since build 209
\par 创建时间:
	2011-05-14 12:25:13 +0800
\par 修改时间:
	2014-02-16 17:47 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Memory
*/


#ifndef YB_INC_ystdex_memory_hpp_
#define YB_INC_ystdex_memory_hpp_ 1

#include "type_op.hpp" // for ../ydef.h, is_pointer, is_array, extent,
//	enable_if_t and remove_extent_t;
#include <memory>

namespace ystdex
{

/*!	\defgroup get_raw Get get_raw Pointers
\brief 取内建指针。
\since build 422
*/
//@{
template<typename _type>
yconstfn _type*
get_raw(_type* const& p) ynothrow
{
	return p;
}
template<typename _type>
yconstfn auto
get_raw(const std::unique_ptr<_type>& p) ynothrow -> decltype(p.get())
{
	return p.get();
}
template<typename _type>
yconstfn _type*
get_raw(const std::shared_ptr<_type>& p) ynothrow
{
	return p.get();
}
template<typename _type>
yconstfn _type*
get_raw(const std::weak_ptr<_type>& p) ynothrow
{
	return p.lock().get();
}
//@}

/*!	\defgroup reset Reset Pointers
\brief 安全删除指定引用的指针指向的对象。
\post 指定引用的指针为空。
\since build 209
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
\ingroup helper_functions
\brief 使用指定类型指针构造 std::unique_ptr 实例。
\tparam _type 被指向类型。
\note 不检查指针是否有效。
\since build 212
*/
//@{
/*!
\tparam _pSrc 指定指针类型。
\pre 静态断言： _pSrc 是内建指针。
*/
template<typename _type, typename _pSrc>
yconstfn std::unique_ptr<_type>
unique_raw(const _pSrc& p)
{
	static_assert(is_pointer<_pSrc>::value, "Invalid type found.");

	return std::unique_ptr<_type>(p);
}
/*!
\tparam _pSrc 指定指针类型。
\pre 静态断言： _pSrc 是内建指针。
*/
template<typename _type, typename _pSrc>
yconstfn std::unique_ptr<_type>
unique_raw(_pSrc&& p)
{
	static_assert(is_pointer<_pSrc>::value, "Invalid type found.");

	return std::unique_ptr<_type>(p);
}
template<typename _type>
yconstfn std::unique_ptr<_type>
unique_raw(_type* p)
{
	return std::unique_ptr<_type>(p);
}
/*!
\note 使用空指针构造空实例。
\since build 319
*/
template<typename _type>
yconstfn std::unique_ptr<_type>
unique_raw(nullptr_t) ynothrow
{
	return std::unique_ptr<_type>();
}
//@}


/*!	\defgroup share_raw Get Shared Pointer
\ingroup helper_functions
\brief 使用指定类型指针构造 std::shared_ptr 实例。
\tparam _type 被指向类型。
\note 不检查指针是否有效。
\since build 204
*/
//@{
/*!
\tparam _pSrc 指定指针类型。
\pre 静态断言： _pSrc 是内建指针。
*/
template<typename _type, typename _pSrc>
yconstfn std::shared_ptr<_type>
share_raw(const _pSrc& p)
{
	static_assert(is_pointer<_pSrc>::value, "Invalid type found.");

	return std::shared_ptr<_type>(p);
}
/*!
\tparam _pSrc 指定指针类型。
\pre 静态断言： _pSrc 是内建指针。
*/
template<typename _type, typename _pSrc>
yconstfn std::shared_ptr<_type>
share_raw(_pSrc&& p)
{
	static_assert(is_pointer<_pSrc>::value, "Invalid type found.");

	return std::shared_ptr<_type>(p);
}
template<typename _type>
yconstfn std::shared_ptr<_type>
share_raw(_type* p)
{
	return std::shared_ptr<_type>(p);
}
/*!
\note 使用空指针构造空实例。
\since build 319
*/
template<typename _type>
yconstfn std::shared_ptr<_type>
share_raw(nullptr_t) ynothrow
{
	return std::shared_ptr<_type>();
}
//@}


/*!
\ingroup helper_functions
\brief 使用 new 和指定参数构造指定类型的 std::unique_ptr 实例。
\tparam _type 被指向类型。
\see http://herbsutter.com/gotw/_102/ 。
\see ISO WG21/N3797 20.7.2[memory.syn] 。
\since build 476
*/
//@{
template<typename _type, typename... _tParams>
yconstfn yimpl(enable_if_t<!is_array<_type>::value, std::unique_ptr<_type>>)
make_unique(_tParams&&... args)
{
	return std::unique_ptr<_type>(new _type(yforward(args)...));
}
template<typename _type, typename... _tParams>
yconstfn yimpl(enable_if_t<is_array<_type>::value && extent<_type>::value == 0,
	std::unique_ptr<_type>>)
make_unique(size_t size)
{
	return std::unique_ptr<_type>(new remove_extent_t<_type>[size]());
}
template<typename _type,  typename... _tParams>
yimpl(enable_if_t<extent<_type>::value != 0, void>)
make_unique(_tParams&&...) = delete;
//@}

/*!
\ingroup helper_functions
\brief 使用 new 和指定参数构造指定类型的 std::shared_ptr 实例。
\tparam _type 被指向类型。
\note 不同于 std::make_shared ，不依赖于具体 allocator 和 deleter 类型信息。
\note std::make_shared 的具体实现一般会导致目标文件增大。
\since build 298
*/
template<typename _type, typename... _tParams>
yconstfn std::shared_ptr<_type>
make_shared(_tParams&&... args)
{
	return std::shared_ptr<_type>(new _type(yforward(args)...));
}

} // namespace ystdex;

#endif

