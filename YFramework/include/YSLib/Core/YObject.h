/*
	© 2009-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YObject.h
\ingroup Core
\brief 平台无关的基础对象。
\version r5732
\author FrankHB <frankhb1989@gmail.com>
\since build 561
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2019-03-21 22:12 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YObject
*/


#ifndef YSL_INC_Core_YObject_h_
#define YSL_INC_Core_YObject_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YCoreUtilities // for any, any_ops::use_holder,
//	std::allocator_arg, in_place_type, ystdex::false_, ystdex::true_,
//	any_ops::holder, ystdex::boxed_value, ystdex::exclude_self_t,
//	std::is_constructible, ystdex::enable_if_t, ystdex::type_id,
//	ystdex::alloc_value_t, any_ops::is_in_place_storable, ystdex::is_sharing,
//	ystdex::is_decayed, ystdex::ref, ystdex::cond_t, ystdex::decay_t,
//	std::allocator_arg_t, in_place_type_t, YSLib::unchecked_any_cast,
//	YSLib::any_cast, YSLib::make_observer, ystdex::copy_or_move,
//	ystdex::pseudo_output;
#include <ystdex/examiner.hpp> // for ystdex::examiners::equal_examiner;
#include <ystdex/operators.hpp> // for ystdex::equality_comparable;

namespace YSLib
{

//! \since build 683
using ystdex::type_info;


//! \ingroup tags
//@{
/*!
\brief 指定对参数指定类型的成员具有所有权的标签。
\note 约定参数 void 表示无所有权。
\since build 747
*/
template<typename = void>
struct OwnershipTag
{};


/*!
\brief 指示转移的标记。
\since build 340
*/
struct MoveTag
{};


/*!
\brief 指示指针的标记。
\since build 340
*/
struct PointerTag
{};
//@}


/*!
\brief 标签类型元运算。
\since build 218
*/
template<class _tOwner, typename _type>
struct HasOwnershipOf : std::is_base_of<OwnershipTag<_type>, _tOwner>
{};


/*!
\brief 持有者操作。
\since build 850
*/
template<class _tHolder>
struct HolderOperations
{
	static_assert(ystdex::is_decayed<_tHolder>(), "Invalid holder type found.");

	template<typename... _tParams>
	YB_ATTR_nodiscard static any
	CreateInPlace(_tParams&&... args)
	{
		return CreateInPlaceImpl(nullptr, yforward(args)...);
	}
	template<class _tAlloc, typename... _tParams>
	YB_ATTR_nodiscard static any
	CreateInPlace(std::allocator_arg_t, const _tAlloc& a,
		_tParams&&... args)
	{
		return CreateInPlaceWithAllocatorImpl(nullptr, a, yforward(args)...);
	}

private:
	template<typename... _tParams>
	YB_ATTR_nodiscard static auto
	CreateInPlaceImpl(nullptr_t, _tParams&&... args)
		-> decltype(any(any_ops::use_holder, in_place_type<_tHolder>,
		yforward(args)...))
	{
		return any(any_ops::use_holder, in_place_type<_tHolder>,
			yforward(args)...);
	}
	//! \since build 851
	template<typename... _tParams>
	YB_ATTR_nodiscard YB_NORETURN static any
	CreateInPlaceImpl(void*, _tParams&&...)
	{
		ystdex::throw_invalid_construction();
	}

	template<class _tAlloc, typename... _tParams>
	YB_ATTR_nodiscard static auto
	CreateInPlaceWithAllocatorImpl(nullptr_t, const _tAlloc& a,
		_tParams&&... args)
		-> decltype(any(std::allocator_arg, a, any_ops::use_holder,
		in_place_type<_tHolder>, yforward(args)...))
	{
		return any(std::allocator_arg, a, any_ops::use_holder,
			in_place_type<_tHolder>, yforward(args)...);
	}
	//! \since build 851
	template<typename... _tParams>
	YB_ATTR_nodiscard YB_NORETURN static any
	CreateInPlaceWithAllocatorImpl(void*, _tParams&&...)
	{
		ystdex::throw_invalid_construction();
	}

public:
	//! \note ystdex::false_ 或 ystdex::true_ 指定的选项创建持有者或抛出异常。
	//@{
	//! \exception ystdex::invalid_construction 参数类型无法用于初始化持有者。
	//@{
	template<typename... _tParams>
	YB_ATTR_nodiscard YB_NORETURN static any
	CreateInPlaceConditionally(ystdex::false_, _tParams&&...)
	{
		ystdex::throw_invalid_construction();
	}
	//! \since build 848
	template<class _tAlloc, typename... _tParams>
	YB_ATTR_nodiscard YB_NORETURN static any
	CreateInPlaceConditionally(std::allocator_arg_t, const _tAlloc&,
		ystdex::false_, _tParams&&...)
	{
		ystdex::throw_invalid_construction();
	}
	//@}
	template<typename... _tParams>
	YB_ATTR_nodiscard static any
	CreateInPlaceConditionally(ystdex::true_, _tParams&&... args)
	{
		return any(any_ops::use_holder, in_place_type<_tHolder>,
			yforward(args)...);
	}
	template<class _tAlloc, typename... _tParams>
	YB_ATTR_nodiscard static any
	CreateInPlaceConditionally(std::allocator_arg_t, const _tAlloc& a,
		ystdex::true_, _tParams&&... args)
	{
		return any(std::allocator_arg, a, any_ops::use_holder,
			in_place_type<_tHolder>, yforward(args)...);
	}
	//@}
};


/*!
\brief 带等于接口的动态泛型持有者接口。
\sa ystdex::any_holder
\since build 332
*/
DeclDerivedI(YF_API, IValueHolder, any_ops::holder)
	/*!
	\brief 创建选项。
	\sa Create
	\since build 761
	*/
	enum Creation
	{
		/*!
		\brief 创建不具有所有权的间接持有者。
		\warning 应适当维护所有权避免未定义行为。

		派生实现应保证持有对应的 lref<T> 类型的值引用当前持有的 T 类型的值。
		*/
		Indirect,
		/*!
		\brief 创建引用的值的副本。

		使用当前持有者引用的值创建副本。
		派生实现应保证持有对应的值是当前持有值的不同副本，
			或和当前持有值共享所有权的同一副本。
		若当前持有者是引用，从引用的值复制。
		*/
		Copy,
		/*!
		\brief 创建引用的值转移的副本。

		使用当前持有者引用的值创建转移的副本。
		派生实现应保证持有对应的值从当前持有的值转移。
		若当前持有者是引用，从引用的值转移。
		*/
		Move
	};

