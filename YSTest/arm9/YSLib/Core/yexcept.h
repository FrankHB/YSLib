// YSLib::Core::YException by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-6-15 20:30:14;
// UTime = 2010-9-2 10:30;
// Version = 0.1126;


#ifndef INCLUDED_YEXCEPT_H_
#define INCLUDED_YEXCEPT_H_

// YException ：异常处理模块。

#include "yobject.h"
#include <new>
#include <typeinfo>
#include <string>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Exceptions)

class Exception : public std::exception
{
public:
	Exception();
	virtual
	~Exception() throw();
};


class GeneralError : public Exception
{
private:
	std::string str;

public:
	GeneralError(const std::string&);
	virtual
	~GeneralError() throw();

	const char*
	what() const throw();
};


class LoggedEvent : public GeneralError
{
private:
	u8 level;

public:
	LoggedEvent(const std::string&, u8 = 0);
	LoggedEvent(const GeneralError&, u8 = 0);
	virtual
	~LoggedEvent() throw();

	DefGetter(u8, Level, level);
};


YSL_END_NAMESPACE(Exceptions)

YSL_END

#endif

