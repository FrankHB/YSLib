// YSLib::Core::YException by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-6-15 20:30:14;
// UTime = 2010-9-23 15:38;
// Version = 0.1140;


#ifndef INCLUDED_YEXCEPT_H_
#define INCLUDED_YEXCEPT_H_

// YException ：异常处理模块。

#include "yobject.h"
#include <new>
#include <typeinfo>
#include <string>

YSL_BEGIN

// YSLib 异常基类。
class Exception : public std::exception
{
public:
	Exception();
	virtual
	~Exception() throw();
};


//一般异常事件类。
class GeneralEvent : public Exception
{
private:
	std::string str;

public:
	GeneralEvent(const std::string&);
	virtual
	~GeneralEvent() throw();

	const char*
	what() const throw();
};


//记录异常事件类。
class LoggedEvent : public GeneralEvent
{
private:
	u8 level;

public:
	LoggedEvent(const std::string&, u8 = 0);
	LoggedEvent(const GeneralEvent&, u8 = 0);
	virtual
	~LoggedEvent() throw();

	DefGetter(u8, Level, level);
};

YSL_END

#endif

