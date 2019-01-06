/*
	© 2011-2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file any.h
\ingroup YStandardEx
\brief 动态泛型类型。
\version r4259
\author FrankHB <frankhb1989@gmail.com>
\since build 247
\par 创建时间:
	2011-09-26 07:55:44 +0800
\par 修改时间:
	2019-01-05 19:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YStandardEx::Any

\see ISO C++17[any] 。
\see http://www.boost.org/doc/libs/1_60_0/doc/html/any/reference.html 。
*/


#ifndef YB_INC_ystdex_any_h_
#define YB_INC_ystdex_any_h_ 1

#include "typeinfo.h" // for internal "typeinfo.h", type_info, exclude_self_t,
//	ystdex::type_id, is_nothrow_move_constructible, and_, is_decayed, bool_,
//	is_copy_constructible, enable_if_t, remove_reference_t, cond_t,
//	is_convertible, is_same, is_class, std::bad_cast, decay_t, _t, yconstraint;
#include "utility.hpp" // for internal "utility.hpp", boxed_value,
//	std::addressof, std::unique_ptr, standard_layout_storage, aligned_storage_t,
//	is_aligned_storable, ystdex::pvoid, default_init_t, default_init;
#include "memory.hpp" // for ystdex::clone_monomorphic_ptr, std::allocator,
//	allocator_delete, exclude_self_params_t, alloc_value_t, rebind_alloc_t,
//	allocator_traits, ystdex::allocate_unique, in_place_type_t,
//	vseq::_a, is_instance_of, std::allocator_arg_t, in_place_type;
#include "exception.h" // for throw_invalid_construction,
//	throw_allocator_mismatch_error;
#include "ref.hpp" // for is_reference_wrapper, unwrap_reference_t;
#include <initializer_list> // for std::initializer_list;

namespace ystdex
{

/*!
\ingroup YBase_replacement_extensions
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
class YB_API YB_ATTR_novtable holder
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
	YB_ATTR_nodiscard YB_PURE virtual void*
	get() const = 0;

	//! \since build 683
	YB_ATTR_nodiscard YB_PURE virtual const type_info&
	type() const ynothrow = 0;
};

/*!
\brief 判断是否持有相同对象。
\relates holder
\since build 748
*/
YB_ATTR_nodiscard YB_PURE inline bool
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
	YB_ATTR_nodiscard void*
	get() const override
	{
		return std::addressof(this->value);
	}

	//! \since build 683
	YB_ATTR_nodiscard YB_PURE const type_info&
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
	YB_ATTR_nodiscard void*
	get() const override
	{
		return p_held.get();
	}

	//! \since build 683
	YB_ATTR_nodiscard YB_PURE const type_info&
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
\sa any_manager
\since build 354

指示管理操作的标量。
管理操作用于维护储存或引用的对象，这些对象可能不通过管理操作构造。
按管理操作中参数位置，可以处理目标对象和源对象。
通常源对象是被管理的对象，目标对象可能是被管理的对象或操作结果对象。
管理操作被管理器作为 static 成员函数或成员模板实现。
未被处理的对象在实现中应被忽略，不依赖其中的状态。
特定操作对接受的目标和源对象有前置条件。
*/
enum base_op : op_code
{
	/*!
	\brief 空操作：表示不处理的默认值。
	\since build 355
	*/
	no_op,
	/*!
	\brief 取被管理的对象的类型。

	取源对象的类型，结果为 const type_info& 。
	要求已构造结果对象类型 const type_info* ，用于保存结果。
	若源对象不存在，则结果为 ystdex::type_id<void> 。
	*/
	get_type,
	/*!
	\brief 取指向被管理的对象的指针。

	取指向源对象的指针，结果为 void* 。
	要求已构造结果对象类型 void* ，用于保存结果。
	若源对象不存在，则结果为空指针。
	*/
	get_ptr,
	/*!
	\brief 复制被管理的对象的处理器。
	\sa invalid_construction

	复制构造对象，置于结果对象。
	要求无构造目标对象和已构造的源对象。
	复制操作由对象的复制构造函数确定。
	若源对象不满足可被复制构造，管理操作的实现应抛出 invalid_construction 异常。
	*/
	clone,
	/*!
	\brief 销毁被管理的对象。

	销毁目标对象。
	要求已构造目标对象。
	*/
	destroy,
	/*!
	\brief 取持有者的类型。
	\sa holder_handler

	取源对象关联的持有者的类型，结果为 const type_info& 。
	要求已构造结果对象类型 const type_info* ，用于保存结果。
	若持有者不存在，则结果为 ystdex::type_id<void> 。
	*/
	get_holder_type,
	/*!
	\brief 取指向持有者的指针。
	\sa holder

	取指向源对象关联的持有者的指针，结果为 holder* 。
	要求已构造结果对象类型 holder* ，用于保存结果。
	若持有者不存在，则结果为空指针。
	*/
	get_holder_ptr,
	/*!
	\sa allocator_value_handler
	\sa allocator_holder_handler
	\since build 848
	*/
	//@{
	/*!
	\brief 取重绑定到 byte 的分配器的类型。

	取源对象关联的分配器的类型，结果为 const type_info& 。
	要求已构造结果对象类型 const type_info* ，用于保存结果。
	若分配器不存在，则结果为 ystdex::type_id<void> 。
	*/
	get_allocator_type,
	/*!
	\brief 以分配器复制。

	以指定分配器复制构造对象，置于结果对象。
	要求重绑定到 byte 的分配器的指针类型的目标对象；
	要求以 std::unique_ptr 实例为类型的非空指针作为源对象，
		且其删除器是持有分配器的 allocator_delete 实例；
	要求目标对象表示的分配器和源对象中删除器的分配器类型相同。
	若源对象不满足可被复制构造，管理操作的实现应抛出 invalid_construction 异常。
	*/
	clone_with_allocator,
	/*!
	\brief 以分配器转移。

	以指定分配器转移（可能转移存储或转移构造）对象，置于结果对象。
	要求重绑定到 byte 的分配器的指针类型的目标对象；
	要求以 std::unique_ptr 实例为类型的非空指针作为源对象，
		且其删除器是持有分配器的 allocator_delete 实例；
	要求目标对象表示的分配器和源对象中删除器的分配器类型相同。
	当目标对象表示的分配器和源对象中删除器的分配器相等，直接交换存储；
		否则转移构造对象。
	若需转移构造对象，且源对象不满足可被转移构造，
		管理操作的实现应抛出 invalid_construction 异常。
	*/
	transfer_with_allocator,
	//@}
	/*!
	\brief 结束操作标记。
	\since build 355

	可被扩展操作码使用的起始值。
	*/
	end_base_op
};


//! \since build 848
//@{
/*!
\build 能被动态泛型对象平凡原地存储的数据类型。
\note 保证适合原地分配平凡的非类类型可调用对象。
*/
union any_trivial_local_data
{
	void* obj_ptr;
	const void* const_obj_ptr;
	void(*fn_ptr)();
	void* any_trivial_local_data::*mem_ptr;
	void(any_trivial_local_data::*mem_fn_ptr)();
};


/*!
\build 能被动态泛型对象原地存储的数据类型。
\note 保证适合原地分配平凡的可调用对象以及使用默认分配器的分配器指针。
*/
union any_local_data
{
	//! \since build 849
	using allocated_t = std::unique_ptr<yimpl(int),
		allocator_delete<yimpl(std::allocator<int>)>>;