	/*!
	\brief 判断相等。
	\pre 参数为空指针值或持有的对象去除 const 后具有和参数相同动态类型的对象。
	\return 持有的对象相等，或持有空对象且参数为空指针值。
	\since bulid 752

	比较参数和持有的对象。
	派生实现应保证返回的值满足 EqualityComparable 中对 == 操作的要求。
	*/
	YB_ATTR_nodiscard YB_PURE DeclIEntry(bool Equals(const void*) const)
	/*!
	\brief 取被持有对象的共享所有者总数。
	\since build 786
	*/
	YB_ATTR_nodiscard YB_PURE DeclIEntry(size_t OwnsCount() const ynothrow)
	/*!
	\sa Creation
	\since build 761
	*/
	//@{
	/*!
	\brief 创建新的持有者。
	\return 包含新创建的持有者的动态泛型对象。
	\exception ystdex::invalid_construction 参数类型无法用于初始化持有者。
	\sa Creation
	\sa HolderOperations

	按参数指定的选项创建按指定选项持有的对象。
	派生实现应保证返回的值满足选项指定的条件，且变换不改变当前逻辑状态，
	除 mutable 的数据成员可被转移；否则，应抛出异常。
	*/
	YB_ATTR_nodiscard DeclIEntry(any Create(Creation) const)

	/*!
	\brief 提供创建持有者的默认实现。
	\sa Create
	*/
	template<typename _type>
	YB_ATTR_nodiscard static any
	CreateHolder(Creation, _type&);
	//@}
EndDecl


//! \since build 454
//@{
/*!
\brief 判断动态泛型的持有值是否相等。
\return 若可通过 == 比较则为比较结果，否则为 true 。
*/
//@{
template<typename _type1, typename _type2>
struct HeldEqual : private ystdex::examiners::equal_examiner
{
	using ystdex::examiners::equal_examiner::are_equal;
};

template<typename _type1, typename _type2>
struct HeldEqual<weak_ptr<_type1>, weak_ptr<_type2>>
{
	YB_ATTR_nodiscard YB_PURE static bool
	are_equal(const weak_ptr<_type1>& x, const weak_ptr<_type2>& y)
	{
		return x == y;
	}
};

template<typename _type1, typename _type2, typename _type3, typename _type4>
struct HeldEqual<pair<_type1, _type2>, pair<_type3, _type4>>
{
	YB_ATTR_nodiscard YB_PURE static yconstfn bool
	are_equal(const pair<_type1, _type2>& x, const pair<_type3, _type4>& y)
	{
		return x.first == y.first && x.second == y.second;
	}
};
//@}


/*!
\brief 判断动态泛型的持有值是否相等。
\note 不直接使用 ystdex::examiners::equal_examiner 判断，
	因为可以有其它重载操作符。
\since build 454
*/
template<typename _type1, typename _type2>
YB_ATTR_nodiscard YB_PURE yconstfn bool
AreEqualHeld(const _type1& x, const _type2& y)
{
	return HeldEqual<_type1, _type2>::are_equal(x, y);
}
//@}


/*!
\note 比较使用 ADL AreEqualHeld 。
\sa AreEqualHeld
*/
//@{
/*!
\brief 带等于接口的值类型动态泛型持有者。
\tparam _type 持有的值类型。
\sa ystdex::value_holder
\since build 332
*/
template<typename _type>
class ValueHolder
	: protected ystdex::boxed_value<_type>, implements IValueHolder
{
	static_assert(std::is_object<_type>(), "Non-object type found.");
	static_assert(!ystdex::is_cv<_type>(), "Cv-qualified type found.");

public:
	//! \since build 352
	using value_type = _type;

	//! \since build 677
	DefDeCtor(ValueHolder)
	//! \since build 851
	template<typename _tParam, yimpl(typename = ystdex::exclude_self_t<
		ValueHolder, _tParam>, typename = ystdex::enable_if_t<
		std::is_constructible<_type, _tParam>::value>)>
	ValueHolder(_tParam&& arg)
		ynoexcept(std::is_nothrow_constructible<_type, _tParam>())
		: ystdex::boxed_value<_type>(yforward(arg))
	{}
	//! \since build 677
	using ystdex::boxed_value<_type>::boxed_value;
	//! \since build 555
	DefDeCopyMoveCtorAssignment(ValueHolder)

	//! \since build 761
	YB_ATTR_nodiscard PDefH(any, Create, Creation c) const
		ImplI(IValueHolder)
		ImplRet(CreateHolder(c, this->value))

	//! \since build 752
	YB_ATTR_nodiscard YB_PURE PDefH(bool, Equals, const void* p) const
		ImplI(IValueHolder)
		ImplRet(bool(p) && AreEqualHeld(this->value,
			Deref(static_cast<const value_type*>(p))))

	//! \since build 786
	YB_ATTR_nodiscard YB_PURE PDefH(size_t, OwnsCount, ) const ynothrow
		ImplI(IValueHolder)
		ImplRet(1)

	//! \since build 854
	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_PURE PDefH(void*, get, ) const
		ynothrow ImplI(IValueHolder)
		ImplRet(std::addressof(this->value))

	//! \since build 683
	YB_ATTR_nodiscard YB_PURE PDefH(const type_info&, type, ) const ynothrow
		ImplI(IValueHolder)
		ImplRet(ystdex::type_id<_type>())
};


/*!
\brief 保存分配器的值持有者。
\note 分配器通过配合 any::allocated_holder_handler_t 被使用。
\note 模板参数指定分配器。
\since build 847
*/
template<class _tAlloc>
class AllocatorHolder : private _tAlloc, implements IValueHolder
{
public:
	using value_type = typename ystdex::alloc_value_t<_tAlloc>;

private:
	//! \since build 848
	//@{
	mutable value_type value;

public:
	template<typename _tOther, typename... _tParams>
	AllocatorHolder(const _tOther& a, _tParams&&... args)
		: _tAlloc(a),
		value(yforward(args)...)
	{}
	DefDeCopyMoveCtorAssignment(AllocatorHolder)
	//@}

