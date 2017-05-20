/*
	© 2009-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YObject.h
\ingroup Core
\brief 平台无关的基础对象。
\version r4910
\author FrankHB <frankhb1989@gmail.com>
\since build 561
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2017-05-18 09:37 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YObject
*/


#ifndef YSL_INC_Core_yobject_h_
#define YSL_INC_Core_yobject_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YCoreUtilities // for ystdex::copy_or_move;
#include <ystdex/any.h> // for ystdex::any_ops::holder, ystdex::boxed_value,
//	ystdex::any, ystdex::is_sharing, ystdex::pseudo_output;
#include <ystdex/examiner.hpp> // for ystdex::examiners::equal_examiner;
#include <ystdex/operators.hpp> // for ystdex::equality_comparable;

namespace YSLib
{

//! \since build 683
using ystdex::type_info;


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


/*!
\brief 标签类型元运算。
\since build 218
*/
template<class _tOwner, typename _type>
struct HasOwnershipOf : std::is_base_of<OwnershipTag<_type>, _tOwner>
{};


/*!
\brief 第一个参数指定的选项创建擦除类型的持有者或抛出异常。
\since build 764
*/
//@{
template<class _tHolder, typename... _tParams>
ystdex::any
CreateHolderInPlace(ystdex::true_, _tParams&&... args)
{
	return ystdex::any(ystdex::any_ops::use_holder,
		ystdex::in_place<_tHolder>, yforward(args)...);
}
//! \exception ystdex::invalid_construction 参数类型无法用于初始化持有者。
template<class _tHolder, typename... _tParams>
YB_NORETURN ystdex::any
CreateHolderInPlace(ystdex::false_, _tParams&&...)
{
	ystdex::throw_invalid_construction();
}
//@}


/*!
\brief 带等于接口的动态泛型持有者接口。
\sa ystdex::any_holder
\since build 332
*/
DeclDerivedI(YF_API, IValueHolder, ystdex::any_ops::holder)
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
	DeclIEntry(bool Equals(const void*) const)
	/*!
	\brief 取被持有对象的共享所有者总数。
	\since build 786
	*/
	DeclIEntry(size_t OwnsCount() const ynothrow)
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
	\sa CreateHolder

	按参数指定的选项创建按指定选项持有的对象。
	派生实现应保证返回的值满足选项指定的条件，且变换不改变当前逻辑状态，
	除 mutable 的数据成员可被转移；否则，应抛出异常。
	*/
	DeclIEntry(ystdex::any Create(Creation) const)

	/*!
	\brief 提供创建持有者的默认实现。
	\sa Create
	*/
	template<typename _type>
	static ystdex::any
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
	static bool
	are_equal(const weak_ptr<_type1>& x, const weak_ptr<_type2>& y)
	{
		return x == y;
	}
};

template<typename _type1, typename _type2, typename _type3, typename _type4>
struct HeldEqual<pair<_type1, _type2>, pair<_type3, _type4>>
{
	static yconstfn bool
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
yconstfn bool
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
	//@{
	DefDeCtor(ValueHolder)
	template<typename _tParam,
		yimpl(typename = ystdex::exclude_self_t<ValueHolder, _tParam>)>
	ValueHolder(_tParam&& arg)
		ynoexcept(std::is_nothrow_constructible<_type, _tParam&&>())
		: ystdex::boxed_value<_type>(yforward(arg))
	{}
	using ystdex::boxed_value<_type>::boxed_value;
	//@}
	//! \since build 555
	DefDeCopyMoveCtorAssignment(ValueHolder)

	//! \since build 761
	PDefH(ystdex::any, Create, Creation c) const ImplI(IValueHolder)
		ImplRet(CreateHolder(c, this->value))

	//! \since build 752
	PDefH(bool, Equals, const void* p) const ImplI(IValueHolder)
		ImplRet(bool(p) && AreEqualHeld(this->value,
			Deref(static_cast<const value_type*>(p))))

	//! \since build 786
	PDefH(size_t, OwnsCount, ) const ynothrow ImplI(IValueHolder)
		ImplRet(1)

	//! \since build 409
	PDefH(ValueHolder*, clone, ) const ImplI(IValueHolder)
		ImplRet(ystdex::try_new<ValueHolder>(*this))

	//! \since build 348
	PDefH(void*, get, ) const ImplI(IValueHolder)
		ImplRet(std::addressof(this->value))

	//! \since build 683
	PDefH(const type_info&, type, ) const ynothrow ImplI(IValueHolder)
		ImplRet(ystdex::type_id<_type>())
};


