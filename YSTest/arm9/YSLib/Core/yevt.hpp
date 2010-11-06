// YSLib::Core::YEvent by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-04-23 23:08:23 + 08:00;
// UTime = 2010-11-04 11:34 + 08:00;
// Version = 0.3678;


#ifndef INCLUDED_YEVT_HPP_
#define INCLUDED_YEVT_HPP_

// YEvent ：事件回调模块。

#include "yobject.h"
#include "yfunc.hpp"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Runtime)

//公用事件模板命名空间。
template<class _tSender = YObject, class _tEventArgs = EventArgs>
struct SEventTypeSpace
{
	typedef void FuncType(_tSender&, const _tEventArgs&);
	typedef FuncType* FuncPtrType;
	typedef typename std::pointer_to_binary_function<_tSender&,
		const _tEventArgs&, void> FunctorType;
};


//函数对象类：替换非静态成员二元函数的第一个参数并绑定到指定对象。
template<class _type, typename _tRet, typename _tPara, class _tNew = _type>
struct ExpandMemberFirstBinder
{
private:
	_type* _po;
	_tRet(_type::*_pm)(_tPara);

public:
	//********************************
	//名称:		ExpandMemberFirstBinder
	//全名:		YSLib::Runtime::ExpandMemberFirstBinder<_type, _tRet,
	//				_tPara, _tNew>::ExpandMemberFirstBinder
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_type& obj
	//形式参数:	_tRet(_type::*p)(_tPara)
	//功能概要:	构造：使用对象引用和成员函数指针。
	//备注:		
	//********************************
	ExpandMemberFirstBinder(_type& obj, _tRet(_type::*p)(_tPara))
		: _po(&obj), _pm(p)
	{}
	//********************************
	//名称:		ExpandMemberFirstBinder
	//全名:		YSLib::Runtime::ExpandMemberFirstBinder<_type, _tRet,
	//				_tPara, _tNew>::ExpandMemberFirstBinder
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_tNew & obj
	//形式参数:	_tRet(_type::*p)(_tPara)
	//功能概要:	构造：使用非 _type 类型对象引用和成员函数指针。
	//备注:		使用 dynamic_cast 测试类型。
	//********************************
	ExpandMemberFirstBinder(_tNew& obj, _tRet(_type::*p)(_tPara))
		: _po(dynamic_cast<_type*>(&obj)), _pm(p)
	{}

	//********************************
	//名称:		operator==
	//全名:		YSLib::Runtime::ExpandMemberFirstBinder<_type, _tRet,
	//				_tPara, _tNew>::operator==
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	const
	//形式参数:	const ExpandMemberFirstBinder & rhs
	//功能概要:	比较：相等关系。
	//备注:		
	//********************************
	bool
	operator==(const ExpandMemberFirstBinder& rhs) const
	{
		return _po == rhs._po && _pm == rhs._pm;
	}

	//********************************
	//名称:		operator()
	//全名:		YSLib::Runtime::ExpandMemberFirstBinder<_tN>::operator()
	//可访问性:	public 
	//返回类型:	_tRet
	//修饰符:	
	//形式参数:	_tN & o
	//形式参数:	_tPara arg
	//功能概要:	调用：使用替换对象引用和参数。
	//备注:		检测空指针。
	//********************************
	template<class _tN>
	_tRet
	operator()(_tN& o, _tPara arg)
	{
		if(_po && _pm)
			return (_po->*_pm)(arg);
	}
};


