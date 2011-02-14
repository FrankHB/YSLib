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
\brief 事件回调模块。
\version 0.3958;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-04-23 23:08:23 + 08:00;
\par 修改时间:
	2011-02-13 18:12 + 08:00;
\par 字符集:
	UTF-8;
\par 模块名称:
	YSLib::Core::YEvent;
*/


#ifndef INCLUDED_YEVT_HPP_
#define INCLUDED_YEVT_HPP_

#include "yobject.h"
#include "yfunc.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Runtime)

//! \brief 公用事件模板命名空间。
template<class _tSender = YObject, class _tEventArgs = EventArgs>
struct GSEventTypeSpace
{
	typedef void FuncType(_tSender&, _tEventArgs&);
	typedef FuncType* FuncPtrType;
	typedef typename std::pointer_to_binary_function<_tSender&,
		_tEventArgs&, void> FunctorType;
};


//! \brief 标准事件处理器类模板。
template<class _tSender = YObject, class _tEventArgs = EventArgs>
class GHEvent
	: public Design::Function<typename GSEventTypeSpace<
		_tSender, _tEventArgs>::FuncType>
{
public:
	typedef _tSender SenderType;
	typedef _tEventArgs EventArgsType;
	typedef GSEventTypeSpace<_tSender, _tEventArgs> SEventType;
	typedef typename SEventType::FuncType FuncType;

public:
	/*!
	\brief 构造：使用函数引用。
	*/
	inline
	GHEvent(FuncType& f)
		: Design::Function<FuncType>(f)
	{}
	/*!
	\brief 使用函数类型。
	\note 函数引用除外（匹配以上非模板重载版本）。
	*/
	template<class _tFunc>
	inline
	GHEvent(_tFunc f)
		: Design::Function<FuncType>(f)
	{}
	/*!
	\brief 构造：使用 _tSender 的成员函数指针。
	*/
	inline
	GHEvent(void(_tSender::*pm)(_tEventArgs&))
		: Design::Function<FuncType>(ExpandMemberFirst<
			_tSender, void, _tEventArgs&>(pm))
	{}
	/*!
	\brief 构造：使用成员函数指针。
	*/
	template<class _type>
	inline
	GHEvent(void(_type::*pm)(_tEventArgs&))
		: Design::Function<FuncType>(ExpandMemberFirst<
			_type, void, _tEventArgs&, _tSender>(pm))
	{}
	/*!
	\brief 构造：使用 _tSender 类型对象引用和成员函数指针。
	*/
	inline
	GHEvent(_tSender& obj, void(_tSender::*pm)(_tEventArgs&))
		: Design::Function<FuncType>(ExpandMemberFirstBinder<
			_tSender, void, _tEventArgs&>(obj, pm))
	{}
	/*!
	\brief 构造：使用对象引用和成员函数指针。
	*/
	template<class _type>
	inline
	GHEvent(_type& obj, void(_type::*pm)(_tEventArgs&))
		: Design::Function<FuncType>(ExpandMemberFirstBinder<
			_type, void, _tEventArgs&, _tSender>(obj, pm))
	{}

	/*!
	\brief 调用：二元函数。
	*/
	inline void
	operator()(_tSender& sender, _tEventArgs& e) const
	{
		Design::Function<FuncType>::operator()(sender, e);
	}
};


//事件类模板。

//! \brief 多播事件类模板。
template<
	bool _bMulticast = true,
	class _tSender = YObject, class _tEventArgs = EventArgs
