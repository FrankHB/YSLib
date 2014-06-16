/*
	© 2010-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yevt.hpp
\ingroup Core
\brief 事件回调。
\version r4612
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-04-23 23:08:23 +0800
\par 修改时间:
	2014-04-25 10:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YEvent
*/


#ifndef YSL_INC_Core_yevt_hpp_
#define YSL_INC_Core_yevt_hpp_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YObject
#include YFM_YSLib_Core_YFunc
#include <ystdex/iterator.hpp> // for ystdex::get_value;
#include <ystdex/container.hpp> // for ystdex::erase_all_if;
#include <ystdex/utility.hpp> // for ystdex::cloneable;

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
	DeclIEntry(GIHEvent* clone() const override)
EndDecl


/*!
\brief 标准事件处理器类模板。
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
	: protected std::function<_tRet(_tParams...)>
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
		//! \since build 319
		//@{
		using decayed_type = ystdex::decay_t<_tFunctor>;

#if YB_HAS_NOEXCEPT
		/*!
		\brief 判断使用 noexcept 并避免 constexpr 失败。
		\since build 468
		*/
		static yconstexpr bool is_noexcept_v = noexcept(std::declval<
			const decayed_type>() == std::declval<const decayed_type>());
#endif

		static bool
		AreEqual(const GHEvent& x, const GHEvent& y) ynoexcept(is_noexcept_v)
		{
			if(const auto p = x.template target<decayed_type>())
				if(const auto q = y.template target<decayed_type>())
					return p == q || *p == *q;
			return false;
		}
		//@}
	};

	Comparer comp_eq; //!< 比较函数：相等关系。

public:
	/*!
	\brief 构造：使用函数指针。
	\since build 351
	*/
	yconstfn
	GHEvent(FuncType* f)
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
		: BaseType(f), comp_eq(GetComparer(f, f))
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
		comp_eq(GHEvent::AreAlwaysEqual)
	{}
	/*!
	\brief 构造：使用对象引用和成员函数指针。
	\warning 使用空成员指针构造的函数对象调用引起未定义行为。
	\since build 413
	*/
	template<class _type>
	yconstfn
	GHEvent(_type& obj, _tRet(_type::*pm)(_tParams...))
		: GHEvent([&, pm](_tParams... args){
			return (obj.*pm)(yforward(args)...);
		})
	{}
	yconstfn DefDeCopyCtor(GHEvent)
	yconstfn DefDeMoveCtor(GHEvent)

	DefDeCopyAssignment(GHEvent)
	/*!
	\brief 转移赋值：默认实现。
	*/
	DefDeMoveAssignment(GHEvent)

	yconstfn bool
	operator==(const GHEvent& h) const
	{
		return
#if defined(YF_DLL) || defined(YF_BUILD_DLL)
			BaseType::target_type() == h.BaseType::target_type()
#else
			comp_eq == h.comp_eq
#endif
			&& (comp_eq(*this, h));
	}

	/*!
	\brief 调用。
	*/
	using BaseType::operator();

private:
	//! \since build 319
	//@{
	template<typename _type>
	static yconstfn Comparer
	GetComparer(_type& x, _type& y, decltype(x == y) = {}) ynothrow
	{
		return GEquality<_type>::AreEqual;
	}
	template<typename _type, typename _tUnused>
	static yconstfn Comparer
	GetComparer(_type&, _tUnused&) ynothrow
	{
		return GHEvent::AreAlwaysEqual;
	}

	static yconstfn bool
	AreAlwaysEqual(const GHEvent&, const GHEvent&) ynothrow
	{
		return true;
	}
	//@}
};
//@}


/*!
\brief 事件优先级。
\since build 294
*/
using EventPriority = u8;


/*!
\brief 默认事件优先级。
\since build 294
*/
yconstexpr EventPriority DefaultEventPriority(0x80);


/*!
\brief 事件类模板。
\note 支持顺序多播。
\since build 333
*/
//@{
template<typename>
class GEvent;

//! \warning 非虚析构。
template<typename _tRet, typename... _tParams>
class GEvent<_tRet(_tParams...)>
{
public:
	using HandlerType = GHEvent<_tRet(_tParams...)>;
	using TupleType = typename HandlerType::TupleType;
	using FuncType = typename HandlerType::FuncType;
	/*!
	\brief 容器类型。
	\since build 294
	*/
	using ContainerType
		= multimap<EventPriority, HandlerType, std::greater<EventPriority>>;
	using SizeType = typename ContainerType::size_type;

	/*!
	\brief 响应列表。
	\since build 294
	*/
	ContainerType List;