//标准事件处理器类模板。
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
	//********************************
	//名称:		GEventHandler
	//全名:		YSLib::Runtime::GEventHandler<_tSender, _tEventArgs>
	//				::GEventHandler
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	FuncType & f
	//功能概要:	构造：使用函数引用。
	//备注:		
	//********************************
	inline
	GEventHandler(FuncType& f)
		: Design::Function<FuncType>(f)
	{}
	//********************************
	//名称:		GEventHandler
	//全名:		YSLib::Runtime::GEventHandler::GEventHandler<_tFunc>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	: Design::Function<FuncType>(f)
	//形式参数:	_tFunc f
	//功能概要:	使用函数类型。
	//备注:		函数引用除外（匹配以上非模板重载版本）。
	//********************************
	template<class _tFunc>
	inline
	GEventHandler(_tFunc f)
		: Design::Function<FuncType>(f)
	{}
	//********************************
	//名称:		GEventHandler
	//全名:		YSLib::Runtime::GEventHandler<_tSender, _tEventArgs>
	//				::GEventHandler
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	void(_tSender::*pm)(const _tEventArgs&)
	//功能概要:	构造：使用 _tSender 的成员函数指针。
	//备注:		
	//********************************
	inline
	GEventHandler(void(_tSender::*pm)(const _tEventArgs&))
		: Design::Function<FuncType>(ExpandMemberFirst<
			_tSender, void, const _tEventArgs&>(pm))
	{}
	//********************************
	//名称:		GEventHandler
	//全名:		YSLib::Runtime::GEventHandler<_tSender, _tEventArgs>
	//				::GEventHandler<_type>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	void(_type::*pm)(const _tEventArgs&)
	//功能概要:	构造：使用成员函数指针。
	//备注:		
	//********************************
	template<class _type>
	inline
	GEventHandler(void(_type::*pm)(const _tEventArgs&))
		: Design::Function<FuncType>(ExpandMemberFirst<
			_type, void, const _tEventArgs&, _tSender>(pm))
	{}
	//********************************
	//名称:		GEventHandler
	//全名:		YSLib::Runtime::GEventHandler<_tSender, _tEventArgs>
	//				::GEventHandler
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_type& obj
	//形式参数:	void(_type::*pm)(const _tEventArgs&)
	//功能概要:	构造：使用 _tSender 类型对象引用和成员函数指针。
	//备注:		
	//********************************
	inline
	GEventHandler(_tSender& obj, void(_tSender::*pm)(const _tEventArgs&))
		: Design::Function<FuncType>(ExpandMemberFirstBinder<
			_tSender, void, const _tEventArgs&>(obj, pm))
	{}
	//********************************
	//名称:		GEventHandler
	//全名:		YSLib::Runtime::GEventHandler<_tSender, _tEventArgs>
	//				::GEventHandler<_type>
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	_type & obj
	//形式参数:	void(_type::*pm)(const _tEventArgs&)
	//功能概要:	构造：使用对象引用和成员函数指针。
	//备注:		
	//********************************
	template<class _type>
	inline
	GEventHandler(_type& obj, void(_type::*pm)(const _tEventArgs&))
		: Design::Function<FuncType>(ExpandMemberFirstBinder<
			_type, void, const _tEventArgs&, _tSender>(obj, pm))
	{}

	//********************************
	//名称:		operator()
	//全名:		YSLib::Runtime::GEventHandler<_tSender, _tEventArgs,
	//				FuncType, FuncPtrType>::operator()
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	const
	//形式参数:	_tSender & sender
	//形式参数:	const _tEventArgs & e
	//功能概要:	调用：二元函数。
	//备注:		
	//********************************
	inline void
	operator()(_tSender& sender, const _tEventArgs& e) const
	{
		Design::Function<FuncType>::operator()(sender, e);
	}
};


//事件类模板。

//多播版本。
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
	ListType List; //响应列表。

public:
	//********************************
	//名称:		GEvent
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs,
	//				IEventHandlerType, EventHandlerType>::GEvent
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	无参数构造。
	//备注:		得到空实例。
	//********************************
	GEvent()
	{}

protected:
	//********************************
	//名称:		AddRaw
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>::AddRaw
	//可访问性:	protected 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	const EventHandlerType & h
	//功能概要:	添加事件响应。
	//备注:		不检查是否已经在列表中。
	//********************************
	inline GEvent&
	AddRaw(const EventHandlerType& h)
	{
		List.push_back(h);
		return *this;
	}

