// YSLib::Core::YException by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-6-15 20:30:14;
// UTime = 2010-9-23 15:38;
// Version = 0.1142;


#include "yexcept.h"

YSL_BEGIN

Exception::Exception()
: std::exception()
{}
Exception::~Exception() throw()
{}


GeneralEvent::GeneralEvent(const std::string& s)
: str(s)
{}
GeneralEvent::~GeneralEvent() throw()
{}

const char*
GeneralEvent::what() const throw()
{
	return str.c_str();
}


LoggedEvent::LoggedEvent(const std::string& s, u8 l)
: GeneralEvent(s), level(l)
{}
LoggedEvent::LoggedEvent(const GeneralEvent& e, u8 l)
: GeneralEvent(e), level(l)
{}
LoggedEvent::~LoggedEvent() throw()
{}

YSL_END

