// YSLib::Core::YApplication by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-27 17:12:36;
// UTime = 2010-8-2 14:58;
// Version = 0.1678;


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
YLog& YLog::operator<<(const char*)
{
	return *this;
}
YLog& YLog::operator<<(const std::string& s)
{
	return operator<<(s);
}

void
YLog::Error(const char*)
{}
void
YLog::Error(const std::string& s)
{
	Error(s.c_str());
}
void
YLog::FatalError(const char*)
{
	platform::terminate();
}
void
YLog::FatalError(const std::string& s)
{
	FatalError(s.c_str());
}


const IO::YPath YApplication::CommonAppDataPath("/");
const MString YApplication::CompanyName(G_COMP_NAME);
const MString YApplication::ProductName(G_APP_NAME);
const MString YApplication::ProductVersion(G_APP_VER);

YApplication::YApplication()
: YObject(), Log(DefaultLog), DefaultMQ(), DefaultMQ_Backup(), FontCache(NULL), sShls(), hShell(NULL)
{
	ApplicationExit += Def::Destroy;
}
YApplication::~YApplication()
{
	for(SHLs::const_iterator i(sShls.begin()); i != sShls.end(); ++i)
		YDelete(*i);
	//释放主 Shell 。
//	YDelete(hShellMain);
	ApplicationExit(*this, GetZeroElement<MEventArgs>());
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
YApplication::ResetShellHandle() ythrow()
{
	if(!SetShellHandle(hShellMain))
		Log.FatalError("YApplication::ResetShellHandle();");
}

YSL_END

