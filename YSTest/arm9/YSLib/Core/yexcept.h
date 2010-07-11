// YSLib::Core::YException by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-6-15 20:30:14;
// UTime = 2010-6-24 0:38;
// Version = 0.1049;


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
	std::string what_;

public:
	YGeneralError(const std::string& what);
	virtual
	~YGeneralError() throw();

	const char*
	what() const throw();
};

YSL_END_NAMESPACE(Exceptions)

YSL_END

#endif

