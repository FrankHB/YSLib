/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yevt.hpp
\ingroup Core
\brief 事件回调。
\version r5046;
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132 。
\par 创建时间:
	2010-04-23 23:08:23 +0800;
\par 修改时间:
	2012-07-23 20:07 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YSLib::Core::YEvent;
*/


#ifndef YSL_INC_CORE_YEVT_HPP_
#define YSL_INC_CORE_YEVT_HPP_

#include "yobject.h"
#include "yfunc.hpp"
#include <ystdex/iterator.hpp> // for ystdex::get_value;
#include <ystdex/algorithm.hpp> // for ystdex::erase_all_if;

YSL_BEGIN

/*!
\brief 标准事件处理器类模板。
\note 若使用仿函数，可以不满足 \c EqualityComparable 的接口，即
	可使用返回 \c bool 的 \c operator== ，但此模板类无法检查其语义正确性。
\warning 非虚析构。
\since build 173 。
*/
template<class _tEventArgs>
class GHEvent : protected std::function<void(_tEventArgs&&)>
{
public:
	typedef _tEventArgs EventArgsType;
	typedef void FuncType(_tEventArgs&&);
	typedef std::function<FuncType> BaseType;

private:
	typedef bool(*Comparer)(const GHEvent&, const GHEvent&); //!< 比较函数类型。
	template<class _tFunctor>
	struct GEquality
	{
		//! \since build 319 。
		//@{
		typedef typename std::decay<_tFunctor>::type decayed_type;

#if YB_HAS_NOEXCEPT
		static yconstexpr bool except_helper = noexcept(std::declval<
			const decayed_type>() == std::declval<const decayed_type>());
#endif

		static bool
		AreEqual(const GHEvent& x, const GHEvent& y)
#if YB_HAS_NOEXCEPT
			// NOTE: Use helper to prevent constexpr checking fail.
			ynoexcept(except_helper)
#endif
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
	yconstfn DefDeCopyCtor(GHEvent)
	yconstfn DefDeMoveCtor(GHEvent)
	/*!
	\brief 构造：使用函数指针。
	\since build 294 。
	*/
	yconstfn
	GHEvent(const FuncType* f)
		: std::function<FuncType>(f), comp_eq(GEquality<FuncType>::AreEqual)
	{}
	/*!
	\brief 使用函数对象。
	\since build 327 。
	*/
	template<class _fCallable>
	yconstfn
	GHEvent(_fCallable f, typename std::enable_if<std::is_object<_fCallable>
		::value, int>::type = 0)
		: std::function<FuncType>(std::move(f)),
		comp_eq(GetComparer(f, f))
	{}
	/*!
	\brief 构造：使用对象引用和成员函数指针。
	*/
	template<class _type>
	yconstfn
	GHEvent(_type& obj, void(_type::*pm)(_tEventArgs&&))
		: std::function<FuncType>(ExpandMemberFirstBinder<
			_type, void, _tEventArgs&&>(obj, pm)),
		comp_eq(GEquality<ExpandMemberFirstBinder<
			_type, void, _tEventArgs&&>>::AreEqual)
	{}

	DefDeCopyAssignment(GHEvent)
	/*!
	\brief 转移赋值：默认实现。
	*/
	DefDeMoveAssignment(GHEvent)

	yconstfn bool
	operator==(const GHEvent& h) const
	{
		return this->comp_eq == h.comp_eq && (this->comp_eq(*this, h));
	}

	/*!
	\brief 调用。
	*/
	using BaseType::operator();

private:
	//! \since build 319 。
	//@{
	PDefTmplH1(_type)
	static yconstfn Comparer
	GetComparer(_type& x, _type& y, decltype(x == y) = false) ynothrow
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


/*!
\brief 事件优先级。
\since build 294 。
*/
typedef u8 EventPriority;


/*!
\brief 默认事件优先级。
\since build 294 。
*/
yconstexpr EventPriority DefaultEventPriority(0x80);


/*!
\brief 事件类模板。
\note 支持顺序多播。
\warning 非虚析构。
\since 早于 build 132 。
*/
template<class _tEventArgs>
class GEvent
{
public:
	typedef _tEventArgs EventArgsType;
	typedef void FuncType(_tEventArgs&&);
	typedef GHEvent<_tEventArgs> HandlerType;
	/*!
	\brief 容器类型。
	\since build 294 。
	*/
	typedef multimap<EventPriority, HandlerType, std::greater<EventPriority>>
		ContainerType;
	typedef typename ContainerType::size_type SizeType;

	/*!
	\brief 响应列表。
	\since build 294 。
	*/
	ContainerType List;

