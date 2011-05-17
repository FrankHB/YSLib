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
\version 0.4417;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-04-23 23:08:23 +0800;
\par 修改时间:
	2011-05-16 20:36 +0800;
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

YSL_BEGIN_NAMESPACE(Runtime)

//! \brief 公用事件模板命名空间。
template<class _tSender = YObject, class _tEventArgs = EventArgs>
struct GSEventTypeSpace
{
	typedef void FuncType(_tSender&, _tEventArgs&&);
	typedef FuncType* FuncPtrType;
};


//! \brief 标准事件处理器类模板。
template<class _tSender, class _tEventArgs>
class GHEvent : public std::function<typename GSEventTypeSpace<
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
		AreEqual(const GHEvent& lhs, const GHEvent& rhs)
		{
			auto p(lhs.template target<typename std::decay<_tFunctor>::type>());

			if(p)
			{
				auto q(rhs.template target<typename std::decay<
					_tFunctor>::type>());

				if(q)
					return *p == *q;
			}
			return false;
		}
	};

	Comparer comp_eq; //!< 比较函数：相等关系。

public:
	inline
	GHEvent(const GHEvent&) = default;
	inline
	GHEvent(GHEvent&&) = default;
	/*!
	\brief 构造：使用函数引用。
	*/
	inline
	GHEvent(FuncType* f)
		: std::function<FuncType>(f), comp_eq(GEquality<FuncType>::AreEqual)
	{}
	/*!
	\brief 使用函数类型。
	\note 函数引用除外（匹配以上非模板重载版本）。
	*/
	template<class _tFunc>
	inline
	GHEvent(_tFunc f)
		: std::function<FuncType>(yforward(f)),
		comp_eq(GEquality<_tFunc>::AreEqual)
	{}
	/*!
	\brief 构造：使用 _tSender 的成员函数指针。
	*/
	inline
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
	inline
	GHEvent(void(_type::*pm)(_tEventArgs&&))
		: std::function<FuncType>(ExpandMemberFirst<
			_type, void, _tEventArgs&&, _tSender>(pm)),
		comp_eq(GEquality<ExpandMemberFirst<
			_type, void, _tEventArgs&&, _tSender>>::AreEqual)
	{}
	/*!
	\brief 构造：使用 _tSender 类型对象引用和成员函数指针。
	*/
	inline
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
	inline
	GHEvent(_type& obj, void(_type::*pm)(_tEventArgs&&))
		: std::function<FuncType>(ExpandMemberFirstBinder<
			_type, void, _tEventArgs&&, _tSender>(obj, pm)),
		comp_eq(GEquality<ExpandMemberFirstBinder<
			_type, void, _tEventArgs&&, _tSender>>::AreEqual)
	{}

	inline GHEvent&
	operator=(const GHEvent&) = default;
	/*!
	\brief 移动赋值：默认实现。
	\note GCC 4.5.2 编译错误，疑似编译器 bug 。
	\todo 取消注释。
	*/
//	inline GHEvent&
//	operator=(GHEvent&&) = default;

	inline bool
	operator==(const GHEvent& rhs) const
	{
		return this->comp_eq == rhs.comp_eq && (this->comp_eq(*this, rhs));
	}

	/*!
	\brief 调用。
	*/
	using BaseType::operator();
};


/*!
\brief 事件类模板。
\note 支持顺序多播。
*/
template<class _tSender = YObject, class _tEventArgs = EventArgs>
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
	inline
	GEvent() = default;
	/*!
	\brief 复制构造：默认实现。
	\note 深复制。
	*/
	inline
	GEvent(const GEvent&) = default;
	/*!
	\brief 移动构造：默认实现。
	*/
	inline
	GEvent(GEvent&&) = default;

private:
	/*!
	\brief 私有构造：添加事件处理器。
	*/
	template<typename _tHandler>
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
	operator=(const GEvent& rhs)
	{
		this->List = rhs->List;
	}
	/*!
	\brief 移动赋值：默认实现。
	\note GCC 4.5.2 编译错误，疑似编译器 bug 。
	\todo 取消注释，使用隐式的默认定义。
	*/
//	inline GEvent&
//	operator=(GEvent&&) = default;
	inline GEvent&
	operator=(GEvent&& rhs)
	{
		this->List = std::move(rhs->List);
		return *this;
	}
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
	template<typename _type>
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
	template<typename _type>
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
	template<typename _type>
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
	template<typename _type>
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
	template<typename _type>
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
	inline PDefH1(void, Swap, GEvent& rhs) ynothrow
		ImplRet(this->List.swap(rhs))
};


//! \brief 定义事件处理器委托类型。
#define DefDelegate(_name, _tSender, _tEventArgs) \
	typedef Runtime::GHEvent<_tSender, _tEventArgs> _name;


/*!
\brief 依赖事件项类模板。
*/
template<class _tEvent>
class GDependencyEvent : public GDependency<_tEvent>
{
public:
	typedef typename GDependency<_tEvent>::PointerType PointerType;
	typedef _tEvent EventType;
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
	template<typename _type>
	inline EventType&
	operator+=(_type _arg)
	{
		return this->GetNewRef().operator+=(_arg);
	}

