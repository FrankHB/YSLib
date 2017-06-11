/*
	© 2011-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file any.h
\ingroup YStandardEx
\brief 动态泛型类型。
\version r3072
\author FrankHB <frankhb1989@gmail.com>
\since build 247
\par 创建时间:
	2011-09-26 07:55:44 +0800
\par 修改时间:
	2017-06-08 16:46 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Any

\see WG21 N4606 20.8[any] 。throw_invalid_construction
\see http://www.boost.org/doc/libs/1_60_0/doc/html/any/reference.html 。
*/


#ifndef YB_INC_ystdex_any_h_
#define YB_INC_ystdex_any_h_ 1

#include "typeinfo.h" // for "typeinfo.h", type_info, exclude_self_t,
//	ystdex::type_id, is_nothrow_move_constructible, and_, bool_, enable_if_t,
//	remove_reference_t, cond_t, std::bad_cast, decay_t, _t, yconstraint;
#include "utility.hpp" // "utility.hpp", for boxed_value, std::addressof,
//	std::unique_ptr, ystdex::clone_monomorphic_ptr, standard_layout_storage,
//	aligned_storage_t, is_aligned_storable, ystdex::pvoid, default_init_t;
#include "exception.h" // for throw_invalid_construction;
#include "ref.hpp" // for is_reference_wrapper, unwrap_reference_t;
#include <initializer_list> // for std::initializer_list;

namespace ystdex
{

/*!
\brief any 操作的命名空间。
\note YStandardEx 扩展。
\sa any
\since build 354
*/
namespace any_ops
{

//! \since build 687
template<typename>
struct with_handler_t
{};


/*!
\brief 抽象动态泛型持有者接口。
\since build 454
*/
class YB_API holder
{
public:
	//! \since build 586
	//@{
	holder() = default;
	holder(const holder&) = default;
	//! \brief 虚析构：类定义外默认实现。
	virtual
	~holder();
	//@}

	//! \since build 348
	virtual void*
	get() const = 0;

	//! \since build 683
	virtual const type_info&
	type() const ynothrow = 0;
};

/*!
\brief 判断是否持有相同对象。
\relates holder
\since build 748
*/
inline bool
hold_same(const holder& x, const holder& y)
{
	return x.get() == y.get();
}


/*!
\brief 值类型动态泛型持有者。
\pre 值类型不被 cv-qualifier 修饰。
\since build 331
*/
template<typename _type>
class value_holder : protected boxed_value<_type>, public holder
{
	static_assert(is_object<_type>(), "Non-object type found.");
	static_assert(!is_cv<_type>(), "Cv-qualified type found.");

public:
	//! \since build 352
	using value_type = _type;

	//! \since build 677
	//@{
	value_holder() = default;
	template<typename _tParam,
		yimpl(typename = exclude_self_t<value_holder, _tParam>)>
	value_holder(_tParam&& arg)
		ynoexcept(is_nothrow_constructible<_type, _tParam&&>())
		: boxed_value<_type>(yforward(arg))
	{}
	using boxed_value<_type>::boxed_value;
	//@}
	//! \since build 555
	//@{
	value_holder(const value_holder&) = default;
	value_holder(value_holder&&) = default;

	value_holder&
	operator=(const value_holder&) = default;
	value_holder&
	operator=(value_holder&&) = default;
	//@}

	//! \since build 348
	void*
	get() const override
	{
		return std::addressof(this->value);
	}

