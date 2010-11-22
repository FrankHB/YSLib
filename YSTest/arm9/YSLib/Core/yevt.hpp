/*
	Copyright (C) by Franksoft 2010.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file yevt.hpp
\ingroup Core
\brief 事件回调模块。
\version 0.3729;
\author FrankHB<frankhb1989@gmail.com>
\par 创建时间:
	2010-04-23 23:08:23 + 08:00;
\par 修改时间:
	2010-11-19 10:30 + 08:00;
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
struct SEventTypeSpace
{
	typedef void FuncType(_tSender&, _tEventArgs&);
	typedef FuncType* FuncPtrType;
	typedef typename std::pointer_to_binary_function<_tSender&,
		_tEventArgs&, void> FunctorType;
};


//! \brief 标准事件处理器类模板。
template<class _tSender = YObject, class _tEventArgs = EventArgs>
class GEventHandler
	: public Design::Function<typename SEventTypeSpace<
		_tSender, _tEventArgs>::FuncType>
{
public:
	typedef _tSender SenderType;
	typedef _tEventArgs EventArgsType;
	typedef SEventTypeSpace<_tSender, _tEventArgs> SEventType;
	typedef typename SEventType::FuncType FuncType;

public:
	/*!
	\brief 构造：使用函数引用。
	*/
	inline
	GEventHandler(FuncType& f)
		: Design::Function<FuncType>(f)
	{}
	/*!
	\brief 使用函数类型。
	\note 函数引用除外（匹配以上非模板重载版本）。
	*/
	template<class _tFunc>
	inline
	GEventHandler(_tFunc f)
		: Design::Function<FuncType>(f)
	{}
	/*!
	\brief 构造：使用 _tSender 的成员函数指针。
	*/
	inline
	GEventHandler(void(_tSender::*pm)(_tEventArgs&))
		: Design::Function<FuncType>(ExpandMemberFirst<
			_tSender, void, _tEventArgs&>(pm))
	{}
	/*!
	\brief 构造：使用成员函数指针。
	*/
	template<class _type>
	inline
	GEventHandler(void(_type::*pm)(_tEventArgs&))
		: Design::Function<FuncType>(ExpandMemberFirst<
			_type, void, _tEventArgs&, _tSender>(pm))
	{}
	/*!
	\brief 构造：使用 _tSender 类型对象引用和成员函数指针。
	*/
	inline
	GEventHandler(_tSender& obj, void(_tSender::*pm)(_tEventArgs&))
		: Design::Function<FuncType>(ExpandMemberFirstBinder<
			_tSender, void, _tEventArgs&>(obj, pm))
	{}
	/*!
	\brief 构造：使用对象引用和成员函数指针。
	*/
	template<class _type>
	inline
	GEventHandler(_type& obj, void(_type::*pm)(_tEventArgs&))
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
	typedef SEventTypeSpace<_tSender, _tEventArgs> SEventType;
	typedef typename SEventType::FuncType FuncType;
	typedef typename SEventType::FunctorType FunctorType;
	typedef GEventHandler<_tSender, _tEventArgs> EventHandlerType;
	typedef list<EventHandlerType> ListType;

protected:
	ListType List; //!< 响应列表。

public:
	/*!
	\brief 无参数构造。
	\note 得到空实例。
	*/
	GEvent()
	{}

protected:
	/*!
	\brief 添加事件响应。
	\note 不检查是否已经在列表中。
	*/
	inline GEvent&
	AddRaw(const EventHandlerType& h)
	{
		List.push_back(h);
		return *this;
	}

