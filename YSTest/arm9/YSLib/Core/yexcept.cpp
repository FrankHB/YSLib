// YSLib::Core::YException by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-6-15 20:30:14;
// UTime = 2010-7-14 0:38;
// Version = 0.1080;


#include "yexcept.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Exceptions)

YException::YException()
: YObject(), std::exception()
{}
YException::~YException() throw()
{}


YGeneralError::YGeneralError(const std::string& s)
: str(s)
{}
YGeneralError::~YGeneralError() throw()
{}

const char*
YGeneralError::what() const throw()
{
	return str.c_str();
}


YLoggedError::YLoggedError(const std::string& s, u8 l)
: YGeneralError(s), level(l)
{}
YLoggedError::YLoggedError(const YGeneralError& e, u8 l)
: YGeneralError(e), level(l)
{}
YLoggedError::~YLoggedError() throw()
{}

YSL_END_NAMESPACE(Exceptions)

YSL_END