	//! \since build 683
	const type_info&
	type() const ynothrow override
	{
		return ystdex::type_id<_type>();
	}
};


/*!
\brief 指针类型动态泛型持有者。
\tparam _type 对象类型。
\tparam _tPointer 智能指针类型。
\pre _tPointer 具有 _type 对象所有权。
\pre 静态断言： <tt>is_object<_type>()</tt> 。
\since build 555
*/
template<typename _type, class _tPointer = std::unique_ptr<_type>>
class pointer_holder : public holder
{
	//! \since build 331
	static_assert(is_object<_type>(), "Invalid type found.");

public:
	//! \since build 352
	using value_type = _type;
	using holder_pointer = _tPointer;
	using pointer = typename holder_pointer::pointer;

protected:
	holder_pointer p_held;

public:
	//! \brief 取得所有权。
	pointer_holder(pointer value)
		: p_held(value)
	{}
	//! \since build 352
	//@{
	pointer_holder(const pointer_holder& h)
		: pointer_holder(ystdex::clone_monomorphic_ptr(h.p_held))
	{}
	pointer_holder(pointer_holder&&) = default;
	//@}

	pointer_holder&
	operator=(const pointer_holder&) = default;
	pointer_holder&
	operator=(pointer_holder&&) = default;

	//! \since build 348
	void*
	get() const override
	{
		return p_held.get();
	}

	//! \since build 683
	const type_info&
	type() const ynothrow override
	{
		return p_held ? ystdex::type_id<_type>() : ystdex::type_id<void>();
	}
};


/*!
\brief 操作码。
\sa any_manager
\since build 354

指定动态分发的操作。
以相关的枚举值指定操作名称。
约定 \c get_* 命名的参数要求第一参数中已构造特定类型的对象；否则行为未定义。
*/
using op_code = std::uint32_t;

/*!
\brief 基本操作码。
\since build 354
*/
enum base_op : op_code
{
	//! \since build 355
	no_op,
	//! \note 要求已构造对象类型 const type_info* 。
	get_type,
	//! \note 要求已构造对象类型 void* 。
	get_ptr,
	//! \note 要求无构造对象。
	clone,
	//! \note 要求已构造目标对象。
	destroy,
	//! \note 要求已构造对象类型 const type_info* 。
	get_holder_type,
	//! \note 要求已构造对象类型 holder* 。
	get_holder_ptr,
	//! \since build 355
	end_base_op
};


//! \since build 352
using any_storage = standard_layout_storage<aligned_storage_t<sizeof(void*),
	yalignof(void*)>>;
//! \since build 352
using any_manager = void(*)(any_storage&, any_storage&, op_code);

/*!
\brief 使用指定处理器初始化存储。
\since build 687
*/
template<class _tHandler, typename... _tParams>
any_manager
construct(any_storage& storage, _tParams&&... args)
{
	_tHandler::init(storage, yforward(args)...);
	return _tHandler::manage;
}


/*!
\brief 使用持有者标记。
\since build 680
*/
yconstexpr const struct use_holder_t{} use_holder{};


/*!
\brief 动态泛型对象处理器。
\since build 671
*/
template<typename _type,
	bool _bStoredLocally = and_<is_nothrow_move_constructible<_type>,
	is_aligned_storable<any_storage, _type>>::value>
class value_handler
{
public:
	//! \since build 352
	//@{
	using value_type = _type;
	using local_storage = bool_<_bStoredLocally>;
	//@}

	//! \since build 595
	//@{
	static void
	copy(any_storage& d, const any_storage& s)
	{
		try_init(is_copy_constructible<value_type>(), local_storage(), d,
			get_reference(s));
	}