	YB_ATTR_nodiscard any
	Create(Creation c) const ImplI(IValueHolder)
	{
		return CreateImpl(c,
			any_ops::is_in_place_storable<value_type>());
	}

private:
	//! \since build 848
	YB_ATTR_nodiscard PDefH(any, CreateImpl, Creation c, ystdex::true_)
		const
		ImplRet(IValueHolder::CreateHolder(c, value))
	//! \since build 848
	YB_ATTR_nodiscard any
	CreateImpl(Creation c, ystdex::false_) const
	{
		switch(c)
		{
		case Copy:
			return HolderOperations<AllocatorHolder<_tAlloc>>::CreateInPlace(
				std::allocator_arg, get_allocator(), get_allocator(), value);
		case Move:
			return HolderOperations<AllocatorHolder<_tAlloc>>::CreateInPlace(
				std::allocator_arg, get_allocator(), get_allocator(),
				std::move(value));
		default:
			// NOTE: It is more efficient to construct in place for %Indirect
			//	case.
			return IValueHolder::CreateHolder(c, value);
		}
	}

public:
	YB_ATTR_nodiscard PDefH(bool, Equals, const void* p) const
		ImplI(IValueHolder)
		ImplRet(bool(p)
			&& AreEqualHeld(value, Deref(static_cast<const value_type*>(p))))

	YB_ATTR_nodiscard YB_PURE PDefH(size_t, OwnsCount, ) const ynothrow
		ImplI(IValueHolder)
		ImplRet(1)

	//! \since build 854
	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_PURE PDefH(void*, get, ) const
		ynothrow ImplI(IValueHolder)
		ImplRet(std::addressof(value))

	YB_ATTR_nodiscard YB_PURE
		PDefH(const _tAlloc&, get_allocator, ) const ynothrow
		ImplRet(static_cast<const _tAlloc&>(*this))