>
class GEvent
{
public:
	typedef _tSender SenderType;
	typedef _tEventArgs EventArgsType;
	typedef GSEventTypeSpace<_tSender, _tEventArgs> SEventType;
	typedef typename SEventType::FuncType FuncType;
	typedef typename SEventType::FunctorType FunctorType;
	typedef GHEvent<_tSender, _tEventArgs> _tEventHandler;
	typedef list<_tEventHandler> ListType;

protected:
	ListType List; //!< 响应列表。

public:
	/*!
	\brief 无参数构造。
	\note 空实现；得到空实例。
	*/
	GEvent()
	{}
	/*!
	\brief 复制构造。
	\note 深复制。
	*/
	GEvent(const GEvent& rhs)
		: List(rhs.List)
	{}

protected:
	/*!
	\brief 添加事件响应。
	\note 不检查是否已经在列表中。
	*/
	inline GEvent&
	AddRaw(const _tEventHandler& h)
	{
		List.push_back(h);
		return *this;
	}

public:
	/*!
	\brief 复制赋值：覆盖事件响应。
	*/
	GEvent&
	operator=(const GEvent& rhs)
	{
		GEvent e(rhs);

		e.Swap(*this);
		return *this;
	}
	/*!
	\brief 赋值：覆盖事件响应：使用事件处理器。
	*/
	inline GEvent&
	operator=(const _tEventHandler& h)
	{
		this->Clear();
		return this->AddRaw(h);
	}
	/*!
	\brief 赋值：覆盖事件响应：使用函数引用。
	*/
	inline GEvent&
	operator=(FuncType& f)
	{
		return *this = _tEventHandler(f);
	}
	/*!
	\brief 赋值：覆盖事件响应：使用函数对象。
	*/
	inline GEvent&
	operator=(FunctorType f)
	{
		this->Clear();
		return *this = _tEventHandler(f);
	}
	/*!
	\brief 赋值：覆盖事件响应：使用成员函数指针。
	*/
	template<class _type>
	inline GEvent&
	operator=(void(_type::*pm)(_tEventArgs&))
	{
		this->Clear();
		return *this = _tEventHandler(pm);
	}
	/*!
	\brief 添加事件响应：使用事件处理器。
	*/
	GEvent&
	operator+=(const _tEventHandler& h)
	{
		this->operator-=(h);
		return this->AddRaw(h);
	}
	/*!
	\brief 添加事件响应：使用函数引用。
	*/
	inline GEvent&
	operator+=(FuncType& f)
	{
		return this->operator+=(_tEventHandler(f));
	}
	/*!
	\brief 添加事件响应：使用函数对象。
	*/
	inline GEvent&
	operator+=(FunctorType f)
	{
		return this->operator+=(_tEventHandler(f));
	}
	/*!
	\brief 添加事件响应：使用成员函数指针。
	*/
	template<class _type>
	inline GEvent&
	operator+=(void(_type::*pm)(_tEventArgs&))
	{
		return this->operator+=(_tEventHandler(pm));
	}
	/*!
	\brief 添加事件响应：使用对象引用和成员函数指针。
	*/
	template<class _type>
	inline GEvent&
	Add(_type& obj, void(_type::*pm)(_tEventArgs&))
	{
		return this->operator+=(_tEventHandler(obj, pm));
	}
	/*!
	\brief 移除事件响应：目标为指定事件处理器。
	*/
	GEvent&
	operator-=(const _tEventHandler& h)
	{
		ystdex::erase_all(this->List, h);
		return *this;
	}
	/*!
	\brief 移除事件响应：目标为指定函数引用。
	*/
	inline GEvent&
	operator-=(FuncType& f)
	{
		return this->operator-=(_tEventHandler(f));
	}
	/*!
	\brief 
	*/
	inline GEvent&
	operator-=(FunctorType f)
	{
		return this->operator-=(_tEventHandler(f));
	}
	/*!
	\brief 移除事件响应：目标为指定成员函数指针。
	*/
	template<class _type>
	inline GEvent&
	operator-=(void(_type::*pm)(_tEventArgs&))
	{
		return operator-=(_tEventHandler(pm));
	}
	/*!
	\brief 移除事件响应：目标为指定对象引用和成员函数指针。
	*/
	template<class _type>
	inline GEvent&
	Remove(_type& obj, void(_type::*pm)(_tEventArgs&))
	{
		return operator-=(_tEventHandler(obj, pm));
	}