	static void
	dispose(any_storage& d) ynothrowv
	{
		dispose_impl(local_storage(), d);
	}

private:
	static void
	dispose_impl(false_, any_storage& d) ynothrowv
	{
		delete d.access<value_type*>();
	}
	static void
	dispose_impl(true_, any_storage& d) ynothrowv
	{
		d.destroy<value_type>();
	}
	//@}

public:
	//! \since build 692
	//@{
	static value_type*
	get_pointer(any_storage& s)
	{
		return get_pointer_impl(local_storage(), s);
	}
	static const value_type*
	get_pointer(const any_storage& s)
	{
		return get_pointer_impl(local_storage(), s);
	}

private:
	//! \since build 729
	//@{
	static value_type*
	get_pointer_impl(false_, any_storage& s)
	{
		return s.access<value_type*>();
	}
	static const value_type*
	get_pointer_impl(false_, const any_storage& s)
	{
		return s.access<const value_type*>();
	}
	static value_type*
	get_pointer_impl(true_, any_storage& s)
	{
		return std::addressof(get_reference_impl(true_(), s));
	}
	static const value_type*
	get_pointer_impl(true_, const any_storage& s)
	{
		return std::addressof(get_reference_impl(true_(), s));
	}
	//@}

public:
	static value_type&
	get_reference(any_storage& s)
	{
		return get_reference_impl(local_storage(), s);
	}
	static const value_type&
	get_reference(const any_storage& s)
	{
		return get_reference_impl(local_storage(), s);
	}

private:
	//! \since build 729
	//@{
	static value_type&
	get_reference_impl(false_, any_storage& s)
	{
		const auto p(get_pointer_impl(false_(), s));

		yassume(p);
		return *p;
	}
	static const value_type&
	get_reference_impl(false_, const any_storage& s)
	{
		const auto p(get_pointer_impl(false_(), s));

		yassume(p);
		return *p;
	}
	static value_type&
	get_reference_impl(true_, any_storage& s)
	{
		return s.access<value_type>();
	}
	static const value_type&
	get_reference_impl(true_, const any_storage& s)
	{
		return s.access<const value_type>();
	}
	//@}
	//@}

public:
	//! \since build 595
	template<typename... _tParams>
	static void
	init(any_storage& d, _tParams&&... args)
	{
		init_impl(local_storage(), d, yforward(args)...);
	}

private:
	//! \since build 729
	template<typename... _tParams>
	static YB_ATTR(always_inline) void
	init_impl(false_, any_storage& d, _tParams&&... args)
	{
		d.construct<value_type*>(new value_type(yforward(args)...));
	}
	//! \since build 729
	template<typename... _tParams>
	static YB_ATTR(always_inline) void
	init_impl(true_, any_storage& d, _tParams&&... args)
	{
		d.construct<value_type>(yforward(args)...);
	}

public:
	//! \since build 692
	static void
	manage(any_storage& d, any_storage& s, op_code op)
	{
		switch(op)
		{
		case get_type:
			d = &ystdex::type_id<value_type>();
			break;
		case get_ptr:
			d = ystdex::pvoid(get_pointer(s));
			break;
		case clone:
			copy(d, s);
			break;
		case destroy:
			dispose(d);
			break;
		case get_holder_type:
			d = &ystdex::type_id<void>();
			break;
		case get_holder_ptr:
			d = static_cast<holder*>(nullptr);
		}
	}

private:
	//! \since build 729
	template<typename... _tParams>
	YB_NORETURN static YB_ATTR(always_inline) void
	try_init(false_, _tParams&&...)
	{
		throw_invalid_construction();
	}
	//! \since build 729
	template<class _bInPlace, typename... _tParams>
	static YB_ATTR(always_inline) void
	try_init(true_, _bInPlace b, any_storage& d, _tParams&&... args)
	{
		init_impl(b, d, yforward(args)...);
	}
};


/*!
\brief 动态泛型引用处理器。
\since build 352
*/
template<typename _type>
class ref_handler : public value_handler<_type*>
{
public:
	using value_type = _type;
	using base = value_handler<value_type*>;

	//! \since build 692
	static value_type*
	get_pointer(any_storage& s)
	{
		return base::get_reference(s);
	}

	//! \since build 692
	static value_type&
	get_reference(any_storage& s)
	{
		yassume(get_pointer(s));
		return *get_pointer(s);
	}

	//! \since build 678
	template<typename _tWrapper,
		yimpl(typename = enable_if_t<is_reference_wrapper<_tWrapper>::value>)>
	static auto
	init(any_storage& d, _tWrapper x)
		-> decltype(base::init(d, std::addressof(x.get())))
	{
		base::init(d, std::addressof(x.get()));
	}

