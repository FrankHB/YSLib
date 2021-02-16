/*
	© 2010-2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YEvent.hpp
\ingroup Core
\brief 事件回调。
\version r6102
\author FrankHB <frankhb1989@gmail.com>
\since build 560
\par 创建时间:
	2010-04-23 23:08:23 +0800
\par 修改时间:
	2021-02-06 22:40 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YEvent
*/


#ifndef YSL_INC_Core_yevt_hpp_
#define YSL_INC_Core_yevt_hpp_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YFunc // for module YSLib::Core::YFunc,
//	ystdex::is_expandable, ystdex::is_decayed, std::allocator_arg_t,
//	std::allocator_arg, ystdex::make_expanded, ystdex::default_last_value,
//	std::piecewise_construct, YSLib::forward_as_tuple;
#include <ystdex/examiner.hpp> // for ystdex::examiners::equal_examiner;
#include <ystdex/type_op.hpp> // for ystdex::exclude_self_t,
//	ystdex::exclude_self_params_t;
#include <ystdex/iterator.hpp> // for ystdex::make_transform, ystdex::get_value;
#include <ystdex/container.hpp> // for ystdex::erase_all_if;
#include <ystdex/base.h> // for ystdex::cloneable;
#include <ystdex/operators.hpp> // for ystdex::equality_comparable;
#include <ystdex/swap.hpp> // for ystdex::swap_dependent;
#include <ystdex/optional.h> // for ystdex::optional_last_value;

