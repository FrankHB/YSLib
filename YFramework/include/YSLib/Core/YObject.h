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
\version r4586
\author FrankHB <frankhb1989@gmail.com>
\since build 561
\par 创建时间:
	2009-11-16 20:06:58 +0800
\par 修改时间:
	2017-01-11 11:38 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YObject
*/


#ifndef YSL_INC_Core_yobject_h_
#define YSL_INC_Core_yobject_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YCoreUtilities // for ystdex::identity,
//	ystdex::result_of_t;
#include <ystdex/any.h> // for ystdex::any_ops::holder, ystdex::boxed_value,
//	ystdex::any, ystdex::pseudo_output;
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
\brief 带等于接口的动态泛型持有者接口。
\sa ystdex::any_holder
\since build 332
*/
DeclDerivedI(YF_API, IValueHolder, ystdex::any_ops::holder)
	/*!
	\brief 判断相等。
	\pre 参数为空指针值或持有对象去除 const 后具有和参数相同动态类型的对象。
	\return 持有的对象相等，或持有空对象且参数为空指针值。
	\since bulid 752

	比较参数和持有的对象。
	派生实现应保证返回的值满足 EqualityComparable 中对 == 操作的要求。
	*/
	DeclIEntry(bool Equals(const void*) const)
	/*!
	\brief 判断是否是持有对象的唯一所有者。
	\since build 759
	*/
	DeclIEntry(bool OwnsUnique() const ynothrow)
	/*!
	\brief 创建引用。
	\since build 747

	创建引用持有对象。
	派生实现应保证返回的值持有对应的 lref<T> 类型的值引用当前持有的 T 类型的值。
	*/
	DeclIEntry(ystdex::any Refer() const)
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

	//! \since build 752
	PDefH(bool, Equals, const void* p) const ImplI(IValueHolder)
		ImplRet(bool(p) && AreEqualHeld(this->value,
			Deref(static_cast<const value_type*>(p))))

	//! \since build 759
	PDefH(bool, OwnsUnique, ) const ynothrow ImplI(IValueHolder)
		ImplRet(true)

	//! \since build 747
	ystdex::any
	Refer() const ImplI(IValueHolder);

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
\brief 带等于接口的指针类型动态泛型持有者。
\tparam _tPointer 智能指针类型。
\pre _tPointer 具有 _type 对象所有权。
\sa ystdex::pointer_holder
\since build 555
*/
template<typename _type, class _tPointer = std::unique_ptr<_type>>
class PointerHolder : implements IValueHolder
{
	//! \since build 332
	static_assert(std::is_object<_type>(), "Invalid type found.");

public:
	//! \since build 352
	using value_type = _type;
	using holder_pointer = _tPointer;
	using pointer = typename holder_pointer::pointer;

protected:
	holder_pointer p_held;

public:
	//! \brief 取得所有权。
	PointerHolder(pointer value)
		: p_held(value)
	{}
	//! \since build 352
	//@{
	PointerHolder(const PointerHolder& h)
		: PointerHolder(ystdex::clone_monomorphic_ptr(h.p_held))
	{}
	DefDeMoveCtor(PointerHolder)
	//@}

	//! \since build 353
	DefDeCopyAssignment(PointerHolder)
	DefDeMoveAssignment(PointerHolder)

	//! \since build 752
	PDefH(bool, Equals, const void* p) const ImplI(IValueHolder)
		ImplRet(p ? AreEqualHeld(*p_held,
			Deref(static_cast<const value_type*>(p))) : !get())

	/*!
	\note 使用 ADL owns_unique 。
	\since build 759
	*/
	PDefH(bool, OwnsUnique, ) const ynothrow ImplI(IValueHolder)
		ImplRet(owns_unique(p_held))

	//! \since build 747
	ystdex::any
	Refer() const ImplI(IValueHolder);

	//! \since build 409
	DefClone(const ImplI(IValueHolder), PointerHolder)

	//! \since build 348
	PDefH(void*, get, ) const ImplI(IValueHolder)
		ImplRet(p_held.get())

	//! \since build 683
	PDefH(const type_info&, type, ) const ynothrow ImplI(IValueHolder)
		ImplRet(p_held ? ystdex::type_id<_type>() : ystdex::type_id<void>())
};


/*!
\brief 带等于接口的引用动态泛型持有者。
\tparam _type 持有的被引用的值类型。
\note 不对持有值具有所有权。
\sa ValueHolder
\since build 747
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

	//! \since build 752
	PDefH(bool, Equals, const void* p) const ImplI(IValueHolder)
		ImplRet(bool(p) && AreEqualHeld(Deref(static_cast<const value_type*>(
			get())), Deref(static_cast<const value_type*>(p))))

	//! \since build 759
	PDefH(bool, OwnsUnique, ) const ynothrow ImplI(IValueHolder)
		ImplRet({})

	PDefH(ystdex::any, Refer, ) const ImplI(IValueHolder)
		ImplRet(ystdex::any(ystdex::any_ops::use_holder,
			ystdex::in_place<RefHolder>, *this))

	DefClone(const ImplI(IValueHolder), RefHolder)

	PDefH(void*, get, ) const ImplI(IValueHolder)
		ImplRet(ystdex::pvoid(std::addressof(
			Deref(static_cast<lref<value_type>*>(base.get())).get())))

	PDefH(const type_info&, type, ) const ynothrow ImplI(IValueHolder)
		ImplRet(ystdex::type_id<value_type>())
};
//@}

template<typename _type>
ystdex::any
ValueHolder<_type>::Refer() const
{
	return ystdex::any(ystdex::any_ops::use_holder, ystdex::in_place<RefHolder<
		_type>>, ystdex::ref(this->value));
}

template<typename _type, class _tPointer>
ystdex::any
PointerHolder<_type, _tPointer>::Refer() const
{
	if(const auto& p = p_held.get())
		return ystdex::any(ystdex::any_ops::use_holder,
			ystdex::in_place<RefHolder<_type>>, ystdex::ref(*p));
	ystdex::throw_invalid_construction();
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
	//! \since build 747
	struct holder_refer_tag
	{};

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

private:
	/*!
	\brief 构造：使用持有者。
	\since build 747
	*/
	ValueObject(const IValueHolder& holder, holder_refer_tag)
		: content(holder.Refer())
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

	//! \pre 参数为空指针值或持有对象去除 const 后具有和参数相同动态类型的对象。
	bool
	EqualsRaw(const void*) const;

	/*!
	\pre 间接断言：持有对象非空。
	\pre 持有对象去除 const 后具有和参数相同动态类型的对象。
	*/
	bool
	EqualsUnchecked(const void*) const;
	//@}

	/*!
	\brief 取间接引用的值对象。
	\since build 747
	*/
	ValueObject
	MakeIndirect() const;

	/*!
	\brief 判断是否是持有对象的唯一所有者。
	\since build 759
	*/
	bool
	OwnsUnique() const ynothrow;

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
	vo.emplace<ystdex::decay_t<_type>>(yforward(res), ystdex::true_(),
		std::is_same<ystdex::decay_t<_type>, ValueObject>());
}
template<typename _type, typename... _tParams>
inline void
EmplaceCallResult(ValueObject& vo, _type&& res)
{
	YSLib::EmplaceCallResult(vo, yforward(res),
		std::is_same<ystdex::decay_t<_type>, ystdex::pseudo_output>());
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

	inline
	void Reset()
	{
		reset(ptr);
	}
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