	//! \since build 692
	static void
	manage(any_storage& d, any_storage& s, op_code op)
	{
		switch(op)
		{
		case get_type:
			d = &ystdex::type_id<value_type>();
			break;
		case get_ptr:
			d = ystdex::pvoid(get_pointer(s));
			break;
		default:
			base::manage(d, s, op);
		}
	}
};


/*!
\brief 动态泛型持有者处理器。
\since build 352
*/
template<typename _tHolder>
class holder_handler : public value_handler<_tHolder>
{
	static_assert(is_convertible<_tHolder&, holder&>(),
		"Invalid holder type found.");

public:
	using base = value_handler<_tHolder>;

	//! \since build 595
	static _tHolder*
	get_holder_pointer(any_storage& s)
	{
		return base::get_pointer(s);
	}

private:
	//! \since build 729
	static void
	init(false_, any_storage& d, std::unique_ptr<_tHolder> p)
	{
		d.construct<_tHolder*>(p.release());
	}
	//! \since build 729
	static void
	init(true_, any_storage& d, std::unique_ptr<_tHolder> p)
	{
		d.construct<_tHolder>(std::move(*p));
	}

public:
	//! \since build 395
	static void
	init(any_storage& d, std::unique_ptr<_tHolder> p)
	{
		init(typename base::local_storage(), d, std::move(p));
	}
	//! \since build 678
	using base::init;

	//! \since build 692
	static void
	manage(any_storage& d, any_storage& s, op_code op)
	{
		switch(op)
		{
		case get_type:
			d = &get_holder_pointer(s)->type();
			break;
		case get_ptr:
			d = ystdex::pvoid(get_holder_pointer(s)->get());
			break;
		case get_holder_type:
			d = &ystdex::type_id<_tHolder>();
			break;
		case get_holder_ptr:
			d = static_cast<holder*>(get_holder_pointer(s));
			break;
		default:
			base::manage(d, s, op);
		}
	}
};


/*!
\ingroup trasformation_traits
\brief 根据类型选择引用或值处理器。
\since build 355
*/
template<typename _type>
struct wrap_handler
{
	using value_type = remove_reference_t<unwrap_reference_t<_type>>;
	using type = cond_t<is_reference_wrapper<_type>, ref_handler<value_type>,
		value_handler<value_type>>;
};

} // namespace any_ops;


/*!
\ingroup unary_type_traits
\brief 判断类型是否可以作为 any 引用转换的目标。
\note YStandardEx 扩展。
\sa any_cast
\since build 671
*/
template<typename _type>
using is_any_cast_dest = or_<is_reference<_type>, is_copy_constructible<_type>>;


/*!
\ingroup exceptions
\brief 动态泛型转换失败异常。
\note 基本接口和语义同 boost::bad_any_cast 。
\note 非标准库提案扩展：提供标识转换失败的源和目标类型。
\sa any_cast
\see WG21 N4606 20.8.2[any.bad_any_cast] 。
\since build 586
*/
class YB_API bad_any_cast : public std::bad_cast
{
private:
	//! \since build 683
	lref<const type_info> from_ti, to_ti;

public:
	//! \since build 342
	//@{
	bad_any_cast()
		: std::bad_cast(),
		from_ti(ystdex::type_id<void>()), to_ti(ystdex::type_id<void>())
	{}
	//! \since build 683
	bad_any_cast(const type_info& from_, const type_info& to_)
		: std::bad_cast(),
		from_ti(from_), to_ti(to_)
	{}
	//! \since build 586
	bad_any_cast(const bad_any_cast&) = default;
	/*!
	\brief 虚析构：类定义外默认实现。
	\since build 586
	*/
	~bad_any_cast() override;

	//! \note YStandardEx 扩展。
	//@{
	YB_ATTR_returns_nonnull const char*
	from() const ynothrow;

	//! \since build 683
	const type_info&
	from_type() const ynothrow
	{
		return from_ti.get();
	}

	YB_ATTR_returns_nonnull const char*
	to() const ynothrow;