	/*!
	\brief 无参数构造：默认实现。
	\note 得到空实例。
	*/
	yconstfn DefDeCtor(GEvent)
	/*!
	\brief 构造：添加事件处理器。
	\since build 448
	*/
	template<typename _tHandler,
		yimpl(typename = ystdex::exclude_self_ctor_t<GEvent, _tHandler>)>
	GEvent(_tHandler&& h)
		: List()
	{
		Add(yforward(h));
	}
	/*!
	\brief 复制构造：默认实现。
	\note 深复制。
	*/
	yconstfn DefDeCopyCtor(GEvent)
	/*!
	\brief 转移构造：默认实现。
	*/
	yconstfn DefDeMoveCtor(GEvent)

	/*!
	\brief 赋值：覆盖事件响应：使用单一构造参数指定的指定事件处理器。
	\since build 448
	*/
	template<typename _tHandler,
		yimpl(typename = ystdex::exclude_self_ctor_t<GEvent, _tHandler>)>
	inline GEvent&
	operator=(_tHandler&& _arg)
	{
		return *this = GEvent(yforward(_arg));
	}
	/*!
	\brief 复制赋值：默认实现。
	*/
	DefDeCopyAssignment(GEvent)
	/*!
	\brief 转移赋值：默认实现。
	*/
	DefDeMoveAssignment(GEvent)

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

