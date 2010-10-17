// YSLib::Core::YEvent by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-04-23 23:08:23 + 08:00;
// UTime = 2010-10-17 00:46 + 08:00;
// Version = 0.3250;


#ifndef INCLUDED_YEVT_HPP_
#define INCLUDED_YEVT_HPP_

// YEvent ：事件回调模块。

#include "yobject.h"
#include "yfunc.hpp"
//#include <list>
//#include <set>
//#include <map>
#include "../Adaptor/cont.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Runtime)

//事件处理器接口模板。
template<
	class _tSender = YObject, class _tEventArgs = MEventArgs,
	typename _fEventHandler = void(_tSender&, const _tEventArgs&), typename _pfEventHandler = _fEventHandler*
>
DeclBasedInterface(GIEventHandler, GIClonable<GIEventHandler<_tSender, _tEventArgs, _fEventHandler, _pfEventHandler> >)
	typedef _tSender SenderType;
	typedef _tEventArgs EventArgsType;

	DeclIEntry(void operator()(_tSender&, const _tEventArgs&) const)

	DeclIEntry(std::size_t GetSizeOf() const)
EndDecl

//标准事件处理器接口。
typedef GIEventHandler<> IEventHandler;


//非成员函数事件处理器类模板。
template<
	class _tSender = YObject, class _tEventArgs = MEventArgs,
	typename _fEventHandler = void(_tSender&, const _tEventArgs&), typename _pfEventHandler = _fEventHandler*
>
class GHEventNormal : private GHBase<_pfEventHandler>,
	implements GIEventHandler<_tSender, _tEventArgs>
{
public:
	typedef GIEventHandler<_tSender, _tEventArgs> IHEventHandlerType;

	inline
	GHEventNormal()
	: GHBase<_pfEventHandler>(NULL)
	{}
	inline explicit
	GHEventNormal(const _fEventHandler& f)
	: GHBase<_pfEventHandler>(f)
	{}
	template<class _giEventHandler>
	inline explicit
	GHEventNormal(const _giEventHandler& h)
	: GHBase<_pfEventHandler>(h.GetPtr())
	{}
	virtual DefEmptyDtor(GHEventNormal)

	ImplI(GIEventHandler<_tSender, _tEventArgs>) void
	operator()(_tSender& sender, const _tEventArgs& e) const
	{
		if(GHBase<_pfEventHandler>::GetPtr())
			GHBase<_pfEventHandler>::GetPtr()(sender, e);
	}

	ImplI(GIEventHandler<_tSender, _tEventArgs>) DefGetter(std::size_t, SizeOf, sizeof(*this))

	ImplI(GIEventHandler<_tSender, _tEventArgs>) IHEventHandlerType*
	Clone() const
	{
		return new GHEventNormal(*this);
	}
};

//成员函数事件处理器类模板。
template<
	class _type,
	class _tSender = YObject, class _tEventArgs = MEventArgs,
	typename _fEventHandler = void(_tSender&, const _tEventArgs&), typename _pfEventHandler = _fEventHandler*
>
class GHEventMember : implements GIEventHandler<_tSender, _tEventArgs>
{
public:
	typedef GIEventHandler<_tSender, _tEventArgs> IHEventHandlerType;
	typedef void(_type::*MemFunPtrType)(const _tEventArgs&);

private:
	MemFunPtrType _ptrMemFun;

public:
	inline explicit
	GHEventMember(MemFunPtrType pm = NULL)
	:_ptrMemFun(pm)
	{}
	virtual DefEmptyDtor(GHEventMember)

	ImplI(GIEventHandler<_tSender, _tEventArgs>) void
	operator()(_tSender& sender, const _tEventArgs& e) const
	{
		try
		{
			if(_ptrMemFun)
				(dynamic_cast<_type&>(sender).*_ptrMemFun)(e);
		}
		catch(std::bad_cast&)
		{}
	}

	ImplI(GIEventHandler<_tSender, _tEventArgs>) DefGetter(std::size_t, SizeOf, sizeof(*this))

	ImplI(GIEventHandler<_tSender, _tEventArgs>) IHEventHandlerType*
	Clone() const
	{
		return new GHEventMember(*this);
	}
};

//成员函数对象绑定事件处理器类模板。
template<
	class _type,
	class _tSender = YObject, class _tEventArgs = MEventArgs,
	typename _fEventHandler = void(_tSender&, const _tEventArgs&), typename _pfEventHandler = _fEventHandler*
>
class GHEventMemberBinder : implements GIEventHandler<_tSender, _tEventArgs>
{
public:
	typedef GIEventHandler<_tSender, _tEventArgs> IHEventHandlerType;
	typedef void(_type::*MemFunPtrType)(const _tEventArgs&);

private:
	_type& _obj;
	MemFunPtrType _ptrMemFun;

public:
	inline explicit
	GHEventMemberBinder(_type& obj, MemFunPtrType pm = NULL)
	:_obj(obj), _ptrMemFun(pm)
	{}
	virtual DefEmptyDtor(GHEventMemberBinder)