	//! \since build 683
	const type_info&
	to_type() const ynothrow
	{
		return to_ti.get();
	}
	//@}
	//@}

	virtual YB_ATTR_returns_nonnull const char*
	what() const ynothrow override;
};


//! \since build 687
namespace details
{

struct any_base
{
	//! \since build 692
	mutable any_ops::any_storage storage{};
	any_ops::any_manager manager{};

	any_base() = default;
	template<class _tHandler, typename... _tParams>
	inline
	any_base(any_ops::with_handler_t<_tHandler>, _tParams&&... args)
		: manager(any_ops::construct<_tHandler>(storage, yforward(args)...))
	{}

protected:
	//! \since build 714
	any_base(const any_base& a)
		: manager(a.manager)
	{}
	//! \since build 714
	~any_base() = default;

public:
	YB_API any_ops::any_storage&
	call(any_ops::any_storage&, any_ops::op_code) const;

	YB_API void
	clear() ynothrowv;

	void
	copy(const any_base&);

	void
	destroy() ynothrowv;

	//! \since build 717
	bool
	has_value() const ynothrow
	{
		return manager;
	}

	//! \pre 断言：\c manager 。
	//@{
	YB_API void*
	get() const ynothrowv;

	YB_API any_ops::holder*
	get_holder() const;

	//! \since build 692
	any_ops::any_storage&
	get_storage() const
	{
		return storage;
	}

	YB_API void
	swap(any_base&) ynothrow;

	template<typename _type>
	_type*
	target() ynothrowv
	{
		return type() == ystdex::type_id<_type>() ? static_cast<_type*>(get())
			: nullptr;
	}
	template<typename _type>
	const _type*
	target() const ynothrowv
	{
		return type() == ystdex::type_id<_type>()
			? static_cast<const _type*>(get()) : nullptr;
	}

	YB_API const type_info&
	type() const ynothrowv;

	//! \since build 717
	template<typename _type, typename... _tParams>
	inline _type
	unchecked_access(any_ops::op_code op, _tParams&&... args) const
	{
		any_ops::any_storage t;
		const auto gd(t.pun<_type>(yforward(args)...));

		return unchecked_access<_type>(t, op);
	}
	//! \since build 717
	template<typename _type>
	inline _type
	unchecked_access(default_init_t, any_ops::op_code op) const
	{
		any_ops::any_storage t;
		const auto gd(t.pun_default<_type>());

		return unchecked_access<_type>(t, op);
	}
	//! \since build 686
	template<typename _type>
	inline _type
	unchecked_access(any_ops::any_storage& t, any_ops::op_code op) const
	{
		return call(t, op).access<_type>();
	}
	//@}
};


template<class _tAny>
struct any_emplace
{
	template<typename _type, typename... _tParams>
	void
	emplace(_tParams&&... args)
	{
		emplace_with_handler<any_ops::value_handler<decay_t<_type>>>(
			yforward(args)...);
	}
	//! \since build 717
	template<typename _type, typename _tOther, typename... _tParams>
	void
	emplace(std::initializer_list<_tOther> il, _tParams&&... args)
	{
		emplace_with_handler<any_ops::value_handler<decay_t<_type>>>(
			il, yforward(args)...);
	}
	template<typename _tHolder, typename... _tParams>
	void
	emplace(any_ops::use_holder_t, _tParams&&... args)
	{
		emplace_with_handler<any_ops::holder_handler<decay_t<_tHolder>>>(
			yforward(args)...);
	}

