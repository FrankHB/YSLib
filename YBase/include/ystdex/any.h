/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file any.h
\ingroup YStandardEx
\brief 动态泛型类型。
\version r1288
\author FrankHB <frankhb1989@gmail.com>
\since build 247
\par 创建时间:
	2011-09-26 07:55:44 +0800
\par 修改时间:
	2013-04-14 06:41 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Any
*/


#ifndef YB_INC_ystdex_any_h_
#define YB_INC_ystdex_any_h_ 1

#include "type_op.hpp"
#include <memory> // for std::addressof, std::unique_ptr;
#include <typeinfo> // for typeid, std::bad_cast;

namespace ystdex
{

/*
\brief 用于表示任意不需要复制存储的非聚集 POD 类型。
\note POD 和聚集类型的含义参考 ISO C++11 。
\since build 352
\todo 确定整数和枚举类型的必要性。
*/
union non_aggregate_pod
{
	void* object_ptr;
	const void* const_object_ptr;
	volatile void* volatile_object_ptr;
	const volatile void* const_volatile_object_ptr;
	void(*function_ptr)();
	int(non_aggregate_pod::*member_object_pointer);
	void(non_aggregate_pod::*member_function_pointer)();
};


/*
\brief 任意 POD 类型存储。
\note POD 的含义参考 ISO C++11 。
\since build 351
*/
template<typename _tPOD = typename aligned_storage<sizeof(void*)>::type>
union pod_storage
{
	static_assert(is_pod<_tPOD>::value, "Non-POD underlying type found.");

	typedef _tPOD underlying;

	underlying object;
	byte data[sizeof(underlying)];

	//! \since build 352
	//@{
	pod_storage() = default;
	template<typename _type>
	pod_storage(_type&& x)
	{
		new(access()) typename remove_reference<_type>::type(yforward(x));
	}

	//! \note 为避免类型错误，需要确定类型时应使用显式使用 access 指定类型赋值。
	template<typename _type>
	pod_storage&
	operator=(_type&& x)
	{
		access<typename remove_reference<_type>::type>() = yforward(x);
		return *this;
	}
	//@}

	YB_PURE void*
	access()
	{
		return &data[0];
	}
	yconstfn YB_PURE const void*
	access() const
	{
		return &data[0];
	}
	template<typename _type>
	YB_PURE _type&
	access()
	{
		return *static_cast<_type*>(access());
	}
	template<typename _type>
	yconstfn YB_PURE const _type&
	access() const
	{
		return *static_cast<const _type*>(access());
	}
};


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
	yconstfn YB_PURE operator _type&()
	{
		return *static_cast<_type*>(&*this);
	}

	YB_PURE void*
	operator&() const volatile
	{
		return const_cast<void*>(ptr);
	}
};


//! \since build 354
namespace any_ops
{

/*!
\brief 抽象动态泛型持有者接口。
\since build 331
*/
class holder
{
public:
	//! \since build 353 as workaround for G++ 4.7.1
	//@{
	holder() = default;
	holder(const holder&) = default;
	holder(holder&&) = default;
	//@}
	virtual
	~holder()
	{}

	//! \since build 348
	virtual void*
	get() const = 0;

	virtual holder*
	clone() const = 0;

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
	static_assert(is_object<_type>::value, "Non-object type found.");
	static_assert(!(is_const<_type>::value || is_volatile<_type>::value),
		"Cv-qualified type found.");

public:
	//! \since build 352
	typedef _type value_type;

protected:
	//! \since build 348
	mutable _type held;

public:
	value_holder(const _type& value)
		: held(value)
	{}
	/*!
	\brief 转移构造。
	\note 不一定保证无异常跑出；不被 ystdex::any 直接使用。
	\since build 352 。
	*/
	value_holder(_type&& value) ynoexcept(ynoexcept(_type(std::move(value))))
		: held(std::move(value))
	{}

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
\pre <tt>is_object<_type>::value</tt> 。
\since build 331
*/
template<typename _type>
class pointer_holder : public holder
{
	static_assert(std::is_object<_type>::value, "Invalid type found.");

public:
	//! \since build 352
	typedef _type value_type;

protected:
	//! \since build 348
	_type* p_held;

public:
	pointer_holder(_type* value)
		: p_held(value)
	{}
	//! \since build 352
	//@{
	pointer_holder(const pointer_holder& h)
		: pointer_holder(h.p_held ? new _type(*h.p_held) : nullptr)
	{}
	pointer_holder(pointer_holder&& h)
		: p_held(h.p_held)
	{
		h.p_held = nullptr;
	}
	//@}
	virtual
	~pointer_holder() ynothrow
	{
		delete p_held;
	}