	/*!
	\brief 无参数构造：默认实现。
	\note 得到空实例。
	*/
	yconstfn DefDeCtor(GEvent)
	/*!
	\brief 复制构造：默认实现。
	\note 深复制。
	*/
	yconstfn DefDeCopyCtor(GEvent);
	/*!
	\brief 转移构造：默认实现。
	*/
	yconstfn DefDeMoveCtor(GEvent);

private:
	/*!
	\brief \c private 构造：添加事件处理器。
	\since build 293 。
	*/
	PDefTmplH1(_tHandler)
	GEvent(_tHandler&& h)
		: List()
	{
		Add(yforward(h));
	}

public:
	/*!
	\brief 复制赋值：覆盖事件响应。
	*/
	inline GEvent&
	operator=(const GEvent& e)
	{
		this->List = e.List;
		return *this;
	}
	/*!
	\brief 转移赋值：默认实现。
	*/
	DefDeMoveAssignment(GEvent)
	/*!
	\brief 赋值：覆盖事件响应：使用事件处理器。
	*/
	inline GEvent&
	operator=(const HandlerType& h)
	{
		return *this = GEvent(h);
	}
	/*!
	\brief 赋值：覆盖事件响应：使用事件处理器。
	*/
	inline GEvent&
	operator=(HandlerType&& h)
	{
		return *this = GEvent(h);
	}
	/*!
	\brief 赋值：覆盖事件响应：使用单一构造参数指定的指定事件处理器。
	\since build 293 。
	*/
	PDefTmplH1(_type)
	inline GEvent&
	operator=(_type&& _arg)
	{
		return *this = HandlerType(yforward(_arg));
	}

	/*!
	\brief 添加事件响应：使用 const 事件处理器和优先级。
	\note 不检查是否已经在列表中。
	*/
	inline GEvent&
	operator+=(const HandlerType& h)
	{
		return this->Add(h);
	}
	/*!
	\brief 添加事件响应：使用事件处理器。
	\note 不检查是否已经在列表中。
	*/
	inline GEvent&
	operator+=(HandlerType&& h)
	{
		return this->Add(std::move(h));
	}
	/*!
	\brief 添加事件响应：目标为单一构造参数指定的指定事件处理器。
	\note 不检查是否已经在列表中。
	*/
	PDefTmplH1(_type)
	inline GEvent&
	operator+=(_type&& _arg)
	{
		return this->Add(HandlerType(yforward(_arg)));
	}

	/*!
	\brief 移除事件响应：指定 const 事件处理器。
	*/
	GEvent&
	operator-=(const HandlerType& h)
	{
		ystdex::erase_all_if<ContainerType>(List, List.begin(), List.end(),
			[&](decltype(*this->List.begin())& pr){
			return pr.second == h;
		});
		return *this;
	}
	/*!
	\brief 移除事件响应：指定非 const 事件处理器。
	\note 防止模板 <tt>operator-=</tt> 递归。
	*/
	inline GEvent&
	operator-=(HandlerType&& h)
	{
		return *this -= static_cast<const HandlerType&>(h);
	}
	/*!
	\brief 移除事件响应：目标为单一构造参数指定的指定事件处理器。
	\since build 293 。
	*/
	PDefTmplH1(_type)
	inline GEvent&
	operator-=(_type&& _arg)
	{
		return *this -= HandlerType(yforward(_arg));
	}

	/*!
	\brief 添加事件响应：使用 const 事件处理器和优先级。
	\note 不检查是否已经在列表中。
	\since build 294 。
	*/
	inline GEvent&
	Add(const HandlerType& h, EventPriority prior = DefaultEventPriority)
	{
		this->List.insert(make_pair(prior, h));
		return *this;
	}
	/*!
	\brief 添加事件响应：使用非 const 事件处理器和优先级。
	\note 不检查是否已经在列表中。
	\since build 294 。
	*/
	inline GEvent&
	Add(HandlerType&& h, EventPriority prior = DefaultEventPriority)
	{
		this->List.insert(make_pair(prior, std::move(h)));
		return *this;
	}
	/*!
	\brief 添加事件响应：使用单一构造参数指定的事件处理器和优先级。
	\note 不检查是否已经在列表中。
	\since build 294 。
	*/
	PDefTmplH1(_type)
	inline GEvent&
	Add(_type&& _arg, EventPriority prior = DefaultEventPriority)
	{
		return this->Add(HandlerType(yforward(_arg)), prior);
	}
	/*!
	\brief 添加事件响应：使用对象引用、成员函数指针和优先级。
	\note 不检查是否已经在列表中。
	\since build 294 。
	*/
	template<class _tObj, class _type>
	inline GEvent&
	Add(_tObj& obj, void(_type::*pm)(_tEventArgs&&),
		EventPriority prior = DefaultEventPriority)
	{
		return this->Add(HandlerType(static_cast<_type&>(obj), std::move(pm)),
			prior);
	}