	any_trivial_local_data trivial;
	//! \since build 849
	aligned_storage_t<sizeof(allocated_t), yalignof(allocated_t)> allocated;
};
//@}


//! \since build 352
using any_storage = standard_layout_storage<aligned_storage_t<
	sizeof(any_local_data), yalignof(any_local_data)>>;
/*!
\brief 动态泛型对象管理操作。
\since build 352
*/
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
\ingroup unary_type_traits
\brief 判断类型是否可以作为 any_storage 原地存储。
\sa value_handler
\since build 848
*/
template<typename _type>
using is_in_place_storable = and_<is_nothrow_move_constructible<_type>,
	is_aligned_storable<any_storage, _type>>;


/*!
\brief 使用持有者标记。
\since build 680
*/
yconstexpr const struct use_holder_t{} use_holder{};

/*!
\ingroup unary_type_traits
\brief 检查参数是否可作为持有者类型。
\note 检查必要不充分条件。可用于静态断言排除非持有者类型。
\since build 848
*/
template<class _tHolder>
using check_holder_t = and_<is_decayed<_tHolder>, is_class<_tHolder>,
	is_base_of<holder, _tHolder>, is_convertible<_tHolder&, holder&>>;


/*!	\defgroup any_handlers Any Operation Handlers
\brief 动态泛型对象操作处理器。
\warning 其中的类类型通常不使用对象，一般可被继承但非虚析构。
\since build 847
*/
//@{
/*!
\brief 动态泛型对象值处理器。
\sa any_storage
\since build 671

可选使用局部存储策略动态管理泛型对象的处理器。
局部存储使用第二模板参数指定。
使用 any_storage 提供对象存储：
当使用局部存储时，作为原地存储；否则，存储 new/delete 管理的指针。
作为基础存储策略的实现，不提供分配器或其它高级分配接口。
*/
template<typename _type,
	bool _bStoredLocally = is_in_place_storable<_type>::value>
class value_handler
{
	//! \since build 848
	static_assert(is_decayed<_type>(), "Invalid type found.");
	static_assert(!_bStoredLocally || is_aligned_storable<any_storage, _type>(),
		"No enough space to store object locally.");

public:
	//! \since build 352
	//@{
	using value_type = _type;
	using local_storage = bool_<_bStoredLocally>;
	//@}

private:
	//! \since build 848
	//@{
	// TODO: Extrace as %unique_ptr instance?
	class memory_thunk
	{
	private:
		std::unique_ptr<value_type> p_val;

	public:
		template<typename... _tParams,
			yimpl(typename = exclude_self_params_t<memory_thunk, _tParams...>)>
		inline
		memory_thunk(_tParams&&... args)
			: p_val(make_unique<value_type>(yforward(args)...))
		{}
		memory_thunk(const memory_thunk& mt)
			: p_val(make_unique(*mt.p_val))
		{}
		memory_thunk(memory_thunk&& mt) = default;

		operator value_type&() const ynothrowv
		{
			return get();
		}

		value_type&
		get() const ynothrowv
		{
			yassume(bool(p_val));
			return *p_val;
		}
	};
	static_assert(or_<local_storage, is_aligned_storable<any_storage,
		memory_thunk>>(), "Invalid stored target found.");
	using stored_type = cond_t<local_storage, value_type, memory_thunk>;
	//@}

public:
	//! \since build 595
	//@{
	YB_ATTR(always_inline) static void
	copy(any_storage& d, const any_storage& s)
	{
		init_or_throw(is_copy_constructible<value_type>(), d, get_reference(s));
	}

	YB_ATTR(always_inline) static void
	dispose(any_storage& d) ynothrowv
	{
		d.destroy<stored_type>();
	}
	//@}

	//! \since build 848
	//@{
	YB_ATTR_nodiscard YB_ATTR(always_inline) YB_PURE static value_type*
	get_pointer(any_storage& s) ynothrow
	{
		return std::addressof(get_reference(s));
	}
	YB_ATTR_nodiscard YB_ATTR(always_inline) YB_PURE static const value_type*
	get_pointer(const any_storage& s) ynothrow
	{
		return std::addressof(get_reference(s));
	}

public:
	YB_ATTR_nodiscard YB_ATTR(always_inline) YB_PURE static value_type&
	get_reference(any_storage& s) ynothrow
	{
		return s.access<stored_type>();
	}
	YB_ATTR_nodiscard YB_ATTR(always_inline) YB_PURE static const value_type&
	get_reference(const any_storage& s) ynothrow
	{
		return s.access<stored_type>();
	}
	//@}