	YB_ATTR_nodiscard YB_PURE PDefH(const type_info&, type, ) const ynothrow
		ImplI(IValueHolder)
		ImplRet(ystdex::type_id<value_type>())
};


/*!
\brief 判断指针值是否为有效的空值。
\since build 854
*/
//@{
//! \note 使用 ADL get_raw 。
template<typename _type>
YB_ATTR_nodiscard YB_PURE yconstfn bool
IsNullPointer(const _type& p) ynothrow
{
	return !bool(get_raw(p));
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline bool
IsNullPointer(const weak_ptr<_type>& p) ynothrow
{
	TryRet(!shared_ptr<_type>(p).get())
	CatchIgnore(bad_weak_ptr&)
	CatchExpr(..., yassume(false))
	return {};
}
//@}

/*!
\ingroup get_raw
\brief 取内建指针。
\since build 853
*/
//@{
//! \note 使用 ADL get_raw 。
template<typename _type>
YB_ATTR_nodiscard YB_PURE yconstfn auto
GetRawPtrOf(const _type& p) ynothrow -> decltype(get_raw(p))
{
	return get_raw(p);
}
//! \exception bad_weak_ptr 异常中立：由 shared_ptr 构造函数抛出。
template<typename _type>
YB_ATTR_nodiscard YB_PURE yconstfn _type*
GetRawPtrOf(const weak_ptr<_type>& p)
{
	return shared_ptr<_type>(p).get();
}
//@}


/*!
\ingroup traits
\brief 指针持有者特征。
\since build 784
*/
template<typename _tPointer>
struct PointerHolderTraits : std::pointer_traits<_tPointer>
{
public:
	using holder_pointer = _tPointer;
	using shared = ystdex::is_sharing<holder_pointer>;

	//! \since build 786
	//@{
	YB_ATTR_nodiscard YB_PURE static PDefH(size_t, count_owner,
		const holder_pointer& p_held) ynothrow
		ImplRet(count_owner(shared(), p_held))

private:
	YB_ATTR_nodiscard YB_PURE static PDefH(size_t, count_owner, ystdex::false_,
		const holder_pointer& p_held) ynothrow
		ImplRet(is_owner(p_held) ? 1 : 0)
	YB_ATTR_nodiscard YB_PURE static PDefH(size_t, count_owner, ystdex::true_,
		const holder_pointer& p_held) ynothrow
		ImplRet(size_t(p_held.use_count()))
	//@}

public:
	YB_ATTR_nodiscard YB_PURE static PDefH(auto, get, const holder_pointer&
		p_held) ynoexcept_spec(YSLib::GetRawPtrOf(p_held))
		-> decltype(YSLib::GetRawPtrOf(p_held))
		ImplRet(YSLib::GetRawPtrOf(p_held))

	//! \note 使用 ADL owns_unique 。
	YB_ATTR_nodiscard YB_PURE static PDefH(bool, is_unique_owner,
		const holder_pointer& p_held) ynothrow
		ImplRet(owns_unique(p_held))

	//! \note 对非内建指针使用 ADL owns_nonnull 。
	YB_ATTR_nodiscard YB_PURE static PDefH(bool, is_owner,
		const holder_pointer& p_held) ynothrow
		ImplRet(is_owner(std::is_pointer<holder_pointer>(), p_held))

private:
	YB_ATTR_nodiscard YB_PURE static PDefH(bool, is_owner, ystdex::false_,
		const holder_pointer& p_held) ynothrow
		ImplRet(owns_nonnull(p_held))
	YB_ATTR_nodiscard YB_PURE static PDefH(bool, is_owner, ystdex::true_,
		const holder_pointer& p_held) ynothrow
		ImplRet(bool(p_held))
};


/*!
\brief 带等于接口的指针类型动态泛型持有者。
\tparam _tPointer 持有者指针类型。
\pre _tPointer 具有 _type 对象所有权。
\sa ystdex::pointer_holder
\since build 784

对被持有的对象独占或共享所有权的间接持有者。
第二参数指定持有者指针，支持内建指针及兼容 unique_ptr 和 shared_ptr 实例。
*/
template<typename _type,
	class _tTraits = PointerHolderTraits<unique_ptr<_type>>>
class PointerHolder : implements IValueHolder
{
	//! \since build 332
	static_assert(ystdex::is_decayed<_type>(), "Invalid type found.");

public:
	//! \since build 352
	using value_type = _type;
	//! \since build 846
	using traits_type = _tTraits;
	using holder_pointer = typename traits_type::holder_pointer;
	using pointer
		= decltype(traits_type::get(std::declval<const holder_pointer&>()));
	//! \since build 783
	using shared = typename traits_type::shared;

protected:
	holder_pointer p_held;

public:
	//! \brief 取得所有权。
	explicit
	PointerHolder(pointer value)
		: p_held(value)
	{}
	//! \since build 783
	//@{
	PointerHolder(const holder_pointer& p)
		: p_held(p)
	{}
	PointerHolder(holder_pointer&& p)
		: p_held(std::move(p))
	{}

	//! \since build 352
	PointerHolder(const PointerHolder& h)
		: PointerHolder(h, shared())
	{}

private:
	PointerHolder(const PointerHolder& h, ystdex::false_)
		: PointerHolder(ystdex::clone_monomorphic_ptr(h.p_held))
	{}
	PointerHolder(const PointerHolder& h, ystdex::true_)
		: p_held(h.p_held)
	{}
	//@}

public:
	//! \since build 352
	DefDeMoveCtor(PointerHolder)

	//! \since build 353
	DefDeCopyAssignment(PointerHolder)
	DefDeMoveAssignment(PointerHolder)

	//! \since build 793
	DefGetter(ynothrow, const holder_pointer&, Held, p_held)

	//! \since build 761
	YB_ATTR_nodiscard any
	Create(Creation c) const ImplI(IValueHolder)
	{
		// TODO: Blocked. Use C++17 'if constexpr' to simplify.
		if(shared() && c == IValueHolder::Copy)
			return HolderOperations<PointerHolder>::CreateInPlaceConditionally(
				shared(), p_held);
		if(const auto& p = traits_type::get(p_held))
			return CreateHolder(c, *p);
		ystdex::throw_invalid_construction();
	}

	//! \since build 752
	YB_ATTR_nodiscard YB_PURE PDefH(bool, Equals, const void* p) const
		ImplI(IValueHolder)
		ImplRet(traits_type::is_owner(p_held) && p
			? AreEqualHeld(Deref(traits_type::get(p_held)),
			Deref(static_cast<const value_type*>(p))) : false)

	//! \since build 786
	YB_ATTR_nodiscard YB_PURE PDefH(size_t, OwnsCount, ) const ynothrow
		ImplI(IValueHolder)
		ImplRet(traits_type::count_owner(p_held))

	/*!
	\pre 间接断言：持有非空指针。
	\since build 348
	*/
	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_PURE PDefH(void*, get, ) const
		ImplI(IValueHolder)
		ImplRet(Nonnull(traits_type::get(p_held)))

	//! \since build 683
	YB_ATTR_nodiscard YB_PURE PDefH(const type_info&, type, ) const ynothrow
		ImplI(IValueHolder)
		ImplRet(!YSLib::IsNullPointer(p_held) ? ystdex::type_id<_type>()
			: ystdex::type_id<void>())
};

/*!
\ingroup metafunctions
\relates PointerHolder
\since build 793
*/
template<typename _tPointer>
using HolderFromPointer = PointerHolder<typename PointerHolderTraits<
	_tPointer>::element_type, PointerHolderTraits<_tPointer>>;


/*!
\brief 带等于接口的引用动态泛型持有者。
\tparam _type 持有的被引用的值类型。
\sa ValueHolder
\since build 747

不对持有值具有所有权的间接持有者。
*/
template<typename _type>
class RefHolder : implements IValueHolder
{
	static_assert(std::is_object<_type>(), "Invalid type found.");

public:
	using value_type
		= ystdex::remove_reference_t<ystdex::unwrap_reference_t<_type>>;

private:
	//! \since build 850
	lref<value_type> ref;

public:
	/*!
	\brief 不取得所有权。
	\since build 821
	*/
	RefHolder(_type& r) ynothrow
		: ref(r)
	{}
	DefDeCopyMoveCtorAssignment(RefHolder)

	//! \since build 761
	YB_ATTR_nodiscard PDefH(any, Create, Creation c) const
		ImplI(IValueHolder)
		ImplRet(CreateHolder(c, ref.get()))

	//! \since build 752
	YB_ATTR_nodiscard PDefH(bool, Equals, const void* p) const
		ImplI(IValueHolder)
		ImplRet(bool(p) && AreEqualHeld(Deref(static_cast<const value_type*>(
			get())), Deref(static_cast<const value_type*>(p))))

	//! \since build 786
	YB_ATTR_nodiscard YB_PURE PDefH(size_t, OwnsCount, ) const ynothrow
		ImplI(IValueHolder)
		ImplRet(0)

	YB_ATTR_nodiscard YB_PURE PDefH(void*, get, ) const ImplI(IValueHolder)
		ImplRet(ystdex::pvoid(std::addressof(ref.get())))

	YB_ATTR_nodiscard YB_PURE PDefH(const type_info&, type, ) const ynothrow
		ImplI(IValueHolder)
		ImplRet(ystdex::type_id<value_type>())
};
//@}

template<typename _type>
any
IValueHolder::CreateHolder(Creation c, _type& obj)
{
	switch(c)
	{
	case Indirect:
		return
			HolderOperations<RefHolder<_type>>::CreateInPlace(ystdex::ref(obj));
	case Copy:
		return HolderOperations<ValueHolder<_type>>::CreateInPlace(obj);
	case Move:
		return
			HolderOperations<ValueHolder<_type>>::CreateInPlace(std::move(obj));
	default:
		ystdex::throw_invalid_construction();
	}
}


/*!
\ingroup unary_type_traits
\since build 851
*/
//@{
template<typename _type>
struct IsValueHolder : ystdex::false_
{};

template<typename _type>
struct IsValueHolder<ValueHolder<_type>> : ystdex::true_
{};
//@}


/*!
\brief 值类型对象类。
\pre 满足 CopyConstructible 。
\warning 非虚析构。
\warning 若修改子节点的 name 则行为未定义。
\since build 217

具有值语义和深复制语义的对象。
*/
class YF_API ValueObject : private ystdex::equality_comparable<ValueObject>
{
public:
	/*!
	\brief 储存的内容。
	\since build 748
	*/
	using Content = any;

private:
	//! \since build 848
	template<typename _type, class _tAlloc>
	using alloc_holder_t = AllocatorHolder<ystdex::rebind_alloc_t<_tAlloc,
		ystdex::decay_t<_type>>>;
	//! \since build 851
	template<typename _type>
	using add_vh_t
		// XXX: This cannot use %ystdex::vseq::ctor_of_t because it does not
		//	work for %std::_Tuple_impl instances. See also  See $2019-01
		//	@ %Documentation::Workflow::Annual2019.
		= ystdex::cond_t<IsValueHolder<_type>, _type, ValueHolder<_type>>;

	//! \since build 748
	Content content;

public:
	/*!
	\brief 无参数构造。
	\note 得到空实例。
	\since build 296
	*/
	DefDeCtor(ValueObject)
	/*!
	\brief 构造：使用对象引用。
	\pre obj 可作为转移构造参数。
	\note 使用 ValueHolder 实例视为持有者而不视为被初始化的值。
	\since build 851
	*/
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _type>),
		yimpl(typename = ystdex::exclude_self_t<std::allocator_arg_t, _type>)>
	ValueObject(_type&& obj)
		: content(any_ops::use_holder, in_place_type<add_vh_t<ystdex::decay_t<
		_type>>>, yforward(obj))
	{}
	/*!
	\brief 构造：使用对象引用和构造器。
	\pre obj 可作为转移构造参数。
	\since build 847
	*/
	template<typename _type, class _tAlloc>
	ValueObject(std::allocator_arg_t, const _tAlloc& a, _type&& arg)
		: content(std::allocator_arg, a, any_ops::use_holder,
		in_place_type<alloc_holder_t<_type, _tAlloc>>, a, yforward(arg))
	{
		static_assert(typename any::allocated_holder_handler_t<_tAlloc,
			alloc_holder_t<_type, _tAlloc>>::base::base::local_storage(),
			"Non-local storage found.");
	}
	/*!
	\brief 构造：使用对象初始化参数。
	\tparam _type 目标类型。
	\tparam _tParams 目标类型初始化参数类型。
	\pre _type 可被 _tParams 参数初始化。
	\since build 678
	*/
	template<typename _type, typename... _tParams>
	explicit
	ValueObject(in_place_type_t<_type>, _tParams&&... args)
		: content(any_ops::use_holder,
		in_place_type<ValueHolder<_type>>, yforward(args)...)
	{}
	/*!
	\brief 构造：使用持有者。
	\since builld 783
	*/
	template<typename _tHolder, typename... _tParams>
	ValueObject(any_ops::use_holder_t, in_place_type_t<_tHolder>,
		_tParams&&... args)
		: content(any_ops::use_holder, in_place_type<_tHolder>,
		yforward(args)...)
	{}

private:
	/*!
	\brief 构造：使用持有者和动态创建选项。
	\since build 761
	*/
	ValueObject(const IValueHolder& h, IValueHolder::Creation c)
		: content(h.Create(c))
	{}

public:
	/*!
	\brief 构造：使用无所有权的引用。
	\since build 747
	*/
	template<typename _type>
	ValueObject(_type& obj, OwnershipTag<>)
		: content(any_ops::use_holder,
		in_place_type<RefHolder<_type>>, ystdex::ref(obj))
	{
		static_assert(any_ops::is_in_place_storable<RefHolder<_type>>(),
			"Suboptimal holder found.");
	}
	/*!
	\note 得到包含指针指向的指定对象的实例，并获得所有权。
	\note 使用 PointerHolder 管理资源（默认使用 delete 释放资源）。
	*/
	//@{
	/*!
	\brief 构造：使用对象指针。
	\since build 340
	*/
	template<typename _type>
	ValueObject(_type* p, PointerTag)
		: content(any_ops::use_holder,
		in_place_type<PointerHolder<_type>>, p)
	{}
	/*!
	\brief 构造：使用对象 unique_ptr 指针。
	\since build 450
	*/
	template<typename _type>
	ValueObject(unique_ptr<_type>&& p, PointerTag)
		: ValueObject(p.get(), PointerTag())
	{
		p.release();
	}
	//@}
	//! \since build 332
	DefDeCopyMoveCtorAssignment(ValueObject)
	/*!
	\brief 析构：默认实现。
	\since build 332
	*/
	DefDeDtor(ValueObject)

	/*!
	\brief 判断是否为空或非空。
	\since build 320
	*/
	DefBoolNeg(explicit, content.has_value())

	//! \sa Equals
	//@{
	/*!
	\brief 比较相等：参数都为空或都非空且存储的对象相等。
	\since build 673
	*/
	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const ValueObject& x, const ValueObject& y)
		ImplRet(x.Equals(y))
	//! \since build 753
	//@{
	//! \brief 比较相等：存储的对象值相等。
	//@{
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE friend inline bool
	operator==(const ValueObject& x, const _type& y)
	{
		return x.Equals(y);
	}
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE friend inline bool
	operator==(const _type& x, const ValueObject& y)
	{
		return y.Equals(x);
	}
	//@}