namespace YSLib
{

/*!
\brief 标准事件处理器模板。
\note 若使用仿函数，可以不满足 \c EqualityComparable 的接口，即
	可使用返回 \c bool 的 \c operator== ，但此类模板不检查其语义。
\note 初始化构造函数的可调用对象要求和 ystdex::expanded_function 的类似。
\since build 333
\todo 允许使用特征定制目标的相等性。
*/
//@{
template<typename>
class GHEvent;

//! \warning 非虚析构。
template<typename _tRet, typename... _tParams>
class GHEvent<_tRet(_tParams...)>
	: protected ystdex::expanded_function<_tRet(_tParams...)>,
	private ystdex::equality_comparable<GHEvent<_tRet(_tParams...)>>,
	private ystdex::equality_comparable<GHEvent<_tRet(_tParams...)>, nullptr_t>
{
public:
	using FuncType = _tRet(_tParams...);
	using BaseType = ystdex::expanded_function<FuncType>;

private:
	// XXX: Clang++ 7.1 behaves weirdly with %lref<GHEvent> when using combined
	//	traits by %ystdex::not_ and %ystdex::and_. It seems related to https://bugs.llvm.org/show_bug.cgi?id=38033,
	//	but this does not work whether '-std=c++17' is used.
	//! \since build 850
	template<typename _fCallable>
	using enable_if_expandable_t = ystdex::enable_if_t<
		!std::is_constructible<function<FuncType>, _fCallable>::value
		&& ystdex::is_expandable<function<FuncType>, _fCallable>::value>;
	//! \brief 比较函数类型。
	using Comparer = bool(*)(const GHEvent&, const GHEvent&);
	template<typename _fCallable>
	struct GEquality
	{
		//! \since build 825
		static_assert(ystdex::is_decayed<_fCallable>(), 
			"Decayed functor expected.");

		/*!
		\pre 参数储存的对象为 Decayed 类型。
		\since build 768
		*/
		YB_ATTR_nodiscard YB_PURE static bool
		AreEqual(const GHEvent& x, const GHEvent& y)
			// TODO: Verify if it is a GCC bug. Since the resolution of CWG
			//	1330 is in the working draft N4659, it is likely to be. See also
			//	https://stackoverflow.com/questions/35790350.
#if !(YB_IMPL_GNUCPP >= 70000)
			ynoexcept_spec(ystdex::examiners::equal_examiner::are_equal(Deref(
			x.template target<_fCallable>()),
			Deref(y.template target<_fCallable>())))
#endif
		{
			return ystdex::examiners::equal_examiner::are_equal(
				Deref(x.template target<_fCallable>()),
				Deref(y.template target<_fCallable>()));
		}
	};
	//! \since build 825
	template<typename _fCallable>
	struct GEqualityExpanded
	{
		//! \since build 849
		static_assert(ystdex::is_decayed<_fCallable>(), "Invalid type found.");

		YB_ATTR_nodiscard YB_PURE static bool
		AreEqual(const GHEvent& x, const GHEvent& y) ynoexcept_spec(
			ystdex::examiners::equal_examiner::are_equal(std::declval<const
			_fCallable&>(), std::declval<const _fCallable&>()))
			// TODO: Exception specification?
		{
			// NOTE: Blocked. Use ISO C++14 deduced lambda return type (cf. CWG
			//	975) compatible to G++ attribute.
			const auto get_ref(
				[](const GHEvent& h) YB_ATTR_LAMBDA_QUAL(ynothrowv, YB_PURE){
				return std::ref(Deref(h.template target<ystdex::expanded_caller<
					FuncType, _fCallable>>()).caller);
			});

			return ystdex::examiners::equal_examiner::are_equal(
				get_ref(x).get(), get_ref(y).get());
		}
	};

	/*!
	\brief 比较函数：相等关系。
	\invariant comp_eq
	*/
	Comparer comp_eq;

public:
	/*!
	\note 对函数指针为参数的初始化允许区分重载函数并提供异常规范。
	\since build 878
	*/
	//@{
	//! \brief 构造：使用函数指针。
	GHEvent(FuncType* f = {}) ynothrow
		: BaseType(f),
		comp_eq(GEquality<ystdex::decay_t<FuncType>>::AreEqual)
	{}
	//! \brief 构造：使用分配器和函数指针。
	template<class _tAlloc>
	inline
	GHEvent(std::allocator_arg_t, const _tAlloc& a, FuncType* f = {}) ynothrow
		: BaseType(std::allocator_arg, a, f),
		comp_eq(GEquality<ystdex::decay_t<FuncType>>::AreEqual)
	{}
	//@}
	//! \since build 849
	//@{
	//! \brief 使用函数对象。
	template<class _fCallable, yimpl(typename = ystdex::exclude_self_t<GHEvent,
		_fCallable>, typename = ystdex::enable_if_t<std::is_constructible<
		function<FuncType>, _fCallable>::value>)>
	inline
	GHEvent(_fCallable f)
		: BaseType(std::move(f)),
		comp_eq(GEquality<_fCallable>::AreEqual)
	{}
	//! \brief 使用分配器和函数对象。
	template<class _fCallable, class _tAlloc,
		yimpl(typename = ystdex::exclude_self_t<GHEvent, _fCallable>,
		typename = ystdex::enable_if_t<
		std::is_constructible<function<FuncType>, _fCallable>::value>)>
	inline
	GHEvent(std::allocator_arg_t, const _tAlloc& a, _fCallable&& f)
		: BaseType(std::allocator_arg, a, yforward(f)),
		comp_eq(GEquality<_fCallable>::AreEqual)
	{}
	//! \brief 使用扩展函数对象。
	template<typename _fCallable,
		yimpl(typename = enable_if_expandable_t<_fCallable>)>
	inline
	GHEvent(_fCallable f)
		: BaseType(ystdex::make_expanded<FuncType>(std::move(f))),
		// XXX: Here lambda-expression is buggy on G++ LTO, at least G++ 7.1.0
		//	(failure of multiple definitions).
		comp_eq(GEqualityExpanded<_fCallable>::AreEqual)
	{}
	//! \brief 使用分配器和扩展函数对象。
	template<typename _fCallable, class _tAlloc,
		yimpl(typename = enable_if_expandable_t<_fCallable>)>
	inline
	GHEvent(std::allocator_arg_t, const _tAlloc& a, _fCallable f)
		: BaseType(std::allocator_arg, a,
		ystdex::make_expanded<FuncType>(std::move(f))),
		// XXX: Here lambda-expression is buggy on G++ LTO, at least G++ 7.1.0
		//	(failure of multiple definitions).
		comp_eq(GEqualityExpanded<_fCallable>::AreEqual)
	{}
	//@}
	/*!
	\brief 构造：使用对象引用和成员函数指针。
	\warning 使用空成员指针构造的函数对象调用引起未定义行为。
	\todo 支持相等构造。
	*/
	//@{
	//! \since build 413
	template<class _type>
	yconstfn
	GHEvent(_type& obj, _tRet(_type::*pm)(_tParams...))
		: GHEvent([&, pm](_tParams... args) ynoexcept(
			noexcept((obj.*pm)(yforward(args)...))
			&& std::is_nothrow_copy_constructible<_tRet>::value){
			return (obj.*pm)(yforward(args)...);
		})
	{}
	//! \since build 850
	template<class _type, class _tAlloc>
	yconstfn
	GHEvent(std::allocator_arg_t, const _tAlloc& a, _type& obj,
		_tRet(_type::*pm)(_tParams...))
		: GHEvent(std::allocator_arg, a, [&, pm](_tParams... args) ynoexcept(
			noexcept((obj.*pm)(yforward(args)...))
			&& std::is_nothrow_copy_constructible<_tRet>::value){
			return (obj.*pm)(yforward(args)...);
		})
	{}
	//@}
	//! \throw allocator_mismatch_error 分配器不兼容。
	//@{
	//! \since build 848
	template<class _tAlloc>
	inline
	GHEvent(std::allocator_arg_t, const _tAlloc& a, const GHEvent& x)
		: BaseType(std::allocator_arg, a, x),
		comp_eq(x.comp_eq)
	{}
	//! \since build 848
	template<class _tAlloc>
	inline
	GHEvent(std::allocator_arg_t, const _tAlloc& a, GHEvent&& x)
		: BaseType(std::allocator_arg, a, std::move(x)),
		comp_eq(x.comp_eq)
	{}
	//@}
	DefDeCopyMoveCtorAssignment(GHEvent)

	//! \sa Equals
	//@{
	/*!
	\brief 比较相等：参数都为空或都非空且存储的对象相等。
	\since build 520
	*/
	YB_ATTR_nodiscard YB_PURE friend
		PDefHOp(bool, ==, const GHEvent& x, const GHEvent& y)
		ImplRet(x.Equals(y))
	//! \since build 897
	//@{
	//! \brief 比较相等：等于空值。
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE friend inline bool
	operator==(const GHEvent& x, nullptr_t y)
	{
		return x.Equals(y);
	}
	//! \brief 比较相等：存储的对象值相等。
	//@{
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE friend inline bool
	operator==(const GHEvent& x, const _type& y)
	{
		return x.Equals(y);
	}
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE friend inline bool
	operator==(const _type& x, const GHEvent& y)
	{
		return y.Equals(x);
	}
	//@}

	//! \brief 比较不等：存储的对象值不等。
	//@{
	template<typename _type>
	YB_ATTR_nodiscard friend inline bool
	operator!=(const GHEvent& x, const _type& y)
	{
		return !(x == y);
	}
	template<typename _type>
	YB_ATTR_nodiscard friend inline bool
	operator!=(const _type& x, const GHEvent& y)
	{
		return !(x == y);
	}
	//@}
	//@}
	//@}

	//! \brief 调用。
	using BaseType::operator();

	//! \since build 516
	using BaseType::operator bool;

	//! \since bulid 897
	//@{
	/*!
	\brief 判断相等。

	比较参数和持有的对象。
	*/
	//@{
	YB_ATTR_nodiscard YB_PURE bool
	Equals(nullptr_t) const
	{
		return !bool(*this);
	}
	//! \sa HaveSameTargetType
	YB_ATTR_nodiscard YB_PURE bool
	Equals(const GHEvent& x) const
	{
		return HaveSameTargetType(*this, x) && EqualsUnchecked(x);
	}
	template<typename _type>
	YB_ATTR_nodiscard YB_PURE bool
	Equals(const _type& x) const
	{
		if(const auto p = this->template target<ystdex::decay_t<_type>>())
			return *p == x;
		return {};
	}

	/*!
	\pre 断言：持有对象的动态类型相同。
	\sa HaveSameTargetType
	*/
	YB_ATTR_nodiscard YB_PURE bool
	EqualsUnchecked(const GHEvent& x) const
	{
		YAssert(HaveSameTargetType(*this, x), "Invalid target found.");
		return !bool(*this) || comp_eq(*this, x);
	}
	//@}

	//! \brief 判断持有的对象具有相同的动态类型。
	YB_ATTR_nodiscard YB_PURE static bool
	HaveSameTargetType(const GHEvent& x, const GHEvent& y)
	{
#if defined(YF_DLL) || defined(YF_BUILD_DLL)
			return x.target_type() == y.target_type();
#else
			return x.comp_eq == y.comp_eq;
#endif
	}
	//@}

	//! \since build 834
	friend yconstfn_relaxed
		DefSwap(ynothrow, Comparer, ystdex::swap_dependent(_x, _y))
	friend void
	swap(GHEvent& x, GHEvent& y) ynothrow
	{
		ystdex::swap_dependent(static_cast<BaseType&>(x),
			static_cast<BaseType&>(y));
		swap(x.comp_eq, y.comp_eq);
	}

	//! \since build 773
	using BaseType::target;

	//! \since build 748
	using BaseType::target_type;
};
//@}

/*!
\relates GHEvent
\since build 520
*/
template<typename _tRet, typename... _tParams>
yconstfn bool
operator==(const GHEvent<_tRet(_tParams...)>& x, nullptr_t)
{
	return !x;
}


/*!
\brief 事件优先级。
\since build 294
*/
using EventPriority = std::uint8_t;


/*!
\brief 默认事件优先级。
\since build 294
*/
yconstexpr const EventPriority DefaultEventPriority(0x80);


//! \since build 675
//@{
//! \brief 计数调用器：调用事件处理器并计数。
struct CountedHandlerInvoker
{
	//! \return 成功调用的事件处理器个数。
	template<typename _tIn, typename... _tParams>
	size_t
	operator()(_tIn first, _tIn last, _tParams&&... args) const
	{
		size_t n(0);

		while(first != last)
		{
			(*first)(yforward(args)...);
			yunseq(++n, ++first);
		}
		return n;
	}
};


//! \brief 结果组合调用器。
template<typename _type, typename _tCombiner>
struct GCombinerInvoker
{
	static_assert(ystdex::is_decayed<_tCombiner>(), "Invalid type found.");

public:
	using result_type = _type;

private:
	_tCombiner combiner;

public:
	template<typename _tIn, typename... _tParams>
	result_type
	operator()(_tIn first, _tIn last, _tParams&&... args) const
	{
		// XXX: Blocked. 'yforward' cause G++ 5.2 crash: internal
		//	compiler error: Aborted (program cc1plus).
#if false
		const auto tr([&](_tIn iter){
			return ystdex::make_transform(iter, [&](_tIn i){
				// XXX: Blocked. 'std::forward' still cause G++ 5.2 crash:
				//	internal compiler error: in execute, at cfgexpand.c:6044.
				// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65992.
				return (*i)(std::forward<_tParams>(args)...);
			});
		});
#else
		const auto f([&](_tIn i){
			return (*i)(std::forward<_tParams>(args)...);
		});
		const auto tr([f](_tIn iter){
			return ystdex::make_transform(iter, f);
		});
#endif

		return combiner(tr(first), tr(last));
	}
};


//! \brief 默认结果组合调用器。
template<typename _type>
using GDefaultLastValueInvoker
	= GCombinerInvoker<_type, ystdex::default_last_value<_type>>;

//! \brief 可选结果组合调用器。
template<typename _type>
using GOptionalLastValueInvoker = GCombinerInvoker<ystdex::cond_t<std::is_void<
	_type>, void, ystdex::optional<_type>>, ystdex::optional_last_value<_type>>;



/*!
\brief 事件模板。
\note 支持顺序多播。
*/
//@{
template<typename, typename = CountedHandlerInvoker>
class GEvent;

/*!
\note 深复制。
\warning 非虚析构。
*/
template<typename _tRet, typename... _tParams, typename _tInvoker>
class GEvent<_tRet(_tParams...), _tInvoker>
{
public:
	//! \since build 333
	//@{
	using HandlerType = GHEvent<_tRet(_tParams...)>;
	using FuncType = typename HandlerType::FuncType;
	//@}
	/*!
	\brief 容器类型。
	\since build 294
	*/
	using ContainerType
		= multimap<EventPriority, HandlerType, std::greater<EventPriority>>;
	//! \since build 675
	using InvokerType = _tInvoker;
	//! \since build 850
	using allocator_type = typename ContainerType::allocator_type;
	//! \since build 573
	//@{
	using const_iterator = typename ContainerType::const_iterator;
	using const_reference = typename ContainerType::const_reference;
	using const_reverse_iterator
		= typename ContainerType::const_reverse_iterator;
	using iterator = typename ContainerType::iterator;
	using reference = typename ContainerType::reference;
	using reverse_iterator = typename ContainerType::reverse_iterator;
	using size_type = typename ContainerType::size_type;
	using value_type = typename ContainerType::value_type;
	//@}
	//! \since build 675
	using result_type = decltype(std::declval<_tInvoker&>()(
		std::declval<const_iterator>(), std::declval<const_iterator>(),
		std::declval<_tParams>()...));

