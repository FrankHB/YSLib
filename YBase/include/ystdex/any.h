/*
	© 2011-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file any.h
\ingroup YStandardEx
\brief 动态泛型类型。
\version r1958
\author FrankHB <frankhb1989@gmail.com>
\since build 247
\par 创建时间:
	2011-09-26 07:55:44 +0800
\par 修改时间:
	2015-11-27 19:57 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Any

\see ISO WG21/N4081 6[any] 。
\see http://www.boost.org/doc/libs/1_57_0/doc/html/any/reference.html 。
*/


#ifndef YB_INC_ystdex_any_h_
#define YB_INC_ystdex_any_h_ 1

#include "base.h" // for cloneable;
#include <memory> // for std::addressof, std::unique_ptr;
#include <typeinfo> // for typeid, std::bad_cast;
#include "type_pun.hpp" // for pod_storage, aligned_storage_t,
//	is_aligned_storable;
#include "ref.hpp" // for lref;
#include "cassert.h" // for yconstraint;

namespace ystdex
{

//! \since build 354
namespace any_ops
{

/*!
\brief 抽象动态泛型持有者接口。
\since build 454
*/
class YB_API holder : public cloneable
{
public:
	//! \since build 586
	//@{
	holder() = default;
	holder(const holder&) = default;
	//! \brief 虚析构：类定义外默认实现。
	~holder() override;
	//@}

	//! \since build 348
	virtual void*
	get() const = 0;

	virtual holder*
	clone() const override = 0;

	//! \since build 340
	virtual const std::type_info&
	type() const ynothrow = 0;
};


/*!
\brief 值类型动态泛型持有者。
\pre 值类型不被 cv-qualifier 修饰。
\since build 331
*/
template<typename _type>
class value_holder : public holder
{
	static_assert(is_object<_type>(), "Non-object type found.");
	static_assert(!is_cv<_type>(), "Cv-qualified type found.");

public:
	//! \since build 352
	using value_type = _type;

protected:
	//! \since build 348
	mutable _type held;

public:
	value_holder(const _type& value)
		: held(value)
	{}
	/*!
	\brief 转移构造。
	\note 不一定保证无异常抛出；不被 ystdex::any 直接使用。
	\since build 352
	*/
	value_holder(_type&& value) ynoexcept_spec(_type(std::move(value)))
		: held(std::move(value))
	{}
	//! \since build 555
	//@{
	value_holder(const value_holder&) = default;
	value_holder(value_holder&&) = default;

	value_holder&
	operator=(const value_holder&) = default;
	value_holder&
	operator=(value_holder&&) = default;
	//@}

	value_holder*
	clone() const override
	{
		return new value_holder(held);
	}

	//! \since build 348
	void*
	get() const override
	{
		return std::addressof(held);
	}

	//! \since build 340
	const std::type_info&
	type() const ynothrow override
	{
		return typeid(_type);
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
		: pointer_holder(h.p_held ? new value_type(*h.p_held) : nullptr)
	{}
	pointer_holder(pointer_holder&&) = default;
	//@}

	pointer_holder&
	operator=(const pointer_holder&) = default;
	pointer_holder&
	operator=(pointer_holder&&) = default;

	pointer_holder*
	clone() const override
	{
		return new pointer_holder(*this);
	}

	//! \since build 348
	void*
	get() const override
	{
		return p_held.get();
	}

	//! \since build 340
	const std::type_info&
	type() const ynothrow override
	{
		return p_held ? typeid(_type) : typeid(void);
	}
};


//! \since build 354
using op_code = std::uint32_t;

//! \since build 354
enum base_op : op_code
{
	//! \since build 355
	no_op,
	get_type,
	get_ptr,
	clone,
	destroy,
	get_holder_type,
	get_holder_ptr,
	//! \since build 355
	end_base_op
};


//! \since build 352
//@{
using any_storage
	= pod_storage<aligned_storage_t<sizeof(void*), sizeof(void*)>>;
using any_manager = void(*)(any_storage&, const any_storage&, op_code);


//! \brief 使用持有者标记。
struct holder_tag
{};
//@}


/*!
\brief 动态泛型对象处理器。
\since build 355
*/
template<typename _type,
	bool _bStoredLocally = is_aligned_storable<any_storage, _type>::value>
class value_handler
{
public:
	//! \since build 352
	//@{
	using value_type = _type;
	using local_storage = bool_constant<_bStoredLocally>;
	//@}

