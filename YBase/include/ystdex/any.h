/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file any.h
\ingroup YStandardEx
\brief 动态泛型类型。
\version r985
\author FrankHB<frankhb1989@gmail.com>
\since build 247
\par 创建时间:
	2011-09-26 07:55:44 +0800
\par 修改时间:
	2012-11-04 17:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Any
*/


#ifndef YB_INC_YSTDEX_ANY_H_
#define YB_INC_YSTDEX_ANY_H_ 1

#include "type_op.hpp"
#include <memory> // for std::addressof;
#include <typeinfo> // for typeid, std::bad_cast;

namespace ystdex
{

/*
\brief 任意不需要复制存储的非聚集 POD 类型。
\note POD 和聚集类型的含义参考 ISO C++11 。
\since build 352
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
	
	template<typename _type>
	pod_storage&
	operator=(_type&& x)
	{
		access<typename remove_reference<_type>::type>() = yforward(x);
		return *this;
	}
	//@}

	void*
	access()
	{
		return &data[0];
	}
	yconstfn const void*
	access() const
	{
		return &data[0];
	}
	template<typename _type>
	_type&
	access()
	{
		return *static_cast<_type*>(access());
	}
	template<typename _type>
	yconstfn const _type&
	access() const
	{
		return *static_cast<const _type*>(access());
	}
};


/*!
\brief 任意对象引用类型。
\warning 不检查 cv-qualifier 。
\since build 247
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
	yconstfn operator _type&()
	{
		return *static_cast<_type*>(&*this);
	}

	void*
	operator&() const volatile
	{
		return const_cast<void*>(ptr);
	}
};


/*!
\brief 抽象动态泛型持有者接口。
\since build 331
*/
class any_holder
{
public:
	virtual
	~any_holder()
	{}

	//! \since build 351
	any_holder&
	operator=(const any_holder&) = delete;

	//! \since build 348
	virtual void*
	get() const = 0;

	virtual any_holder*
	clone() const = 0;

	//! \since build 340
	virtual const std::type_info&
	type() const ynothrow = 0;
};


/*!
\brief 值类型动态泛型持有者。
\since build 331
*/
template<typename _type>
class value_holder : public any_holder
{
	static_assert(std::is_object<_type>::value, "Invalid type found.");

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
class pointer_holder : public any_holder
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
		return new pointer_holder(p_held ? new _type(*p_held) : nullptr);
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


//! \since build 352
//@{
enum class any_operation
{
	get_type,
	get_ptr,
	clone,
	destroy,
	get_holder_type,
	get_holder_ptr
};


typedef pod_storage<non_aggregate_pod> any_storage;
typedef void(*any_manager)(any_storage&, const any_storage&, any_operation);

yconstexpr size_t max_any_size = sizeof(any_storage);
yconstexpr size_t max_any_align = yalignof(any_storage);


//! \brief 使用持有者标记。
struct holder_tag
{};

//@}


/*!
\brief 动态泛型对象处理器。
\since build 352
*/
template<typename _type, bool bStoredLocally = (is_pointer<_type>::value
	|| is_member_pointer<_type>::value) && sizeof(_type) <= max_any_size
	&& yalignof(_type) <= max_any_align && max_any_align % yalignof(_type) == 0>
class any_handler
{
public:
	typedef _type value_type;
	typedef integral_constant<bool, bStoredLocally> local_storage;

	static value_type*
	get_pointer(const any_storage& s)
	{
		return const_cast<value_type*>(bStoredLocally ? std::addressof(
			s.access<value_type>()) : s.access<const value_type*>());
	}

	static void
	copy(any_storage& d, const any_storage& s, true_type)
	{
		new(d.access()) value_type(s.access<value_type>());
	}
	static void
	copy(any_storage& d, const any_storage& s, false_type)
	{
		d = new value_type(*s.access<value_type*>());
	}

	static void
	destroy(any_storage& d, true_type)
	{
		d.access<value_type>().~value_type();
	}
	static void
	destroy(any_storage& d, false_type)
	{
		delete d.access<value_type*>();
	}

	static void
	init(any_storage& d, value_type&& x)
	{
		init(d, std::move(x), local_storage());
	}

private:
	static void
	init(any_storage& d, value_type&& x, true_type)
	{
		new(d.access()) value_type(std::move(x));
	}
	static void
	init(any_storage& d, value_type&& x, false_type)
	{
		d = new value_type(std::move(x));
	}

public:
	static void
	manage(any_storage& d, const any_storage& s, any_operation op)
	{
		switch(op)
		{
		case any_operation::get_type:
			d = &typeid(value_type);
			break;
		case any_operation::get_ptr:
			d = get_pointer(s);
			break;
		case any_operation::clone:
			copy(d, s, local_storage());
			break;
		case any_operation::destroy:
			destroy(d, local_storage());
			break;
		case any_operation::get_holder_type:
			d = &typeid(void);
			break;
		case any_operation::get_holder_ptr:
			d = static_cast<any_holder*>(nullptr);
		}
	}
};


/*!
\brief 动态泛型引用处理器。
\since build 352
*/
template<typename _type>
class any_ref_handler : public any_handler<_type*>
{
public:
	typedef _type value_type;
	typedef any_handler<value_type*> base;

	static void
	init(any_storage& d, std::reference_wrapper<value_type> x)
	{
		base::init(d, std::addressof(x.get()));
	}