	/*!
	\brief 调用器。
	\since build 675
	*/
	InvokerType Invoker{};

private:
	/*!
	\brief 响应列表。
	\since build 572
	*/
	ContainerType handlers{};

public:
	/*!
	\brief 无参数构造：默认实现。
	\since build 333
	*/
	DefDeCtor(GEvent)
	//! \since build 850
	GEvent(allocator_type a)
		: handlers(a)
	{}
	/*!
	\brief 构造：使用指定调用器。
	\since build 677
	*/
	GEvent(InvokerType ivk)
		: Invoker(std::move(ivk))
	{}
	//! \since build 850
	GEvent(InvokerType ivk, allocator_type a)
		: Invoker(std::move(ivk)), handlers(a)
	{}
	/*!
	\brief 构造：使用指定调用器并添加事件处理器。
	\since build 849
	*/
	template<typename _tHandler, yimpl(typename = ystdex::exclude_self_t<GEvent,
		_tHandler>, typename = ystdex::enable_if_t<
		std::is_constructible<HandlerType, _tHandler>::value>)>
	GEvent(_tHandler h, InvokerType ivk = {})
		: Invoker(std::move(ivk))
	{
		Add(std::move(h));
	}
	//! \since build 850
	//@{
	template<typename _tHandler, yimpl(typename = ystdex::exclude_self_t<GEvent,
		_tHandler>, typename = ystdex::enable_if_t<
		std::is_constructible<HandlerType, _tHandler>::value>)>
	GEvent(_tHandler h, InvokerType ivk, allocator_type a)
		: Invoker(std::move(ivk)), handlers(a)
	{
		Add(std::move(h), a);
	}
	GEvent(const GEvent& e, allocator_type a)
		: Invoker(e.Invoker), handlers(e.handlers, a)
	{}
	GEvent(GEvent&& e, allocator_type a)
		: Invoker(std::move(e.Invoker)), handlers(std::move(e.handlers), a)
	{}
	//@}
	//! \since build 333
	DefDeCopyMoveCtorAssignment(GEvent)