	//! \since build 595
	template<typename... _tParams>
	YB_ATTR(always_inline) static void
	init(any_storage& d, _tParams&&... args)
	{
		d.construct<stored_type>(yforward(args)...);
	}

private:
	//! \since build 848
	template<typename... _tParams>
	YB_NORETURN YB_ATTR(always_inline) static void
	init_or_throw(false_, _tParams&&...)
	{
		throw_invalid_construction();
	}
	//! \since build 848
	template<typename... _tParams>
	YB_ATTR(always_inline) static void
	init_or_throw(true_, any_storage& d, _tParams&&... args)
	{
		init(d, yforward(args)...);
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
		case get_allocator_type:
			d = &ystdex::type_id<void>();
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
class ref_handler
{
public:
	using value_type = _type;
	using base = value_handler<value_type*>;

	//! \since build 848
	//@{
	YB_ATTR_nodiscard YB_PURE static value_type*
	get_pointer(any_storage& s) ynothrow
	{
		return base::get_reference(s);
	}
	YB_ATTR_nodiscard YB_PURE static const value_type*
	get_pointer(const any_storage& s) ynothrow
	{
		return base::get_reference(s);
	}

	YB_ATTR_nodiscard YB_PURE static value_type&
	get_reference(any_storage& s) ynothrowv
	{
		yassume(get_pointer(s));
		return *get_pointer(s);
	}
	YB_ATTR_nodiscard YB_PURE static const value_type&
	get_reference(const any_storage& s) ynothrowv
	{
		yassume(get_pointer(s));
		return *get_pointer(s);
	}
	//@}

	//! \since build 678
	template<class _tWrapper,
		yimpl(typename = enable_if_t<is_reference_wrapper<_tWrapper>::value>)>
	YB_ATTR(always_inline) static auto
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
\brief 动态泛型对象持有者处理器。
\sa value_handler
\since build 352

可定制分配行为的处理器。
*/
template<typename _tHolder>
class holder_handler : private value_handler<_tHolder>
{
	static_assert(check_holder_t<_tHolder>(), "Invalid holder type found.");

public:
	//! \since build 848
	using holder_type = _tHolder;
	using base = value_handler<holder_type>;

	//! \since build 848
	YB_ATTR_nodiscard YB_PURE static holder_type*
	get_holder_pointer(any_storage& s) ynothrow
	{
		return base::get_pointer(s);
	}

private:
	//! \since build 729
	YB_ATTR(always_inline) static void
	init(false_, any_storage& d, std::unique_ptr<holder_type> p)
	{
		d.construct<holder_type*>(p.release());
	}
	//! \since build 729
	YB_ATTR(always_inline) static void
	init(true_, any_storage& d, std::unique_ptr<holder_type> p)
	{
		d.construct<holder_type>(std::move(*p));
	}

public:
	//! \since build 395
	YB_ATTR(always_inline) static void
	init(any_storage& d, std::unique_ptr<holder_type> p)
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
			d = &ystdex::type_id<holder_type>();
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
\brief 分配器处理器。
\sa value_handler
\since build 848
*/
template<class _tByteAlloc, typename _tValue>
class allocator_value_handler
{
	static_assert(and_<is_decayed<_tByteAlloc>, is_same<alloc_value_t<
		_tByteAlloc>, byte>>(), "Invalid base allocator type found.");
	static_assert(is_decayed<_tValue>(), "Invalid value type found.");

public:
	using value_type = _tValue;

private:
	using ator_type = rebind_alloc_t<_tByteAlloc, _tValue>;
	using ator_traits = allocator_traits<ator_type>;
	using deleter_type = allocator_delete<ator_type>;
	using ator_owned_ptr = std::unique_ptr<value_type, deleter_type>;
	using rebound_ator_type = rebind_alloc_t<ator_type, byte>;

public:
	using base = value_handler<ator_owned_ptr>;

	static void
	copy(any_storage& d, const any_storage& s)
	{
		const auto& p_owned(base::get_reference(s));

		init_or_throw(is_copy_constructible<value_type>(), d,
			ator_type(p_owned.get_deleter().get_allocator()), p_owned);
	}

private:
	template<typename _func>
	static void
	do_with_allocator(any_storage& d, any_storage& s, _func f)
	{
		const auto p_a(d.access<rebound_ator_type*>());
		const auto& p_owned(base::get_reference(s));

		yassume(p_a);
		yassume(bool(p_owned));
		d.~any_storage();
		::new(&d) any_storage;
		f(ator_type(*p_a), p_owned);
	}

public:
	YB_ATTR_nodiscard YB_PURE static value_type*
	get_pointer(any_storage& s) ynothrow
	{
		return base::get_reference(s).get();
	}
	YB_ATTR_nodiscard YB_PURE static const value_type*
	get_pointer(const any_storage& s) ynothrow
	{
		return base::get_reference(s).get();
	}

	// XXX: This is not used here, but for client code (e.g. %ystdex::function).
	//	Extract as handler traits function template or free function template?
	YB_ATTR_nodiscard YB_PURE static value_type&
	get_reference(any_storage& s) ynothrowv
	{
		yassume(get_pointer(s));
		return *get_pointer(s);
	}
	YB_ATTR_nodiscard YB_PURE static const value_type&
	get_reference(const any_storage& s) ynothrowv
	{
		yassume(get_pointer(s));
		return *get_pointer(s);
	}

	template<typename... _tParams>
	YB_ATTR(always_inline) static void
	init(any_storage& d, _tParams&&... args)
	{
		base::init(d, ystdex::allocate_unique<value_type>(yforward(args)...));
	}

private:
	template<typename... _tParams>
	YB_NORETURN YB_ATTR(always_inline) static void
	init_or_throw(false_, _tParams&&...)
	{
		throw_invalid_construction();
	}
	template<typename... _tParams>
	YB_ATTR(always_inline) static void
	init_or_throw(true_, any_storage& d, const ator_type& a,
		const ator_owned_ptr& p_owned)
	{
		init(d, a, ystdex::as_const(*p_owned));
	}
	template<typename... _tParams>
	YB_ATTR(always_inline) static void
	init_or_throw(true_, any_storage& d, const ator_type& a,
		ator_owned_ptr&& p_owned)
	{
		init(d, a, std::move(*p_owned));
	}

public:
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
		case get_allocator_type:
			d = &ystdex::type_id<rebound_ator_type>();
			break;
		case clone_with_allocator:
			do_with_allocator(d, s,
				[&](const rebound_ator_type& a, const ator_owned_ptr& p_owned){
				init_or_throw(is_copy_constructible<value_type>(), d,
					ator_type(a), p_owned);
			});
			break;
		case transfer_with_allocator:
			do_with_allocator(d, s,
				[&](const rebound_ator_type& a, const ator_owned_ptr& p_owned){
				if(typename allocator_traits<rebound_ator_type>
					::is_always_equal() || rebound_ator_type(
					p_owned.get_deleter().get_allocator()) == a)
					std::swap(d, s);
				else
					init_or_throw(is_move_constructible<value_type>(), d,
						ator_type(a), std::move(p_owned));
			});
			break;
		default:
			base::manage(d, s, op);
		}
	}
};


/*!
\brief 使用分配器的动态泛型对象持有者处理器。
\sa holder_handler
\since build 848
*/
template<class _tByteAlloc, class _tHolder>
class allocator_holder_handler
	: private allocator_value_handler<_tByteAlloc, _tHolder>
{
	static_assert(check_holder_t<_tHolder>(), "Invalid holder type found.");

public:
	using holder_type = _tHolder;
	using base = allocator_value_handler<_tByteAlloc, _tHolder>;

	YB_ATTR_nodiscard YB_PURE static holder_type*
	get_holder_pointer(any_storage& s) ynothrow
	{
		return base::get_pointer(s);
	}

	using base::init;

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
			d = &ystdex::type_id<holder_type>();
			break;
		case get_holder_ptr:
			d = static_cast<holder*>(get_holder_pointer(s));
			break;
		default:
			base::manage(d, s, op);
		}
	}
};
//@}


/*!
\ingroup binary_type_traits
\since build 848
*/
//@{
//! \brief 取重绑定 byte 分配器的使用分配器构造时的处理器。
template<class _tAlloc, typename _type>
using allocator_value_handler_t = allocator_value_handler<
	rebind_alloc_t<_tAlloc, byte>, _type>;
//! \brief 取重绑定 byte 分配器的使用分配器和持有者构造时的处理器。
template<class _tAlloc, class _tHolder>
using allocator_holder_handler_t = allocator_holder_handler<
	rebind_alloc_t<_tAlloc, byte>, _tHolder>;
//@}


/*!
\ingroup trasformation_traits
\brief 根据类型选择引用或值处理器。
\since build 355
*/
template<typename _type>
struct wrap_handler
{
	//! \since build 848
	static_assert(is_decayed<_type>(), "Invalid type found.");

