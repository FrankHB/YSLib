// YSLib::Core::YException by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-6-15 20:30:14;
// UTime = 2010-7-14 0:18;
// Version = 0.1063;


#ifndef INCLUDED_YEXCEPT_H_
#define INCLUDED_YEXCEPT_H_

// YException ：异常处理模块。

#include "yobject.h"
#include <new>
#include <typeinfo>
#include <string>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Exceptions)

class YException : public YObject, public std::exception
{
public:
	YException();
	virtual
	~YException() throw();
};


class YGeneralError : public YException
{
private:
	std::string str;

public:
	YGeneralError(const std::string&);
	virtual
	~YGeneralError() throw();

	const char*
	what() const throw();
};


class YLoggedError : public YGeneralError
{
private:
	u8 level;

public:
	YLoggedError(const std::string&, u8 = 0);
	YLoggedError(const YGeneralError&, u8 = 0);
	virtual
	~YLoggedError() throw();

	DefGetter(u8, Level, level);
};


YSL_END_NAMESPACE(Exceptions)

YSL_END

#endif

