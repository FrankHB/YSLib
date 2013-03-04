/*
	Copyright by FrankHB 2011 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file memory.hpp
\ingroup YStandardEx
\brief 存储和智能指针特性。
\version r423
\author FrankHB <frankhb1989@gmail.com>
\since build 209
\par 创建时间:
	2011-05-14 12:25:13 +0800
\par 修改时间:
	2013-03-02 07:25 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Memory
*/


#ifndef YB_INC_ystdex_memory_hpp_
#define YB_INC_ystdex_memory_hpp_ 1

#include "type_op.hpp" // for ../ydef.h and is_pointer;
#include <memory>
#include <cstring> // for std::memcpy and std::memmove;

namespace ystdex
{

/*!	\defgroup is_dereferencable Is Dereferencable Iterator
\brief 判断迭代器实例是否确定可解引用。
\tparam _tIterator 迭代器类型。
\note 注意返回 \c false 不表示参数实际不可解引用。
\note 默认实现对参数转换为 \c bool 类型判断是否为 true 。
\since build 249
*/
//@{
template<typename _tIterator>
yconstfn bool
is_dereferencable(const _tIterator&)
{
	return false;
}
template<typename _type>
yconstfn bool
is_dereferencable(_type* p)
{
	return bool(p);
}
//@}


/*!	\defgroup is_undereferencable Is Undereferencable Iterator
\brief 判断迭代器实例是否为可解引用。
\tparam _tIterator 迭代器类型。
\note 注意返回 \c false 不表示参数实际可解引用。
\note 默认实现对参数转换为 \c bool 类型判断是否为 false 。
\since build 250
*/
//@{
template<typename _tIterator>
yconstfn bool
is_undereferencable(const _tIterator&)
{
	return false;
}
template<typename _type>
yconstfn bool
is_undereferencable(_type* p)
{
	return !bool(p);
}
//@}


/*!	\defgroup raw Get Raw Pointers
\brief 取内建指针。
\since build 319
*/
//@{
template<typename _type>
yconstfn _type*
raw(_type* const& p) ynothrow
{
	return p;
}
template<typename _type>
yconstfn auto
raw(const std::unique_ptr<_type>& p) ynothrow -> decltype(p.get())
{
	return p.get();
}
template<typename _type>
yconstfn _type*
raw(const std::shared_ptr<_type>& p) ynothrow
{
	return p.get();
}
template<typename _type>
yconstfn _type*
raw(const std::weak_ptr<_type>& p) ynothrow
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
\since build 293
\todo 其它 deleter 的重载。
*/
template<typename _type, typename... _tParams>
yconstfn std::unique_ptr<_type>
make_unique(_tParams&&... args)
{
	return std::unique_ptr<_type>(new _type(yforward(args)...));
}

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