	using value_type = remove_reference_t<unwrap_reference_t<_type>>;
	//! \since build 848
	static_assert(is_object<value_type>(), "Invalid value type found.");
	using type = cond_t<is_reference_wrapper<_type>, ref_handler<value_type>,
		value_handler<value_type>>;
};

} // namespace any_ops;


/*!
\ingroup exceptions
\brief 动态泛型转换失败异常。
\note 基本接口和语义同 boost::bad_any_cast 。
\note 非标准扩展：提供标识转换失败的源和目标类型。
\sa any_cast
\see ISO C++17[any.bad_any_cast] 。
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

	//! \ingroup YBase_replacement_extensions
	//@{
	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_PURE const char*
	from() const ynothrow;

	//! \since build 683
	YB_ATTR_nodiscard YB_PURE const type_info&
	from_type() const ynothrow
	{
		return from_ti.get();
	}

	YB_ATTR_nodiscard YB_ATTR_returns_nonnull YB_PURE const char*
	to() const ynothrow;

	//! \since build 683
	YB_ATTR_nodiscard YB_PURE const type_info&
	to_type() const ynothrow
	{
		return to_ti.get();
	}
	//@}
	//@}

	YB_ATTR_nodiscard YB_ATTR_returns_nonnull yimpl(YB_PURE) virtual const char*
	what() const ynothrow override;
};


//! \since build 687
namespace details
{

struct any_base
{
	// NOTE: Data members are only initialized when needed. If manager is empty,
	//	the storage is not used.
	//! \since build 692
	mutable any_ops::any_storage storage;
	any_ops::any_manager manager;

protected:
	// NOTE: Even this base class is not templated, all member functions are
	//	inlined intentionally for performance reasons (e.g. to avoid overhead of
	//	calls across dynamic library boundaries and difficulties on
	//	inter-procedural optimizations). This is particularly important when
	//	%any is used to implementation some general type-erased wrappers like
	//	%function.
	//! \since build 848
	//@{
	yconstexpr
	any_base() ynothrow
		: storage(), manager()
	{}
	explicit
	any_base(default_init_t) ynothrow
		: manager()
	{}
	template<class _tHandler, typename... _tParams>
	explicit inline
	any_base(any_ops::with_handler_t<_tHandler>, _tParams&&... args)
		: manager(any_ops::construct<_tHandler>(storage, yforward(args)...))
	{}
	//@}
	//! \since build 714
	any_base(const any_base& a)
		: manager(a.manager)
	{}
	//! \since build 714
	~any_base() = default;

public:
	any_ops::any_storage&
	call(any_ops::any_storage& t, any_ops::op_code op) const
	{
		yconstraint(manager);
		manager(t, storage, op);
		return t;
	}

	//! \since build 848
	void
	checked_destroy() ynothrow
	{
		if(manager)
			destroy();
	}

	void
	destroy() ynothrowv
	{
		yconstraint(manager);
		manager(storage, storage, any_ops::destroy);
	}

	//! \pre 间接断言：\c manager 。
	//@{
	YB_ATTR_nodiscard YB_PURE void*
	get() const ynothrowv
	{
		return unchecked_access<void*>(default_init, any_ops::get_ptr);
	}

	YB_ATTR_nodiscard YB_PURE any_ops::holder*
	get_holder() const
	{
		return unchecked_access<any_ops::holder*>(default_init,
			any_ops::get_holder_ptr);
	}
	//@}

	//! \since build 692
	YB_ATTR_nodiscard YB_PURE any_ops::any_storage&
	get_storage() const
	{
		return storage;
	}

	//! \since build 717
	YB_ATTR_nodiscard YB_PURE bool
	has_value() const ynothrow
	{
		return manager;
	}

	void
	move_from(any_base& a) ynothrow
	{
		if(a.manager)
			yunseq(storage = a.storage, a.manager = {});
	}

	void
	swap(any_base& a) ynothrow
	{
		std::swap(storage, a.storage),
		std::swap(manager, a.manager);
	}

	//! \pre 间接断言：\c manager 。
	//@{
	template<typename _type>
	YB_ATTR_nodiscard _type*
	target() ynothrowv
	{
		return type() == ystdex::type_id<_type>() ? static_cast<_type*>(get())
			: nullptr;
	}
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE const _type*
	target() const ynothrowv
	{
		return type() == ystdex::type_id<_type>()
			? static_cast<const _type*>(get()) : nullptr;
	}

	YB_ATTR_nodiscard YB_PURE const type_info&
	type() const ynothrowv
	{
		return *unchecked_access<const type_info*>(default_init,
			any_ops::get_type);
	}

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
	//! \since build 848
	//@{
	//! \see LWG 2857 。 
	//@{
	template<typename _type, typename... _tParams>
	decay_t<_type>&
	emplace(_tParams&&... args)
	{
		emplace_with_handler<any_ops::value_handler<decay_t<_type>>>(
			yforward(args)...);
		return unchecked_target<_type>();
	}
	template<typename _type, typename _tOther, typename... _tParams>
	decay_t<_type>&
	emplace(std::initializer_list<_tOther> il, _tParams&&... args)
	{
		emplace_with_handler<any_ops::value_handler<decay_t<_type>>>(il,
			yforward(args)...);
		return unchecked_target<_type>();
	}
	//@}
	// NOTE: The return is similar but different to overloads above.
	//! \ingroup YBase_replacement_extensions
	template<typename _tHolder, typename... _tParams>
	decay_t<_tHolder>&
	emplace(any_ops::use_holder_t, _tParams&&... args)
	{
		emplace_with_handler<any_ops::holder_handler<decay_t<_tHolder>>>(
			yforward(args)...);

		const auto p_holder(static_cast<decay_t<_tHolder>*>(
			static_cast<_tAny&>(*this).unchecked_get_holder()));

		yassume(p_holder);
		return *p_holder;
	}
	//@}

	template<typename _tHandler, typename... _tParams>
	void
	emplace_with_handler(_tParams&&... args)
	{
		auto& a(static_cast<_tAny&>(*this));

		a.checked_destroy();
		try
		{
			a.manager = any_ops::construct<decay_t<_tHandler>>(a.storage,
				yforward(args)...);
		}
		catch(...)
		{
			a.manager = {};
			throw;
		}
	}

	//! \since build 848
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE decay_t<_type>&
	unchecked_target()
	{
		return static_cast<decay_t<_type>&>(
			static_cast<_tAny&>(*this).unchecked_get());
	}
};

} // namespace details;


/*!
\ingroup YBase_replacement_features
\brief 基于类型擦除的动态泛型对象。
\warning 非虚析构。
\see ISO C++17[any.class] 。
\see http://www.boost.org/doc/libs/1_69_0/doc/html/any/reference.html#any.ValueType 。
\since build 331

值传递语义的动态泛型对象包装。
基本接口和语义同 ISO C++17 std::any 提议和 boost::any （对应接口以前者为准）。
和 std::any 不同，支持不满足 CopyConstructible 但满足 MoveConstructible 的对象。
作为扩展，基于命名空间 any_ops 支持更多的可扩展的底层接口。
使用 any_ops::with_handler_t 显式指定处理器，可调用这些接口。
保存对象状态的内部数据存储在 any_ops::any_storage ，通过处理器访问。
处理器是满足特定条件的类类型。
处理器以管理操作 any_ops::any_manager 引用。
处理器具有静态成员函数 manage ，可初始化 any_ops::any_manager 。
处理器具有静态成员函数或函数模板 init ，可初始化 any_ops::any_storage 中的值。
当前支持的处理器都定义在命名空间 any_ops 。
处理器还满足其它一些当前未指定的要求。
管理操作在初始化时被指定，当且仅当空对象时为空，并被复位为空对象的操作复位为空值。
赋值和其它修改对象的其它操作可能修改管理操作。
空对象的内部数据是未指定的有效值，可能是未决定值，不应被读取，否则行为未定义。
管理操作蕴含的处理器对内部数据进行操作时，应满足以下一致性要求，否则行为未定义：
	处理器中的初始化操作应保证初始化后的状态和处理器一致，
		即储存动态类型符合处理器预期的对象；
	读取内部存储的操作应和管理操作蕴含的处理器一致；
	写入内部数据后应保证之后的状态和管理操作蕴含的处理器一致。
以引用包装存储时无视 cv 修饰符。访问不检查动态类型，应注意避免未定义行为。
具有相同类型对象的 any 对象，使用不同的处理器应保证具有相同的动态类型，即成员函数
	type 和 unchekced_type 返回相同的结果。
默认情形调用的 any_ops::value_handler 的实例作为处理器，对所有的对象具有所有权。
使用 any_ops::ref_handler 处理引用包装类型，此时不具有所有权，但仍可复制值。
使用 any_ops::wrap_handler 根据类型选择按值或按引用保存的处理器。
	尽管都能复制值，但所有权不同容易引起误用，所以默认不直接以 any 的构造函数提供。
构造支持分配器。初始化需要分配时：
不使用分配器的初始化，使用 new 和 delete 管理资源分配；
使用分配器的初始化，由 allocator_traits 实例的 construct 和 destroy 管理资源分配。
使用分配器时，保存分配器的副本。
*/
class YB_API any : private details::any_base, private details::any_emplace<any>
{
	//! \since build 737
	friend details::any_emplace<any>;

private:
	//! \since build 849
	template<typename _type>
	using exclude_tag_t = enable_if_t<!is_instance_of<decay_t<_type>,
		vseq::_a<in_place_type_t>>::value>;
	//! \since build 848
	//@{
	template<bool _vInPlace = false>
	struct alloc_tag : bool_<_vInPlace>
	{};

public:
	/*!
	\ingroup unary_type_traits
	\brief 判断使用分配器构造时是否忽略分配器而在原地存储。

	使用 std::allocator_arg_t 构造时，决定是否使用原地存储而非分配器构造的特征；
		但不在显式指定处理器的构造和 any&& 参数中被使用。
	因为原地存储隐含使用不同的处理器，而使用 any_ops::any_storage
		的方式根据不同的处理器可能不同，为使用户代码能满足一致性要求，
		这个特征提供为公开接口。
	使用分配器存储时，根据直接使用值或持有者的构造，使用以下之一的处理器：
	any_ops::allocator_value_handler ；
	any_ops::allocator_holder_handler 。
	使用原地存储时，用以下处理器代替：
	any_ops::value_handler ；
	any_ops::holder_handler 。
	原地存储的处理器的 value_type 在以上情形中分别是被构造的对象类型和持有者类型。 
	*/
	template<typename _type>
	using opt_in_place_t
		= alloc_tag<any_ops::is_in_place_storable<decay_t<_type>>::value>;
	//! \ingroup binary_type_traits
	//@{
	//! \brief 根据分配器类型和存储对象的类型取使用分配器构造时的处理器。
	template<class _tAlloc, typename _type>
	using allocated_value_handler_t = cond_t<any::opt_in_place_t<_type>,
		any_ops::value_handler<_type>,
		any_ops::allocator_value_handler_t<_tAlloc, _type>>;
	//! \brief 根据分配器类型和存储对象的类型取使用分配器和持有者构造时的处理器。
	template<class _tAlloc, class _tHolder>
	using allocated_holder_handler_t = cond_t<any::opt_in_place_t<_tHolder>,
		any_ops::holder_handler<_tHolder>,
		any_ops::allocator_holder_handler_t<_tAlloc, _tHolder>>;
	//@}
	//@}