	//! \brief 比较不等：存储的对象值不等。
	//@{
	template<typename _type>
	YB_ATTR_nodiscard friend inline bool
	operator!=(const ValueObject& x, const _type& y)
	{
		return !(x == y);
	}
	template<typename _type>
	YB_ATTR_nodiscard friend inline bool
	operator!=(const _type& x, const ValueObject& y)
	{
		return !(x == y);
	}
	//@}
	//@}
	//@}

	/*!
	\brief 取储存的内容。
	\since build 748
	*/
	DefGetter(const ynothrow, const Content&, Content, content)
	//! \since build 752
	//@{
	//! \brief 取持有者指针。
	YB_ATTR_nodiscard YB_PURE IValueHolder*
	GetHolderPtr() const;
	/*!
	\brief 取持有者引用。
	\pre 持有者指针非空。
	*/
	YB_ATTR_nodiscard YB_PURE IValueHolder&
	GetHolderRef() const;
	//@}
	/*!
	\brief 取指定类型的对象。
	\pre 间接断言：存储对象类型和访问的类型一致。
	\exception 异常中立：由持有者抛出。
	\since build 853
	*/
	//@{
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE inline _type&
	GetObject()
	{
		return Deref(YSLib::unchecked_any_cast<_type>(&content));
	}
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE inline const _type&
	GetObject() const
	{
		return Deref(YSLib::unchecked_any_cast<const _type>(&content));
	}
	//@}