	pointer_holder*
	clone() const override
	{
		return new pointer_holder(*this);
	}

	//! \since build 348
	void*
	get() const override
	{
		return p_held;
	}

	//! \since build 340
	const std::type_info&
	type() const ynothrow override
	{
		return p_held ? typeid(_type) : typeid(void);
	}
};


//! \since build 354
typedef std::uint32_t op_code;

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
typedef pod_storage<non_aggregate_pod> any_storage;
typedef void(*any_manager)(any_storage&, const any_storage&, op_code);


//! \brief 使用持有者标记。
struct holder_tag
{};
//@}


/*!
\brief 动态泛型对象处理器。
\since build 355
*/
template<typename _type, bool bStoredLocally = sizeof(_type)
	<= sizeof(any_storage) && yalignof(_type) <= yalignof(any_storage)
	&& yalignof(any_storage) % yalignof(_type) == 0>
class value_handler
{
public:
	//! \since build 352
	//@{
	typedef _type value_type;
	typedef integral_constant<bool, bStoredLocally> local_storage;

	static value_type*
	get_pointer(const any_storage& s)
	{
		return const_cast<value_type*>(bStoredLocally ? std::addressof(
			s.access<value_type>()) : s.access<const value_type*>());
	}
	//@}

	//! \since build 355
	static value_type&
	get_reference(const any_storage& s)
	{
		yassume(get_pointer(s));

		return *get_pointer(s);
	}

	//! \since build 352
	static void
	copy(any_storage& d, const any_storage& s, true_type)
	{
		new(d.access()) value_type(s.access<value_type>());
	}
	//! \since build 352
	static void
	copy(any_storage& d, const any_storage& s, false_type)
	{
		d = new value_type(*s.access<value_type*>());
	}

	//! \since build 355
	//@{
	static void
	uninit(any_storage& d, true_type)
	{
		d.access<value_type>().~value_type();
	}
	static void
	uninit(any_storage& d, false_type)
	{
		delete d.access<value_type*>();
	}

	template<typename _tValue>
	static void
	init(any_storage& d, _tValue&& x)
	{
		init_impl(d, yforward(x), local_storage());
	}
	//@}

private:
	//! \since build 356
	//@{
	template<typename _tValue>
	static void
	init_impl(any_storage& d, _tValue&& x, true_type)
	{
		new(d.access()) value_type(yforward(x));
	}
	template<typename _tValue>
	static void
	init_impl(any_storage& d, _tValue&& x, false_type)
	{
		d = new value_type(yforward(x));
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
			copy(d, s, local_storage());
			break;
		case destroy:
			uninit(d, local_storage());
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
	typedef _type value_type;
	typedef value_handler<value_type*> base;

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

	static void
	init(any_storage& d, std::reference_wrapper<value_type> x)
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
	static_assert(is_convertible<_tHolder&, holder&>::value,
		"Invalid holder type found.");

public:
	typedef typename _tHolder::value_type value_type;
	typedef value_handler<_tHolder> base;
	typedef typename base::local_storage local_storage;

	static value_type*
	get_pointer(const any_storage& s)
	{
		return static_cast<value_type*>(base::get_pointer(s)->_tHolder::get());
	}

private:
	//! \since build 395
	static void
	init(any_storage& d, std::unique_ptr<_tHolder> p, true_type)
	{
		new(d.access()) _tHolder(std::move(*p));
	}
	//! \since build 395
	static void
	init(any_storage& d, std::unique_ptr<_tHolder> p, false_type)
	{
		d = p.release();
	}

public:
	//! \since build 395
	static void
	init(any_storage& d, std::unique_ptr<_tHolder> p)
	{
		init(d, std::move(p), local_storage());
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
		case clone:
			base::copy(d, s, local_storage());
			break;
		case destroy:
			base::uninit(d, local_storage());
			break;
		case get_holder_type:
			d = &typeid(_tHolder);
			break;
		case get_holder_ptr:
			d = static_cast<holder*>(base::get_pointer(s));
		}
	}
};

} // namespace any_ops;


/*!
\brief 基于类型擦除的动态泛型对象。
\note 值语义。基本接口和语义同 std::any 提议和 boost::any （对应接口以前者为准）。
\warning 非虚析构。
\see http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3508.html#synopsis 。
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
	//! \since build 376
	//@{
	template<typename _type, typename = typename
		std::enable_if<!is_same<_type&, any&>::value, int>::type>
	any(_type&& x)
		: manager(any_ops::value_handler<typename
			remove_reference<_type>::type>::manage)
	{
		any_ops::value_handler<typename remove_rcv<_type>::type>::init(storage,
			yforward(x));
	}
	template<typename _type>
	any(std::reference_wrapper<_type> x)
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
		: manager(any_ops::holder_handler<any_ops::value_holder<typename
		remove_rcv<_type>::type>>::manage)
	{
		any_ops::holder_handler<any_ops::value_holder<typename
			remove_cv<_type>::type>>::init(storage, yforward(x));
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
\brief 动态泛型转换失败异常。
\note 基本接口和语义同 boost::bad_any_cast 。
\see any_cast 。
\since build 331
*/
class bad_any_cast : public std::bad_cast
{
private:
	//! \since build 342
	const char* from_name;
	//! \since build 342
	const char* to_name;

public:
	//! \since build 342
	//@{
	bad_any_cast()
		: std::bad_cast(),
		from_name("unknown"), to_name("unknown")
	{};
	bad_any_cast(const std::type_info& from_type, const std::type_info& to_type)
		: std::bad_cast(),
		from_name(from_type.name()), to_name(to_type.name())
	{}