	/*!
	\brief 调用函数。
	*/
	void
	operator()(_tSender& sender, _tEventArgs& e) const
	{
		for(typename list<_tEventHandler>
				::const_iterator i(this->List.begin());
				i != this->List.end(); ++i)
			(*i)(sender, e);
	}

	/*!
	\brief 取列表中的响应数。
	*/
	inline typename ListType::size_type
	GetSize() const
	{
		return this->List.size();
	}

	/*!
	\brief 清除：移除所有事件响应。
	*/
	inline void
	Clear()
	{
		this->List.clear();
	}

	/*
	\brief 交换。
	*/
	void
	Swap(GEvent& rhs) ythrow()
	{
		rhs.List.swap(this->List);
	}
};

//! \brief 单播事件类模板。
template<class _tSender, class _tEventArgs>
struct GEvent<false, _tSender, _tEventArgs>
	: public GHEvent<_tSender, _tEventArgs>
{
	typedef _tSender SenderType;
	typedef _tEventArgs EventArgs;
	typedef GSEventTypeSpace<_tSender, _tEventArgs> SEventType;
	typedef typename SEventType::FuncType FuncType;
	typedef typename SEventType::FunctorType FunctorType;
	typedef typename SEventType::_tEventHandler _tEventHandler;

	/*!
	\brief 无参数构造。
	*/
	inline
	GEvent()
		: _tEventHandler()
	{}

	/*!
	\brief 
	*/
	inline GEvent&
	operator=(const _tEventHandler* p)
	{
		_tEventHandler::_ptr = p;
		return *this;
	}
	/*!
	\brief 添加事件响应：使用事件处理器。
	*/
	inline GEvent&
	operator+=(const _tEventHandler& h)
	{
		return *this = h;
	}
	/*!
	\brief 添加事件响应：使用函数引用。
	*/
	inline GEvent&
	operator+=(FuncType& f)
	{
		return operator+=(_tEventHandler(f));
	}
	/*!
	\brief 添加事件响应：使用函数对象。
	*/
	inline GEvent&
	operator+=(FunctorType f)
	{
		return operator+=(_tEventHandler(f));
	}
	/*!
	\brief 移除事件响应：目标为指定事件处理器。
	*/
	GEvent&
	operator-=(const _tEventHandler& h)
	{
		if(_tEventHandler::_ptr == h)
			_tEventHandler::_ptr = NULL;
		return *this;
	}
	/*!
	\brief 移除事件响应：目标为指定函数引用。
	*/
	inline GEvent&
	operator-=(FuncType& f)
	{
		return operator-=(_tEventHandler(f));
	}
	/*!
	\brief 移除事件响应：目标为指定函数对象。
	*/
	inline GEvent&
	operator-=(FunctorType f)
	{
		return operator-=(_tEventHandler(f));
	}

	/*!
	\brief 取事件处理器指针。
	*/
	inline _tEventHandler*
	GetHandlerPtr()
	{
		return _tEventHandler::_ptr;
	}

	/*!
	\brief 清除：移除所有事件响应。
	*/
	inline void
	Clear()
	{
		_tEventHandler::_ptr = NULL;
	}
};


//! \brief 定义事件处理器委托类型。
#define DefDelegate(_name, _tSender, _tEventArgs) \
	typedef Runtime::GHEvent<_tSender, _tEventArgs> _name;


#ifdef YSL_EVENT_MULTICAST

//! \brief 标准多播事件类。
typedef GEvent<> Event;

//! \brief 多播事件类型。
template<class _tEventHandler>
struct GSEventTemplate
{
	typedef Runtime::GEvent<true,
		typename _tEventHandler::SenderType,
		typename _tEventHandler::EventArgsType
	> EventType;
};

#else

//! \brief 标准单播事件类。
typedef GEvent<false> Event;

//! \brief 单播事件类型。
template<class _tEventHandler>
struct GSEventTemplate
{
	typedef Runtime::GEvent<false,
		typename _tEventHandler::SenderType,
		typename _tEventHandler::EventArgsType
	> EventType;
};

#endif


//! \brief 事件类型宏。
#define EventT(_tEventHandler) \
	Runtime::GSEventTemplate<_tEventHandler>::EventType

