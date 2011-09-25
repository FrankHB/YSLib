/*
	Copyright (C) by Franksoft 2010 - 2011.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yevt.hpp
\ingroup Core
\brief 事件回调。
\version r4672;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-04-23 23:08:23 +0800;
\par 修改时间:
	2011-09-23 18:45 +0800;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YEvent;
*/


#ifndef INCLUDED_CORE_YEVT_HPP_
#define INCLUDED_CORE_YEVT_HPP_

#include "yobject.h"
#include "yfunc.hpp"

YSL_BEGIN

//! \brief 公用事件模板命名空间。
template<class _tSender, class _tEventArgs>
struct GSEventTypeSpace
{
	typedef void FuncType(_tSender&, _tEventArgs&&);
	typedef FuncType* FuncPtrType;
};


/*!
\brief 标准事件处理器类模板。
\note 若使用函数对象，可以不满足 \c EqualityComparable 的接口，即
	可使用返回 \c bool 的 \c operator== ，但此模板类无法检查其语义正确性。
*/
template<class _tSender, class _tEventArgs>
class GHEvent : protected std::function<typename GSEventTypeSpace<
	_tSender, _tEventArgs>::FuncType>
{
public:
	typedef _tSender SenderType;
	typedef _tEventArgs EventArgsType;
	typedef GSEventTypeSpace<_tSender, _tEventArgs> SEventType;
	typedef typename SEventType::FuncType FuncType;
	typedef std::function<FuncType> BaseType;

private:
	typedef bool(*Comparer)(const GHEvent&, const GHEvent&); //!< 比较函数类型。
	template<class _tFunctor>
	struct GEquality
	{
		static bool
		AreEqual(const GHEvent& x, const GHEvent& y)
		{
			auto p(x.template target<typename std::decay<_tFunctor>::type>());

			if(p)
			{
				auto q(y.template target<typename std::decay<
					_tFunctor>::type>());

				if(q)
					return p == q || *p == *q;
			}
			return false;
		}
	};

	Comparer comp_eq; //!< 比较函数：相等关系。

public:
	yconstexprf DefDeCopyCtor(GHEvent)
	yconstexprf DefDeMoveCtor(GHEvent)
	/*!
	\brief 构造：使用函数指针。
	\note 匹配函数引用。
	*/
	yconstexprf
	GHEvent(FuncType* f)
		: std::function<FuncType>(f), comp_eq(GEquality<FuncType>::AreEqual)
	{}
	/*!
	\brief 使用函数对象类型。
	*/
	template<class _tFunc>
	yconstexprf
	GHEvent(_tFunc f)
		: std::function<FuncType>(yforward(f)),
		comp_eq(GetComparer(f, f))
	{}
	/*!
	\brief 构造：使用 _tSender 的成员函数指针。
	*/
	yconstexprf
	GHEvent(void(_tSender::*pm)(_tEventArgs&&))
		: std::function<FuncType>(ExpandMemberFirst<
			_tSender, void, _tEventArgs&&>(pm)),
		comp_eq(GEquality<ExpandMemberFirst<
			_tSender, void, _tEventArgs&&>>::AreEqual)
	{}
	/*!
	\brief 构造：使用成员函数指针。
	*/
	template<class _type>
	yconstexprf
	GHEvent(void(_type::*pm)(_tEventArgs&&))
		: std::function<FuncType>(ExpandMemberFirst<
			_type, void, _tEventArgs&&, _tSender>(pm)),
		comp_eq(GEquality<ExpandMemberFirst<
			_type, void, _tEventArgs&&, _tSender>>::AreEqual)
	{}
	/*!
	\brief 构造：使用 _tSender 类型对象引用和成员函数指针。
	*/
	yconstexprf
	GHEvent(_tSender& obj, void(_tSender::*pm)(_tEventArgs&&))
		: std::function<FuncType>(ExpandMemberFirstBinder<
			_tSender, void, _tEventArgs&&>(obj, pm)),
		comp_eq(GEquality<ExpandMemberFirstBinder<
			_tSender, void, _tEventArgs&&>>::AreEqual)
	{}
	/*!
	\brief 构造：使用对象引用和成员函数指针。
	*/
	template<class _type>
	yconstexprf
	GHEvent(_type& obj, void(_type::*pm)(_tEventArgs&&))
		: std::function<FuncType>(ExpandMemberFirstBinder<
			_type, void, _tEventArgs&&, _tSender>(obj, pm)),
		comp_eq(GEquality<ExpandMemberFirstBinder<
			_type, void, _tEventArgs&&, _tSender>>::AreEqual)
	{}

	DefDeCopyAssignment(GHEvent)
	/*!
	\brief 转移赋值：默认实现。
	*/
	DefDeMoveAssignment(GHEvent)

	yconstexprf bool
	operator==(const GHEvent& h) const
	{
		return this->comp_eq == h.comp_eq && (this->comp_eq(*this, h));
	}

	/*!
	\brief 调用。
	*/
	using BaseType::operator();

private:
	PDefTH1(_type)
	static yconstexprf Comparer
	GetComparer(_type& x, _type& y, decltype(x == y) = false)
	{
		return GEquality<_type>::AreEqual;
	}
	template<typename _type, typename _tUnused>
	static yconstexprf Comparer
	GetComparer(_type&, _tUnused&)
	{
		return GHEvent::AreAlwaysEqual;
	}

	static yconstexprf bool
	AreAlwaysEqual(const GHEvent& x, const GHEvent& y)
	{
		return true;
	}
};


