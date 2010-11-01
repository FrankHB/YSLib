// YSLib::Core::YException by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-06-15 20:30:14 + 08:00;
// UTime = 2010-10-31 12:21 + 08:00;
// Version = 0.1198;


#ifndef INCLUDED_YEXCEPT_H_
#define INCLUDED_YEXCEPT_H_

// YException ：异常处理模块。

#include "yobject.h"
#include <new>
#include <typeinfo>
#include "../Adaptor/cont.h"
//#include <string>

YSL_BEGIN

// YSLib 异常基类。
class Exception : public std::exception
{
public:
	//********************************
	//名称:		Exception
	//全名:		YSLib::Exception::Exception
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//功能概要:	无参数构造。
	//备注:		
	//********************************
	Exception();
	//********************************
	//名称:		~Exception
	//全名:		YSLib::Exception::~Exception
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	throw()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//********************************
	virtual
	~Exception() throw();
};


//一般异常事件类。
class GeneralEvent : public Exception
{
private:
	string str;

public:
	//********************************
	//名称:		GeneralEvent
	//全名:		YSLib::GeneralEvent::GeneralEvent
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const string &
	//功能概要:	构造：使用异常字符串。
	//备注:		
	//********************************
	GeneralEvent(const string&);
	//********************************
	//名称:		~GeneralEvent
	//全名:		YSLib::GeneralEvent::~GeneralEvent
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	throw()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//********************************
	virtual
	~GeneralEvent() throw();

	//********************************
	//名称:		what
	//全名:		YSLib::GeneralEvent::what
	//可访问性:	public 
	//返回类型:	const char*
	//修饰符:	const throw()
	//功能概要:	取异常字符串。
	//备注:		无异常抛出。
	//********************************
	const char*
	what() const throw();
};


//记录异常事件类。
class LoggedEvent : public GeneralEvent
{
private:
	u8 level;

public:
	//********************************
	//名称:		LoggedEvent
	//全名:		YSLib::LoggedEvent::LoggedEvent
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const string &
	//形式参数:	u8
	//功能概要:	构造：使用异常字符串和异常等级。
	//备注:		
	//********************************
	LoggedEvent(const string&, u8 = 0);
	//********************************
	//名称:		LoggedEvent
	//全名:		YSLib::LoggedEvent::LoggedEvent
	//可访问性:	public 
	//返回类型:	
	//修饰符:	
	//形式参数:	const GeneralEvent &
	//形式参数:	u8
	//功能概要:	构造：使用一般异常事件对象和异常等级。
	//备注:		
	//********************************
	LoggedEvent(const GeneralEvent&, u8 = 0);
	//********************************
	//名称:		~LoggedEvent
	//全名:		YSLib::LoggedEvent::~LoggedEvent
	//可访问性:	virtual public 
	//返回类型:	
	//修饰符:	throw()
	//功能概要:	析构。
	//备注:		无异常抛出。
	//********************************
	virtual
	~LoggedEvent() throw();

	DefGetter(u8, Level, level);
};

YSL_END

#endif