//! \brief 声明事件。
#define DeclEvent(_tEventHandler, _name) \
	EventT(_tEventHandler) _name;

//! \brief 声明事件引用。
#define DeclEventRef(_tEventHandler, _name) \
	EventT(_tEventHandler)& _name;

//! \brief 声明事件接口函数。
#define DeclIEventEntry(_tEventHandler, _name) \
	DeclIEntry(const EventT(_tEventHandler)& _yJOIN(Get, _name)() const)

//! \brief 定义事件访问器。
#define DefEventGetter(_tEventHandler, _name, _member) \
	DefGetter(const EventT(_tEventHandler)&, _name, _member)
#define DefEventGetterBase(_tEventHandler, _name, _base) \
	DefGetterBase(const EventT(_tEventHandler)&, _name, _base)
#define DefEventGetterMember(_tEventHandler, _name, _member) \
	DefGetterMember(const EventT(_tEventHandler)&, _name, _member)

//! \brief 定义事件可修改访问器。
#define DefMutableEventGetter(_tEventHandler, _name, _member) \
	DefMutableGetter(EventT(_tEventHandler)&, _name, _member)
#define DefMutableEventGetterBase(_tEventHandler, _name, _base) \
	DefMutableGetterBase(EventT(_tEventHandler)&, _name, _base)
#define DefMutableEventGetterMember(_tEventHandler, _name, _member) \
	DefMutableGetterMember(EventT(_tEventHandler)&, _name, _member)

//! \brief 事件处理器接口模板。
template<class _tSender = YObject, class _tEventArgs = EventArgs>
DeclInterface(GIHEvent)
	DeclIEntry(void operator()(_tSender&, _tEventArgs&) const)
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

	void
	operator()(YObject& sender, EventArgs& e) const
	{
		SenderType* p(dynamic_cast<SenderType*>(&sender));

		//需要确保 EventArgs 对象能够转换至 _tEventArgs 对象。
		if(p)
			_tEvent::operator()(*p, reinterpret_cast<EventArgsType&>(e));
	}
};


//! \brief 事件映射表模板。
template<class _tEventSpace, class _tEvent = Event>
class GEventMap
{
public:
	typedef typename _tEventSpace::EventID ID;
	typedef _tEvent EventType;
	typedef typename EventType::SenderType SenderType;
	typedef typename EventType::EventArgsType EventArgsType;
	typedef GIHEvent<YObject, EventArgs> ItemType;

private:
	mutable map<ID, SmartPtr<ItemType> > m_map; //!< 映射表。

public:
	/*!
	\brief 无参数构造。
	\note 得到空实例。
	*/
	GEventMap()
		: m_map()
	{}

	/*!
	\brief 取指定 id 对应的 _tEventHandler 类型事件。
	*/
	template<class _tEventHandler>
	typename Runtime::GSEventTemplate<_tEventHandler>::EventType&
	GetEvent(const ID& id) const
	{
		typedef typename Runtime::GSEventTemplate<_tEventHandler>::EventType
			EventType;

		std::pair<typename map<ID, SmartPtr<ItemType> >::iterator, bool>
			pr(ystdex::search_map(m_map, id));

		if(pr.second)
			pr.first = m_map.insert(pr.first,
				std::pair<ID, SmartPtr<ItemType> >(id,
				new GEventWrapper<EventType>()));
		return dynamic_cast<EventType&>(*pr.first->second);
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


//! \brief 标准事件回调函数抽象类模板。
template<class _tResponser, class _tEventArgs>
struct GAHEventCallback : public _tEventArgs
{
	/*!
	\brief 构造：使用事件参数。
	*/
	inline explicit
	GAHEventCallback(_tEventArgs& e)
		: _tEventArgs(e)
	{}
	DeclIEntry(bool operator()(_tResponser&))
};

YSL_END_NAMESPACE(Runtime)

//! \brief 标准事件处理器委托。
DefDelegate(HEvent, YObject, EventArgs)

YSL_END

#endif