public:
	/*!
	\brief 赋值：覆盖事件响应：使用事件处理器。
	*/
	inline GEvent&
	operator=(const EventHandlerType& h)
	{
		Clear();
		return AddRaw(h);
	}
	/*!
	\brief 赋值：覆盖事件响应：使用函数引用。
	*/
	inline GEvent&
	operator=(FuncType& f)
	{
		return *this = EventHandlerType(f);
	}
	/*!
	\brief 赋值：覆盖事件响应：使用函数对象。
	*/
	inline GEvent&
	operator=(FunctorType f)
	{
		Clear();
		return *this = EventHandlerType(f);
	}
	/*!
	\brief 赋值：覆盖事件响应：使用成员函数指针。
	*/
	template<class _type>
	inline GEvent&
	operator=(void(_type::*pm)(_tEventArgs&))
	{
		Clear();
		return *this = EventHandlerType(pm);
	}
	/*!
	\brief 添加事件响应：使用事件处理器。
	*/
	GEvent&
	operator+=(const EventHandlerType& h)
	{
		operator-=(h);
		return AddRaw(h);
	}
	/*!
	\brief 添加事件响应：使用函数引用。
	*/
	inline GEvent&
	operator+=(FuncType& f)
	{
		return operator+=(EventHandlerType(f));
	}
	/*!
	\brief 添加事件响应：使用函数对象。
	*/
	inline GEvent&
	operator+=(FunctorType f)
	{
		return operator+=(EventHandlerType(f));
	}
	/*!
	\brief 添加事件响应：使用成员函数指针。
	*/
	template<class _type>
	inline GEvent&
	operator+=(void(_type::*pm)(_tEventArgs&))
	{
		return operator+=(EventHandlerType(pm));
	}
	/*!
	\brief 添加事件响应：使用对象引用和成员函数指针。
	*/
	template<class _type>
	inline GEvent&
	Add(_type& obj, void(_type::*pm)(_tEventArgs&))
	{
		return operator+=(EventHandlerType(obj, pm));
	}
	/*!
	\brief 移除事件响应：目标为指定事件处理器。
	*/
	GEvent&
	operator-=(const EventHandlerType& h)
	{
		erase_all(List, h);
		return *this;
	}
	/*!
	\brief 移除事件响应：目标为指定函数引用。
	*/
	inline GEvent&
	operator-=(FuncType& f)
	{
		return operator-=(EventHandlerType(f));
	}
	/*!
	\brief 
	*/
	inline GEvent&
	operator-=(FunctorType f)
	{
		return operator-=(EventHandlerType(f));
	}
	/*!
	\brief 移除事件响应：目标为指定成员函数指针。
	*/
	template<class _type>
	inline GEvent&
	operator-=(void(_type::*pm)(_tEventArgs&))
	{
		return operator-=(EventHandlerType(pm));
	}
	/*!
	\brief 移除事件响应：目标为指定对象引用和成员函数指针。
	*/
	template<class _type>
	inline GEvent&
	Remove(_type& obj, void(_type::*pm)(_tEventArgs&))
	{
		return operator-=(EventHandlerType(obj, pm));
	}

	/*!
	\brief 调用函数。
	*/
	void
	operator()(_tSender& sender, _tEventArgs& e) const
	{
		for(typename list<EventHandlerType>
				::const_iterator i(List.begin());
				i != List.end(); ++i)
			(*i)(sender, e);
	}

	/*!
	\brief 取列表中的响应数。
	*/
	inline typename ListType::size_type
	GetSize() const
	{
		return List.size();
	}

	/*!
	\brief 清除：移除所有事件响应。
	*/
	inline void
	Clear()
	{
		List.clear();
	}
};

