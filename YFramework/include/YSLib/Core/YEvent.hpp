/*
	© 2010-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YEvent.hpp
\ingroup Core
\brief 事件回调。
\version r5301
\author FrankHB <frankhb1989@gmail.com>
\since build 560
\par 创建时间:
	2010-04-23 23:08:23 +0800
\par 修改时间:
	2017-06-04 23:55 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YEvent
*/


#ifndef YSL_INC_Core_yevt_hpp_
#define YSL_INC_Core_yevt_hpp_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YObject // for ystdex::examiners::equal_examiner;
#include YFM_YSLib_Core_YFunc
#include <ystdex/iterator.hpp> // for ystdex::get_value;
#include <ystdex/container.hpp> // for ystdex::erase_all_if;
#include <ystdex/base.h> // for ystdex::cloneable;
#include <ystdex/operators.hpp> // for ystdex::equality_comparable;
#include <ystdex/functional.hpp> // for ystdex::make_expanded,
//	ystdex::default_last_value;
#include <ystdex/optional.h> // for ystdex::optional_last_value;

namespace YSLib
{

/*!
\brief 事件处理器接口模板。
\since build 333
*/
template<typename... _tParams>
DeclDerivedI(, GIHEvent, ystdex::cloneable)
	DeclIEntry(size_t operator()(_tParams...) const)
	//! \since build 409
	DeclIEntry(GIHEvent* clone() const ImplI(ystdex::cloneable))
EndDecl

template<typename... _tParams>
GIHEvent<_tParams...>::DefDeDtor(GIHEvent)


/*!
\brief 标准事件处理器模板。
\note 若使用仿函数，可以不满足 \c EqualityComparable 的接口，即
	可使用返回 \c bool 的 \c operator== ，但此模板类无法检查其语义正确性。
\since build 333
*/
//@{
template<typename>
class GHEvent;

//! \warning 非虚析构。
template<typename _tRet, typename... _tParams>
class GHEvent<_tRet(_tParams...)>
	: protected std::function<_tRet(_tParams...)>,
	private ystdex::equality_comparable<GHEvent<_tRet(_tParams...)>>,
	private ystdex::equality_comparable<GHEvent<_tRet(_tParams...)>, nullptr_t>
{
public:
	using TupleType = tuple<_tParams...>;
	using FuncType = _tRet(_tParams...);
	using BaseType = std::function<FuncType>;

private:
	//! \brief 比较函数类型。
	using Comparer = bool(*)(const GHEvent&, const GHEvent&);
	template<class _tFunctor>
	struct GEquality
	{
		//! \since build 748
		using Decayed = ystdex::decay_t<_tFunctor>;

		/*!
		\pre 参数储存的对象为 Decayed 类型。
		\since build 768
		*/
		static bool
		AreEqual(const GHEvent& x, const GHEvent& y)
			// TODO: Verify if it is a GCC bug. Since the resolution of CWG
			//	1330 is in the working draft M4659, it likes to be. See also
			//	https://stackoverflow.com/questions/35790350/noexcept-inheriting-constructors-and-the-invalid-use-of-an-incomplete-type-that.
#if !(YB_IMPL_GNUC >= 70000)
			ynoexcept_spec(
			ystdex::examiners::equal_examiner::are_equal(Deref(
			x.template target<Decayed>()), Deref(y.template target<Decayed>())))
#endif
		{
			return ystdex::examiners::equal_examiner::are_equal(
				Deref(x.template target<Decayed>()),
				Deref(y.template target<Decayed>()));
		}
	};

	/*!
	\brief 比较函数：相等关系。
	\invariant comp_eq
	*/
	Comparer comp_eq;

public:
	/*!
	\brief 构造：使用函数指针。
	\since build 516
	*/
	yconstfn
	GHEvent(FuncType* f = {})
		: BaseType(f), comp_eq(GEquality<FuncType>::AreEqual)
	{}
	/*!
	\brief 使用函数对象。
	\since build 494
	*/
	template<class _fCallable>
	yconstfn
	GHEvent(_fCallable f, ystdex::enable_if_t<
		std::is_constructible<BaseType, _fCallable>::value, int> = 0)
		: BaseType(f),
		comp_eq(GEquality<ystdex::decay_t<_fCallable>>::AreEqual)
	{}
	/*!
	\brief 使用扩展函数对象。
	\since build 447
	\todo 推断比较相等操作。
	*/
	template<class _fCallable>
	yconstfn
	GHEvent(_fCallable&& f, ystdex::enable_if_t<
		!std::is_constructible<BaseType, _fCallable>::value, int> = 0)
		: BaseType(ystdex::make_expanded<_tRet(_tParams...)>(yforward(f))),
		comp_eq([](const GHEvent&, const GHEvent&) ynothrow{
			return true;
		})
	{}
	/*!
	\brief 构造：使用对象引用和成员函数指针。
	\warning 使用空成员指针构造的函数对象调用引起未定义行为。
	\since build 413
	*/
	template<class _type>
	yconstfn
	GHEvent(_type& obj, _tRet(_type::*pm)(_tParams...))
		: GHEvent([&, pm](_tParams... args) ynoexcept(
			noexcept((obj.*pm)(yforward(args)...))
			&& std::is_nothrow_copy_constructible<_tRet>::value){
			return (obj.*pm)(yforward(args)...);
		})
	{}
	DefDeCopyMoveCtorAssignment(GHEvent)

	//! \since build 520
	friend yconstfn bool
	operator==(const GHEvent& x, const GHEvent& y)
	{
		return
#if defined(YF_DLL) || defined(YF_BUILD_DLL)
			x.target_type() == y.target_type()
#else
			x.comp_eq == y.comp_eq
#endif
			&& (!bool(x) || x.comp_eq(x, y));
	}

	//! \brief 调用。
	using BaseType::operator();

	//! \since build 516
	using BaseType::operator bool;

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
	/*!
	\exception std::bad_function_call 以外异常中立。
	\return 成功调用的事件处理器个数。
	*/
	template<typename _tIn, typename... _tParams>
	size_t
	operator()(_tIn first, _tIn last, _tParams&&... args) const
	{
		size_t n(0);

		while(first != last)
		{
			TryExpr((*first)(yforward(args)...))
			CatchIgnore(std::bad_function_call&)
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
	using TupleType = typename HandlerType::TupleType;
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
	\note 得到空实例。
	\since build 333
	*/
	yconstfn DefDeCtor(GEvent)
	/*!
	\brief 构造：使用指定调用器。
	\since build 677
	*/
	GEvent(InvokerType ivk)
		: Invoker(std::move(ivk))
	{}
	/*!
	\brief 构造：使用指定调用器并添加事件处理器。
	\since build 598
	*/
	template<typename _tHandler,
		yimpl(typename = ystdex::exclude_self_t<GEvent, _tHandler>)>
	GEvent(_tHandler&& h, InvokerType ivk = {})
		: Invoker(std::move(ivk))
	{
		Add(yforward(h));
	}
	//! \since build 333
	DefDeCopyMoveCtorAssignment(GEvent)

	/*!
	\brief 赋值：覆盖事件响应：使用单一构造参数指定的指定事件处理器。
	\since build 448
	*/
	template<typename _tHandler,
		yimpl(typename = ystdex::exclude_self_t<GEvent, _tHandler>)>
	inline GEvent&
	operator=(_tHandler&& _arg)
	{
		return *this = GEvent(yforward(_arg));
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
	*/
	template<typename _type>
	inline GEvent&
	operator+=(_type&& _arg)
	{
		return Add(HandlerType(yforward(_arg)));
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
	\since build 293
	*/
	template<typename _type>
	inline GEvent&
	operator-=(_type&& _arg)
	{
		return *this -= HandlerType(yforward(_arg));
	}
	//@}

	/*!
	\brief 插入事件响应。
	\note 不检查是否已经在列表中。
	\sa Insert
	\since build 294
	*/
	//@{
	//! \note 使用 const 事件处理器和优先级。
	inline PDefH(GEvent&, Add, const HandlerType& h,
		EventPriority prior = DefaultEventPriority)
		ImplRet(Insert(h, prior), *this)
	//! \note 使用非 const 事件处理器和优先级。
	inline PDefH(GEvent&, Add, HandlerType&& h,
		EventPriority prior = DefaultEventPriority)
		ImplRet(Insert(std::move(h), prior), *this)
	//! \note 使用单一构造参数指定的事件处理器和优先级。
	template<typename _type>
	inline GEvent&
	Add(_type&& _arg, EventPriority prior = DefaultEventPriority)
	{
		return Add(HandlerType(yforward(_arg)), prior);
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
		return Add(HandlerType(static_cast<_type&>(obj), std::move(pm)), prior);
	}
	//@}

	/*!
	\brief 插入事件响应。
	\note 不检查是否已经在列表中。
	\since build 572
	*/
	//@{
	//! \note 使用 const 事件处理器和优先级。
	inline PDefH(typename ContainerType::iterator, Insert, const HandlerType& h,
		EventPriority prior = DefaultEventPriority)
		ImplRet(handlers.emplace(prior, h))
	//! \note 使用非 const 事件处理器和优先级。
	inline PDefH(typename ContainerType::iterator, Insert, HandlerType&& h,
		EventPriority prior = DefaultEventPriority)
		ImplRet(handlers.emplace(prior, std::move(h)))
	//! \note 使用单一构造参数指定的事件处理器和优先级。
	template<typename _type>
	inline typename ContainerType::iterator
	Insert(_type&& _arg, EventPriority prior = DefaultEventPriority)
	{
		return Insert(HandlerType(yforward(_arg)), prior);
	}
	//! \note 使用对象引用、成员函数指针和优先级。
	template<class _tObj, class _type>
	inline typename ContainerType::iterator
	Insert(_tObj& obj, _tRet(_type::*pm)(_tParams...),
		EventPriority prior = DefaultEventPriority)
	{
		return
			Insert(HandlerType(static_cast<_type&>(obj), std::move(pm)), prior);
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
	Contains(_type&& _arg) const
	{
		return Contains(HandlerType(yforward(_arg)));
	}

	/*!
	\brief 调用：传递参数到调用器。
	\since build 675
	*/
	PDefHOp(result_type, (), _tParams... args) const
		ImplRet(Invoker(handlers.cbegin() | ystdex::get_value,
			handlers.cend() | ystdex::get_value, yforward(args)...))

	//! \since build 573
	PDefH(const_iterator, cbegin, ) const ynothrow
		ImplRet(handlers.cbegin())

	//! \since build 572
	//@{
	PDefH(iterator, begin, ) ynothrow
		ImplRet(handlers.begin())
	PDefH(iterator, begin, ) const ynothrow
		ImplRet(handlers.begin())

	//! \since build 573
	PDefH(const_iterator, cend, ) const ynothrow
		ImplRet(handlers.cend())

	//! \brief 清除：移除所有事件响应。
	PDefH(void, clear, ) ynothrow
		ImplRet(handlers.clear())

	//! \since build 573
	//@{
	PDefH(size_type, count, EventPriority prior) const ynothrow
		ImplRet(handlers.count(prior))

	PDefH(const_reverse_iterator, crbegin, ) const ynothrow
		ImplRet(handlers.crbegin())

	PDefH(const_reverse_iterator, crend, ) const ynothrow
		ImplRet(handlers.crend())
	//@}

	PDefH(bool, empty, ) const ynothrow
		ImplRet(handlers.empty())

	PDefH(iterator, end, ) ynothrow
		ImplRet(handlers.end())
	PDefH(iterator, end, ) const ynothrow
		ImplRet(handlers.end())
	//@}

	//! \since build 573
	//@{
	PDefH(reverse_iterator, rbegin, ) ynothrow
		ImplRet(handlers.rbegin())

	PDefH(reverse_iterator, rend, ) ynothrow
		ImplRet(handlers.rend())

	//! \brief 取列表中的响应数。
	PDefH(size_type, size, ) const ynothrow
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
\brief 使用 RAII 管理的事件辅助类。
\warning 非虚析构。
\since build 429
*/
template<typename... _tEventArgs>
class GEventGuard
{
public:
	using EventType = GEvent<_tEventArgs...>;
	using HandlerType = GHEvent<_tEventArgs...>;
	//! \since build 554
	lref<EventType> Event;
	HandlerType Handler;

	template<typename _type>
	GEventGuard(EventType& evt, _type&& handler,
		EventPriority prior = DefaultEventPriority)
		: Event(evt), Handler(yforward(handler))
	{
		Event.get().Add(Handler, prior);
	}
	~GEventGuard()
	{
		Event.get() -= Handler;
	}
};


/*!
\brief 依赖事件项模板。
\warning 非虚析构。
\since build 195
*/
template<class _tEvent, class _tOwnerPointer = shared_ptr<_tEvent>>
class GDependencyEvent : public GDependency<_tEvent, _tOwnerPointer>
{
public:
	using DependentType = typename GDependency<_tEvent>::DependentType;
	using PointerType = typename GDependency<_tEvent>::PointerType;
	using ConstReferenceType
		= typename GDependency<_tEvent>::ConstReferenceType;
	using ReferentType = typename GDependency<_tEvent>::ReferentType;
	using ReferenceType = typename GDependency<_tEvent>::ReferenceType;
	using EventType = DependentType;
	using SEventType = typename EventType::SEventType;
	using FuncType = typename EventType::FuncType;
	using HandlerType = typename EventType::HandlerType;
	using SizeType = typename EventType::SizeType;

	GDependencyEvent(PointerType p = PointerType())
		: GDependency<_tEvent>(p)
	{}

	/*!
	\brief 添加事件响应。
	*/
	template<typename _type>
	inline ReferenceType
	operator+=(_type _arg)
	{
		return this->GetNewRef().operator+=(_arg);
	}

	/*!
	\brief 移除事件响应。
	*/
	template<typename _type>
	inline ReferenceType
	operator-=(_type _arg)
	{
		return this->GetNewRef().operator-=(_arg);
	}

	/*!
	\brief 添加事件响应：使用对象引用和成员函数指针。
	\since build 413
	*/
	template<class _type, typename _tRet, typename... _tParams>
	inline ReferenceType
	Add(_type& obj, _tRet(_type::*pm)(_tParams...))
	{
		return this->GetNewRef().Add(obj, pm);
	}

	/*!
	\brief 移除事件响应：目标为指定对象引用和成员函数指针。
	\since build 413
	*/
	template<class _type, typename _tRet, typename... _tParams>
	inline ReferenceType
	Remove(_type& obj, _tRet(_type::*pm)(_tParams...))
	{
		return this->GetNewRef().Remove(obj, pm);
	}

	/*!
	\brief 调用函数。
	\since build 413
	*/
	template<typename... _tParams>
	inline SizeType
	operator()(_tParams&&... args) const
	{
		return this->GetRef().operator()(yforward(args)...);
	}

	/*!
	\brief 取列表中的响应数。
	*/
	DefGetterMem(const ynothrow, SizeType, Size, this->GetRef())

	/*!
	\brief 清除：移除所有事件响应。
	\since build 530
	*/
	PDefH(void, clear, )
		ImplExpr(this->GetNewRef().clear())
};


//! \since build 413
template<typename... _tParams>
struct EventArgsHead
{
	using type = ystdex::conditional_t<sizeof...(_tParams) == 0, void,
		ystdex::tuple_element_t<0, tuple<_tParams...>>>;
};

template<typename... _tParams>
struct EventArgsHead<tuple<_tParams...>> : EventArgsHead<_tParams...>
{};


/*!
\brief 事件处理器适配器模板。
\warning 非虚析构。
\since build 494
*/
//@{
template<typename _type, typename _func = std::function<void(_type&)>>
class GHandlerAdaptor : private GHandlerAdaptor<void, _func>
{
private:
	//! \since build 537
	using Base = GHandlerAdaptor<void, _func>;

public:
	//! \since build 537
	using typename Base::CallerType;

	//! \since build 537
	using Base::Caller;
	//! \since build 554
	lref<_type> ObjectRef;

	GHandlerAdaptor(_type& obj, CallerType f)
		: Base(f), ObjectRef(obj)
	{}
	template<typename _fCaller>
	GHandlerAdaptor(_type& obj, _fCaller&& f)
		: Base(ystdex::make_expanded<CallerType>(yforward(f))), ObjectRef(obj)
	{}
	//! \since build 537
	//@{
	DefDeCopyMoveCtorAssignment(GHandlerAdaptor)

	using Base::operator();
	//@}

	/*!
	\since build 673
	\todo 实现比较 Function 相等。
	*/
	friend PDefHOp(bool, ==, const GHandlerAdaptor& x, const GHandlerAdaptor& y)
		ynothrow
		ImplRet(std::addressof(x.ObjectRef.get())
			== std::addressof(y.ObjectRef.get()))
};

//! \since build 537
template<typename _func>
class GHandlerAdaptor<void, _func>
{
public:
	using CallerType = ystdex::decay_t<_func>;

	CallerType Caller;

	GHandlerAdaptor(CallerType f)
		: Caller(f)
	{}
	template<typename _fCaller, yimpl(
		typename = ystdex::exclude_self_t<GHandlerAdaptor, _fCaller>)>
	GHandlerAdaptor(_fCaller&& f)
		: Caller(ystdex::make_expanded<CallerType>(yforward(f)))
	{}
	DefDeCopyCtor(GHandlerAdaptor)
	DefDeMoveCtor(GHandlerAdaptor)

	DefDeCopyAssignment(GHandlerAdaptor)
	DefDeMoveAssignment(GHandlerAdaptor)

	//! \todo 使用 <tt>noexpcept</tt> 。
	template<typename... _tParams>
	void
	operator()(_tParams&&... args) const
	{
		TryExpr(Caller(yforward(args)...))
		CatchIgnore(std::bad_function_call&)
	}

	/*!
	\since build 673
	\todo 实现比较 Function 相等。
	*/
	friend PDefHOp(bool, ==, const GHandlerAdaptor& x, const GHandlerAdaptor& y)
		ynothrow
		ImplRet(&x == &y)
};
//@}


/*!
\brief 事件包装类模板。
\since build 173
*/
template<class _tEvent, typename _tBaseArgs>
class GEventWrapper : public _tEvent, implements GIHEvent<_tBaseArgs>
{
public:
	using EventType = _tEvent;
	using BaseArgsType = _tBaseArgs;
	using EventArgsType = _t<EventArgsHead<typename _tEvent::TupleType>>;

	/*!
	\brief 委托调用。
	\warning 需要确保 BaseArgsType 引用的对象能够转换至 EventArgsType 。
	\since build 331
	*/
	size_t
	operator()(BaseArgsType e) const ImplI(GIHEvent<_tBaseArgs>)
	{
		return EventType::operator()(EventArgsType(yforward(e)));
	}

	//! \since build 409
	DefClone(const ImplI(GIHEvent<_tBaseArgs>), GEventWrapper)
};


/*!
\brief 事件项类型。
\warning 非虚析构。
\since build 242
*/
template<typename _tBaseArgs>
class GEventPointerWrapper
{
public:
	using ItemType = GIHEvent<_tBaseArgs>;
	using PointerType = unique_ptr<ItemType>;

private:
	PointerType ptr;

public:
	//! \since build 586
	template<typename _type, yimpl(
		typename = ystdex::exclude_self_t<GEventPointerWrapper, _type>)>
	inline
	GEventPointerWrapper(_type&& p)
		ynoexcept(std::is_nothrow_constructible<PointerType, _type>())
		: ptr(Nonnull(p))
	{}
	/*!
	\brief 复制构造：深复制。
	*/
	GEventPointerWrapper(const GEventPointerWrapper& item)
		: ptr(ystdex::clone_polymorphic(Deref(item.ptr)))
	{}
	DefDeMoveCtor(GEventPointerWrapper)

	yconstfn DefCvt(const ynothrow, const ItemType&, *ptr)
	yconstfn DefCvt(const ynothrow, ItemType&, *ptr)
};


/*!
\brief 定义扩展事件映射类。
\since build 240
*/
#define DefExtendEventMap(_n, _b) \
	DefExtendClass(YF_API, _n, public _b)

} // namespace YSLib;

#endif