	ImplI(GIEventHandler<_tSender, _tEventArgs>) void
	operator()(_tSender& sender, const _tEventArgs& e) const
	{
		if(_ptrMemFun)
			(_obj.*_ptrMemFun)(e);
	}

	ImplI(GIEventHandler<_tSender, _tEventArgs>) DefGetter(std::size_t, SizeOf, sizeof(*this))

	ImplI(GIEventHandler<_tSender, _tEventArgs>) IHEventHandlerType*
	Clone() const
	{
		return new GHEventMemberBinder(*this);
	}
};

template<class _tSender, class _tEventArgs>
bool
operator==(GIEventHandler<_tSender, _tEventArgs>& l, GIEventHandler<_tSender, _tEventArgs>& r)
{
	if(l.GetSizeOf() != r.GetSizeOf())
		return false;
	return !std::memcmp(&l, &r, l.GetSizeOf());
}


//事件处理器类模板。
template<
	class _tSender = YObject, class _tEventArgs = MEventArgs,
	typename _fEventHandler = void(_tSender&, const _tEventArgs&), typename _pfEventHandler = _fEventHandler*
>
class GEventHandler : implements GIEventHandler<_tSender, _tEventArgs>
{
public:
	typedef GIEventHandler<_tSender, _tEventArgs> IHEventHandlerType;

protected:
	IHEventHandlerType* _h_ptr;

public:
	inline
	GEventHandler()
	: _h_ptr(NULL)
	{}
	inline explicit
	GEventHandler(const _fEventHandler& f)
	: _h_ptr(new GHEventNormal<_tSender, _tEventArgs>(f))
	{}
	// GCC 4.5.0 之前版本对构造函数模板解析有误，无法显式实例化此实例。
	template<class _type>
	inline
//	GEventHandler(typename GHEventMember<_type, _tSender, _tEventArgs>::MemFunPtrType pm)
	GEventHandler(void(_type::*pm)(const _tEventArgs&))
	: _h_ptr(new GHEventMember<_type, _tSender, _tEventArgs>(pm))
	{}
	template<class _type>
	inline
//	GEventHandler(_type& obj, typename GHEventMember<_type, _tSender, _tEventArgs>::MemFunPtrType pm)
	GEventHandler(_type& obj, void(_type::*pm)(const _tEventArgs&))
	: _h_ptr(new GHEventMemberBinder<_type, _tSender, _tEventArgs>(obj, pm))
	{}
	inline explicit
	GEventHandler(const GEventHandler& h)
	: _h_ptr(h._h_ptr->Clone())
	{}
	inline virtual
	~GEventHandler()
	{
		delete _h_ptr;
	}

	bool
	operator==(const GEventHandler& h)
	{
		IHEventHandlerType* const pl(_h_ptr);
		IHEventHandlerType* const pr(h._h_ptr);

		return pl == pr || ((pl && pr) && *pl == *pr);
	}
	ImplI(GIEventHandler<_tSender, _tEventArgs>) void
	operator()(_tSender& sender, const _tEventArgs& e) const
	{
		if(_h_ptr)
			(*_h_ptr)(sender, e);
	}

	ImplI(GIEventHandler<_tSender, _tEventArgs>) DefGetter(std::size_t, SizeOf, _h_ptr->GetSizeOf())

	ImplI(GIEventHandler<_tSender, _tEventArgs>) IHEventHandlerType*
	Clone() const
	{
		return new GEventHandler(*this);
	}
};


//标准事件处理器类。
typedef GEventHandler<> EventHandler;


//事件类模板。

//多播版本。
template<
	bool _bMulticast = true,
	class _tSender = YObject, class _tEventArgs = MEventArgs,
	class _iEventHandler = GIEventHandler<_tSender, _tEventArgs>,
	class _tEventHandler = GEventHandler<_tSender, _tEventArgs>
>
class GEvent
{
public:
	typedef void FuncType(_tSender&, const _tEventArgs&);
	typedef list<_tEventHandler> ListType;

protected:
	list<_tEventHandler> EventHandlerList;

public:
	GEvent()
	{}