	//! \since build 595
	//@{
	static void
	copy(any_storage& d, const any_storage& s)
	{
		init_impl(local_storage(), d, get_reference(s));
	}

	static void
	dispose(any_storage& d) ynothrowv
	{
		dispose_impl(local_storage(), d);
	}

private:
	static void
	dispose_impl(false_type, any_storage& d) ynothrowv
	{
		delete d.access<value_type*>();
	}
	static void
	dispose_impl(true_type, any_storage& d) ynothrowv
	{
		d.access<value_type>().~value_type();
	}
	//@}

public:
	//! \since build 352
	static inline value_type*
	get_pointer(const any_storage& s)
	{
		return get_pointer_impl(local_storage(), s);
	}

private:
	//! \since build 595
	static inline value_type*
	get_pointer_impl(false_type, const any_storage& s)
	{
		return s.access<value_type*>();
	}
	//! \since build 595
	static inline value_type*
	get_pointer_impl(true_type, const any_storage& s)
	{
		return std::addressof(get_reference_impl(true_type(), s));
	}

public:
	//! \since build 355
	static value_type&
	get_reference(const any_storage& s)
	{
		return get_reference_impl(local_storage(), s);
	}

private:
	//! \since build 595
	//@{
	static value_type&
	get_reference_impl(false_type, const any_storage& s)
	{
		const auto p(get_pointer_impl(false_type(), s));

		yassume(p);
		return *p;
	}
	static value_type&
	get_reference_impl(true_type, const any_storage& s)
	{
		return s.access<value_type>();
	}

public:
	template<typename... _tParams>
	static void
	init(any_storage& d, _tParams&&... args)
	{
		init_impl(local_storage(), d, yforward(args)...);
	}

private:
	template<typename... _tParams>
	static void
	init_impl(false_type, any_storage& d, _tParams&&... args)
	{
		d = new value_type(yforward(args)...);
	}
	template<typename... _tParams>
	static void
	init_impl(true_type, any_storage& d, _tParams&&... args)
	{
		new(d.access()) value_type(yforward(args)...);
	}
	//@}

public:
	//! \since build 352
	static void
	manage(any_storage& d, const any_storage& s, op_code op)
	{
		switch(op)
		{
		case get_type:
			d = &typeid(value_type);
			break;
		case get_ptr:
			d = get_pointer(s);
			break;
		case clone:
			copy(d, s);
			break;
		case destroy:
			dispose(d);
			break;
		case get_holder_type:
			d = &typeid(void);
			break;
		case get_holder_ptr:
			d = static_cast<holder*>(nullptr);
		}
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

	//! \since build 355
	static value_type*
	get_pointer(const any_storage& s)
	{
		return base::get_reference(s);
	}

	//! \since build 355
	static value_type&
	get_reference(const any_storage& s)
	{
		yassume(get_pointer(s));
		return *get_pointer(s);
	}

	//! \since build 554
	static void
	init(any_storage& d, lref<value_type> x)
	{
		base::init(d, std::addressof(x.get()));
	}

	static void
	manage(any_storage& d, const any_storage& s, op_code op)
	{
		switch(op)
		{
		case get_type:
			d = &typeid(value_type);
			break;
		case get_ptr:
			d = get_pointer(s);
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
	using value_type = typename _tHolder::value_type;
	using base = value_handler<_tHolder>;

	//! \since build 595
	static _tHolder*
	get_holder_pointer(const any_storage& s)
	{
		return base::get_pointer(s);
	}

	static value_type*
	get_pointer(const any_storage& s)
	{
		const auto p(get_holder_pointer(s));

		yassume(p);
		return static_cast<value_type*>(p->_tHolder::get());
	}

private:
	//! \since build 595
	static void
	init(true_type, any_storage& d, std::unique_ptr<_tHolder> p)
	{
		new(d.access()) _tHolder(std::move(*p));
	}
	//! \since build 595
	static void
	init(false_type, any_storage& d, std::unique_ptr<_tHolder> p)
	{
		d = p.release();
	}

public:
	//! \since build 395
	static void
	init(any_storage& d, std::unique_ptr<_tHolder> p)
	{
		init(typename base::local_storage(), d, std::move(p));
	}
	static void
	init(any_storage& d, _tHolder&& x)
	{
		base::init(d, std::move(x));
	}
	template<typename... _tParams>
	static void
	init(any_storage& d, _tParams&&... args)
	{
		init(d, _tHolder(yforward(args)...));
	}

	static void
	manage(any_storage& d, const any_storage& s, op_code op)
	{
		switch(op)
		{
		case get_type:
			d = &typeid(value_type);
			break;
		case get_ptr:
			d = get_pointer(s);
			break;
		case get_holder_type:
			d = &typeid(_tHolder);
			break;
		case get_holder_ptr:
			d = static_cast<holder*>(get_holder_pointer(s));
			break;
		default:
			base::manage(d, s, op);
		}
	}
};

} // namespace any_ops;


/*!
\ingroup exceptions
\brief 动态泛型转换失败异常。
\note 基本接口和语义同 boost::bad_any_cast 。
\note 非标准库提案扩展：提供标识转换失败的源和目标类型。
\sa any_cast
\see ISO WG21/N4081 6.2[any.bad_any_cast] 。
\since build 586
*/
class YB_API bad_any_cast : public std::bad_cast
{
private:
	//! \since build 586
	lref<const std::type_info> from_ti, to_ti;

public:
	//! \since build 342
	//@{
	bad_any_cast()
		: std::bad_cast(),
		from_ti(typeid(void)), to_ti(typeid(void))
	{}
	bad_any_cast(const std::type_info& from_, const std::type_info& to_)
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

	const char*
	from() const ynothrow
	{
		return from_type() == typeid(void) ? "unknown" : from_type().name();
	}

	//! \since build 586
	const std::type_info&
	from_type() const ynothrow
	{
		return from_ti.get();
	}

	const char*
	to() const ynothrow
	{
		return to_type() == typeid(void) ? "unknown" : to_type().name();
	}

	//! \since build 586
	const std::type_info&
	to_type() const ynothrow
	{
		return to_ti.get();
	}
	//@}

	virtual YB_ATTR(returns_nonnull) const char*
	what() const ynothrow override
	{
		return "Failed conversion: any_cast.";
	}
};


/*!
\brief 基于类型擦除的动态泛型对象。
\note 值语义。基本接口和语义同 std::experimental::any 提议
	和 boost::any （对应接口以前者为准）。
\warning 非虚析构。
\see ISO WG21/N4081 6.3[any.class] 。
\see http://www.boost.org/doc/libs/1_53_0/doc/html/any/reference.html#any.ValueType 。
\since build 331
\todo allocator_arg 支持。
*/
class YB_API any
{
protected:
	//! \since build 355
	//@{
	any_ops::any_storage storage;
	any_ops::any_manager manager;
	//@}

public:
	//! \post <tt>this->empty()</tt> 。
	yconstfn
	any() ynothrow
		: storage(), manager()
	{}
	//! \since build 448
	template<typename _type, yimpl(typename = exclude_self_ctor_t<any, _type>)>
	any(_type&& x)
		: manager(any_ops::value_handler<decay_t<_type>>::manage)
	{
		any_ops::value_handler<decay_t<_type>>::init(storage, yforward(x));
	}
	//! \since build 376
	//@{
	//! \since build 554
	template<typename _type>
	any(lref<_type> x)
		: manager(any_ops::ref_handler<_type>::manage)
	{
		any_ops::ref_handler<_type>::init(storage, x);
	}
	/*!
	\brief 构造：使用指定持有者。
	\since build 395
	*/
	template<typename _tHolder>
	any(any_ops::holder_tag, std::unique_ptr<_tHolder> p)
		: manager(any_ops::holder_handler<_tHolder>::manage)
	{
		any_ops::holder_handler<_tHolder>::init(storage, std::move(p));
	}
	template<typename _type>
	any(_type&& x, any_ops::holder_tag)
		: manager(any_ops::holder_handler<
		any_ops::value_holder<decay_t<_type>>>::manage)
	{
		any_ops::holder_handler<any_ops::value_holder<
			decay_t<_type>>>::init(storage, yforward(x));
	}
	//@}
	any(const any&);
	any(any&& a) ynothrow
		: any()
	{
		a.swap(*this);
	}
	//! \since build 382
	~any();

	template<typename _type>
	any&
	operator=(const _type& x)
	{
		any(x).swap(*this);
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

	bool
	operator!() const ynothrow
	{
		return empty();
	}

	explicit
	operator bool() const ynothrow
	{
		return !empty();
	}

	bool
	empty() const ynothrow
	{
		return !manager;
	}

	//! \since build 352
	void*
	get() const ynothrow;

	any_ops::holder*
	get_holder() const;

	void
	clear() ynothrow;

	void
	swap(any& a) ynothrow;

	//! \since build 352
	//@{
	template<typename _type>
	_type*
	target() ynothrow
	{
		return type() == typeid(_type) ? static_cast<_type*>(get()) : nullptr;
	}
	template<typename _type>
	const _type*
	target() const ynothrow
	{
		return type() == typeid(_type)
			? static_cast<const _type*>(get()) : nullptr;
	}
	//@}

	//! \since build 340
	const std::type_info&
	type() const ynothrow;
};

/*!
\relates any
\see ISO WG21/N4081 6.4[any.nonmembers] 。
*/
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
\brief 动态泛型转换。
\return 当 <tt>p && p->type() == typeid(remove_pointer_t<_tPointer>)</tt> 时
	为指向对象的指针，否则为空指针。
\note 语义同 boost::any_cast 。
\relates any
\since build 398
\todo 检验特定环境（如使用动态库时）比较 std::type_info::name() 的必要性。
*/
//@{
//@{
template<typename _tPointer>
inline _tPointer
any_cast(any* p) ynothrow
{
	return p ? p->target<remove_pointer_t<_tPointer>>() : nullptr;
}
template<typename _tPointer>
inline _tPointer
any_cast(const any* p) ynothrow
{
	return p ? p->target<remove_pointer_t<_tPointer>>() : nullptr;
}
//@}
/*!
\throw bad_any_cast 当 <tt>x.type()
	!= typeid(remove_reference_t<_tValue>)</tt> 。
*/
//@{
template<typename _tValue>
inline _tValue
any_cast(any& x)
{
	const auto p(any_cast<remove_reference_t<_tValue>*>(&x));

	if(!p)
		throw bad_any_cast(x.type(), typeid(_tValue));
	return static_cast<_tValue>(*p);
}
template<typename _tValue>
_tValue
any_cast(const any& x)
{
	const auto p(any_cast<remove_reference_t<_tValue>*>(&x));

	if(!p)
		throw bad_any_cast(x.type(), typeid(_tValue));
	return static_cast<_tValue>(*p);
}
//@}
//@}

/*!
\brief 非安全动态泛型转换。
\note 语义同 boost::unsafe_any_cast 。
\pre 断言： <tt>p</tt> 。
\relates any
\since build 331
*/
//@{
template<typename _type>
inline _type*
unsafe_any_cast(any* p)
{
	yconstraint(p);
	return static_cast<_type*>(p->get());
}

template<typename _type>
inline const _type*
unsafe_any_cast(const any* p)
{
	yconstraint(p);
	return static_cast<const _type*>(p->get());
}
//@}
//@}

} // namespace ystdex;

#endif

