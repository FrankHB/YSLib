/*
	© 2015-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file placement.hpp
\ingroup YStandardEx
\brief 放置对象管理操作。
\version r516
\author FrankHB <frankhb1989@gmail.com>
\since build 715
\par 创建时间:
	2016-08-03 18:56:31 +0800
\par 修改时间:
	2016-08-06 15:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Placement

提供放置对象分配和释放操作，包括 WG21 P0040R3 提议的部分接口。
*/


#ifndef YB_INC_ystdex_placement_hpp_
#define YB_INC_ystdex_placement_hpp_ 1

#include "addressof.hpp" // for "addressof.hpp", empty_base, integral_constant,
//	YB_ASSUME, ystdex::constfn_addressof, yforward, is_lvalue_reference,
//	std::pair, std::unique_ptr;
#include <new> // for placement ::operator new from standard library;
#include <iterator> // for std::iterator_traits;
#include "deref_op.hpp" // for is_undereferenceable;
#include "cassert.h" // for yconstraint;

namespace ystdex
{

/*!
\brief 默认初始化标记。
\since build 677
*/
yconstexpr const struct default_init_t{} default_init{};

/*!
\brief 值初始化标记。
\since build 705
*/
yconstexpr const struct value_init_t{} value_init{};


/*!
\see WG21 P0032R3 。
\see WG21 N4606 20.2.7[utility.inplace] 。
*/
//@{
//! \brief 原地标记类型。
struct in_place_tag
{
	in_place_tag() = delete;
};

//! \brief 原地空标记类型。
using in_place_t = in_place_tag(&)(yimpl(empty_base<>));

//! \brief 原地类型标记模板。
template<typename _type>
using in_place_type_t = in_place_tag(&)(yimpl(empty_base<_type>));

//! \brief 原地索引标记模板。
template<size_t _vIdx>
using in_place_index_t
	= in_place_tag(&)(yimpl(integral_constant<size_t, _vIdx>));

/*!
\ingroup helper_functions
\brief 原地标记函数。
\warning 调用引起未定义行为。
*/
yimpl(YB_NORETURN) inline in_place_tag
in_place(yimpl(empty_base<>))
{
	YB_ASSUME(false);
}
template<typename _type>
yimpl(YB_NORETURN) in_place_tag
in_place(yimpl(empty_base<_type>))
{
	YB_ASSUME(false);
}
template<size_t _vIdx>
yimpl(YB_NORETURN) in_place_tag
in_place(yimpl(integral_constant<size_t, _vIdx>))
{
	YB_ASSUME(false);
}
//@}


/*!
\tparam _type 构造的对象类型。
\param obj 构造的存储对象。
\since build 716
*/
//@{
//! \brief 以默认初始化在对象中构造。
template<typename _type, typename _tObj>
inline _type*
construct_default_within(_tObj& obj)
{
	return ::new(static_cast<void*>(static_cast<_tObj*>(
		ystdex::constfn_addressof(obj)))) _type;
}

/*!
\brief 以值初始化在对象中构造。
\tparam _tParams 用于构造对象的参数包类型。
\param args 用于构造对象的参数包。
*/
template<typename _type, typename _tObj, typename... _tParams>
inline _type*
construct_within(_tObj& obj, _tParams&&... args)
{
	return ::new(static_cast<void*>(static_cast<_tObj*>(
		ystdex::constfn_addressof(obj)))) _type(yforward(args)...);
}
//@}

/*!
\brief 以默认初始化原地构造。
\tparam _tParams 用于构造对象的参数包类型。
\param args 用于构造对象的参数包。
\since build 716
*/
template<typename _type>
inline void
construct_default_in(_type& obj)
{
	ystdex::construct_default_within<_type>(obj);
}

/*!
\brief 以值初始化原地构造。
\tparam _tParams 用于构造对象的参数包类型。
\param args 用于构造对象的参数包。
\since build 692
*/
template<typename _type, typename... _tParams>
inline void
construct_in(_type& obj, _tParams&&... args)
{
	ystdex::construct_within<_type>(obj, yforward(args)...);
}

//! \since build 602
//@{
//! \tparam _tIter 迭代器类型。
//@{
/*!
\tparam _tParams 用于构造对象的参数包类型。
\param args 用于构造对象的参数包。
\pre 断言：指定范围末尾以外的迭代器满足 <tt>!is_undereferenceable</tt> 。
*/
//@{
/*!
\brief 使用指定参数在迭代器指定的位置以指定参数初始化构造对象。
\param i 迭代器。
\note 显式转换为 void* 指针以实现标准库算法 uninitialized_* 实现类似的语义。
\see libstdc++ 5 和 Microsoft VC++ 2013 标准库在命名空间 std 内对指针类型的实现：
	_Construct 模板。
*/
template<typename _tIter, typename... _tParams>
void
construct(_tIter i, _tParams&&... args)
{
	using value_type = typename std::iterator_traits<_tIter>::value_type;

	yconstraint(!is_undereferenceable(i));
	ystdex::construct_within<value_type>(*i, yforward(args)...);
}

/*!
\brief 使用指定参数在迭代器指定的位置以默认初始化构造对象。
\param i 迭代器。
\since build 716
*/
template<typename _tIter>
void
construct_default(_tIter i)
{
	using value_type = typename std::iterator_traits<_tIter>::value_type;

	yconstraint(!is_undereferenceable(i));
	ystdex::construct_default_within<value_type>(*i);
}

/*!
\brief 使用指定的参数重复构造迭代器范围内的对象序列。
\note 参数被传递的次数和构造的对象数相同。
*/
template<typename _tIter, typename... _tParams>
void
construct_range(_tIter first, _tIter last, _tParams&&... args)
{
	for(; first != last; ++first)
		ystdex::construct(first, yforward(args)...);
}
//@}


/*!
\brief 原地销毁。
\see WG21 N4606 20.10.10.7[specialized.destroy] 。
*/
//@{
/*!
\see libstdc++ 5 和 Microsoft VC++ 2013 标准库在命名空间 std 内对指针类型的实现：
	_Destroy 模板。
*/
template<typename _type>
inline void
destroy_at(_type* location)
{
	yconstraint(location);
	location->~_type();
}

//! \see libstdc++ 5 标准库在命名空间 std 内对迭代器范围的实现： _Destroy 模板。
template<typename _tFwd>
inline void
destroy(_tFwd first, _tFwd last)
{
	for(; first != last; ++first)
		ystdex::destroy_at(std::addressof(*first));
}

template<typename _tFwd, typename _tSize>
inline _tFwd
destroy_n(_tFwd first, _tSize n)
{
	// XXX: To reduce dependency on resolution of LWG 2598.
	static_assert(is_lvalue_reference<decltype(*first)>(),
		"Invalid iterator reference type found.");

	// XXX: Excessive order refinment by ','?
	for(; n > 0; static_cast<void>(++first), --n)
		ystdex::destroy_at(std::addressof(*first));
	return first;
}
//@}
//@}


/*!
\brief 原地析构。
\tparam _type 用于析构对象的类型。
\param obj 析构的对象。
\since build 693
\sa destroy_at
*/
template<typename _type>
inline void
destruct_in(_type& obj)
{
	obj.~_type();
}

/*!
\brief 析构迭代器指向的对象。
\param i 迭代器。
\pre 断言：<tt>!is_undereferenceable(i)</tt> 。
\sa destroy
*/
template<typename _tIter>
void
destruct(_tIter i)
{
	using value_type = typename std::iterator_traits<_tIter>::value_type;

	yconstraint(!is_undereferenceable(i));
	ystdex::destruct_in<value_type>(*i);
}

/*!
\brief 析构d迭代器范围内的对象序列。
\note 保证顺序析构。
\sa destroy
*/
template<typename _tIter>
void
destruct_range(_tIter first, _tIter last)
{
	for(; first != last; ++first)
		ystdex::destruct(first);
}
//@}


#define YB_Impl_UninitGuard_Begin \
	auto i = first; \
	\
	try \
	{

// NOTE: The order of destruction is unspecified.
#define YB_Impl_UninitGuard_End \
	} \
	catch(...) \
	{ \
		ystdex::destruct_range(first, i); \
		throw; \
	}

//! \since build 716
//@{
//! \brief 在范围内未初始化放置构造。
//@{
//! \see WG21 N4606 20.10.2[uninitialized.construct.default] 。
//@{
template<typename _tFwd>
inline void
uninitialized_default_construct(_tFwd first, _tFwd last)
{
	YB_Impl_UninitGuard_Begin
		for(; first != last; ++first)
			ystdex::construct_default(first);
	YB_Impl_UninitGuard_End
}

template<typename _tFwd, typename _tSize>
_tFwd
uninitialized_default_construct_n(_tFwd first, _tSize n)
{
	YB_Impl_UninitGuard_Begin
		// XXX: Excessive order refinment by ','?
		for(; n > 0; static_cast<void>(++first), --n)
			ystdex::construct_default(first);
	YB_Impl_UninitGuard_End
}
//@}

//! \see WG21 N4606 20.10.3[uninitialized.construct.value] 。
template<typename _tFwd>
inline void
uninitialized_value_construct(_tFwd first, _tFwd last)
{
	YB_Impl_UninitGuard_Begin
		ystdex::construct_range(first, last);
	YB_Impl_UninitGuard_End
}

template<typename _tFwd, typename _tSize>
_tFwd
uninitialized_value_construct_n(_tFwd first, _tSize n)
{
	YB_Impl_UninitGuard_Begin
		// XXX: Excessive order refinment by ','?
		for(; n > 0; static_cast<void>(++first), --n)
			ystdex::construct(first);
	YB_Impl_UninitGuard_End
	return first;
}
//@}


/*!
\brief 转移初始化范围。
\see WG21 N4606 20.10.10.5[uninitialized.move] 。
\since build 716
*/
//@{
template<typename _tIn, class _tFwd>
_tFwd
uninitialized_move(_tIn first, _tIn last, _tFwd result)
{
	YB_Impl_UninitGuard_Begin
		for(; first != last; static_cast<void>(++result), ++first)
			ystdex::construct(result, std::move(*first));
	YB_Impl_UninitGuard_End
	return result;
}

template<typename _tIn, typename _tSize, class _tFwd>
std::pair<_tIn, _tFwd>
uninitialized_move_n(_tIn first, _tSize n, _tFwd result)
{
	YB_Impl_UninitGuard_Begin
		// XXX: Excessive order refinment by ','?
		for(; n > 0; ++result, static_cast<void>(++first), --n)
			ystdex::construct(result, std::move(*first));
	YB_Impl_UninitGuard_End
	return {first, result};
}
//@}


/*!
\brief 在迭代器指定的未初始化的范围上构造对象。
\tparam _tFwd 输出范围前向迭代器类型。
\tparam _tParams 用于构造对象的参数包类型。
\param first 输出范围起始迭代器。
\param args 用于构造对象的参数包。
\note 参数被传递的次数和构造的对象数相同。
\note 接口不保证失败时的析构顺序。
*/
//@{
/*!
\param last 输出范围终止迭代器。
\note 和 std::unitialized_fill 类似，但允许指定多个初始化参数。
\see WG21 N4431 20.7.12.3[uninitialized.fill] 。
*/
template<typename _tFwd, typename... _tParams>
void
uninitialized_construct(_tFwd first, _tFwd last, _tParams&&... args)
{
	YB_Impl_UninitGuard_Begin
		for(; i != last; ++i)
			ystdex::construct(i, yforward(args)...);
	YB_Impl_UninitGuard_End
}

/*!
\tparam _tSize 范围大小类型。
\param n 范围大小。
\note 和 std::unitialized_fill_n 类似，但允许指定多个初始化参数。
\see WG21 N4431 20.7.12.4[uninitialized.fill.n] 。
*/
template<typename _tFwd, typename _tSize, typename... _tParams>
void
uninitialized_construct_n(_tFwd first, _tSize n, _tParams&&... args)
{
	YB_Impl_UninitGuard_Begin
		// NOTE: This form is by specification (WG21 N4431) of
		//	'std::unitialized_fill' literally.
		for(; n--; ++i)
			ystdex::construct(i, yforward(args)...);
	YB_Impl_UninitGuard_End
}
//@}
//@}

#undef YB_Impl_UninitGuard_End
#undef YB_Impl_UninitGuard_Begin


//! \since build 716
//@{
//! \brief 默认初始化构造分配器。
template<typename _type, class _tAlloc = std::allocator<_type>>
class default_init_allocator : public _tAlloc
{
public:
	using allocator_type = _tAlloc;
	using traits_type = std::allocator_traits<allocator_type>; 
	template<typename _tOther>
	struct rebind
	{
		using other = default_init_allocator<_tOther,
			typename traits_type::template rebind_alloc<_tOther>>;
	};

