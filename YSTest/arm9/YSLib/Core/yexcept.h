// YSLib::Core::YException by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-6-15 20:30:14;
// UTime = 2010-7-26 6:13;
// Version = 0.1114;


#ifndef INCLUDED_YEXCEPT_H_
#define INCLUDED_YEXCEPT_H_

// MException ：异常处理模块。

#include "yobject.h"
#include <new>
#include <typeinfo>
#include <string>

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Exceptions)

class MException : public std::exception
{
public:
	MException();
	virtual
	~MException() throw();
};


class MGeneralError : public MException
{
private:
	std::string str;

public:
	MGeneralError(const std::string&);
	virtual
	~MGeneralError() throw();

	const char*
	what() const throw();
};


class MLoggedEvent : public MGeneralError
{
private:
	u8 level;

public:
	MLoggedEvent(const std::string&, u8 = 0);
	MLoggedEvent(const MGeneralError&, u8 = 0);
	virtual
	~MLoggedEvent() throw();

	DefGetter(u8, Level, level);
};


YSL_END_NAMESPACE(Exceptions)

YSL_END

#endif