	template<typename _tHandler, typename... _tParams>
	void
	emplace_with_handler(_tParams&&... args)
	{
		auto& a(static_cast<_tAny&>(*this));

		a.reset();
		a.manager = any_ops::construct<decay_t<_tHandler>>(a.storage,
			yforward(args)...);
	}
};

} // namespace details;


/*!
\brief 基于类型擦除的动态泛型对象。
\note 值语义。基本接口和语义同 std::experimental::any 提议
	和 boost::any （对应接口以前者为准）。
\warning 非虚析构。
\see WG21 N4606 20.8.3[any.class] 。
\see http://www.boost.org/doc/libs/1_53_0/doc/html/any/reference.html#any.ValueType 。
\since build 331
*/
class YB_API any : private details::any_base, private details::any_emplace<any>
{
	//! \since build 737
	friend details::any_emplace<any>;

public:
	//! \post \c !has_value() 。
	yconstfn
	any() ynothrow = default;
	/*!
	\note 引用包装类型的处理为 YStandardEx 扩展，此时不具有所有权。
	\warning 引用包装存储无视 cv 修饰符。访问不检查动态类型，应注意避免未定义行为。
	\since build 448
	*/
	template<typename _type, yimpl(typename = exclude_self_t<any, _type>)>
	inline
	any(_type&& x)
		: any(any_ops::with_handler_t<
		_t<any_ops::wrap_handler<decay_t<_type>>>>(), yforward(x))
	{}
	//! \note YStandardEx 扩展。
	//@{
	//! \since build 717
	template<typename _type, typename... _tParams>
	inline
	any(in_place_type_t<_type>, _tParams&&... args)
		: any(any_ops::with_handler_t<any_ops::value_handler<_type>>(),
		yforward(args)...)
	{}
	/*!
	\brief 构造：使用指定持有者。
	\since build 680
	*/
	//@{
	template<typename _tHolder>
	inline
	any(any_ops::use_holder_t, std::unique_ptr<_tHolder> p)
		: any(any_ops::with_handler_t<any_ops::holder_handler<_tHolder>>(),
		std::move(p))
	{}
	template<typename _tHolder>
	inline
	any(any_ops::use_holder_t, _tHolder&& h)
		: any(any_ops::with_handler_t<
		any_ops::holder_handler<decay_t<_tHolder>>>(), yforward(h))
	{}
	//! \since build 717
	template<typename _tHolder, typename... _tParams>
	inline
	any(any_ops::use_holder_t, in_place_type_t<_tHolder>, _tParams&&... args)
		: any(any_ops::with_handler_t<any_ops::holder_handler<_tHolder>>(),
		yforward(args)...)
	{}
	//@}
	template<typename _type>
	inline
	any(_type&& x, any_ops::use_holder_t)
		: any(any_ops::with_handler_t<any_ops::holder_handler<
		any_ops::value_holder<decay_t<_type>>>>(), yforward(x))
	{}
	//! \since build 687
	template<class _tHandler, typename... _tParams>
	inline
	any(any_ops::with_handler_t<_tHandler> t, _tParams&&... args)
		: any_base(t, yforward(args)...)
	{}
	//@}
	any(const any&);
	any(any&& a) ynothrow
		: any()
	{
		a.swap(*this);
	}
	//! \since build 382
	~any();

	//! \since build 687
	template<typename _type, yimpl(typename = exclude_self_t<any, _type>)>
	any&
	operator=(_type&& x)
	{
		any(yforward(x)).swap(*this);
		return *this;
	}
	/*!
	\brief 复制赋值：使用复制和交换。
	\since build 332
	*/
	any&
	operator=(const any& a)
	{
		any(a).swap(*this);
		return *this;
	}
	/*!
	\brief 转移赋值：使用复制和交换。
	\since build 332
	*/
	any&
	operator=(any&& a) ynothrow
	{
		any(std::move(a)).swap(*this);
		return *this;
	}

	//! \since build 717
	using any_base::has_value;

	//! \note YStandardEx 扩展。
	//@{
	//! \since build 352
	void*
	get() const ynothrow
	{
		return manager ? unchecked_get() : nullptr;
	}

	any_ops::holder*
	get_holder() const
	{
		return manager ? unchecked_get_holder() : nullptr;
	}
	//@}

protected:
	/*!
	\note YStandardEx 扩展。
	\since build 687
	*/
	//@{
	using any_base::get_storage;