/*!
\brief 事件类模板。
\note 支持顺序多播。
*/
template<class _tSender, class _tEventArgs>
class GEvent
{
public:
	typedef _tSender SenderType;
	typedef _tEventArgs EventArgsType;
	typedef GSEventTypeSpace<_tSender, _tEventArgs> SEventType;
	typedef typename SEventType::FuncType FuncType;
	typedef GHEvent<_tSender, _tEventArgs> HandlerType;
	typedef list<HandlerType> ListType;
	typedef typename ListType::size_type SizeType;

protected:
	ListType List; //!< 响应列表。

public:
	/*!
	\brief 无参数构造：默认实现。
	\note 得到空实例。
	*/
	yconstexprf DefDeCtor(GEvent)
	/*!
	\brief 复制构造：默认实现。
	\note 深复制。
	*/
	yconstexprf DefDeCopyCtor(GEvent);
	/*!
	\brief 转移构造：默认实现。
	*/
	yconstexprf DefDeMoveCtor(GEvent);

private:
	/*!
	\brief \c private 构造：添加事件处理器。
	*/
	PDefTH1(_tHandler)
	GEvent(_tHandler h)
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
		this->List = e->List;
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
	*/
	PDefTH1(_type)
	inline GEvent&
	operator=(_type _arg)
	{
		return *this = HandlerType(yforward(_arg));
	}

	/*!
	\brief 添加事件响应：使用事件处理器。
	\note 不检查是否已经在列表中。
	*/
	inline GEvent&
	operator+=(const HandlerType& h)
	{
		this->List.push_back(h);
		return *this;
	}
	/*!
	\brief 添加事件响应：使用事件处理器。
	\note 不检查是否已经在列表中。
	*/
	inline GEvent&
	operator+=(HandlerType&& h)
	{
		this->List.push_back(std::move(h));
		return *this;
	}
	/*!
	\brief 添加事件响应：目标为单一构造参数指定的指定事件处理器。
	\note 不检查是否已经在列表中。
	*/
	PDefTH1(_type)
	inline GEvent&
	operator+=(_type _arg)
	{
		return *this += HandlerType(yforward(_arg));
	}

	/*!
	\brief 移除事件响应：目标为指定事件处理器。
	*/
	inline GEvent&
	operator-=(const HandlerType& h)
	{
		this->List.remove(h);
		return *this;
	}
	/*!
	\brief 移除事件响应：目标为指定事件处理器。
	*/
	inline GEvent&
	operator-=(HandlerType&& h)
	{
		this->List.remove(std::move(h));
		return *this;
	}
	/*!
	\brief 移除事件响应：目标为单一构造参数指定的指定事件处理器。
	*/
	PDefTH1(_type)
	inline GEvent&
	operator-=(_type _arg)
	{
		return *this -= HandlerType(yforward(_arg));
	}

	/*!
	\brief 添加事件响应：使用对象引用和成员函数指针。
	\note 不检查是否已经在列表中。
	*/
	template<class _type>
	inline GEvent&
	Add(_type& obj, void(_type::*pm)(_tEventArgs&&))
	{
		return *this += HandlerType(obj, std::move(pm));
	}