	static void
	manage(any_storage& d, const any_storage& s, any_operation op)
	{
		switch(op)
		{
		case any_operation::get_type:
			d = &typeid(value_type);
			break;
		case any_operation::get_ptr:
			d = *base::get_pointer(s);
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
class any_holder_handler : public any_handler<_tHolder>
{
	static_assert(is_convertible<_tHolder&, any_holder&>::value,
		"Invalid holder type found.");

public:
	typedef typename _tHolder::value_type value_type;
	typedef any_handler<_tHolder> base;
	typedef typename base::local_storage local_storage;

	static value_type*
	get_pointer(const any_storage& s)
	{
		return static_cast<value_type*>(base::get_pointer(s)->_tHolder::get());
	}

private:
	static void
	init(any_storage& d, _tHolder* p, true_type)
	{
		new(d.access()) _tHolder(std::move(*p));
	}
	static void
	init(any_storage& d, _tHolder* p, false_type)
	{
		d = p;
	}

public:
	static void
	init(any_storage& d, _tHolder* p)
	{
		init(d, p, local_storage());
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
	manage(any_storage& d, const any_storage& s, any_operation op)
	{
		switch(op)
		{
		case any_operation::get_type:
			d = &typeid(value_type);
			break;
		case any_operation::get_ptr:
			d = get_pointer(s);
			break;
		case any_operation::clone:
			base::copy(d, s, local_storage());
			break;
		case any_operation::destroy:
			base::destroy(d, local_storage());
			break;
		case any_operation::get_holder_type:
			d = &typeid(_tHolder);
			break;
		case any_operation::get_holder_ptr:
			d = static_cast<any_holder*>(base::get_pointer(s));
		}
	}
};


/*!
\brief 基于类型擦除的动态泛型对象。
\note 值语义。基本接口和语义同 boost::any 。
\warning 非虚析构。
\see http://www.boost.org/doc/libs/1_50_0/doc/html/any/reference.html#\
any.ValueType 。
\since build 331
*/
class any
{
private:
	//! \since build 352
	//@{
	any_storage storage;
	any_manager manager;
	//@}

public:
	yconstfn
	any() ynothrow
		: storage(), manager()
	{}
	//! \since build 352
	//@{
	template<typename _type>
	any(_type&& x)
		: manager(any_handler<typename remove_reference<_type>::type>::manage)
	{
		any_handler<typename remove_reference<_type>::type>::init(storage,
			yforward(x));
	}
	template<typename _type>
	any(std::reference_wrapper<_type> x)
		: manager(any_ref_handler<_type>::manage)
	{
		any_ref_handler<_type>::init(storage, x);
	}
	//! \brief 构造：使用指定持有者。
	template<typename _tHolder>
	any(holder_tag, _tHolder* p)
		: manager(any_holder_handler<_tHolder>::manage)
	{
		any_holder_handler<_tHolder>::init(storage, p);
	}
	template<typename _type>
	any(_type&& x, holder_tag)
		: manager(any_holder_handler<value_holder<typename
		remove_reference<_type>::type>>::manage)
	{
		any_holder_handler<value_holder<typename
			remove_reference<_type>::type>>::init(storage, yforward(x));
	}
	//@}
	any(const any& a)
		: any()
	{
		if(a)
		{
			manager = a.manager,
			a.manager(storage, a.storage, any_operation::clone);
		}
	}
	any(any&& a) ynothrow
		: any()
	{
		a.swap(*this);
	}
	~any() ynothrow
	{
		if(manager)
			manager(storage, storage, any_operation::destroy);
	}

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
	get() const ynothrow
	{
		if(manager)
		{
			any_storage t;

			manager(t, storage, any_operation::get_ptr);
			return t.access<void*>();
		}
		return nullptr;
	}

	any_holder*
	get_holder() const
	{
		if(manager)
		{
			any_storage t;

			manager(t, storage, any_operation::get_holder_ptr);
			return t.access<any_holder*>();
		}
		return nullptr;
	}

	void
	clear() ynothrow
	{
		if(manager)
		{
			manager(storage, storage, any_operation::destroy);
			manager = nullptr;
		}
	}

	void
	swap(any& a) ynothrow
	{
		std::swap(storage, a.storage),
		std::swap(manager, a.manager);
	}

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
	type() const ynothrow
	{
		if(manager)
		{
			any_storage t;

			manager(t, storage, any_operation::get_type);
			return *t.access<const std::type_info*>();
		}
		return typeid(void);
	}
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
\since build 331
\todo 检验特定环境（如使用动态库时）比较 std::type_info::name() 的必要性。
*/
//@{
template<typename _type>
inline _type*
any_cast(any* p)
{
	return p ? p->target<_type>() : nullptr;
}

template<typename _type>
inline const _type*
any_cast(const any* p)
{
	return p ? p->target<_type>() : nullptr;
}

template<typename _type>
_type
any_cast(const any& x)
{
	const auto tmp(any_cast<typename std::remove_reference<_type>::type>(
		std::addressof(const_cast<any&>(x))));

	if(!tmp)
		throw bad_any_cast(x.type(), typeid(_type));
	return static_cast<_type>(*tmp);
}
//@}

/*!
\brief 非安全动态泛型转换。
\note 语义同 boost::unsafe_any_cast 。
\pre 断言检查： <tt>p</tt> 。
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

#endif