	using any_base::call;
	//@}

public:
	//! \since build 717
	void
	reset() ynothrow;

	/*!
	\note YStandardEx 扩展。
	\since build 687
	*/
	//@{
	using any_emplace<any>::emplace;

	using any_emplace<any>::emplace_with_handler;
	//@}

	void
	swap(any& a) ynothrow
	{
		any_base::swap(a);
	}

	/*!
	\brief 取目标指针。
	\return 若存储目标类型和模板参数相同则为指向存储对象的指针值，否则为空指针值。
	\note YStandardEx 扩展。
	\since build 352
	*/
	//@{
	template<typename _type>
	_type*
	target() ynothrow
	{
		return manager ? any_base::template target<_type>() : nullptr;
	}
	template<typename _type>
	const _type*
	target() const ynothrow
	{
		return manager ? any_base::template target<_type>() : nullptr;
	}
	//@}

	//! \since build 683
	const type_info&
	type() const ynothrow
	{
		return manager ? unchecked_type() : ystdex::type_id<void>();
	}

	/*!
	\note YStandardEx 扩展。
	\pre 断言：\c has_value() 。
	\since build 677
	*/
	//@{
protected:
	//! \since build 687
	using any_base::unchecked_access;

public:
	//! \brief 取包含对象的指针。
	void*
	unchecked_get() const ynothrowv
	{
		return any_base::get();
	}

	//! \brief 取持有者指针。
	any_ops::holder*
	unchecked_get_holder() const
	{
		return any_base::get_holder();
	}