	// NOTE: Efficiency consideration is same to %details::any_base.
	//! \post \c !has_value() 。
	//@{
	yconstfn
	any() ynothrow = default;
	//! \since build 848
	explicit
	any(default_init_t) ynothrow
		: any_base(default_init)
	{}
	//@}
	/*!
	\note YStandardEx 扩展：允许不满足 CopyConstructible 的类型的值作为参数。
	\since build 849
	*/
	template<typename _type, yimpl(typename = exclude_self_t<any, _type>,
		typename = exclude_tag_t<_type>)>
	inline
	any(_type&& x)
		: any(any_ops::with_handler_t<any_ops::value_handler<decay_t<_type>>>(),
		yforward(x))
	{}
	/*!
	\ingroup YBase_replacement_extensions
	\since build 849
	*/
	template<typename _type, class _tAlloc, yimpl(
		typename = exclude_self_t<any, _type>, typename = exclude_tag_t<_type>)>
	inline
	any(std::allocator_arg_t, const _tAlloc& a, _type&& x)
		: any(opt_in_place_t<_type>(), a, yforward(x))
	{}
	//! \since build 717
	template<typename _type, typename... _tParams>
	explicit inline
	any(in_place_type_t<_type>, _tParams&&... args)
		: any(any_ops::with_handler_t<any_ops::value_handler<_type>>(),
		yforward(args)...)
	{}
	//! \since build 848
	//@{
	//! \ingroup YBase_replacement_extensions
	template<typename _type, class _tAlloc, typename... _tParams>
	inline
	any(std::allocator_arg_t, const _tAlloc& a, in_place_type_t<_type>,
		_tParams&&... args)
		: any(opt_in_place_t<_type>(), a, in_place_type<_type>,
		yforward(args)...)
	{}
	//! \note YStandardEx 扩展：允许不满足 CopyConstructible 的类型的值作为参数。
	template<typename _type, typename _type2, typename... _tParams,
		yimpl(typename = enable_if_t<is_constructible<decay_t<_type>,
		std::initializer_list<_type2>&, _tParams...>::value>)>
	explicit inline
	any(in_place_type_t<_type>, std::initializer_list<_type2> il,
		_tParams&&... args)
		: any(any_ops::with_handler_t<any_ops::value_handler<_type>>(), il,
		yforward(args)...)
	{}
	//! \ingroup YBase_replacement_extensions
	template<typename _type, typename _type2, class _tAlloc,
		typename... _tParams,
		yimpl(typename = enable_if_t<is_constructible<decay_t<_type>,
		std::initializer_list<_type2>&, _tParams...>::value>)>
	inline
	any(std::allocator_arg_t, const _tAlloc& a, in_place_type_t<_type>,
		std::initializer_list<_type2> il, _tParams&&... args)
		: any(opt_in_place_t<_type>(), a, in_place_type<_type>, il,
		yforward(args)...)
	{}
	//@}
	//! \ingroup YBase_replacement_extensions
	//@{
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
	//! \since build 848
	template<typename _tHolder, class _tAlloc>
	inline
	any(std::allocator_arg_t, const _tAlloc& a, any_ops::use_holder_t,
		_tHolder&& h)
		: any(opt_in_place_t<_tHolder>(), a, any_ops::use_holder,
		yforward(h))
	{}
	//! \since build 717
	template<typename _tHolder, typename... _tParams>
	inline
	any(any_ops::use_holder_t, in_place_type_t<_tHolder>, _tParams&&... args)
		: any(any_ops::with_handler_t<any_ops::holder_handler<_tHolder>>(),
		yforward(args)...)
	{}
	//! \since build 848
	template<typename _tHolder, class _tAlloc, typename... _tParams>
	inline
	any(std::allocator_arg_t, const _tAlloc& a, any_ops::use_holder_t,
		in_place_type_t<_tHolder>, _tParams&&... args)
		: any(opt_in_place_t<_tHolder>(), a, any_ops::use_holder,
		in_place_type<_tHolder>, yforward(args)...)
	{}
	//@}
	template<typename _type>
	inline
	any(_type&& x, any_ops::use_holder_t)
		: any(any_ops::with_handler_t<any_ops::holder_handler<
		any_ops::value_holder<decay_t<_type>>>>(), yforward(x))
	{}
	//! \since build 848
	template<class _tAlloc, typename _type>
	inline
	any(std::allocator_arg_t, const _tAlloc& a, _type&& x,
		any_ops::use_holder_t)
		: any(opt_in_place_t<rebind_alloc_t<_tAlloc, any_ops::value_holder<
		decay_t<_type>>>>(), a, yforward(x), any_ops::use_holder)
	{}
	//! \since build 687
	template<class _tHandler, typename... _tParams>
	explicit inline
	any(any_ops::with_handler_t<_tHandler> t, _tParams&&... args)
		: any_base(t, yforward(args)...)
	{}
	//! \since build 848
	//@{
	template<class _tHandler, class _tAlloc, typename... _tParams>
	inline
	any(std::allocator_arg_t, const _tAlloc& a,
		any_ops::with_handler_t<_tHandler> t, _tParams&&... args)
		: any_base(t, a, yforward(args)...)
	{}

private:
	template<class _tAlloc, typename... _tParams>
	inline
	any(alloc_tag<true>, const _tAlloc&, _tParams&&... args)
		: any(yforward(args)...)
	{}
	template<typename _type, class _tAlloc>
	inline
	any(alloc_tag<>, const _tAlloc& a, _type&& x)
		: any(any_ops::with_handler_t<any_ops::allocator_value_handler_t<
		_tAlloc, decay_t<_type>>>(), a, yforward(x))
	{}
	template<typename _type, class _tAlloc, typename... _tParams>
	inline
	any(alloc_tag<>, const _tAlloc& a, in_place_type_t<_type>,
		_tParams&&... args)
		: any(any_ops::with_handler_t<any_ops::allocator_value_handler_t<
		_tAlloc, _type>>(), a, yforward(args)...)
	{}
	template<typename _tHolder, class _tAlloc>
	inline
	any(alloc_tag<>, const _tAlloc& a, any_ops::use_holder_t, _tHolder&& h)
		: any(any_ops::with_handler_t<any_ops::allocator_holder_handler_t<
		_tAlloc, decay_t<_tHolder>>>(), a, yforward(h))
	{}
	template<typename _tHolder, class _tAlloc, typename... _tParams>
	inline
	any(alloc_tag<>, const _tAlloc& a, any_ops::use_holder_t,
		in_place_type_t<_tHolder>, _tParams&&... args)
		: any(any_ops::with_handler_t<any_ops::allocator_holder_handler_t<
		_tAlloc, _tHolder>>(), a, yforward(args)...)
	{}
	template<class _tAlloc, typename _type>
	inline
	any(alloc_tag<>, const _tAlloc& a, _type&& x, any_ops::use_holder_t)
		: any(any_ops::with_handler_t<any_ops::allocator_holder_handler_t<
		_tAlloc, any_ops::value_holder<decay_t<_type>>>>(), a, yforward(x))
	{}
	//@}
	//@}

public:
	any(const any& x)
		: any_base(x)
	{
		if(manager)
			manager(storage, x.storage, any_ops::clone);
	}
	/*!
	\ingroup YBase_replacement_extensions
	\throw allocator_mismatch_error 分配器不兼容。
	*/
	template<class _tAlloc>
	any(std::allocator_arg_t, const _tAlloc& a, const any& x)
		: any_base(x)
	{
		do_alloc_init(a, x, any_ops::clone_with_allocator);
	}
	//! \post 非标准扩展：转移后被转移的参数值满足 !has_value() 。
	//@{
	any(any&& x) ynothrow
		: any_base(x)
	{
		move_from(x);
	}
	/*!
	\ingroup YBase_replacement_extensions
	\throw allocator_mismatch_error 分配器不兼容。
	*/
	template<class _tAlloc>
	any(std::allocator_arg_t, const _tAlloc& a, any&& x)
		: any_base(x)
	{
		do_alloc_init(a, x, any_ops::transfer_with_allocator);
	}
	//@}
	//! \since build 382
	~any()
	{
		checked_destroy();
	}