	/*!
	\brief 访问指定类型对象。
	\exception std::bad_cast 空实例或类型检查失败 。
	\since build 334
	*/
	//@{
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE inline _type&
	Access()
	{
		return YSLib::any_cast<_type&>(content);
	}
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE inline const _type&
	Access() const
	{
		return YSLib::any_cast<const _type&>(content);
	}
	//@}

	/*!
	\brief 访问指定类型对象指针。
	\since build 670
	*/
	//@{
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE inline observer_ptr<_type>
	AccessPtr() ynothrow
	{
		return YSLib::make_observer(YSLib::any_cast<_type>(&content));
	}
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE inline observer_ptr<const _type>
	AccessPtr() const ynothrow
	{
		return YSLib::make_observer(YSLib::any_cast<const _type>(&content));
	}
	//@}

	/*!
	\brief 清除。
	\post <tt>*this == ValueObject()</tt> 。
	\since build 296
	*/
	PDefH(void, Clear, ) ynothrow
		ImplExpr(content.reset())

	/*!
	\brief 取自身的复制初始化转移结果：按是否具有唯一所有权选择转移或复制值对象。
	\since build 787
	*/
	YB_ATTR_nodiscard PDefH(ValueObject, CopyMove, )
		ImplRet(ystdex::copy_or_move(!OwnsUnique(), *this))

