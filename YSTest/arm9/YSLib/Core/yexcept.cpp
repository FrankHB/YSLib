// YSLib::Core::YException by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-06-15 20:30:14 + 08:00;
// UTime = 2010-10-09 10:38 + 08:00;
// Version = 0.1158;


#include "yexcept.h"

YSL_BEGIN

Exception::Exception()
	: std::exception()
{}
Exception::~Exception() throw()
{}


GeneralEvent::GeneralEvent(const string& s)
	: Exception(),
	str(s)
{}
GeneralEvent::~GeneralEvent() throw()
{}

const char*
GeneralEvent::what() const throw()
{
	return str.c_str();
}


LoggedEvent::LoggedEvent(const string& s, u8 l)
	: GeneralEvent(s),
	level(l)
{}
LoggedEvent::LoggedEvent(const GeneralEvent& e, u8 l)
	: GeneralEvent(e),
	level(l)
{}
LoggedEvent::~LoggedEvent() throw()
{}

YSL_END

