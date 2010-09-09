// YSLib::Core::YException by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-6-15 20:30:14;
// UTime = 2010-9-2 10:30;
// Version = 0.1134;


#include "yexcept.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Exceptions)

Exception::Exception()
: std::exception()
{}
Exception::~Exception() throw()
{}


GeneralError::GeneralError(const std::string& s)
: str(s)
{}
GeneralError::~GeneralError() throw()
{}

const char*
GeneralError::what() const throw()
{
	return str.c_str();
}


LoggedEvent::LoggedEvent(const std::string& s, u8 l)
: GeneralError(s), level(l)
{}
LoggedEvent::LoggedEvent(const GeneralError& e, u8 l)
: GeneralError(e), level(l)
{}
LoggedEvent::~LoggedEvent() throw()
{}

YSL_END_NAMESPACE(Exceptions)

YSL_END