	/*!
	\brief 取包含对象的类型。
	\since build 683
	*/
	const type_info&
	unchecked_type() const ynothrowv
	{
		return any_base::type();
	}
	//@}
};

//! \relates any
//@{
//! \see WG21 N4606 20.8.4[any.nonmembers] 。
//@{
/*!
\brief 交换对象。
\since build 398
*/
inline void
swap(any& x, any& y) ynothrow
{
	x.swap(y);
}

/*!
\ingroup helper_functions
\brief 创建 any 对象。
\see WG21 P0032R3 。
\since build 717
*/
//@{
template<typename _type, typename... _tParams>
any
make_any(_tParams&&... args)
{
	return any(in_place<_type>, yforward(args)...);
}
template<typename _type, typename _tOther, typename... _tParams>
any
make_any(std::initializer_list<_tOther> il, _tParams&&... args)
{
	return any(in_place<_type>, il, yforward(args)...);
}
//@}

/*!
\brief 动态泛型转换。
\return 当 <tt>p
	&& p->type() == ystdex::type_id<remove_pointer_t<_tPointer>>()</tt> 时
	为指向对象的指针，否则为空指针。
\note 语义同 \c boost::any_cast 。
\since build 671
\todo 检验特定环境（如使用动态库时）比较 std::type_info::name() 的必要性。
*/
//@{
//@{
template<typename _type>
inline _type*
any_cast(any* p) ynothrow
{
	return p ? p->target<_type>() : nullptr;
}
template<typename _type>
inline const _type*
any_cast(const any* p) ynothrow
{
	return p ? p->target<_type>() : nullptr;
}
//@}
/*!
\throw bad_any_cast 当 <tt>x.type()
	!= ystdex::type_id<remove_reference_t<_tValue>>()</tt> 。
*/
//@{
template<typename _tValue>
_tValue
any_cast(any& x)
{
	static_assert(is_any_cast_dest<_tValue>(),
		"Invalid cast destination type found.");

	if(const auto p = x.template target<remove_reference_t<_tValue>>())
		return static_cast<_tValue>(*p);
	throw bad_any_cast(x.type(), ystdex::type_id<_tValue>());
}
template<typename _tValue>
_tValue
any_cast(const any& x)
{
	static_assert(is_any_cast_dest<_tValue>(),
		"Invalid cast destination type found.");

	if(const auto p = x.template target<const remove_reference_t<_tValue>>())
		return static_cast<_tValue>(*p);
	throw bad_any_cast(x.type(), ystdex::type_id<_tValue>());
}
//! \since build 671
template<typename _tValue>
_tValue
any_cast(any&& x)
{
	static_assert(is_any_cast_dest<_tValue>(),
		"Invalid cast destination type found.");

	if(const auto p = x.template target<remove_reference_t<_tValue>>())
		return static_cast<_tValue>(*p);
	throw bad_any_cast(x.type(), ystdex::type_id<_tValue>());
}
//@}
//@}
//@}

//! \note YSandardEx 扩展。
//@{
/*!
\brief 判断是否持有相同对象。
\since build 748
*/
inline bool
hold_same(const any& x, const any& y)
{
	return x.get() == y.get();
}
/*!
\brief 未检查的动态泛型转换。
\note 对非空对象语义同非公开接口 \c boost::unsafe_any_cast 。
\since build 677
*/
//@{
/*!
\pre 断言： <tt>p && p->has_value()
	&& p->unchecked_type() == ystdex::type_id<_type>()</tt> 。
*/
template<typename _type>
inline _type*
unchecked_any_cast(any* p) ynothrowv
{
	yconstraint(p && p->has_value()
		&& p->unchecked_type() == ystdex::type_id<_type>());
	return static_cast<_type*>(p->unchecked_get());
}

/*!
\pre 断言： <tt>p && p->has_value()
	&& p->unchecked_type() == ystdex::type_id<const _type>()</tt> 。
*/
template<typename _type>
inline const _type*
unchecked_any_cast(const any* p) ynothrowv
{
	yconstraint(p && p->has_value()
		&& p->unchecked_type() == ystdex::type_id<const _type>());
	return static_cast<const _type*>(p->unchecked_get());
}
//@}

/*!
\brief 非安全动态泛型转换。
\note 语义同非公开接口 \c boost::unsafe_any_cast 。
\since build 673
*/
//@{
//! \pre 断言： <tt>p && p->type() == ystdex::type_id<_type>()</tt> 。
template<typename _type>
inline _type*
unsafe_any_cast(any* p) ynothrowv
{
	yconstraint(p && p->type() == ystdex::type_id<_type>());
	return static_cast<_type*>(p->get());
}

//! \pre 断言： <tt>p && p->type() == ystdex::type_id<const _type>()</tt> 。
template<typename _type>
inline const _type*
unsafe_any_cast(const any* p) ynothrowv
{
	yconstraint(p && p->type() == ystdex::type_id<const _type>());
	return static_cast<const _type*>(p->get());
}
//@}
//@}
//@}


/*!
\note YStandardEx 扩展。
\sa void_ref
\since build 743
*/
class YB_API void_ref_any
{
private:
	mutable any data;

public:
	void_ref_any() = default;
	template<typename _type,
		yimpl(typename = exclude_self_t<void_ref_any, _type>)>
	void_ref_any(_type&& x)
		: void_ref_any(yforward(x), is_convertible<_type&&, void_ref>())
	{}

private:
	template<typename _type>
	void_ref_any(_type&& x, true_)
		: data(void_ref(yforward(x)))
	{}
	template<typename _type>
	void_ref_any(_type&& x, false_)
		: data(yforward(x))
	{}

public:
	void_ref_any(const void_ref_any&) = default;
	void_ref_any(void_ref_any&&) = default;

	void_ref_any&
	operator=(const void_ref_any&) = default;
	void_ref_any&
	operator=(void_ref_any&&) = default;

	//! \pre 间接断言：参数指定初始化对应的类型。
	template<typename _type,
		yimpl(typename = exclude_self_t<void_ref_any, _type>)>
	YB_PURE
	operator _type() const
	{
		if(data.type() == ystdex::type_id<void_ref>())
			return *ystdex::unchecked_any_cast<void_ref>(&data);
		return std::move(*ystdex::unchecked_any_cast<_type>(&data));
	}
};

} // namespace ystdex;

#endif

