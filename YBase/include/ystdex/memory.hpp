/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file memory.hpp
\ingroup YStandardEx
\brief 存储和智能指针特性。
\version r665
\author FrankHB <frankhb1989@gmail.com>
\since build 209
\par 创建时间:
	2011-05-14 12:25:13 +0800
\par 修改时间:
	2015-02-05 19:56 +0800
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

/*!
\brief 使用显式析构函数调用和 \c std::free 的删除器。
\note 数组类型的特化无定义。
\since build 561

除使用 \c std::free 代替 \c ::operator delete，和 \c std::default_deleter
的非数组类型元相同。注意和直接使用 \c std::free 不同，会调用析构函数且不适用于数组。
*/
//@{
template<typename _type>
struct free_delete
{
	yconstfn free_delete() ynothrow = default;
	template<typename _type2,
		yimpl(typename = enable_if_t<is_convertible<_type2*, _type*>::value>)>
	free_delete(const free_delete<_type2>&) ynothrow
	{}

	void
	operator()(_type* p) const
	{
		p->~_type();
		std::free(p);
	}
};

template<typename _type>
struct free_delete<_type[]>;
//@}


/*!
\brief 绑定值的删除器。
\tparam _type 绑定的值的类型。
\tparam _tPointer 指针类型。
\pre _tPointer 满足 \c NullablePointer 要求。
\since build 550
*/
template<typename _type, typename _tPointer = void*>
struct bound_deleter
{
	using pointer = _tPointer;

	mutable _type value;

	yconstfn
	bound_deleter() = default;
	template<typename _tParam,
		yimpl(typename = ystdex::exclude_self_ctor_t<bound_deleter, _tParam>)>
	yconstfn
	bound_deleter(_tParam&& arg)
		: value(yforward(arg))
	{}
	template<typename _tParam1, typename _tParam2, typename... _tParams>
	yconstfn
	bound_deleter(_tParam1&& arg1, _tParam2&& arg2, _tParams&&... args)
		: value(yforward(arg1), yforward(arg2), yforward(args)...)
	{}
	yconstfn
	bound_deleter(const bound_deleter&) = default;
	yconstfn
	bound_deleter(bound_deleter&&) = default;

	//! \since build 551
	bound_deleter&
	operator=(const bound_deleter&) = default;
	//! \since build 551
	bound_deleter&
	operator=(bound_deleter&&) = default;

	//! \brief 删除：空操作。
	inline void
	operator()(pointer) const ynothrow
	{}
};


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
//! \since build 550
template<typename _type, typename _fDeleter>
yconstfn auto
get_raw(const std::unique_ptr<_type, _fDeleter>& p) ynothrow
	-> decltype(p.get())
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
	return {};
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
	return {};
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
//! \since build 562
template<typename _type, typename _tDeleter, typename _pSrc>
yconstfn std::unique_ptr<_type, _tDeleter>
unique_raw(_pSrc&& p, _tDeleter&& d)
{
	static_assert(is_pointer<_pSrc>::value, "Invalid type found.");

	return std::unique_ptr<_type, _tDeleter>(p, yforward(d));
}
//! \since build 562
template<typename _type, typename _tDeleter>
yconstfn std::unique_ptr<_type, _tDeleter>
unique_raw(_type* p, _tDeleter&& d)
{
	return std::unique_ptr<_type, _tDeleter>(p, yforward(d));
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
\brief 使用 new 和指定参数构造指定类型的 std::unique_ptr 对象。
\tparam _type 被指向类型。
*/
//@{
/*!
\note 使用值初始化。
\see http://herbsutter.com/gotw/_102/ 。
\see ISO WG21/N3656 。
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
yimpl(enable_if_t<extent<_type>::value != 0>)
make_unique(_tParams&&...) = delete;
//@}

/*!
\note 使用默认初始化。
\see ISO WG21/N3588 A4 。
\since build 526
*/
//@{
template<typename _type, typename... _tParams>
yconstfn yimpl(enable_if_t<!is_array<_type>::value, std::unique_ptr<_type>>)
make_unique_default_init()
{
	return std::unique_ptr<_type>(new _type);
}
template<typename _type, typename... _tParams>
yconstfn yimpl(enable_if_t<is_array<_type>::value && extent<_type>::value == 0,
	std::unique_ptr<_type>>)
make_unique_default_init(size_t size)
{
	return std::unique_ptr<_type>(new remove_extent_t<_type>[size]);
}
template<typename _type,  typename... _tParams>
yimpl(enable_if_t<extent<_type>::value != 0>)
make_unique_default_init(_tParams&&...) = delete;
//@}
/*!
\ingroup helper_functions
\brief 使用指定类型的初始化列表构造指定类型的 std::unique_ptr 对象。
\tparam _type 被指向类型。
\tparam _tValue 初始化列表的元素类型。
\since build 574
*/
template<typename _type, typename _tValue>
yconstfn std::unique_ptr<_type>
make_unique(std::initializer_list<_tValue> il)
{
	return ystdex::make_unique<_type>(il);
}

/*!
\note 使用删除器。
\since build 562
*/
//@{
template<typename _type, typename _tDeleter, typename... _tParams>
yconstfn yimpl(enable_if_t<!is_array<_type>::value, std::unique_ptr<_type>>)
make_unique_with(_tDeleter&& d)
{
	return std::unique_ptr<_type, _tDeleter>(new _type, yforward(d));
}
template<typename _type, typename _tDeleter, typename... _tParams>
yconstfn yimpl(enable_if_t<is_array<_type>::value && extent<_type>::value == 0,
	std::unique_ptr<_type>>)
make_unique_with(_tDeleter&& d, size_t size)
{
	return std::unique_ptr<_type, _tDeleter>(new remove_extent_t<_type>[size],
		yforward(d));
}
template<typename _type, typename _tDeleter, typename... _tParams>
yimpl(enable_if_t<extent<_type>::value != 0>)
make_unique_with(_tDeleter&&, _tParams&&...) = delete;
//@}


//@}

/*!
\ingroup helper_functions
\brief 使用指定类型的初始化列表构造指定类型的 std::shared_ptr 对象。
\tparam _type 被指向类型。
\tparam _tValue 初始化列表的元素类型。
\since build 529
*/
template<typename _type, typename _tValue>
yconstfn std::shared_ptr<_type>
make_shared(std::initializer_list<_tValue> il)
{
	return std::make_shared<_type>(il);
}

} // namespace ystdex;

#endif

