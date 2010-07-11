// YSLib::Core::YException by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-6-15 20:30:14;
// UTime = 2010-6-15 21:10;
// Version = 0.1064;


#include "yexcept.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Exceptions)

YException::YException()
: YObject(), std::exception()
{}
YException::~YException() throw()
{}


YGeneralError::YGeneralError(const std::string& what)
: what_(what)
{}
YGeneralError::~YGeneralError() throw()
{}

const char*
YGeneralError::what() const throw()
{
	return what_.c_str();
}

YSL_END_NAMESPACE(Exceptions)

YSL_END