	/*!
	\brief 赋值：覆盖事件响应：使用单一构造参数指定的指定事件处理器。
	\since build 448
	*/
	template<typename _tHandler,
		yimpl(typename = ystdex::exclude_self_t<GEvent, _tHandler>)>
	inline GEvent&
	operator=(_tHandler&& arg)
	{
		return *this = GEvent(yforward(arg));
	}

	//! \since build 333
	//@{
	/*!
	\brief 添加事件响应：使用 const 事件处理器和优先级。
	\note 不检查是否已经在列表中。
	*/
	inline PDefHOp(GEvent&, +=, const HandlerType& h)
		ImplRet(Add(h))
	/*!
	\brief 添加事件响应：使用事件处理器。
	\note 不检查是否已经在列表中。
	*/
	inline PDefHOp(GEvent&, +=, HandlerType&& h)
		ImplRet(Add(std::move(h)))
	/*!
	\brief 添加事件响应：目标为单一构造参数指定的指定事件处理器。
	\note 不检查是否已经在列表中。
	\since build 897
	*/
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<HandlerType, _type>)>
	inline GEvent&
	operator+=(_type&& arg)
	{
		return Add(HandlerType(yforward(arg)));
	}

	//! \brief 移除事件响应：指定 const 事件处理器。
	GEvent&
	operator-=(const HandlerType& h)
	{
		ystdex::erase_all_if<ContainerType>(handlers, handlers.cbegin(),
			handlers.cend(), [&](decltype(*handlers.cbegin()) pr){
			return pr.second == h;
		});
		return *this;
	}
	/*!
	\brief 移除事件响应：指定非 const 事件处理器。
	\note 防止模板 <tt>operator-=</tt> 递归。
	*/
	inline PDefHOp(GEvent&, -=, HandlerType&& h)
		ImplRet(*this -= static_cast<const HandlerType&>(h))
	/*!
	\brief 移除事件响应：目标为单一构造参数指定的指定事件处理器。
	\since build 897
	*/
	template<typename _type,
		yimpl(typename = ystdex::exclude_self_t<HandlerType, _type>)>
	inline GEvent&
	operator-=(_type&& arg)
	{
		return *this -= HandlerType(yforward(arg));
	}
	//@}