	/*!
	\brief 取以指定持有者选项创建的副本。
	\since build 761
	*/
	YB_ATTR_nodiscard ValueObject
	Create(IValueHolder::Creation) const;

	/*!
	\brief 判断相等。
	\sa IValueHolder::Equals
	\since bulid 752

	比较参数和持有的对象。
	*/
	//@{
	YB_ATTR_nodiscard YB_PURE bool
	Equals(const ValueObject&) const;
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE bool
	Equals(const _type& x) const
	{
		if(const auto p_holder = content.get_holder())
			return p_holder->type() == ystdex::type_id<_type>()
				&& EqualsRaw(std::addressof(x));
		return {};
	}

	//! \pre 参数为空指针值或持有的对象去除 const 后具有和参数相同动态类型的对象。
	YB_ATTR_nodiscard YB_PURE bool
	EqualsRaw(const void*) const;

	/*!
	\pre 间接断言：持有的对象非空。
	\pre 持有的对象去除 const 后具有和参数相同动态类型的对象。
	*/
	YB_ATTR_nodiscard YB_PURE bool
	EqualsUnchecked(const void*) const;
	//@}

	/*!
	\brief 取引用的值对象的副本。
	\since build 761
	*/
	YB_ATTR_nodiscard PDefH(ValueObject, MakeCopy, ) const
		ImplRet(Create(IValueHolder::Copy))

	/*!
	\brief 取引用的值对象的转移副本。
	\since build 761
	*/
	YB_ATTR_nodiscard PDefH(ValueObject, MakeMove, ) const
		ImplRet(Create(IValueHolder::Move))

	/*!
	\brief 取引用的值对象的初始化副本：按是否具有唯一所有权选择转移或复制对象副本。
	\since build 764
	*/
	YB_ATTR_nodiscard PDefH(ValueObject, MakeMoveCopy, ) const
		ImplRet(Create(OwnsUnique() ? IValueHolder::Move : IValueHolder::Copy))

	/*!
	\brief 取间接引用的值对象。
	\since build 747
	*/
	YB_ATTR_nodiscard PDefH(ValueObject, MakeIndirect, ) const
		ImplRet(Create(IValueHolder::Indirect))

	/*!
	\brief 取所有者持有的对象的共享所有者的总数。
	\since build 786
	*/
	YB_ATTR_nodiscard YB_PURE size_t
	OwnsCount() const ynothrow;

	/*!
	\brief 判断是否是持有的对象的唯一所有者。
	\since build 759
	*/
	YB_ATTR_nodiscard YB_PURE PDefH(bool, OwnsUnique, ) const ynothrow
		ImplRet(OwnsCount() == 1)

	//! \since build 759
	//@{
	template<typename _type, typename... _tParams>
	void
	emplace(_tParams&&... args)
	{
		using Holder = ValueHolder<ystdex::decay_t<_type>>;

		content.emplace<Holder>(any_ops::use_holder,
			Holder(yforward(args)...));
	}
	template<typename _type>
	void
	emplace(_type* p, PointerTag)
	{
		using Holder = PointerHolder<ystdex::decay_t<_type>>;

		content.emplace<Holder>(any_ops::use_holder, Holder(p));
	}
	//@}

	/*!
	\brief 交换。
	\since build 710
	*/
	friend PDefH(void, swap, ValueObject& x, ValueObject& y) ynothrow
		ImplExpr(x.content.swap(y.content))

	/*!
	\brief 取持有的对象的类型。
	\sa any::type
	\since build 799
	*/
	YB_ATTR_nodiscard PDefH(const type_info&, type, ) const ynothrow
		ImplRet(content.type())
};