//! \brief 单播事件类模板。
template<class _tSender, class _tEventArgs>
struct GEvent<false, _tSender, _tEventArgs>
	: public GEventHandler<_tSender, _tEventArgs>
{
	typedef SEventTypeSpace<_tSender, _tEventArgs> SEventType;
	typedef typename SEventType::FuncType FuncType;
	typedef typename SEventType::FunctorType FunctorType;
	typedef typename SEventType::EventHandlerType EventHandlerType;

	/*!
	\brief 无参数构造。
	*/
	inline
	GEvent()
		: EventHandlerType()
	{}

	/*!
	\brief 
	*/
	inline GEvent&
	operator=(const EventHandlerType* p)
	{
		EventHandlerType::_ptr = p;
		return *this;
	}
	/*!
	\brief 添加事件响应：使用事件处理器。
	*/
	inline GEvent&
	operator+=(const EventHandlerType& h)
	{
		return *this = h;
	}
	/*!
	\brief 添加事件响应：使用函数引用。
	*/
	inline GEvent&
	operator+=(FuncType& f)
	{
		return operator+=(EventHandlerType(f));
	}
	/*!
	\brief 添加事件响应：使用函数对象。
	*/
	inline GEvent&
	operator+=(FunctorType f)
	{
		return operator+=(EventHandlerType(f));
	}
	/*!
	\brief 移除事件响应：目标为指定事件处理器。
	*/
	GEvent&
	operator-=(const EventHandlerType& h)
	{
		if(EventHandlerType::_ptr == h)
			EventHandlerType::_ptr = NULL;
		return *this;
	}
	/*!
	\brief 移除事件响应：目标为指定函数引用。
	*/
	inline GEvent&
	operator-=(FuncType& f)
	{
		return operator-=(EventHandlerType(f));
	}
	/*!
	\brief 移除事件响应：目标为指定函数对象。
	*/
	inline GEvent&
	operator-=(FunctorType f)
	{
		return operator-=(EventHandlerType(f));
	}

	/*!
	\brief 取事件处理器指针。
	*/
	inline EventHandlerType*
	GetHandlerPtr()
	{
		return EventHandlerType::_ptr;
	}

	/*!
	\brief 清除：移除所有事件响应。
	*/
	inline void
	Clear()
	{
		EventHandlerType::_ptr = NULL;
	}
};


//! \brief 定义事件处理器委托。
#define DefDelegate(_name, _tSender, _tEventArgs)\
	typedef Runtime::GEventHandler<_tSender, _tEventArgs> _name;


#ifdef YSL_EVENT_MULTICAST

//! \brief 标准多播事件类。
typedef GEvent<> Event;

//! \brief 多播事件类型。
template<class EventHandlerType>
struct GSEventTemplate
{
	typedef Runtime::GEvent<true,
		typename EventHandlerType::SenderType,
		typename EventHandlerType::EventArgsType
	> EventType;
};

#else

//! \brief 标准单播事件类。
typedef GEvent<false> Event;

//! \brief 单播事件类型。
template<class EventHandlerType>
struct GSEventTemplate
{
	typedef Runtime::GEvent<false,
		typename EventHandlerType::SenderType,
		typename EventHandlerType::EventArgsType
	> EventType;
};

#endif


//! \brief 定义事件。
#	define DefEvent(EventHandlerType, _name) \
		Runtime::GSEventTemplate<EventHandlerType>::EventType _name;

//! \brief 定义事件接口。
#	define DeclIEventEntry(EventHandlerType, _name) \
		DeclIEntry(const Runtime::GSEventTemplate<EventHandlerType> \
			::EventType& _yJOIN(Get, _name)() const)

//! \brief 定义事件访问器。
#	define DefEventGetter(EventHandlerType, _name) \
		DefGetter(const Runtime::GSEventTemplate<EventHandlerType> \
			::EventType&, _name, _name)


//! \brief 事件映射表模板。
template<class _tEventSpace, class _tEvent = Event>
class GEventMap
{
public:
	typedef typename _tEventSpace::EventID ID;
	typedef _tEvent Event;

private:
	map<ID, Event> Map; //!< 映射表。

public:
	/*!
	\brief 无参数构造。
	\note 得到空实例。
	*/
	GEventMap()
	: Map()
	{}

	/*!
	\brief 取指定 id 对应的事件。
	*/
	inline Event&
	operator[](const ID& id)
	{
		return Map[id];
	}

	/*!
	\brief 清除映射表。
	*/
	inline void
	Clear()
	{
		Map.clear();
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
DefDelegate(EventHandler, YObject, EventArgs)

YSL_END

#endif