	const char*
	from() const ynothrow
	{
		return from_name;
	}

	const char*
	to() const ynothrow
	{
		return to_name;
	}
	//@}

	virtual const char*
	what() const ynothrow override
	{
		return "Failed conversion: any_cast.";
	}
};


/*!
\brief 动态泛型转换。
\note 语义同 boost::any_cast 。
\since build 398
\todo 检验特定环境（如使用动态库时）比较 std::type_info::name() 的必要性。
*/
//@{
/*!
\return 当 <tt>x != nullptr && x->type() == typeid(remove_pointer<
	_tPointer>::type)</tt> 为指向对象的指针，否则为空指针。
*/
//@{
template<typename _tPointer>
inline _tPointer
any_cast(any* p) ynothrow
{
	return p ? p->target<typename remove_pointer<_tPointer>::type>() : nullptr;
}
template<typename _tPointer>
inline _tPointer
any_cast(const any* p) ynothrow
{
	return p ? p->target<typename remove_pointer<_tPointer>::type>() : nullptr;
}
//@}
/*!
\throw bad_any_cast 当 <tt>x.type()
	!= typeid(remove_reference<_tValue>::type)</tt> 。
*/
//@{
template<typename _tValue>
inline _tValue
any_cast(any& x)
{
	const auto tmp(any_cast<typename remove_reference<_tValue>::type*>(&x));

	if(!tmp)
		throw bad_any_cast(x.type(), typeid(_tValue));
	return static_cast<_tValue>(*tmp);
}
template<typename _tValue>
_tValue
any_cast(const any& x)
{
	const auto tmp(any_cast<typename remove_reference<_tValue>::type*>(&x));

	if(!tmp)
		throw bad_any_cast(x.type(), typeid(_tValue));
	return static_cast<_tValue>(*tmp);
}
//@}
//@}

/*!
\brief 非安全动态泛型转换。
\note 语义同 boost::unsafe_any_cast 。
\pre 断言： <tt>p</tt> 。
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


/*!
\brief 伪输出对象。
\note 吸收所有赋值操作。
\since build 273
*/
struct pseudo_output
{
	template<typename... _tParams>
	inline pseudo_output&
	operator=(_tParams&&...)
	{
		return *this;
	}
};

} // namespace ystdex;


namespace std
{

/*!
\brief \c std::swap 的 \c ystdex::any 重载。
\since build 398
*/
inline void
swap(ystdex::any& x, ystdex::any& y) ynothrow
{
	x.swap(y);
}

} // namespace std;

#endif