	/*!
	\brief 移除事件响应。
	*/
	template<typename _type>
	inline EventType&
	operator-=(_type _arg)
	{
		return this->GetNewRef().operator-=(_arg);
	}

	/*!
	\brief 添加事件响应：使用对象引用和成员函数指针。
	*/
	template<class _type>
	inline EventType&
	Add(_type& obj, void(_type::*pm)(EventArgsType&&))
	{
		return this->GetNewRef().Add(obj, pm);
	}

	/*!
	\brief 移除事件响应：目标为指定对象引用和成员函数指针。
	*/
	template<class _type>
	inline EventType&
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


//! \brief 标准多播事件类。
typedef GEvent<> Event;

//! \brief 多播事件类型。
template<class _tEventHandler>
struct GSEvent
{
	typedef Runtime::GEvent<typename _tEventHandler::SenderType,
		typename _tEventHandler::EventArgsType> EventType;
	typedef Runtime::GDependencyEvent<EventType> DependencyType;
};


//! \brief 事件类型宏。
#define EventT(_tEventHandler) \
	Runtime::GSEvent<_tEventHandler>::EventType
#define DepEventT(_tEventHandler) \
	Runtime::GSEvent<_tEventHandler>::DependencyType

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
template<class _tSender = YObject, class _tEventArgs = EventArgs>
DeclInterface(GIHEvent)
	DeclIEntry(size_t operator()(_tSender&, _tEventArgs&&) const)
EndDecl


//! \brief 事件处理器包装类模板。
template<class _tEvent = Event>
class GEventWrapper : public _tEvent,
	implements GIHEvent<YObject, EventArgs>
{
public:
	typedef _tEvent EventType;
	typedef typename EventType::SenderType SenderType;
	typedef typename EventType::EventArgsType EventArgsType;

	/*!
	\brief 委托调用。
	\warning 需要确保 EventArgs&& 引用的对象能够转换至 EventArgsType&& 引用。
	*/
	size_t
	operator()(YObject& sender, EventArgs&& e) const
	{
		SenderType* p(dynamic_cast<SenderType*>(&sender));

		
		return p ? EventType::operator()(*p,
			static_cast<EventArgsType&&>(std::move(e))) : 0;
	}
};


//! \brief 事件映射表模板。
template<typename _tEventSpace>
class GEventMap
{
public:
	typedef _tEventSpace ID;
	typedef GIHEvent<YObject, EventArgs> ItemType;
	typedef shared_ptr<ItemType> PointerType;
	typedef pair<ID, PointerType> PairType;
	typedef map<ID, PointerType> MapType;

private:
	typedef pair<typename MapType::iterator, bool> InternalPairType; \
		//!< 搜索表结果类型。

	mutable MapType m_map; //!< 映射表。

public:
	/*!
	\brief 无参数构造。
	\note 得到空实例。
	*/
	GEventMap()
		: m_map()
	{}

	/*!
	\brief 访问映射表数据。
	\throw 参数越界。
	\note 仅抛出以上异常。
	*/
	inline PointerType&
	at(const ID& k) const ythrow(std::out_of_range)
	{
		return m_map.at(k);
	}

private:
	InternalPairType
	GetSerachResult(const ID& id) const
	{
		return search_map(m_map, id);
	}

public:
	/*!
	\brief 取指定 id 对应的 _tEventHandler 类型事件。
	*/
	template<class _tEventHandler>
	typename EventT(_tEventHandler)&
	GetEvent(const ID& id) const
	{
		InternalPairType pr(GetSerachResult(id));

		typedef typename Runtime::GSEvent<_tEventHandler>::EventType
			EventType;

		if(pr.second)
			pr.first = m_map.insert(pr.first, PairType(id,
				PointerType(new GEventWrapper<EventType>())));
		return dynamic_cast<EventType&>(*pr.first->second);
	}

	template<class _tEventHandler>
	size_t
	DoEvent(const ID& id, typename _tEventHandler::SenderType& sender,
		typename _tEventHandler::EventArgsType&& e) const
	{
		InternalPairType pr(GetSerachResult(id));

		typedef typename Runtime::GSEvent<_tEventHandler>::EventType
			EventType;

		return pr.second ? 0
			: dynamic_cast<EventType&>(*pr.first->second)(sender, std::move(e));
	}

	/*!
	\brief 插入映射项。
	\return 是否插入成功。
	*/
	bool
	Insert(const ID& id, PointerType p)
	{
		return m_map.insert(PairType(id, p)).second;
	}

	/*!
	\brief 清除映射表。
	*/
	inline void
	Clear()
	{
		m_map.clear();
	}
};

YSL_END_NAMESPACE(Runtime)

//! \brief 标准事件处理器委托。
DefDelegate(HEvent, YObject, EventArgs)

YSL_END

#endif

