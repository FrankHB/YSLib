// YSLib::Core::YException by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-6-15 20:30:14;
// UTime = 2010-7-26 6:10;
// Version = 0.1124;


#include "yexcept.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Exceptions)

MException::MException()
: std::exception()
{}
MException::~MException() throw()
{}


MGeneralError::MGeneralError(const std::string& s)
: str(s)
{}
MGeneralError::~MGeneralError() throw()
{}

const char*
MGeneralError::what() const throw()
{
	return str.c_str();
}


MLoggedEvent::MLoggedEvent(const std::string& s, u8 l)
: MGeneralError(s), level(l)
{}
MLoggedEvent::MLoggedEvent(const MGeneralError& e, u8 l)
: MGeneralError(e), level(l)
{}
MLoggedEvent::~MLoggedEvent() throw()
{}

YSL_END_NAMESPACE(Exceptions)

YSL_END