	/*!
	\brief 添加事件响应：使用事件处理器。
	*/
	inline GEvent&
	AddUnique(const HandlerType& h)
	{
		return *this -= h += h;
	}
	/*!
	\brief 添加事件响应：使用事件处理器。
	*/
	inline GEvent&
	AddUnique(HandlerType&& h)
	{
		return *this -= std::move(h) += (std::move(h));
	}
	/*!
	\brief 添加事件响应：目标为单一构造参数指定的指定事件处理器。
	*/
	PDefTH1(_type)
	inline GEvent&
	AddUnique(_type _arg)
	{
		return this->AddUnique(HandlerType(yforward(_arg)));
	}
	/*!
	\brief 添加事件响应：使用对象引用和成员函数指针。
	*/
	template<class _type>
	inline GEvent&
	AddUnique(_type& obj, void(_type::*pm)(_tEventArgs&&))
	{
		return this->AddUnique(HandlerType(obj, std::move(pm)));
	}

	/*!
	\brief 移除事件响应：目标为指定对象引用和成员函数指针。
	*/
	template<class _type>
	inline GEvent&
	Remove(_type& obj, void(_type::*pm)(_tEventArgs&&))
	{
		return *this -= HandlerType(obj, std::move(pm));
	}

	/*!
	\brief 判断是否包含指定事件响应。
	*/
	inline bool
	Contains(const HandlerType& h) const
	{
		return std::find(this->List.begin(), this->List.end(), h)
			!= this->List.end();
	}
	/*!
	\brief 判断是否包含单一构造参数指定的事件响应。
	*/
	PDefTH1(_type)
	inline bool
	Contains(_type _arg) const
	{
		return this->Contains(HandlerType(yforward(_arg)));
	}

	/*!
	\brief 调用函数。
	*/
	SizeType
	operator()(_tSender& sender, _tEventArgs&& e) const
	{
		SizeType n(0);

		for(auto i(this->List.cbegin()); i != this->List.cend(); ++i, ++n)
			(*i)(sender, std::move(e));
		return n;
	}

	/*!
	\brief 取列表中的响应数。
	*/
	inline DefGetter(SizeType, Size, this->List.size())

	/*!
	\brief 清除：移除所有事件响应。
	*/
	inline PDefH0(void, Clear)
		ImplRet(this->List.clear())

	/*
	\brief 交换。
	*/
	inline PDefH1(void, Swap, GEvent& e) ynothrow
		ImplRet(this->List.swap(e))
};


//! \brief 定义事件处理器委托类型。
#define DefDelegate(_name, _tSender, _tEventArgs) \
	typedef GHEvent<_tSender, _tEventArgs> _name;


/*!
\brief 依赖事件项类模板。
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
	typedef typename EventType::SenderType SenderType;
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
	PDefTH1(_type)
	inline ReferenceType
	operator+=(_type _arg)
	{
		return this->GetNewRef().operator+=(_arg);
	}

	/*!
	\brief 移除事件响应。
	*/
	PDefTH1(_type)
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
	operator()(SenderType& sender, EventArgsType&& e) const
	{
		return this->GetRef().operator()(sender, std::move(e));
	}

	/*!
	\brief 取列表中的响应数。
	*/
	inline DefGetterMember(SizeType, Size, this->GetRef())

	/*!
	\brief 清除：移除所有事件响应。
	*/
	inline PDefH0(void, Clear)
		ImplBodyMember0(this->GetNewRef(), Clear)
};


//! \brief 多播事件类型。
template<class _tEventHandler>
struct GSEvent
{
	typedef GEvent<typename _tEventHandler::SenderType,
		typename _tEventHandler::EventArgsType> EventType;
	typedef GDependencyEvent<EventType> DependencyType;
};


//! \brief 事件类型宏。
#define EventT(_tEventHandler) \
	GSEvent<_tEventHandler>::EventType
#define DepEventT(_tEventHandler) \
	GSEvent<_tEventHandler>::DependencyType

//! \brief 声明事件。
#define DeclEvent(_tEventHandler, _name) \
	EventT(_tEventHandler) _name;
#define DeclDepEvent(_tEventHandler, _name) \
	DepEventT(_tEventHandler) _name;

//! \brief 声明事件引用。
#define DeclEventRef(_tEventHandler, _name) \
	EventT(_tEventHandler)& _name;
#define DeclDepEventRef(_tEventHandler, _name) \
	DepEventT(_tEventHandler)& _name;

