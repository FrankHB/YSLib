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
\brief 泛型类型对象。
\version r1464;
\author FrankHB<frankhb1989@gmail.com>
\since build 247 。
\par 创建时间:
	2011-09-26 07:55:44 +0800;
\par 修改时间:
	2012-08-18 20:23 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YStandardEx::Any;
*/


#ifndef YB_INC_YSTDEX_ANY_H_
#define YB_INC_YSTDEX_ANY_H_

#include "../ydef.h"
#include <memory> // for std::addressof;
#include "type_op.hpp" // for remove_reference;

namespace ystdex
{

/*
\brief 任意非可复制构造的非聚集类型。
\since build 207 。
*/
union no_copy_t
{
	void* object_ptr;
	const void* const_object_ptr;
	void(*function_ptr)();
	void(no_copy_t::*member_function_pointer)();
};


/*
\brief 任意 POD 类型。
\note POD 含义参考 ISO C++ 2011 。
\since build 207 。
*/
union any_pod_t
{
	no_copy_t _unused;
	byte plain_old_data[sizeof(no_copy_t)];

	void*
	access()
	{
		return &plain_old_data[0];
	}
	yconstfn const void*
	access() const
	{
		return &plain_old_data[0];
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
\since build 247 。
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
\brief 抽象 any 持有对象。
\since build 331 。
*/
class any_holder
{
public:
	virtual
	~any_holder()
	{}

	virtual const std::type_info&
	type() const = 0;

	virtual void*
	get() = 0;

	virtual any_holder*
	clone() const = 0;
};


/*!
\brief 值类型 any 持有对象。
\since build 331 。
*/
template<typename _type>
class value_holder : public any_holder
{
public:
	_type held;

	value_holder(const _type& value)
		: held(value)
	{}
	value_holder(_type&& value) ynothrow
		: held(std::move(value))
	{}

	value_holder&
	operator=(const value_holder&) = delete;

	value_holder*
	clone() const override
	{
		return new value_holder(held);
	}

	void*
	get() override 
	{
		return std::addressof(held);
	}

	const std::type_info&
	type() const override
	{
		return typeid(_type);
	}
};


/*!
\brief 指针类型 any 持有对象。
\tparam _type 对象类型。
\pre <tt>is_object<_type>::value</tt> 。
\since build 331 。
*/
template<typename _type>
class pointer_holder : public any_holder
{
	static_assert(is_object<_type>::value, "Invalid type found.");

public:
	_type* p_held;

	pointer_holder(_type* value)
		: p_held(value)
	{}
	virtual
	~pointer_holder() ynothrow
	{
		delete p_held;
	}

	pointer_holder&
	operator=(const pointer_holder&) = delete;

	pointer_holder*
	clone() const override
	{
		return new pointer_holder(p_held ? new _type(*p_held) : nullptr);
	}

	void*
	get() override 
	{
		return p_held;
	}

	const std::type_info&
	type() const override
	{
		return p_held ? typeid(_type) : typeid(void);
	}
};


/*!
\brief 任意对象类型。
\note 值语义。基本接口和语义同 boost::any 。
\warning 非虚析构。
\see http://www.boost.org/doc/libs/1_50_0/doc/html/any/reference.html#\
any.ValueType
\since build 331 。
*/
class any
{
private:
	mutable any_holder* p_holder;

public:
	yconstfn
	any() ynothrow
		: p_holder(nullptr)
	{}
	any(any_holder* p)
		: p_holder(p)
	{}
	template<typename _type>
	yconstfn
	any(const _type& obj)
		: p_holder(new value_holder<_type>(obj))
	{}
	//! \brief 使用对象指针初始化并获得所有权。
	template<typename _type>
	yconstfn
	any(_type* p_obj, int)
		: p_holder(new pointer_holder<_type>(p_obj))
	{}
	any(const any& a)
		: p_holder(a.p_holder ? a.p_holder->clone() : nullptr)
	{}
	any(any&& a) ynothrow
		: p_holder(a.p_holder)
	{
		a.p_holder = nullptr;
	}
	~any() ynothrow
	{
		delete p_holder;
	}

	template<typename _type>
	any&
	operator=(const _type& rhs)
	{
		any(rhs).swap(*this);
		return *this;
	}
	any&
	operator=(any rhs) ynothrow
	{
		rhs.swap(*this);
		return *this;
	}

	bool
	operator!() const ynothrow
	{
		return !p_holder;
	}

	explicit
	operator bool() const ynothrow
	{
		return p_holder;
	}

	/*!
	\pre 断言检查： \c p_holder 。
	\warning 无类型检查。
	*/
	template<typename _type>
	_type*
	get() const
	{
		yassume(p_holder);

		return static_cast<_type*>(p_holder->get());
	}

	any_holder*
	get_holder() const
	{
		return p_holder;
	}

	void
	clear() ynothrow
	{
		delete p_holder;
		p_holder = nullptr;
	}

	bool
	empty() const ynothrow
	{
		return !p_holder;
	}

	void
	swap(any& rhs) ynothrow
	{
		std::swap(p_holder, rhs.p_holder);
	}

	template<typename _type>
	_type*
	target() const
	{
		return type() == typeid(_type) ? get<_type>() : nullptr;
	}

	const std::type_info&
	type() const
	{
		return p_holder ? p_holder->type() : typeid(void);
	}
};


/*!
\brief any_cast 转换失败异常。
\note 基本接口和语义同 boost::bad_any_cast 。
\see any_cast 。
\since build 331 。
*/
class bad_any_cast : public std::bad_cast
{
public:
	virtual const char*
	what() const ynothrow override
	{
		return "Failed conversion: any_cast.";
	}
};


/*!
\brief any 转换。
\note 语义同 boost::any_cast 。
\since build 331 。
\todo 检验特定环境（如使用动态库时）比较 std::type_info::name() 的必要性。
*/
//@{
template<typename _type>
inline _type*
any_cast(any* p)
{
	return p && p->target<_type>();
}

template<typename _type>
inline const _type*
any_cast(const any* p)
{
	return p && p->target<_type>();
}

template<typename _type>
_type
any_cast(const any& x)
{
	const auto tmp(any_cast<typename remove_reference<_type>::type>(
		std::addressof(x)));

	if(!tmp)
		throw bad_any_cast();
	return std::forward<_type>(*tmp);
}
//@}

/*!
\brief 非安全 any 转换。
\note 语义同 boost::unsafe_any_cast 。
\pre 断言检查 <tt>p</tt> 。
\since build 331 。
*/
//@{
template<typename _type>
inline _type*
unsafe_any_cast(any* p)
{
	yassume(p);

	return p->get<_type>();
}

template<typename _type>
inline const _type*
unsafe_any_cast(const any* p)
{
	yassume(p);

	return p->get<_type>();
}
//@}


/*!
\brief 伪输出对象。
\note 吸收所有赋值操作。
\since build 273 。
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