public:
	//********************************
	//名称:		operator=
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>::operator=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	const EventHandlerType & h
	//功能概要:	赋值：覆盖事件响应：使用事件处理器。
	//备注:		
	//********************************
	inline GEvent&
	operator=(const EventHandlerType& h)
	{
		Clear();
		return AddRaw(h);
	}
	//********************************
	//名称:		operator=
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>::operator=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	FuncType & f
	//功能概要:	赋值：覆盖事件响应：使用函数引用。
	//备注:		
	//********************************
	inline GEvent&
	operator=(FuncType& f)
	{
		return *this = EventHandlerType(f);
	}
	//********************************
	//名称:		operator=
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>::operator=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	FunctorType f
	//功能概要:	赋值：覆盖事件响应：使用函数对象。
	//备注:		
	//********************************
	inline GEvent&
	operator=(FunctorType f)
	{
		Clear();
		return *this = EventHandlerType(f);
	}
	//********************************
	//名称:		operator=
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>
	//				::operator=<_type>
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	void
	//形式参数:	_type:: * pm
	//形式参数:	const _tEventArgs & 
	//功能概要:	赋值：覆盖事件响应：使用成员函数指针。
	//备注:		
	//********************************
	template<class _type>
	inline GEvent&
	operator=(void(_type::*pm)(const _tEventArgs&))
	{
		Clear();
		return *this = EventHandlerType(pm);
	}
	//********************************
	//名称:		operator+=
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>::operator+=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	const EventHandlerType & h
	//功能概要:	添加事件响应：使用事件处理器。
	//备注:		
	//********************************
	GEvent&
	operator+=(const EventHandlerType& h)
	{
		operator-=(h);
		return AddRaw(h);
	}
	//********************************
	//名称:		operator+=
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>::operator+=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	FuncType & f
	//功能概要:	添加事件响应：使用函数引用。
	//备注:		
	//********************************
	inline GEvent&
	operator+=(FuncType& f)
	{
		return operator+=(EventHandlerType(f));
	}
	//********************************
	//名称:		operator=
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>::operator+=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	FunctorType f
	//功能概要:	添加事件响应：使用函数对象。
	//备注:		
	//********************************
	inline GEvent&
	operator+=(FunctorType f)
	{
		return operator+=(EventHandlerType(f));
	}
	//********************************
	//名称:		operator+=
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>
	//				::operator+=<_type>
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	void(_type::*pm)(const _tEventArgs&)
	//功能概要:	添加事件响应：使用成员函数指针。
	//备注:		
	//********************************
	template<class _type>
	inline GEvent&
	operator+=(void(_type::*pm)(const _tEventArgs&))
	{
		return operator+=(EventHandlerType(pm));
	}
	//********************************
	//名称:		Add
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>::Add<_type>
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	_type& obj
	//形式参数:	void(_type::*pm)(const _tEventArgs&)
	//功能概要:	添加事件响应：使用对象引用和成员函数指针。
	//备注:		
	//********************************
	template<class _type>
	inline GEvent&
	Add(_type& obj, void(_type::*pm)(const _tEventArgs&))
	{
		return operator+=(EventHandlerType(obj, pm));
	}
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>::operator-=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	const EventHandlerType & h
	//功能概要:	移除事件响应：目标为指定事件处理器。
	//备注:		
	//********************************
	GEvent&
	operator-=(const EventHandlerType& h)
	{
		erase_all(List, h);
		return *this;
	}
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>::operator-=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	FuncType & f
	//功能概要:	移除事件响应：目标为指定函数引用。
	//备注:		
	//********************************
	inline GEvent&
	operator-=(FuncType& f)
	{
		return operator-=(EventHandlerType(f));
	}
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>::operator-=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	FunctorType f
	//功能概要:	
	//备注:		
	//********************************
	inline GEvent&
	operator-=(FunctorType f)
	{
		return operator-=(EventHandlerType(f));
	}
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>
	//				::operator-=<_type>
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	FuncType & f
	//功能概要:	移除事件响应：目标为指定成员函数指针。
	//备注:		
	//********************************
	template<class _type>
	inline GEvent&
	operator-=(void(_type::*pm)(const _tEventArgs&))
	{
		return operator-=(EventHandlerType(pm));
	}
	//********************************
	//名称:		Remove
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>
	//				::Remove<_type>
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	_type & obj
	//形式参数:	void(_type::*pm)(const _tEventArgs&)
	//功能概要:	移除事件响应：目标为指定对象引用和成员函数指针。
	//备注:		
	//********************************
	template<class _type>
	inline GEvent&
	Remove(_type& obj, void(_type::*pm)(const _tEventArgs&))
	{
		return operator-=(EventHandlerType(obj, pm));
	}

	//********************************
	//名称:		operator()
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>::operator()
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	const
	//形式参数:	_tSender & sender
	//形式参数:	const _tEventArgs & e
	//功能概要:	调用函数。
	//备注:		
	//********************************
	void
	operator()(_tSender& sender, const _tEventArgs& e) const
	{
		for(typename list<EventHandlerType>
				::const_iterator i(List.begin());
				i != List.end(); ++i)
			(*i)(sender, e);
	}

	//********************************
	//名称:		GetSize
	//全名:		YSLib::Runtime::GEvent<_bMulticast, _tSender, _tEventArgs>
	//				::GetSize
	//可访问性:	public 
	//返回类型:	typename ListType::size_type
	//修饰符:	const
	//功能概要:	取列表中的响应数。
	//备注:		
	//********************************
	inline typename ListType::size_type
	GetSize() const
	{
		return List.size();
	}

	//********************************
	//名称:		Clear
	//全名:		YSLib::Runtime::GEvent<true, _tSender, _tEventArgs>::Clear
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清除：移除所有事件响应。
	//备注:		
	//********************************
	inline void
	Clear()
	{
		List.clear();
	}
};