	using allocator_type::allocator_type;

	template<typename _tOther>
	void
	construct(_tOther* p)
		ynoexcept(std::is_nothrow_default_constructible<_tOther>::value)
	{
		::new(static_cast<void*>(p)) _tOther;
	}
	template<typename _tOther, typename... _tParams>
	void
	construct(_type* p, _tParams&&... args)
	{
		traits_type::construct(static_cast<allocator_type&>(*this), p,
			yforward(args)...);
	}
};


/*!
\brief 放置存储的对象删除器：释放时调用伪析构函数。
\tparam _type 被删除的对象类型。
\pre _type 满足 Destructible 。
*/
template<typename _type, typename _tPointer = _type*>
struct placement_delete
{
	using pointer = _tPointer;

	yconstfn placement_delete() ynothrow = default;
	template<typename _type2,
		yimpl(typename = enable_if_convertible_t<_type2*, _type*>)>
	placement_delete(const placement_delete<_type2>&) ynothrow
	{}

	//! \note 使用 ADL destroy_at 。
	void
	operator()(pointer p) const ynothrowv
	{
		destroy_at(p);
	}
};


//! \brief 独占放置存储的对象所有权的指针。
template<typename _type, typename _tPointer = _type*>
using placement_ptr
	= std::unique_ptr<_type, placement_delete<_type, _tPointer>>;
//@}

} // namespace ystdex;

#endif