	/*!
	\brief 插入事件响应。
	\note 不检查是否已经在列表中。
	\sa Insert
	\since build 294
	*/
	//@{
	//! \note 使用单一构造参数指定的事件处理器和优先级。
	template<typename _type>
	inline GEvent&
	Add(_type&& arg, EventPriority prior = DefaultEventPriority)
	{
		Insert(yforward(arg), prior);
		return *this;
	}
	/*!
	\note 使用对象引用、成员函数指针和优先级。
	\since build 413
	*/
	template<class _tObj, class _type>
	inline GEvent&
	Add(_tObj& obj, _tRet(_type::*pm)(_tParams...),
		EventPriority prior = DefaultEventPriority)
	{
		Insert(obj, pm, prior);
		return *this;
	}
	//@}

	//! \since build 897
	//@{
	PDefH(typename ContainerType::iterator, Emplace, EventPriority prior,
		const HandlerType& h)
		ImplRet(handlers.emplace(std::piecewise_construct,
			YSLib::forward_as_tuple(prior), YSLib::forward_as_tuple(h)))
	PDefH(typename ContainerType::iterator, Emplace, EventPriority prior,
		HandlerType&& h)
		ImplRet(handlers.emplace(std::piecewise_construct,
			YSLib::forward_as_tuple(prior),
			YSLib::forward_as_tuple(std::move(h))))
	template<typename... _tEmplaceParams, yimpl(typename
		= ystdex::exclude_self_params_t<HandlerType, _tEmplaceParams...>)>
	inline typename ContainerType::iterator
	Emplace(EventPriority prior, _tEmplaceParams&&... args)
	{
		return handlers.emplace(std::piecewise_construct,
			YSLib::forward_as_tuple(prior), YSLib::forward_as_tuple(
			std::allocator_arg, handlers.get_allocator(), yforward(args)...));
	}
	//@}