//单播版本。
template<class _tSender, class _tEventArgs>
struct GEvent<false, _tSender, _tEventArgs>
	: public GEventHandler<_tSender, _tEventArgs>
{
	typedef SEventTypeSpace<_tSender, _tEventArgs> SEventType;
	typedef typename SEventType::FuncType FuncType;
	typedef typename SEventType::FunctorType FunctorType;
	typedef typename SEventType::EventHandlerType EventHandlerType;

	//********************************
	//名称:		GEvent
	//全名:		YSLib::Runtime::GEvent<false, _tSender, _tEventArgs>::GEvent
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	无参数构造。
	//备注:		
	//********************************
	inline
	GEvent()
		: EventHandlerType()
	{}

	//********************************
	//名称:		operator=
	//全名:		YSLib::Runtime::GEvent<false, _tSender, _tEventArgs>::operator=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	const EventHandlerType * p
	//功能概要:	
	//备注:		
	//********************************
	inline GEvent&
	operator=(const EventHandlerType* p)
	{
		EventHandlerType::_ptr = p;
		return *this;
	}
	//********************************
	//名称:		operator+=
	//全名:		YSLib::Runtime::GEvent<false, _tSender, _tEventArgs>::operator+=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	const EventHandlerType & h
	//功能概要:	添加事件响应：使用事件处理器。
	//备注:		
	//********************************
	inline GEvent&
	operator+=(const EventHandlerType& h)
	{
		return *this = h;
	}
	//********************************
	//名称:		operator+=
	//全名:		YSLib::Runtime::GEvent<false, _tSender, _tEventArgs>::operator+=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	FuncType & f
	//功能概要:	添加事件响应：使用函数引用。
	//备注:		
	//********************************
	inline GEvent&
	operator+=(FuncType& f)
	{
		return operator+=(EventHandlerType(f));
	}
	//********************************
	//名称:		operator+=
	//全名:		YSLib::Runtime::GEvent<false, _tSender, _tEventArgs>::operator+=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	FunctorType f
	//功能概要:	添加事件响应：使用函数对象。
	//备注:		
	//********************************
	inline GEvent&
	operator+=(FunctorType f)
	{
		return operator+=(EventHandlerType(f));
	}
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Runtime::GEvent<false, _tSender, _tEventArgs>::operator-=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	const EventHandlerType & h
	//功能概要:	移除事件响应：目标为指定事件处理器。
	//备注:		
	//********************************
	GEvent&
	operator-=(const EventHandlerType& h)
	{
		if(EventHandlerType::_ptr == h)
			EventHandlerType::_ptr = NULL;
		return *this;
	}
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Runtime::GEvent<false, _tSender, _tEventArgs>::operator-=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	FuncType & f
	//功能概要:	移除事件响应：目标为指定函数引用。
	//备注:		
	//********************************
	inline GEvent&
	operator-=(FuncType& f)
	{
		return operator-=(EventHandlerType(f));
	}
	//********************************
	//名称:		operator-=
	//全名:		YSLib::Runtime::GEvent<false, _tSender, _tEventArgs>::operator-=
	//可访问性:	public 
	//返回类型:	GEvent&
	//修饰符:	
	//形式参数:	FunctorType f
	//功能概要:	移除事件响应：目标为指定函数对象。
	//备注:		
	//********************************
	inline GEvent&
	operator-=(FunctorType f)
	{
		return operator-=(EventHandlerType(f));
	}

	//********************************
	//名称:		GetHandlerPtr
	//全名:		YSLib::Runtime::GEvent<false, _tSender, _tEventArgs>
	//				::GetHandlerPtr
	//可访问性:	public 
	//返回类型:	EventHandlerType*
	//修饰符:	
	//功能概要:	取事件处理器指针。
	//备注:		
	//********************************
	inline EventHandlerType*
	GetHandlerPtr()
	{
		return EventHandlerType::_ptr;
	}

	//********************************
	//名称:		Clear
	//全名:		YSLib::Runtime::GEvent<false, _tSender, _tEventArgs>::Clear
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清除：移除所有事件响应。
	//备注:		
	//********************************
	inline void
	Clear()
	{
		EventHandlerType::_ptr = NULL;
	}
};