	GEvent&
	operator+=(const _tEventHandler& h)
	{
		operator-=(h);
		EventHandlerList.push_back(h);
		return *this;
	}
	inline GEvent&
	operator+=(const FuncType& f)
	{
		return operator+=(_tEventHandler(f));
	}
	template<class _type>
	inline GEvent&
	//使显式实例化非必要。
//	operator+=(typename GHEventMember<_type, _tSender, _tEventArgs>::MemFunPtrType pm)
	operator+=(void(_type::*pm)(const _tEventArgs&))
	{
		return operator+=(_tEventHandler(pm));
	//	return operator+=(_tEventHandler<_type>(pm));
	}
	template<class _type>
	inline GEvent&
	Add(_type& obj, void(_type::*pm)(const _tEventArgs&))
	{
		return operator+=(_tEventHandler(obj, pm));
	//	return operator+=(_tEventHandler<_type>(obj, pm));
	}
	GEvent&
	operator-=(const _tEventHandler& h)
	{
		erase_all(EventHandlerList, h);
		return *this;
	}
	inline GEvent&
	operator-=(const FuncType& f)
	{
		return operator-=(_tEventHandler(f));
	}
	template<class _type>
	inline GEvent&
	operator-=(typename GHEventMember<_type, _tSender, _tEventArgs>::MemFunPtrType pm)
	{
		return operator-=(_tEventHandler(pm));
	}
	template<class _type>
	inline GEvent&
	Remove(_type& obj, void(_type::*pm)(const _tEventArgs&))
	{
		return operator-=(_tEventHandler(obj, pm));
	}

	void
	operator()(_tSender& sender, const _tEventArgs& e) const
	{
		for(typename list<_tEventHandler>::const_iterator i(EventHandlerList.begin()); i != EventHandlerList.end(); ++i)
			(*i)(sender, e);
	}
};

//单播版本。
template<
	class _tSender, class _tEventArgs,
	class _iEventHandler,
	class _tEventHandler
>
struct GEvent<false, _tSender, _tEventArgs, _iEventHandler, _tEventHandler> : public _tEventHandler
{
	typedef void FuncType(_tSender&, const _tEventArgs&);

	inline GEvent()
	: _tEventHandler()
	{}

	inline GEvent&
	operator=(const _tEventHandler* p)
	{
		_tEventHandler::_ptr = p;
	}
	inline GEvent&
	operator+=(const _tEventHandler& h)
	{
		return *this = h;
	}
	inline GEvent&
	operator+=(const FuncType& f)
	{
		return operator+=(_tEventHandler(f));
	}
	GEvent&
	operator-=(const _tEventHandler& h)
	{
		if(_tEventHandler::_ptr == h)
			_tEventHandler::_ptr = NULL;
		return *this;
	}
	inline GEvent&
	operator-=(const FuncType& f)
	{
		return operator-=(_tEventHandler(f));
	}

	inline _tEventHandler*
	GetHandlerPtr()
	{
		return _tEventHandler::_ptr;
	}
};


//定义事件处理器委托。
#define DefDelegate(_name, _tSender, _tEventArgs)\
	typedef Runtime::GEventHandler<_tSender, _tEventArgs> _name;


#ifdef YSL_EVENT_MULTICAST

//标准多播事件类。
typedef GEvent<> Event;

//多播事件类型。
template<class _tEventHandler>
struct GSEventTemplate
{
	typedef Runtime::GEvent<true,
		typename _tEventHandler::SenderType,
		typename _tEventHandler::EventArgsType
	> EventType;
};

#else

//标准单播事件类。
typedef GEvent<false> Event;

//单播事件类型。
template<class _tEventHandler>
struct GSEventTemplate
{
	typedef Runtime::GEvent<false,
		typename _tEventHandler::SenderType,
		typename _tEventHandler::EventArgsType
	> EventType;
};

#endif


//定义事件。
#	define DefEvent(_tEventHandler, _name)\
		Runtime::GSEventTemplate<_tEventHandler>::EventType _name;

//定义事件接口。
#	define DeclIEventEntry(_tEventHandler, _name)\
		DeclIEntry(const Runtime::GSEventTemplate<_tEventHandler>::EventType& _yJOIN(Get, _name)() const)

//定义事件访问器。
#	define DefEventGetter(_tEventHandler, _name)\
		DefGetter(const Runtime::GSEventTemplate<_tEventHandler>::EventType&, _name, _name)


//事件映射表模板。
template<class _tEventSpace, class _tEvent = Event>
class GEventMap
{
public:
	typedef typename _tEventSpace::EventID ID;
	typedef _tEvent Event;

private:
	map<ID, Event> Map;

public:
	GEventMap()
	: Map()
	{}

	inline Event&
	operator[](const ID& id)
	{
		return Map[id];
	}

	inline void
	clear()
	{
		Map.clear();
	}
};


//标准事件回调函数抽象类模板。
template<class _tResponser, class _tEventArgs>
struct GAHEventCallback : public _tEventArgs
{
	inline explicit
	GAHEventCallback(const _tEventArgs& e)
	: _tEventArgs(e)
	{}
	DeclIEntry(bool operator()(_tResponser&))
};

YSL_END_NAMESPACE(Runtime)

//标准事件处理器委托。
DefDelegate(EventHandler, YObject, MEventArgs)

YSL_END

#endif

