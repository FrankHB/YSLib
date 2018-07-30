/*
	© 2011-2016, 2018 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ref.hpp
\ingroup YStandardEx
\brief 引用包装。
\version r464
\author FrankHB <frankhb1989@gmail.com>
\since build 588
\par 创建时间:
	2015-03-28 22:29:20 +0800
\par 修改时间:
	2018-07-30 06:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Ref

扩展标准库头 <functional> ，提供替代 std::reference_wrapper 的接口。
*/


#ifndef YB_INC_ystdex_ref_hpp_
#define YB_INC_ystdex_ref_hpp_ 1

#include "addressof.hpp" // for "addressof.hpp", ystdex::addressof, false_,
//	true_, exclude_self_t, cond_t, not_, is_object;
#include "invoke.hpp" // for "invoke.hpp", std::reference_wrapper,
//	invoke_result_t, ystdex::invoke;

namespace ystdex
{

//! \since build 554
//@{
/*!
\brief 左值引用包装。
\tparam _type 被包装的类型。
\note 满足 TrivialCopyable 要求。
\see WG21 N4277 。
\see WG21 P0357R1 。

类似 std::reference_wrapper 和 \c boost::reference_wrapper 公共接口兼容的
	引用包装类实现。
和 std::reference_wrapper 不同而和 \c boost::reference_wrapper 类似，
	不提供 weak result type ，不要求模板参数为完整类型。
*/
//@{
template<typename _type>
class lref
{
public:
	using type = _type;

private:
	_type* ptr;

public:
	yconstfn
	lref(_type& t) ynothrow
		: ptr(ystdex::addressof(t))
	{}
	//! \since build 661
	lref(_type&&) = delete;
	yconstfn
	lref(std::reference_wrapper<_type> t) ynothrow
		: lref(t.get())
	{}

	//! \since build 556
	lref(const lref&) = default;

	//! \since build 556
	lref&
	operator=(const lref&) = default;

	yconstfn
	operator _type&() const ynothrow
	{
		return *ptr;
	}

	yconstfn _type&
	get() const ynothrow
	{
		return *ptr;
	}

	/*!
	\pre 要求对象是完整类型。
	\since build 832
	*/
	template<typename... _tParams>
	yconstfn invoke_result_t<_type&, _tParams...>
	operator()(_tParams&&... args) const
	{
		return ystdex::invoke(*ptr, yforward(args)...);
	}
};

/*!
\brief 构造引用包装。
\relates lref
*/
//@{
template<typename _type>
yconstfn lref<_type>
ref(_type& t)
{
	return lref<_type>(t);
}
template<class _type>
void
ref(const _type&&) = delete;

template<typename _type>
yconstfn lref<const _type>
cref(const _type& t)
{
	return lref<const _type>(t);
}
template<class _type>
void
cref(const _type&&) = delete;
//@}
//@}


/*!
\ingroup unary_type_traits
\brief 判断模板参数指定的类型是否为引用包装的实例。
\note 接口含义类似 boost::is_reference_wrapper 。
\since build 675
*/
//@{
template<typename _type>
struct is_reference_wrapper : false_
{};

template<typename _type>
struct is_reference_wrapper<std::reference_wrapper<_type>> : true_
{};

template<typename _type>
struct is_reference_wrapper<lref<_type>> : true_
{};
//@}

/*!
\ingroup transformation_traits
\see WG21 P0318R0 。
*/
//@{
/*!
\brief 取引用包装的类型或未被包装的模板参数类型。
\note 接口含义类似 boost::unwrap_reference 。
\since build 675
*/
//@{
template<typename _type>
struct unwrap_reference
{
	using type = _type;
};

template<typename _type>
using unwrap_reference_t = _t<unwrap_reference<_type>>;

template<typename _type>
struct unwrap_reference<std::reference_wrapper<_type>>
{
	using type = _type&;
};

template<typename _type>
struct unwrap_reference<lref<_type>>
{
	using type = _type&;
};
//@}


//! \since build 732
//@{
template<typename _type>
struct decay_unwrap : unwrap_reference<decay_t<_type>>
{};

template<typename _type>
using decay_unwrap_t = _t<decay_unwrap<_type>>;
//@}
//@}


/*!
\brief 包装间接操作的引用适配器。
\since build 723
*/
template<typename _type, typename _tReference = lref<_type>>
class indirect_ref_adaptor
{
public:
	using value_type = _type;
	using reference = _tReference;

private:
	reference ref;

public:
	indirect_ref_adaptor(value_type& r)
		: ref(r)
	{}

	auto
	get() ynothrow -> decltype(ref.get().get())
	{
		return ref.get().get();
	}
};


/*!
\brief 解除引用包装。
\note 默认仅提供对 std::reference_wrapper 和 lref 的实例类型的重载。
\note 使用 ADL 。
\since build 348
*/
//@{
template<typename _type>
yconstfn _type&
unref(_type&& x) ynothrow
{
	return x;
}
//! \since build 554
template<typename _type>
yconstfn _type&
unref(const lref<_type>& x) ynothrow
{
	return x.get();
}
//@}


/*!
\brief 任意对象引用类型。
\warning 不检查 cv-qualifier 。
\since build 247
\todo 右值引用版本。
*/
class void_ref
{
private:
	const volatile void* ptr;

public:
	template<typename _type>
	yconstfn
	void_ref(_type& obj)
		: ptr(&obj)
	{}

	template<typename _type>
	YB_PURE yconstfn
	operator _type&() const
	{
		return *static_cast<_type*>(&*this);
	}

	YB_PURE void*
	operator&() const volatile
	{
		return const_cast<void*>(ptr);
	}
};

} // namespace ystdex;

#endif

