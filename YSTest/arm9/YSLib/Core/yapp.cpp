// YSLib::Core::YApplication by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-27 17:12:36 + 08:00;
// UTime = 2010-11-08 18:14 + 08:00;
// Version = 0.1946;


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
YLog& YLog::operator<<(const string& s)
{
	return operator<<(s);
}

void
YLog::Error(const char*)
{}
void
YLog::Error(const string& s)
{
	Error(s.c_str());
}

void
YLog::FatalError(const char*)
{
	terminate();
}
void
YLog::FatalError(const string& s)
{
	FatalError(s.c_str());
}


const IO::Path YApplication::CommonAppDataPath("/");
const String YApplication::CompanyName(G_COMP_NAME);
const String YApplication::ProductName(G_APP_NAME);
const String YApplication::ProductVersion(G_APP_VER);

YApplication::YApplication()
	: YObject(),
	Log(DefaultLog), pMessageQueue(new YMessageQueue),
		pMessageQueueBackup(new YMessageQueue), sShls(), hShell(NULL)
{
	ApplicationExit += Def::Destroy;
}
YApplication::~YApplication() ythrow()
{
	for(SHLs::const_iterator i(sShls.begin()); i != sShls.end(); ++i)
		YDelete(*i);
	//释放主 Shell 。
//	YDelete(hShellMain);
	EventArgs e;

	ApplicationExit(*this, e);
	delete pMessageQueue;
	delete pMessageQueueBackup;
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

YMessageQueue&
YApplication::GetDefaultMessageQueue() ythrow(LoggedEvent)
{
	if(!pMessageQueue)
		throw LoggedEvent("Get default message queue failed @@ YApplication.");
	return *pMessageQueue;
}
YMessageQueue&
YApplication::GetBackupMessageQueue() ythrow(LoggedEvent)
{
	if(!pMessageQueueBackup)
		throw LoggedEvent("Get backup message queue failed @@ YApplication.");
	return *pMessageQueueBackup;
}

bool
YApplication::SetShellHandle(HSHL h)
{
	if(Contains(h))
	{
		if(hShell == h)
			return false;
		if(hShell)
			hShell->OnDeactivated(Message(
				NULL, SM_DEACTIVATED, 0xF0, handle_cast<WPARAM>(hShell)));
		hShell = h;
		hShell->OnActivated(Message(
				NULL, SM_ACTIVATED, 0xF0, handle_cast<WPARAM>(h)));
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