	/*!
	\brief 插入事件响应。
	\note 不检查是否已经在列表中。
	\since build 572
	*/
	//@{
	//! \note 使用单一构造参数指定的事件处理器和优先级。
	template<typename _type>
	inline typename ContainerType::iterator
	Insert(_type&& arg, EventPriority prior = DefaultEventPriority)
	{
		return Emplace(prior, yforward(arg));
	}
	//! \note 使用对象引用、成员函数指针和优先级。
	template<class _tObj, class _type>
	inline typename ContainerType::iterator
	Insert(_tObj& obj, _tRet(_type::*pm)(_tParams...),
		EventPriority prior = DefaultEventPriority)
	{
		return Emplace(prior, obj, pm);
	}
	//@}

	/*!
	\brief 移除事件响应：目标为指定对象引用和成员函数指针。
	\since build 413
	*/
	template<class _tObj, class _type>
	inline GEvent&
	Remove(_tObj& obj, _tRet(_type::*pm)(_tParams...))
	{
		return *this -= HandlerType(static_cast<_type&>(obj), std::move(pm));
	}

	/*!
	\brief 判断是否包含指定事件响应。
	\since build 333
	*/
	bool
	Contains(const HandlerType& h) const
	{
		using ystdex::get_value;

		return std::count(handlers.cbegin() | get_value,
			handlers.cend() | get_value, h) != 0;
	}
	/*!
	\brief 判断是否包含单一构造参数指定的事件响应。
	\since build 293
	*/
	template<typename _type>
	inline bool
	Contains(_type&& arg) const
	{
		return Contains(HandlerType(yforward(arg)));
	}

	/*!
	\brief 调用：传递参数到调用器。
	\since build 675
	*/
	PDefHOp(result_type, (), _tParams... args) const
		ImplRet(Invoker(handlers.cbegin() | ystdex::get_value,
			handlers.cend() | ystdex::get_value, yforward(args)...))

	//! \since build 573
	YB_ATTR_nodiscard YB_PURE PDefH(const_iterator, cbegin, ) const ynothrow
		ImplRet(handlers.cbegin())

