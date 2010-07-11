// YSLib::Service::YShellInitialization by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-10-21 23:15:08;
// UTime = 2010-6-23 3:42;
// Version = 0.1558;


#include "ysinit.h"
#include "../Adapter/yfont.h"

using namespace stdex;
using namespace platform;

YSL_BEGIN

static bool bMainConsole(false);

void
InitYSConsole()
{
	if(!bMainConsole)
		YConsoleInit(true, RGB15(31, 31, 31), RGB15(0, 31, 0));
}


static inline void
fatalError()
{
	terminate();
}

void
epicFail()
{
	YDebugSetStatus();
	//videoSetMode(MODE_0_2D);
	YDebugBegin();
}

static void
printFailInfo(const char* t, const char* s)
{
	epicFail();

	const char* line =
		"--------------------------------";

	iprintf("%s%s%s\n%s\n%s", line, t, line, s, line);
}


static void
defFontFail()
{
	const char* title =
		"    Fontface Caching Failure    ";
	const char* warning =
		" Please make sure the fonts are "
		" stored in correct directory.   ";

	printFailInfo(title, warning);
	fatalError();
}

void
libfatFail()
{
	const char* title =
		"         LibFAT Failure         ";
	const char* warning =
		" An error is preventing the     "
		" program from accessing         "
		" external files.                "
		"                                "
		" If you're using an emulator,   "
		" make sure it supports DLDI     "
		" and that it's activated.       "
		"                                "
		" In case you're seeing this     "
		" screen on a real DS, make sure "
		" you've applied the correct     "
		" DLDI patch (most modern        "
		" flashcards do this             "
		" automatically).                "
		"                                "
		" Note: Some cards only          "
		" autopatch .nds files stored in "
		" the root folder of the card.   ";

	printFailInfo(title, warning);
	fatalError();
}

static void
installFail()
{
	const char* title =
		"      Invalid Installation      ";
	const char* warning =
		" Please make sure the data is   "
		" stored in correct directory.   ";

	printFailInfo(title, warning);
	fatalError();
}

void
InitialSystemFontCache()
{
	iputs("Loading font files...");
	CreateFontCache(pDefaultFontCache, DEF_FONT_PATH);
	if(pDefaultFontCache && DEF_FONT_DIRECTORY)
		pDefaultFontCache->LoadFontFileDirectory(DEF_FONT_DIRECTORY);
	checkSystemFontCache();
	iprintf("%u font file(s) are loaded\nsuccessfully.\n", pDefaultFontCache->GetFilesN());

	const Drawing::YTypeface* pf = pDefaultFontCache->GetDefaultTypefacePtr();

	if(pf)
	{
		Drawing::YFont::InitialDefault();
		iprintf("Default font face \n\"%s\":\"%s\"\nis set successfully.\n", pf->GetFamilyName(), pf->GetStyleName());
	}
	else
	{
		iputs("Set default font face failed.");
		defFontFail();
	}
}

void
checkInstall()
{
	iputs("Checking installation...");
	if(!(fexists(DEF_FONT_PATH) || direxists(DEF_FONT_DIRECTORY)))
		installFail();
	iputs("OK!");
}

void
checkSystemFontCache()
{
	if(!(pDefaultFontCache && pDefaultFontCache->GetTypesN()))
		defFontFail();
}

YSL_END