	//! \since build 849
	template<typename _type, yimpl(typename = exclude_self_t<any, _type>,
		typename = exclude_tag_t<_type>)>
	inline any&
	operator=(_type&& x)
	{
		// NOTE: Avoiding of %swap is significantly more efficient with
		//	contemporary language implementations.
		return *this = any(yforward(x));
	}
	/*!
	\brief 复制赋值：使用复制和交换。
	\since build 332
	*/
	any&
	operator=(const any& a)
	{
		// NOTE: Ditto.
		return *this = any(a);
	}
	/*!
	\brief 转移赋值：使用复制和交换。
	\since build 332
	*/
	any&
	operator=(any&& a) ynothrow
	{
		// NOTE: Ditto.
		checked_destroy();
		manager = a.manager;
		move_from(a);
		return *this;
	}

private:
	//! \since build 848
	template<class _tAlloc>
	inline void
	do_alloc_init(const _tAlloc& a, const any& x, any_ops::op_code op)
	{
		do_alloc_init_impl(rebind_alloc_t<_tAlloc, byte>(a), x, op);
	}

	template<class _tByteAlloc>
	void
	do_alloc_init_impl(const _tByteAlloc& a, const any& x, any_ops::op_code op)
	{
		static_assert(and_<is_decayed<_tByteAlloc>, is_same<alloc_value_t<
			_tByteAlloc>, byte>>(), "Invalid base allocator type found.");

		if(manager)
		{
			const auto& alloc_tp(*x.template unchecked_access<const type_info*>(
				default_init, any_ops::get_allocator_type));

			if(ystdex::type_id<_tByteAlloc>() == alloc_tp)
			{
				_tByteAlloc ra(a);
				const auto
					gd(storage.pun<_tByteAlloc*>(std::addressof(ra)));

				x.call(storage, op);
			}
			else
				throw_allocator_mismatch_error();
		}
	}

public:
	//! \since build 717
	using any_base::has_value;

