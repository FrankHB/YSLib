/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ref.hpp
\ingroup YStandardEx
\brief 引用包装。
\version r257
\author FrankHB <frankhb1989@gmail.com>
\since build 588
\par 创建时间:
	2015-03-28 22:29:20 +0800
\par 修改时间:
	2015-09-30 10:07 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Ref
*/


#ifndef YB_INC_ystdex_ref_hpp_
#define YB_INC_ystdex_ref_hpp_ 1

#include "../ydef.h"
#include <functional> // for std::reference_wrapper;
#include <memory> // for std::addressof;

namespace ystdex
{

//! \since build 554
//@{
/*!
\brief 左值引用包装。
\tparam _type 被包装的类型。

类似 std::reference_wrapper 和 \c boost::reference_wrapper 公共接口兼容的
	引用包装类实现。
和 std::reference_wrapper 不同而和 \c boost::reference_wrapper 类似，
	不要求模板参数为完整类型。
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
		: ptr(std::addressof(t))
	{}
	yconstfn
	lref(std::reference_wrapper<_type> t) ynothrow
		: lref(t.get())
	{}

	//! \since build 556
	lref(const lref&) = default;

	//! \since build 556
	lref&
	operator=(const lref&) = default;

	operator _type&() const ynothrow
	{
		return *ptr;
	}

	_type&
	get() const ynothrow
	{
		return *ptr;
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
template <class _type>
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
\brief 取引用包装实例特征。
\since build 348
*/
//@{
template<typename _type>
struct wrapped_traits : false_type
{
	using type = _type;
};

template<class _tWrapped>
struct wrapped_traits<std::reference_wrapper<_tWrapped>> : true_type
{
	using type = _tWrapped;
};

//! \since build 554
template<class _tWrapped>
struct wrapped_traits<lref<_tWrapped>> : true_type
{
	using type = _tWrapped;
};
//@}

/*!
\ingroup metafunctions
\since build 525
*/
template<typename _type>
using wrapped_traits_t = typename wrapped_traits<_type>::type;


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
	yconstfn YB_PURE
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


/*!
\brief 伪输出对象。
\note 吸收所有赋值操作。
\since build 273
*/
struct pseudo_output
{
	//! \since build 636
	template<typename... _tParams>
	yconstfn
	pseudo_output(_tParams&&...)
	{}

	//! \since build 640
	//@{
	template<typename _tParam,
		yimpl(exclude_self_ctor_t<pseudo_output, _tParam>)>
	yconstfn const pseudo_output&
	operator=(_tParam&&) const
	{
		return *this;
	}
	template<typename... _tParams>
	yconstfn const pseudo_output&
	operator()(_tParams&&...) const
	{
		return *this;
	}
	//@}
};


/*!
\ingroup metafunctions
\since build 636
*/
//@{
//! \brief 若类型不是空类型则取后备结果类型（默认为 pseudo_output ）。
template<typename _type, typename _tRes = pseudo_output>
using nonvoid_result_t
	= conditional_t<!is_void<_type>::value, _type, pseudo_output>;

//! \brief 若类型不是对象类型则取后备结果类型（默认为 pseudo_output ）。
template<typename _type, typename _tRes = pseudo_output>
using object_result_t
	= conditional_t<is_object<_type>::value, _type, pseudo_output>;
//@}

} // namespace ystdex;

#endif

