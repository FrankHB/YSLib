// YSLib::Core::YApplication by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-27 17:12:36;
// UTime = 2010-7-10 6:44;
// Version = 0.1640;


#include "yapp.h"

YSL_BEGIN

YLog::YLog()
{}
YLog::~YLog()
{}

YLog& YLog::operator<<(char)
{
	return *this;
}
YLog& YLog::operator<<(const std::string&)
{
	return *this;
}

void
YLog::Error(const std::string&)
{}
void
YLog::FatalError(const std::string&)
{
	platform::terminate();
}


const IO::YPath YApplication::CommonAppDataPath("/");
const YString YApplication::CompanyName(G_COMP_NAME);
const YString YApplication::ProductName(G_APP_NAME);
const YString YApplication::ProductVersion(G_APP_VER);

YApplication::YApplication()
: YObject(), log(DefaultLog), fc(NULL), mq(DefaultMQ), sShls(), hShell(NULL)
{
	ApplicationExit += Def::Destroy;
}
YApplication::~YApplication()
{
	for(SHLs::const_iterator i(sShls.begin()); i != sShls.end(); ++i)
		YDelete(*i);
	//释放主 Shell 。
//	YDelete(hShellMain);
	ApplicationExit(*this, YEventArgs::Empty);
}

void
YApplication::operator+=(YShell& shl)
{
	sShls.insert(&shl);
}
bool
YApplication::operator-=(YShell& shl)
{
	return sShls.erase(&shl);
}

bool
YApplication::Contains(HSHL hShl) const
{
	return sShls.count(hShl);
}

bool
YApplication::SetShellHandle(HSHL h)
{
	if(Contains(h))
	{
		if(hShell == h)
			return false;
		if(hShell)
			hShell->OnDeactivated(MMSG(NULL, SM_DEACTIVATED, 0xF0, handle_cast<WPARAM>(hShell)));
		hShell = h;
		hShell->OnActivated(MMSG(NULL, SM_ACTIVATED, 0xF0, handle_cast<WPARAM>(h)));
	}
	return h;
}

void
YApplication::ResetShellHandle()
{
	if(!SetShellHandle(hShellMain))
		log.FatalError("YApplication::ResetShellHandle();");
}

YSL_END