/*!
\relates ValueObject
\since build 748
*/
//@{
//! \since build 749
//@{
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline observer_ptr<_type>
AccessPtr(ValueObject& vo) ynothrow
{
	return vo.AccessPtr<_type>();
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline observer_ptr<const _type>
AccessPtr(const ValueObject& vo) ynothrow
{
	return vo.AccessPtr<_type>();
}
//@}

/*!
\brief 以指定参数按需构造替换值。
\since build 759

默认对 ValueObject 及引用值会被直接复制或转移赋值；
其它情形调用 ValueObject::emplace 。
使用第三和第四参数分别指定非默认情形下不忽略值及使用赋值。
*/
//@{
template<typename _type, typename... _tParams>
void
EmplaceCallResult(ValueObject&, _type&&, ystdex::false_) ynothrow
{}
template<typename _type, typename... _tParams>
inline void
EmplaceCallResult(ValueObject& vo, _type&& res, ystdex::true_, ystdex::true_)
	ynoexcept_spec(vo = yforward(res))
{
	vo = yforward(res);
}
template<typename _type, typename... _tParams>
inline void
EmplaceCallResult(ValueObject& vo, _type&& res, ystdex::true_, ystdex::false_)
{
	vo.emplace<ystdex::decay_t<_type>>(yforward(res));
}
template<typename _type, typename... _tParams>
inline void
EmplaceCallResult(ValueObject& vo, _type&& res, ystdex::true_)
{
	YSLib::EmplaceCallResult(vo, yforward(res), ystdex::true_(),
		std::is_same<ystdex::decay_t<_type>, ValueObject>());
}
template<typename _type, typename... _tParams>
inline void
EmplaceCallResult(ValueObject& vo, _type&& res)
{
	YSLib::EmplaceCallResult(vo, yforward(res), ystdex::not_<
		std::is_same<ystdex::decay_t<_type>, ystdex::pseudo_output>>());
}
//@}

template<typename _type, typename... _tParams>
YB_ATTR_nodiscard _type&
EmplaceIfEmpty(ValueObject& vo, _tParams&&... args)
{
	if(!vo)
	{
		vo.emplace<_type>(yforward(args)...);
		return vo.GetObject<_type>();
	}
	return vo.Access<_type>();
}

//! \brief 判断是否持有相同对象。
YB_ATTR_nodiscard YB_PURE inline
	PDefH(bool, HoldSame, const ValueObject& x, const ValueObject& y)
	ImplRet(ystdex::hold_same(x.GetContent(), y.GetContent()))
//@}


/*!
\brief 依赖项模板。
\tparam _type 被依赖的对象类型，需能被无参数构造。
\tparam _tOwnerPointer 依赖所有者指针类型。
\warning 依赖所有者指针需要实现所有权语义，
	否则出现无法释放资源引起内存泄漏或其它非预期行为。
\since build 195
\todo 线程模型及安全性。

基于被依赖的默认对象，可通过写时复制策略创建新对象；可能为空。
*/
template<typename _type, class _tOwnerPointer = shared_ptr<_type>>
class GDependency
{
public:
	using DependentType = _type;
	using PointerType = _tOwnerPointer;
	using ConstReferenceType = decltype(*PointerType());
	using ReferentType = ystdex::remove_const_t<ystdex::remove_reference_t<
		ConstReferenceType>>;
	using ReferenceType = ReferentType&;

private:
	PointerType ptr;

public:
	inline
	GDependency(PointerType p = PointerType())
		: ptr(p)
	{
		GetCopyOnWritePtr();
	}

	DefDeCopyAssignment(GDependency)
	DefDeMoveAssignment(GDependency)

	DefCvt(const ynothrow, ConstReferenceType, *ptr)
	DefCvt(ynothrow, ReferenceType, *ptr)
	DefCvt(const ynothrow, bool, bool(ptr))

	DefGetter(const ynothrow, ConstReferenceType, Ref,
		operator ConstReferenceType())
	DefGetter(ynothrow, ReferenceType, Ref, operator ReferenceType())
	DefGetter(ynothrow, ReferenceType, NewRef, *GetCopyOnWritePtr())

	//! \post 返回值非空。
	YB_ATTR_nodiscard PointerType
	GetCopyOnWritePtr()
	{
		if(!ptr)
			ptr = PointerType(new DependentType());
		else if(!ptr.unique())
			ptr = PointerType(ystdex::clone_monomorphic(Deref(ptr)));
		return Nonnull(ptr);
	}

	PDefH(void, Reset, )
		ImplExpr(reset(ptr))
};


/*!
\brief 范围模块类。
\warning 非虚析构。
\since build 193
*/
template<typename _type>
class GMRange
{
public:
	using ValueType = _type;

protected:
	ValueType max_value; //!< 最大取值。
	ValueType value; //!< 值。

	/*!
	\brief 构造：使用指定最大取值和值。
	*/
	GMRange(ValueType m, ValueType v)
		: max_value(m), value(v)
	{}

public:
	DefGetter(const ynothrow, ValueType, MaxValue, max_value)
	DefGetter(const ynothrow, ValueType, Value, value)
};

} // namespace YSLib;

#endif