	/*!
	\brief 添加单一事件响应：使用事件处理器和优先级。
	\since build 294 。
	*/
	inline GEvent&
	AddUnique(const HandlerType& h, EventPriority prior = DefaultEventPriority)
	{
		return (*this -= h).Add(h, prior);
	}
	/*!
	\brief 添加单一事件响应：使用事件处理器。
	\since build 294 。
	*/
	inline GEvent&
	AddUnique(HandlerType&& h, EventPriority prior = DefaultEventPriority)
	{
		return (*this -= h).Add(std::move(h), prior);
	}
	/*!
	\brief 添加单一事件响应：使用为单一构造参数指定的事件处理器和优先级。
	\since build 294 。
	*/
	PDefTmplH1(_type)
	inline GEvent&
	AddUnique(_type&& _arg, EventPriority prior = DefaultEventPriority)
	{
		return this->AddUnique(HandlerType(yforward(_arg)), prior);
	}
	/*!
	\brief 添加单一事件响应：使用对象引用和成员函数指针。
	\since build 294 。
	*/
	template<class _tObj, class _type>
	inline GEvent&
	AddUnique(_type& obj, void(_type::*pm)(_tEventArgs&&),
		EventPriority prior = DefaultEventPriority)
	{
		return this->AddUnique(HandlerType(static_cast<_type&>(obj),
			std::move(pm)), prior);
	}

	/*!
	\brief 移除事件响应：目标为指定对象引用和成员函数指针。
	\since build 276 。
	*/
	template<class _tObj, class _type>
	inline GEvent&
	Remove(_tObj& obj, void(_type::*pm)(_tEventArgs&&))
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

		return std::find(this->List.cbegin() | get_value,
			this->List.cend() | get_value, h) != this->List.cend();
	}
	/*!
	\brief 判断是否包含单一构造参数指定的事件响应。
	\since build 293 。
	*/
	PDefTmplH1(_type)
	inline bool
	Contains(_type&& _arg) const
	{
		return this->Contains(HandlerType(yforward(_arg)));
	}

	/*!
	\brief 调用事件处理器。
	\return 成功调用的事件处理器个数。
	\exception std::bad_function_call 以外异常中立。
	*/
	SizeType
	operator()(_tEventArgs&& e) const
	{
		using ystdex::get_value;

		SizeType n(0);

		std::for_each(this->List.cbegin(), this->List.cend(),
			[&](decltype(*this->List.cbegin())& pr){
			try
			{
				pr.second(std::move(e));
			}
			catch(std::bad_function_call&)
			{}
			++n;
		});
		return n;
	}

	/*!
	\brief 取列表中的响应数。
	*/
	inline DefGetter(const ynothrow, SizeType, Size, this->List.size())

	/*!
	\brief 清除：移除所有事件响应。
	*/
	inline PDefH(void, Clear)
		ImplRet(this->List.clear())

	/*
	\brief 交换。
	*/
	inline PDefH(void, Swap, GEvent& e) ynothrow
		ImplRet(this->List.swap(e))
};


/*!
\brief 定义事件处理器委托类型。
\since build 268 。
*/
#define DeclDelegate(_name, _tEventArgs) typedef GHEvent<_tEventArgs> _name;


/*!
\brief 依赖事件项类模板。
\warning 非虚析构。
\since build 195 。
*/
template<class _tEvent, class _tOwnerPointer = shared_ptr<_tEvent>>
class GDependencyEvent : public GDependency<_tEvent, _tOwnerPointer>
{
public:
	typedef typename GDependency<_tEvent>::DependentType DependentType;
	typedef typename GDependency<_tEvent>::PointerType PointerType;
	typedef typename GDependency<_tEvent>::ConstReferenceType
		ConstReferenceType;
	typedef typename GDependency<_tEvent>::ReferentType ReferentType;
	typedef typename GDependency<_tEvent>::ReferenceType ReferenceType;
	typedef DependentType EventType;
	typedef typename EventType::EventArgsType EventArgsType;
	typedef typename EventType::SEventType SEventType;
	typedef typename EventType::FuncType FuncType;
	typedef typename EventType::HandlerType HandlerType;
	typedef typename EventType::SizeType SizeType;

	GDependencyEvent(PointerType p = PointerType())
		: GDependency<_tEvent>(p)
	{}

	/*!
	\brief 添加事件响应。
	*/
	PDefTmplH1(_type)
	inline ReferenceType
	operator+=(_type _arg)
	{
		return this->GetNewRef().operator+=(_arg);
	}

	/*!
	\brief 移除事件响应。
	*/
	PDefTmplH1(_type)
	inline ReferenceType
	operator-=(_type _arg)
	{
		return this->GetNewRef().operator-=(_arg);
	}