//定义事件处理器委托。
#define DefDelegate(_name, _tSender, _tEventArgs)\
	typedef Runtime::GEventHandler<_tSender, _tEventArgs> _name;


#ifdef YSL_EVENT_MULTICAST

//标准多播事件类。
typedef GEvent<> Event;

//多播事件类型。
template<class EventHandlerType>
struct GSEventTemplate
{
	typedef Runtime::GEvent<true,
		typename EventHandlerType::SenderType,
		typename EventHandlerType::EventArgsType
	> EventType;
};

#else

//标准单播事件类。
typedef GEvent<false> Event;

//单播事件类型。
template<class EventHandlerType>
struct GSEventTemplate
{
	typedef Runtime::GEvent<false,
		typename EventHandlerType::SenderType,
		typename EventHandlerType::EventArgsType
	> EventType;
};

#endif


//定义事件。
#	define DefEvent(EventHandlerType, _name) \
		Runtime::GSEventTemplate<EventHandlerType>::EventType _name;

//定义事件接口。
#	define DeclIEventEntry(EventHandlerType, _name) \
		DeclIEntry(const Runtime::GSEventTemplate<EventHandlerType> \
			::EventType& _yJOIN(Get, _name)() const)

//定义事件访问器。
#	define DefEventGetter(EventHandlerType, _name) \
		DefGetter(const Runtime::GSEventTemplate<EventHandlerType> \
			::EventType&, _name, _name)


//事件映射表模板。
template<class _tEventSpace, class _tEvent = Event>
class GEventMap
{
public:
	typedef typename _tEventSpace::EventID ID;
	typedef _tEvent Event;

private:
	map<ID, Event> Map; //映射表。

public:
	//********************************
	//名称:		GEventMap
	//全名:		YSLib::Runtime::GEventMap<_tEventSpace, _tEvent>::GEventMap
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	无参数构造。
	//备注:		得到空实例。
	//********************************
	GEventMap()
	: Map()
	{}

	//********************************
	//名称:		operator[]
	//全名:		YSLib::Runtime::GEventMap<_tEventSpace, _tEvent>::operator[]
	//可访问性:	public 
	//返回类型:	Event&
	//修饰符:	
	//形式参数:	const ID & id
	//功能概要:	取指定 id 对应的事件。
	//备注:		
	//********************************
	inline Event&
	operator[](const ID& id)
	{
		return Map[id];
	}

	//********************************
	//名称:		Clear
	//全名:		YSLib::Runtime::GEventMap<_tEventSpace, _tEvent>::Clear
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清除映射表。
	//备注:		
	//********************************
	inline void
	Clear()
	{
		Map.clear();
	}
};


//标准事件回调函数抽象类模板。
template<class _tResponser, class _tEventArgs>
struct GAHEventCallback : public _tEventArgs
{
	//********************************
	//名称:		GAHEventCallback
	//全名:		YSLib::Runtime::GAHEventCallback<_tResponser, _tEventArgs>::GAHEventCallback
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const _tEventArgs & e
	//功能概要:	构造：使用事件参数。
	//备注:		
	//********************************
	inline explicit
	GAHEventCallback(const _tEventArgs& e)
	: _tEventArgs(e)
	{}
	DeclIEntry(bool operator()(_tResponser&))
};

YSL_END_NAMESPACE(Runtime)

//标准事件处理器委托。
DefDelegate(EventHandler, YObject, EventArgs)

YSL_END

#endif