/*!
\ingroup type_traits_operations
\brief 指针持有者特征。
\since build 784
*/
template<typename _tPointer>
struct PointerHolderTraits : std::pointer_traits<_tPointer>
{
	using holder_pointer = _tPointer;
	using shared = ystdex::is_sharing<holder_pointer>;

	//! \since build 786
	//@{
	static PDefH(size_t, count_owner, const holder_pointer& p_held) ynothrow
		ImplRet(count_owner(shared(), p_held))

private:
	static PDefH(size_t, count_owner, ystdex::false_,
		const holder_pointer& p_held) ynothrow
		ImplRet(is_owner(p_held) ? 1 : 0)
	static PDefH(size_t, count_owner, ystdex::true_,
		const holder_pointer& p_held) ynothrow
		ImplRet(size_t(p_held.use_count()))
	//@}

public:
	//! \note 使用 ADL get_raw 。
	static PDefH(auto, get, const holder_pointer& p_held)
		ynoexcept_spec(get_raw(p_held)) -> decltype(get_raw(p_held))
		ImplRet(get_raw(p_held))

	//! \note 使用 ADL owns_unique 。
	static PDefH(bool, is_unique_owner, const holder_pointer& p_held) ynothrow
		ImplRet(owns_unique(p_held))

	//! \note 对非内建指针使用 ADL owns_nonnull 。
	static PDefH(bool, is_owner, const holder_pointer& p_held) ynothrow
		ImplRet(is_owner(std::is_pointer<holder_pointer>(), p_held))

private:
	static PDefH(bool, is_owner, ystdex::false_, const holder_pointer& p_held)
		ynothrow
		ImplRet(owns_nonnull(p_held))
	static PDefH(bool, is_owner, ystdex::true_, const holder_pointer& p_held)
		ynothrow
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
	//! \since build 784
	using traits = _tTraits;
	using holder_pointer = typename traits::holder_pointer;
	using pointer
		= decltype(traits::get(std::declval<const holder_pointer&>()));
	//! \since build 783
	using shared = typename traits::shared;

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

	//! \since build 788
	DefGetter(ynothrow, const holder_pointer&, HolderPtr, p_held)

	//! \since build 761
	ystdex::any
	Create(Creation c) const ImplI(IValueHolder)
	{
		if(shared() && c == IValueHolder::Copy)
			return CreateHolderInPlace<PointerHolder>(shared(), p_held);
		if(const auto& p = traits::get(p_held))
			return CreateHolder(c, *p);
		ystdex::throw_invalid_construction();
	}

	//! \since build 752
	PDefH(bool, Equals, const void* p) const ImplI(IValueHolder)
		ImplRet(traits::is_owner(p_held) && p
			? AreEqualHeld(Deref(traits::get(p_held)),
			Deref(static_cast<const value_type*>(p))) : !get())

	//! \since build 786
	PDefH(size_t, OwnsCount, ) const ynothrow ImplI(IValueHolder)
		ImplRet(traits::count_owner(p_held))

	//! \since build 409
	DefClone(const ImplI(IValueHolder), PointerHolder)

	//! \since build 348
	PDefH(void*, get, ) const ImplI(IValueHolder)
		ImplRet(traits::get(p_held))

	//! \since build 683
	PDefH(const type_info&, type, ) const ynothrow ImplI(IValueHolder)
		ImplRet(traits::is_owner(p_held)
			? ystdex::type_id<_type>() : ystdex::type_id<void>())
};

/*!
\ingroup metafunctions
\relates PointerHolder
\since build 789
*/
template<typename _tPointer>
using HolerFromPointer = PointerHolder<typename PointerHolderTraits<
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
	ValueHolder<lref<value_type>> base; 

public:
	//! \brief 不取得所有权。
	RefHolder(_type& r)
		: base(r)
	{}
	DefDeCopyMoveCtorAssignment(RefHolder)

	//! \since build 761
	PDefH(ystdex::any, Create, Creation c) const ImplI(IValueHolder)
		ImplRet(CreateHolder(c, Ref()))

	//! \since build 752
	PDefH(bool, Equals, const void* p) const ImplI(IValueHolder)
		ImplRet(bool(p) && AreEqualHeld(Deref(static_cast<const value_type*>(
			get())), Deref(static_cast<const value_type*>(p))))

	//! \since build 786
	PDefH(size_t, OwnsCount, ) const ynothrow ImplI(IValueHolder)
		ImplRet(0)

private:
	//! \since build 761
	PDefH(value_type&, Ref, ) const
		ImplRet(Deref(static_cast<lref<value_type>*>(base.get())).get())

public:
	DefClone(const ImplI(IValueHolder), RefHolder)

	PDefH(void*, get, ) const ImplI(IValueHolder)
		ImplRet(ystdex::pvoid(std::addressof(Ref())))

	PDefH(const type_info&, type, ) const ynothrow ImplI(IValueHolder)
		ImplRet(ystdex::type_id<value_type>())
};
//@}