	/*!
	\brief 添加事件响应：使用对象引用和成员函数指针。
	*/
	template<class _type>
	inline ReferenceType
	Add(_type& obj, void(_type::*pm)(EventArgsType&&))
	{
		return this->GetNewRef().Add(obj, pm);
	}

	/*!
	\brief 移除事件响应：目标为指定对象引用和成员函数指针。
	*/
	template<class _type>
	inline ReferenceType
	Remove(_type& obj, void(_type::*pm)(EventArgsType&&))
	{
		return this->GetNewRef().Remove(obj, pm);
	}

	/*!
	\brief 调用函数。
	*/
	inline SizeType
	operator()(EventArgsType&& e) const
	{
		return this->GetRef().operator()(std::move(e));
	}

	/*!
	\brief 取列表中的响应数。
	*/
	inline DefGetterMem(const ynothrow, SizeType, Size, this->GetRef())

	/*!
	\brief 清除：移除所有事件响应。
	*/
	inline PDefH(void, Clear)
		ImplBodyMem(this->GetNewRef(), Clear)
};


/*!
\brief 事件类型宏。
\since build 188 。
*/
//@{
#define EventT(_tEventHandler) \
	GEvent<_tEventHandler::EventArgsType>
#define DepEventT(_tEventHandler) \
	typename GDependencyEvent(EventT(_tEventHandler))
//@}

/*!
\brief 声明事件。
\since build 188 。
*/
//@{
#define DeclEvent(_tEventHandler, _name) \
	EventT(_tEventHandler) _name;
#define DeclDepEvent(_tEventHandler, _name) \
	DepEventT(_tEventHandler) _name;
//@}

/*!
\brief 声明事件引用。
\since build 188 。
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
//! since build 166 。
#define DeclIEventEntry(_tEventHandler, _name) \
	DeclIEntry(const EventT(_tEventHandler)& yJOIN(Get, _name)() const)
//! since build 188 。
#define DeclIDepEventEntry(_tEventHandler, _name) \
	DeclIEntry(const DepEventT(_tEventHandler)& yJOIN(Get, _name)() const)
//@}

/*!
\brief 定义事件访问器。
\since build 188 。
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
\brief 事件处理器接口模板。
\since build 188 。
*/
template<class _tEventArgs>
DeclI(GIHEvent)
	DeclIEntry(size_t operator()(_tEventArgs&&) const)
	DeclIEntry(GIHEvent* Clone() const)
EndDecl


/*!
\brief 事件处理器包装类模板。
\since build 173 。
*/
template<class _tEvent, typename _tBaseArgs>
class GEventWrapper : public _tEvent,
	implements GIHEvent<_tBaseArgs>
{
public:
	typedef _tEvent EventType;
	typedef _tBaseArgs BaseArgsType;
	typedef typename EventType::EventArgsType EventArgsType;

	inline ImplI(GIHEvent<_tBaseArgs>) DefClone(GEventWrapper, Clone)

	/*!
	\brief 委托调用。
	\warning 需要确保 BaseArgsType&& 引用的对象能够转换至 EventArgsType&& 引用。
	*/
	inline ImplI(GIHEvent<_tBaseArgs>) size_t
	operator()(BaseArgsType&& e) const
	{
		return EventType::operator()(static_cast<EventArgsType&&>(
			std::move(e)));
	}
};


/*!
\brief 事件项类型。
\warning 非虚析构。
\since build 242 。
*/
PDefTmplH1(_tBaseArgs)
class GEventPointerWrapper
{
public:
	typedef GIHEvent<_tBaseArgs> ItemType;
	typedef unique_ptr<ItemType> PointerType;

private:
#if YB_HAS_NOEXCEPT
	template<typename _type>
	struct except_helper_t
	{
		static yconstexpr bool value
			= noexcept(PointerType(std::declval<_type>()));
	};
#endif

	PointerType ptr;

public:
	//! \since build 319 。
	template<typename _type>
	inline
	GEventPointerWrapper(_type&& p)
#if YB_HAS_NOEXCEPT
		// NOTE: Use helper to prevent constexpr checking fail.
		ynoexcept(except_helper_t<_type>::value)
#endif
		: ptr(yforward(p))
	{
		YAssert(bool(p), "Null pointer found.");
	}
	/*!
	\brief 复制构造：深复制。
	*/
	GEventPointerWrapper(const GEventPointerWrapper& item)
		: ptr(ClonePolymorphic(item.ptr))
	{}
	DefDeMoveCtor(GEventPointerWrapper)

	yconstfn DefCvt(const ynothrow, const ItemType&, *ptr)
	yconstfn DefCvt(ynothrow, ItemType&, *ptr)
};


/*!
\brief 定义扩展事件类。
\since build 240 。
*/
#define DefExtendEventMap(_n, _b) \
	DefExtendClass1(_n, _b, public)

YSL_END

#endif