//! \brief 声明事件接口函数。
#define DeclIEventEntry(_tEventHandler, _name) \
	DeclIEntry(const EventT(_tEventHandler)& _yJOIN(Get, _name)() const)
#define DeclIDepEventEntry(_tEventHandler, _name) \
	DeclIEntry(const DepEventT(_tEventHandler)& _yJOIN(Get, _name)() const)

//! \brief 定义事件访问器。
#define DefEventGetter(_tEventHandler, _name, _member) \
	DefGetter(const EventT(_tEventHandler)&, _name, _member)
#define DefEventGetterBase(_tEventHandler, _name, _base) \
	DefGetterBase(const EventT(_tEventHandler)&, _name, _base)
#define DefEventGetterMember(_tEventHandler, _name, _member) \
	DefGetterMember(const EventT(_tEventHandler)&, _name, _member)
#define DefDepEventGetter(_tEventHandler, _name, _member) \
	DefGetter(const DepEventT(_tEventHandler)&, _name, _member)
#define DefDepEventGetterBase(_tEventHandler, _name, _base) \
	DefGetterBase(const DepEventT(_tEventHandler)&, _name, _base)
#define DefDepEventGetterMember(_tEventHandler, _name, _member) \
	DefGetterMember(const DepEventT(_tEventHandler)&, _name, _member)

//! \brief 定义事件可修改访问器。
#define DefMutableEventGetter(_tEventHandler, _name, _member) \
	DefMutableGetter(EventT(_tEventHandler)&, _name, _member)
#define DefMutableEventGetterBase(_tEventHandler, _name, _base) \
	DefMutableGetterBase(EventT(_tEventHandler)&, _name, _base)
#define DefMutableEventGetterMember(_tEventHandler, _name, _member) \
	DefMutableGetterMember(EventT(_tEventHandler)&, _name, _member)
#define DefMutableDepEventGetter(_tEventHandler, _name, _member) \
	DefMutableGetter(DepEventT(_tEventHandler)&, _name, _member)
#define DefMutableDepEventGetterBase(_tEventHandler, _name, _base) \
	DefMutableGetterBase(DepEventT(_tEventHandler)&, _name, _base)
#define DefMutableDepEventGetterMember(_tEventHandler, _name, _member) \
	DefMutableGetterMember(DepEventT(_tEventHandler)&, _name, _member)


//! \brief 事件处理器接口模板。
template<class _tSender, class _tEventArgs>
DeclInterface(GIHEvent)
	DeclIEntry(size_t operator()(_tSender&, _tEventArgs&&) const)
	DeclIEntry(GIHEvent* Clone() const)
EndDecl


//! \brief 事件处理器包装类模板。
template<class _tEvent>
class GEventWrapper : public _tEvent,
	implements GIHEvent<typename _tEvent::SenderType, EventArgs>
{
public:
	typedef _tEvent EventType;
	typedef typename EventType::SenderType SenderType;
	typedef typename EventType::EventArgsType EventArgsType;

	inline virtual DefClone(GEventWrapper, Clone)

	/*!
	\brief 委托调用。
	\warning 需要确保 EventArgs&& 引用的对象能够转换至 EventArgsType&& 引用。
	*/
	inline virtual size_t
	operator()(SenderType& sender, EventArgs&& e) const
	{
		return EventType::operator()(sender,
			static_cast<EventArgsType&&>(std::move(e)));
	}
};


/*!
\brief 事件项类型。
\warning 非虚析构。
*/
PDefTH2(_tSender, _tEventSpace)
class GEventPointerWrapper
{
public:
	typedef GIHEvent<_tSender, EventArgs> ItemType;
	typedef unique_ptr<ItemType> PointerType;

private:
	PointerType ptr;

public:
	template<typename _type>
	inline GEventPointerWrapper(_type&& p)
		: ptr(yforward(p))
	{
		YAssert(is_not_null(p), "Null pointer found @ GEventItem::GEventItem;");
	}
	/*!
	\brief 复制构造：深复制。
	*/
	GEventPointerWrapper(const GEventPointerWrapper& item)
		: ptr(ClonePolymorphic(item.ptr))
	{}
	DefDeMoveCtor(GEventPointerWrapper)


	yconstexprf DefConverter(const ItemType&, *ptr)
	yconstexprf DefMutableConverter(ItemType&, *ptr)
};


//! \brief 定义扩展事件类。
#define DefExtendEventMap(_n, _b) \
	DefExtendClass1(_n, _b, public)

YSL_END

#endif