	//! \ingroup YBase_replacement_extensions
	//@{
	//! \since build 352
	YB_ATTR_nodiscard YB_PURE void*
	get() const ynothrow
	{
		return manager ? unchecked_get() : nullptr;
	}

	YB_ATTR_nodiscard YB_PURE any_ops::holder*
	get_holder() const
	{
		return manager ? unchecked_get_holder() : nullptr;
	}
	//@}

	/*!
	\ingroup YBase_replacement_features
	\since build 848
	*/
	//@{
	yimpl(using) any_base::get_storage;

	yimpl(using) any_base::call;
	//@}

	//! \since build 717
	void
	reset() ynothrow
	{
		if(manager)
		{
			destroy();
			manager = {};
		}
	}

	//! \note 非强异常安全：若构造目标抛出异常，原目标已被销毁。
	//@{
	//! \since build 687
	yimpl(using) any_emplace<any>::emplace;

	/*!
	\ingroup YBase_replacement_features
	\since build 687
	*/
	yimpl(using) any_emplace<any>::emplace_with_handler;
	//@}

	//! \brief 交换对象。
	//@{
	void
	swap(any& a) ynothrow
	{
		any_base::swap(a);
	}
	//! \since build 848
	friend void
	swap(any& x, any& y) ynothrow
	{
		x.swap(y);
	}
	//@}