template<typename _type>
ystdex::any
IValueHolder::CreateHolder(Creation c, _type& obj)
{
	switch(c)
	{
	case Indirect:
		return CreateHolderInPlace<RefHolder<_type>>(ystdex::true_(),
			ystdex::ref(obj));
	case Copy:
		return CreateHolderInPlace<ValueHolder<_type>>(
			std::is_copy_constructible<_type>(), obj);
	case Move:
		return CreateHolderInPlace<ValueHolder<_type>>(
			std::is_move_constructible<_type>(), std::move(obj));
	default:
		ystdex::throw_invalid_construction();
	}
}


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
	using Content = ystdex::any;

private:
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
	\since build 448
	*/
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<ValueObject, _type>)>
	ValueObject(_type&& obj)
		: content(ystdex::any_ops::use_holder,
		ystdex::in_place<ValueHolder<ystdex::decay_t<_type>>>, yforward(obj))
	{}
	/*!
	\brief 构造：使用对象初始化参数。
	\tparam _type 目标类型。
	\tparam _tParams 目标类型初始化参数类型。
	\pre _type 可被 _tParams 参数初始化。
	\since build 678
	*/
	template<typename _type, typename... _tParams>
	ValueObject(ystdex::in_place_type_t<_type>, _tParams&&... args)
		: content(ystdex::any_ops::use_holder,
		ystdex::in_place<ValueHolder<_type>>, yforward(args)...)
	{}
	/*!
	\brief 构造：使用持有者。
	\since builld 783
	*/
	template<typename _tHolder, typename... _tParams>
	ValueObject(ystdex::any_ops::use_holder_t,
		ystdex::in_place_type_t<_tHolder>, _tParams&&... args)
		: content(ystdex::any_ops::use_holder,
		ystdex::in_place<_tHolder>, yforward(args)...)
	{}

private:
	/*!
	\brief 构造：使用持有者和动态创建选项。
	\since build 761
	*/
	ValueObject(const IValueHolder& holder, IValueHolder::Creation c)
		: content(holder.Create(c))
	{}