	/*!
	\brief 移除事件响应：指定 const 事件处理器。
	*/
	GEvent&
	operator-=(const HandlerType& h)
	{
		ystdex::erase_all_if<ContainerType>(List, List.begin(), List.end(),
			[&](decltype(*List.begin())& pr){
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

	/*!
	\brief 添加事件响应：使用 const 事件处理器和优先级。
	\note 不检查是否已经在列表中。
	\since build 294
	*/
	inline PDefH(GEvent&, Add, const HandlerType& h,
		EventPriority prior = DefaultEventPriority)
		ImplRet(List.emplace(prior, h), *this)
	/*!
	\brief 添加事件响应：使用非 const 事件处理器和优先级。
	\note 不检查是否已经在列表中。
	\since build 294
	*/
	inline PDefH(GEvent&, Add, HandlerType&& h,
		EventPriority prior = DefaultEventPriority)
		ImplRet(List.emplace(prior, std::move(h)), *this)
	/*!
	\brief 添加事件响应：使用单一构造参数指定的事件处理器和优先级。
	\note 不检查是否已经在列表中。
	\since build 294
	*/
	template<typename _type>
	inline GEvent&
	Add(_type&& _arg, EventPriority prior = DefaultEventPriority)
	{
		return Add(HandlerType(yforward(_arg)), prior);
	}
	/*!
	\brief 添加事件响应：使用对象引用、成员函数指针和优先级。
	\note 不检查是否已经在列表中。
	\since build 413
	*/
	template<class _tObj, class _type>
	inline GEvent&
	Add(_tObj& obj, _tRet(_type::*pm)(_tParams...),
		EventPriority prior = DefaultEventPriority)
	{
		return Add(HandlerType(static_cast<_type&>(obj), std::move(pm)), prior);
	}

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
	*/
	bool
	Contains(const HandlerType& h) const
	{
		using ystdex::get_value;

		return std::find(List.cbegin() | get_value, List.cend() | get_value, h)
			!= List.cend();
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
	\brief 调用事件处理器。
	\return 成功调用的事件处理器个数。
	\exception std::bad_function_call 以外异常中立。
	*/
	SizeType
	operator()(_tParams... args) const
	{
		SizeType n(0);

		for(const auto& pr : List)
		{
			try
			{
				pr.second(yforward(args)...);
			}
			catch(std::bad_function_call&)
			{}
			++n;
		}
		return n;
	}

	/*!
	\brief 取列表中的响应数。
	*/
	DefGetter(const ynothrow, SizeType, Size, List.size())

	/*!
	\brief 清除：移除所有事件响应。
	*/
	PDefH(void, Clear, )
		ImplRet(List.clear())

	/*
	\brief 交换。
	\since build 409
	*/
	PDefH(void, swap, GEvent& e) ynothrow
		ImplRet(List.swap(e))
};
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
\relates GEvent
\since build 409
*/
template<typename _tRet, typename... _tParams>
inline DefSwap(ynothrow, GEvent<_tRet(_tParams...)>)


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
	std::reference_wrapper<EventType> Event;
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
\brief 定义事件处理器委托类型。
\since build 268
*/
#define DeclDelegate(_name, _tEventArgs) \
	using _name = GHEvent<void(_tEventArgs)>;


/*!
\brief 依赖事件项类模板。
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
	*/
	PDefH(void, Clear, )
		ImplBodyMem(this->GetNewRef(), Clear, )
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
\brief 事件类型宏。
\since build 188
*/
//@{
#define EventT(_tEventHandler) \
	GEvent<void(typename EventArgsHead<_tEventHandler::TupleType>::type)>
#define DepEventT(_tEventHandler) \
	typename GDependencyEvent(EventT(_tEventHandler))
//@}

/*!
\brief 声明事件。
\since build 188
*/
//@{
#define DeclEvent(_tEventHandler, _name) \
	EventT(_tEventHandler) _name;
#define DeclDepEvent(_tEventHandler, _name) \
	DepEventT(_tEventHandler) _name;
//@}

/*!
\brief 声明事件引用。
\since build 188
*/
//@{
#define DeclEventRef(_tEventHandler, _name) \
	EventT(_tEventHandler)& _name;
#define DeclDepEventRef(_tEventHandler, _name) \
	DepEventT(_tEventHandler)& _name;
//@}

/*!
\brief 声明事件接口函数。
*/
//@{
//! since build 166
#define DeclIEventEntry(_tEventHandler, _name) \
	DeclIEntry(const EventT(_tEventHandler)& YPP_Concat(Get, _name)() const)
//! since build 188
#define DeclIDepEventEntry(_tEventHandler, _name) \
	DeclIEntry(const DepEventT(_tEventHandler)& YPP_Concat(Get, _name)() const)
//@}

/*!
\brief 定义事件访问器。
\since build 188
*/
//@{
#define DefEventGetter(_q, _tEventHandler, _name, _member) \
	DefGetter(_q, EventT(_tEventHandler)&, _name, _member)
#define DefEventGetterBase(_q, _tEventHandler, _name, _base) \
	DefGetterBase(_q, EventT(_tEventHandler)&, _name, _base)
#define DefEventGetterMem(_q, _tEventHandler, _name, _member) \
	DefGetterMem(_q, EventT(_tEventHandler)&, _name, _member)
#define DefDepEventGetter(_q, _tEventHandler, _name, _member) \
	DefGetter(_q, DepEventT(_tEventHandler)&, _name, _member)
#define DefDepEventGetterBase(_q, _tEventHandler, _name, _base) \
	DefGetterBase(_q, DepEventT(_tEventHandler)&, _name, _base)
#define DefDepEventGetterMem(_q, _tEventHandler, _name, _member) \
	DefGetterMem(_q, DepEventT(_tEventHandler)&, _name, _member)
//@}


/*!
\brief 事件处理器适配器模板。
\warning 非虚析构。
\since build 494
*/
template<typename _type, typename _fCallable = std::function<void(_type&)>>
class GHandlerAdaptor
{
public:
	using CallerType = ystdex::decay_t<_fCallable>;

	std::reference_wrapper<_type> ObjectRef;
	CallerType Caller;

	GHandlerAdaptor(_type& obj, CallerType f)
		: ObjectRef(obj), Caller(f)
	{}
	template<typename _fCaller>
	GHandlerAdaptor(_type& obj, _fCaller&& f)
		: ObjectRef(obj), Caller(ystdex::make_expanded<CallerType>(yforward(f)))
	{}

	//! \todo 使用 <tt>noexpcept</tt> 。
	template<typename... _tParams>
	void
	operator()(_tParams&&... args) const
	{
		try
		{
			Caller(yforward(args)...);
		}
		catch(std::bad_function_call&)
		{}
	}

	//! \todo 实现比较 Function 相等。
	PDefHOp(bool, ==, const GHandlerAdaptor& adaptor) const ynothrow
		ImplRet(std::addressof(adaptor.ObjectRef.get())
			== std::addressof(ObjectRef.get()))
};


/*!
\brief 事件包装类模板。
\since build 173
*/
template<class _tEvent, typename _tBaseArgs>
class GEventWrapper : public _tEvent,
	implements GIHEvent<_tBaseArgs>
{
public:
	using EventType = _tEvent;
	using BaseArgsType = _tBaseArgs;
	using EventArgsType
		= typename EventArgsHead<typename _tEvent::TupleType>::type;

	/*!
	\brief 委托调用。
	\warning 需要确保 BaseArgsType 引用的对象能够转换至 EventArgsType 。
	\since build 331
	*/
	inline ImplI(GIHEvent<_tBaseArgs>) size_t
	operator()(BaseArgsType e) const
	{
		return EventType::operator()(EventArgsType(yforward(e)));
	}

	//! \since build 409
	inline ImplI(GIHEvent<_tBaseArgs>)
	DefClone(const override, GEventWrapper)
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
#if YB_HAS_NOEXCEPT
	/*
	\brief 避免 constexpr 失败的 noexcept 检查。
	\since build 468
	*/
	template<typename _type>
	struct is_noexcept
	{
		static yconstexpr bool value
			= noexcept(PointerType(std::declval<_type>()));
	};
#endif

	PointerType ptr;

public:
	//! \since build 319
	template<typename _type>
	inline
	GEventPointerWrapper(_type&& p) ynoexcept(is_noexcept<_type>::value)
		: ptr(yforward(p))
	{
		YAssertNonnull(p);
	}
	/*!
	\brief 复制构造：深复制。
	*/
	GEventPointerWrapper(const GEventPointerWrapper& item)
		: ptr(ClonePolymorphic(item.ptr))
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