	/*!
	\ingroup YBase_replacement_features
	\brief 取目标指针。
	\return 若存储目标类型和模板参数相同则为指向存储对象的指针值，否则为空指针值。
	\since build 352
	*/
	//@{
	template<typename _type>
	YB_ATTR_nodiscard _type*
	target() ynothrow
	{
		return manager ? any_base::template target<_type>() : nullptr;
	}
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE const _type*
	target() const ynothrow
	{
		return manager ? any_base::template target<_type>() : nullptr;
	}
	//@}

	//! \since build 683
	YB_ATTR_nodiscard YB_PURE const type_info&
	type() const ynothrow
	{
		return manager ? unchecked_type() : ystdex::type_id<void>();
	}

	/*!
	\ingroup YBase_replacement_features
	\pre 断言：\c has_value() 。
	\since build 677
	*/
	//@{
	//! \since build 848
	yimpl(using) any_base::unchecked_access;

	//! \brief 取包含对象的指针。
	YB_ATTR_nodiscard void*
	unchecked_get() const ynothrowv
	{
		return any_base::get();
	}

	//! \brief 取持有者指针。
	YB_ATTR_nodiscard YB_PURE any_ops::holder*
	unchecked_get_holder() const
	{
		return any_base::get_holder();
	}

	/*!
	\brief 取包含对象的类型。
	\since build 683
	*/
	YB_ATTR_nodiscard YB_PURE const type_info&
	unchecked_type() const ynothrowv
	{
		return any_base::type();
	}
	//@}
};

//! \relates any
//@{
//! \ingroup YBase_replacement_features
//@{
//! \see ISO C++17[any.nonmembers] 。
//@{
/*!
\ingroup helper_functions
\brief 创建 any 对象。
\see WG21 P0032R3 。
\since build 717
*/
//@{
template<typename _type, typename... _tParams>
YB_ATTR_nodiscard inline any
make_any(_tParams&&... args)
{
	return any(in_place_type<_type>, yforward(args)...);
}
template<typename _type, typename _tOther, typename... _tParams>
YB_ATTR_nodiscard inline any
make_any(std::initializer_list<_tOther> il, _tParams&&... args)
{
	return any(in_place_type<_type>, il, yforward(args)...);
}
//@}

/*!
\brief 动态泛型转换。
\return 当 <tt>p
	&& p->type() == ystdex::type_id<remove_pointer_t<_tPointer>>()</tt> 时
	为指向对象的指针，否则为空指针。
\note 语义同 \c boost::any_cast 。
\since build 671
\todo 检验特定实现环境（如使用动态库时）比较 std::type_info::name() 的必要性。
*/
//@{
//@{
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline _type*
any_cast(any* p) ynothrow
{
	return p ? p->target<_type>() : nullptr;
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline const _type*
any_cast(const any* p) ynothrow
{
	return p ? p->target<_type>() : nullptr;
}
//@}
/*!
\throw bad_any_cast 当 <tt>x.type()
	!= ystdex::type_id<remove_reference_t<_type>>()</tt> 。
\note LWG 2509 已被 LWG 2769 的解覆盖。
\see LWG 2768 。
\see LWG 2769 。
*/
//@{
template<typename _type>
YB_ATTR_nodiscard YB_PURE _type
any_cast(any& x)
{
	using target_t = remove_cvref_t<_type>;
	static_assert(is_constructible<_type, target_t&>(),
		"Invalid cast destination type found.");

	if(const auto p = x.template target<target_t>())
		return static_cast<_type>(*p);
	throw bad_any_cast(x.type(), ystdex::type_id<_type>());
}
template<typename _type>
YB_ATTR_nodiscard YB_PURE _type
any_cast(const any& x)
{
	using target_t = remove_cvref_t<_type>;
	static_assert(is_constructible<_type, const target_t&>(),
		"Invalid cast destination type found.");

	if(const auto p = x.template target<target_t>())
		return static_cast<_type>(*p);
	throw bad_any_cast(x.type(), ystdex::type_id<_type>());
}
//! \since build 671
template<typename _type>
YB_ATTR_nodiscard YB_PURE _type
any_cast(any&& x)
{
	using target_t = remove_cvref_t<_type>;
	static_assert(is_constructible<_type, target_t>(),
		"Invalid cast destination type found.");

	if(const auto p = x.template target<target_t>())
		return static_cast<_type>(std::move(*p));
	throw bad_any_cast(x.type(), ystdex::type_id<_type>());
}
//@}
//@}
//@}

//! \ingroup YBase_replacement_extensions
//@{
/*!
\brief 判断是否持有相同对象。
\since build 748
*/
YB_ATTR_nodiscard YB_PURE inline bool
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
YB_ATTR_nodiscard YB_PURE inline _type*
unchecked_any_cast(any* p) ynothrowv
{
	yconstraint(p && p->has_value()
		&& p->unchecked_type() == ystdex::type_id<_type>());
	return static_cast<_type*>(p->unchecked_get());
}

/*!
\pre 断言： <tt>p && p->has_value()
	&& p->unchecked_type() == ystdex::type_id<_type>()</tt> 。
*/
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline const _type*
unchecked_any_cast(const any* p) ynothrowv
{
	yconstraint(p && p->has_value()
		&& p->unchecked_type() == ystdex::type_id<_type>());
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
YB_ATTR_nodiscard YB_PURE inline _type*
unsafe_any_cast(any* p) ynothrowv
{
	yconstraint(p && p->type() == ystdex::type_id<_type>());
	return static_cast<_type*>(p->get());
}

//! \pre 断言： <tt>p && p->type() == ystdex::type_id<_type>()</tt> 。
template<typename _type>
YB_ATTR_nodiscard YB_PURE inline const _type*
unsafe_any_cast(const any* p) ynothrowv
{
	yconstraint(p && p->type() == ystdex::type_id<_type>());
	return static_cast<const _type*>(p->get());
}
//@}
//@}
//@}
//@}


/*!
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
	YB_ATTR_nodiscard YB_PURE
	operator _type() const
	{
		if(data.type() == ystdex::type_id<void_ref>())
			return *ystdex::unchecked_any_cast<void_ref>(&data);
		// XXX: This is pure for emulated rvalues.
		return std::move(*ystdex::unchecked_any_cast<_type>(&data));
	}
};

} // namespace ystdex;

#endif