	//! \since build 572
	//@{
	YB_ATTR_nodiscard YB_PURE PDefH(iterator, begin, ) ynothrow
		ImplRet(handlers.begin())
	YB_ATTR_nodiscard YB_PURE PDefH(iterator, begin, ) const ynothrow
		ImplRet(handlers.begin())

	//! \since build 573
	YB_ATTR_nodiscard YB_PURE PDefH(const_iterator, cend, ) const ynothrow
		ImplRet(handlers.cend())

	//! \brief 清除：移除所有事件响应。
	PDefH(void, clear, ) ynothrow
		ImplRet(handlers.clear())

	//! \since build 573
	//@{
	YB_ATTR_nodiscard YB_PURE PDefH(size_type, count, EventPriority prior) const
		ynothrow
		ImplRet(handlers.count(prior))

	YB_ATTR_nodiscard YB_PURE
		PDefH(const_reverse_iterator, crbegin, ) const ynothrow
		ImplRet(handlers.crbegin())

	YB_ATTR_nodiscard YB_PURE
		PDefH(const_reverse_iterator, crend, ) const ynothrow
		ImplRet(handlers.crend())
	//@}

	YB_ATTR_nodiscard PDefH(bool, empty, ) const ynothrow
		ImplRet(handlers.empty())

	YB_ATTR_nodiscard YB_PURE PDefH(iterator, end, ) ynothrow
		ImplRet(handlers.end())
	YB_ATTR_nodiscard YB_PURE PDefH(iterator, end, ) const ynothrow
		ImplRet(handlers.end())
	//@}

	//! \since build 862
	YB_ATTR_nodiscard YB_PURE
		PDefH(allocator_type, get_allocator, ) const ynothrow
		ImplRet(handlers.get_allocator())

	//! \since build 573
	//@{
	YB_ATTR_nodiscard YB_PURE PDefH(reverse_iterator, rbegin, ) ynothrow
		ImplRet(handlers.rbegin())

	YB_ATTR_nodiscard YB_PURE PDefH(reverse_iterator, rend, ) ynothrow
		ImplRet(handlers.rend())

	//! \brief 取列表中的响应数。
	YB_ATTR_nodiscard YB_PURE PDefH(size_type, size, ) const ynothrow
		ImplRet(handlers.size())
	//@}

	/*!
	\brief 交换。
	\since build 710
	*/
	friend PDefH(void, swap, GEvent& x, GEvent& y) ynothrow
		ImplRet(x.handlers.swap(y.handlers))
};
//@}
//@}

/*!
\brief 添加单一事件响应：删除后添加。
\since build 333
*/
//@{
template<typename _tRet, typename... _tParams>
inline GEvent<_tRet(_tParams...)>&
AddUnique(GEvent<_tRet(_tParams...)>& evt,
	const typename GEvent<_tRet(_tParams...)>::HandlerType& h,
	EventPriority prior = DefaultEventPriority)
{
	return (evt -= h).Add(h, prior);
}
template<typename _tRet, typename... _tParams>
inline GEvent<_tRet(_tParams...)>&
AddUnique(GEvent<_tRet(_tParams...)>& evt, typename GEvent<_tRet(_tParams...)>
	::HandlerType&& h, EventPriority prior = DefaultEventPriority)
{
	return (evt -= h).Add(std::move(h), prior);
}
template<typename _type, typename _tRet, typename... _tParams>
inline GEvent<_tRet(_tParams...)>&
AddUnique(GEvent<_tRet(_tParams...)>& evt, _type&& arg,
	EventPriority prior = DefaultEventPriority)
{
	return AddUnique(evt, HandlerType(yforward(arg)), prior);
}
//! \since build 413
template<class _type, typename _tRet, typename... _tParams>
inline GEvent<_tRet(_tParams...)>&
AddUnique(GEvent<_tRet(_tParams...)>& evt, _type& obj,
	_tRet(_type::*pm)(_tParams...), EventPriority prior = DefaultEventPriority)
{
	return AddUnique(evt, HandlerType(static_cast<_type&>(obj), std::move(pm)),
		prior);
}
//@}


/*!
\brief 定义扩展事件映射类。
\since build 240
*/
#define DefExtendEventMap(_n, _b) \
	DefExtendClass(YF_API, _n, public _b)

} // namespace YSLib;

#endif