public:
	/*!
	\brief 构造：使用无所有权的引用。
	\since build 747
	*/
	template<typename _type>
	ValueObject(_type& obj, OwnershipTag<>)
		: content(ystdex::any_ops::use_holder,
		ystdex::in_place<RefHolder<_type>>, ystdex::ref(obj))
	{}
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
		: content(ystdex::any_ops::use_holder,
		ystdex::in_place<PointerHolder<_type>>, p)
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
	/*!
	\brief 复制构造：默认实现。
	\since build 332
	*/
	DefDeCopyCtor(ValueObject)
	/*!
	\brief 转移构造：默认实现。
	\since build 332
	*/
	DefDeMoveCtor(ValueObject)
	/*!
	\brief 析构：默认实现。
	\since build 332
	*/
	DefDeDtor(ValueObject)

	//! \since build 332
	//@{
	DefDeCopyAssignment(ValueObject)
	DefDeMoveAssignment(ValueObject)
	//@}

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
	friend PDefHOp(bool, ==, const ValueObject& x, const ValueObject& y)
		ImplRet(x.Equals(y))
	//! \since build 753
	//@{
	//! \brief 比较相等：存储的对象值相等。
	//@{
	template<typename _type>
	friend inline bool
	operator==(const ValueObject& x, const _type& y)
	{
		return x.Equals(y);
	}
	template<typename _type>
	friend inline bool
	operator==(const _type& x, const ValueObject& y)
	{
		return y.Equals(x);
	}
	//@}

	//! \brief 比较不等：存储的对象值不等。
	//@{
	template<typename _type>
	friend inline bool
	operator!=(const ValueObject& x, const _type& y)
	{
		return !(x == y);
	}
	template<typename _type>
	friend inline bool
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
	//! \build 取持有者指针。
	IValueHolder*
	GetHolderPtr() const;
	/*!
	\build 取持有者引用。
	\pre 持有者指针非空。
	*/
	IValueHolder&
	GetHolderRef() const;
	//@}
	/*!
	\brief 取指定类型的对象。
	\pre 间接断言：存储对象类型和访问的类型一致。
	\since build 673
	*/
	//@{
	template<typename _type>
	inline _type&
	GetObject() ynothrowv
	{
		return Deref(ystdex::unchecked_any_cast<_type>(&content));
	}
	template<typename _type>
	inline const _type&
	GetObject() const ynothrowv
	{
		return Deref(ystdex::unchecked_any_cast<const _type>(&content));
	}
	//@}
	//! \since build 683
	DefGetter(const ynothrow, const type_info&, Type, content.type())

	/*!
	\brief 访问指定类型对象。
	\exception std::bad_cast 空实例或类型检查失败 。
	\since build 334
	*/
	//@{
	template<typename _type>
	inline _type&
	Access()
	{
		return ystdex::any_cast<_type&>(content);
	}
	template<typename _type>
	inline const _type&
	Access() const
	{
		return ystdex::any_cast<const _type&>(content);
	}
	//@}

	/*!
	\brief 访问指定类型对象指针。
	\since build 670
	*/
	//@{
	template<typename _type>
	inline observer_ptr<_type>
	AccessPtr() ynothrow
	{
		return YSLib::make_observer(ystdex::any_cast<_type>(&content));
	}
	template<typename _type>
	inline observer_ptr<const _type>
	AccessPtr() const ynothrow
	{
		return YSLib::make_observer(ystdex::any_cast<const _type>(&content));
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
	PDefH(ValueObject, CopyMove, )
		ImplRet(ystdex::copy_or_move(!OwnsUnique(), *this))

	/*!
	\brief 取以指定持有者选项创建的副本。
	\since build 761
	*/
	ValueObject
	Create(IValueHolder::Creation) const;

	/*!
	\brief 判断相等。
	\sa IValueHolder::Equals
	\since bulid 752

	比较参数和持有的对象。
	*/
	//@{
	bool
	Equals(const ValueObject&) const;
	template<typename _type>
	bool
	Equals(const _type& x) const
	{
		if(const auto p_holder = content.get_holder())
			return p_holder->type() == ystdex::type_id<_type>()
				&& EqualsRaw(std::addressof(x));
		return {};
	}

	//! \pre 参数为空指针值或持有的对象去除 const 后具有和参数相同动态类型的对象。
	bool
	EqualsRaw(const void*) const;

	/*!
	\pre 间接断言：持有的对象非空。
	\pre 持有的对象去除 const 后具有和参数相同动态类型的对象。
	*/
	bool
	EqualsUnchecked(const void*) const;
	//@}

 	/*!
	\brief 取引用的值对象的副本。
	\since build 761
	*/
	PDefH(ValueObject, MakeCopy, ) const
		ImplRet(Create(IValueHolder::Copy))

 	/*!
	\brief 取引用的值对象的转移副本。
	\since build 761
	*/
	PDefH(ValueObject, MakeMove, ) const
		ImplRet(Create(IValueHolder::Move))

 	/*!
	\brief 取引用的值对象的初始化副本：按是否具有唯一所有权选择转移或复制对象副本。
	\since build 764
	*/
	PDefH(ValueObject, MakeMoveCopy, ) const
		ImplRet(Create(OwnsUnique() ? IValueHolder::Move : IValueHolder::Copy))

 	/*!
	\brief 取间接引用的值对象。
	\since build 747
	*/
	PDefH(ValueObject, MakeIndirect, ) const
		ImplRet(Create(IValueHolder::Indirect))

	/*!
	\brief 取所有者持有的对象的共享所有者的总数。
	\since build 786
	*/
	size_t
	OwnsCount() const ynothrow;

	/*!
	\brief 判断是否是持有的对象的唯一所有者。
	\since build 759
	*/
	PDefH(bool, OwnsUnique, ) const ynothrow
		ImplRet(OwnsCount() == 1)

	//! \since build 759
	//@{
	template<typename _type, typename... _tParams>
	void
	emplace(_tParams&&... args)
	{
		using Holder = ValueHolder<ystdex::decay_t<_type>>;

		content.emplace<Holder>(ystdex::any_ops::use_holder,
			Holder(yforward(args)...));
	}
	template<typename _type>
	void
	emplace(_type* p, PointerTag)
	{
		using Holder = PointerHolder<ystdex::decay_t<_type>>;

		content.emplace<Holder>(ystdex::any_ops::use_holder, Holder(p));
	}
	//@}

	/*!
	\brief 交换。
	\since build 710
	*/
	friend PDefH(void, swap, ValueObject& x, ValueObject& y) ynothrow
		ImplExpr(x.content.swap(y.content))
};

/*!
\relates ValueObject
\since build 748
*/
//@{
//! \since build 749
//@{
template<typename _type>
inline observer_ptr<_type>
AccessPtr(ValueObject& vo) ynothrow
{
	return vo.AccessPtr<_type>();
}
template<typename _type>
inline observer_ptr<const _type>
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
使用第三和第四个参数分别指定非默认情形下不忽略值及使用赋值。
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
_type&
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
inline PDefH(bool, HoldSame, const ValueObject& x, const ValueObject& y)
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
	PointerType
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

