// YSLib::Service::YShellInitialization by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-10-21 23:15:08 + 08:00;
// UTime = 2010-10-09 10:02 + 08:00;
// Version = 0.1695;


#include "ysinit.h"
#include "../Adaptor/yfont.h"

using namespace stdex;
using namespace platform;
using std::puts;

YSL_BEGIN

void
InitYSConsole()
{
	YConsoleInit(true, Color::Lime);
}

void
EpicFail()
{
	YDebugSetStatus();
	YDebugBegin();
}

namespace
{
	inline void
	fatalError()
	{
		terminate();
	}

	void
	printFailInfo(const char* t, const char* s)
	{
		EpicFail();

		const char* line =
			"--------------------------------";

		iprintf("%s%s%s\n%s\n%s", line, t, line, s, line);
	}

	void
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
	installFail(const char* str)
	{
		const char* title =
			"      Invalid Installation      ";
		const char* warning =
			" Please make sure the data is   "
			" stored in correct directory.   ";

		printFailInfo(title, warning);
		iprintf(" %s\n cannot be found!\n", str);
		fatalError();
	}
}

void
LibfatFail()
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


void
InitializeSystemFontCache()
{
	puts("Loading font files...");
	CreateFontCache(pDefaultFontCache, DEF_FONT_PATH);
	if(pDefaultFontCache != NULL && DEF_FONT_DIRECTORY != NULL)
		pDefaultFontCache->LoadFontFileDirectory(DEF_FONT_DIRECTORY);
	CheckSystemFontCache();
	iprintf("%u font file(s) are loaded\nsuccessfully.\n", pDefaultFontCache->GetFilesN());
	puts("Setting default font face...");

	const Drawing::Typeface* const pf(pDefaultFontCache->GetDefaultTypefacePtr());

	if(pf != NULL && Drawing::Font::InitializeDefault())
		iprintf("\"%s\":\"%s\",\nsuccessfully.\n", pf->GetFamilyName(), pf->GetStyleName());
	else
	{
		puts("failed.");
		defFontFail();
	}
}

void
DestroySystemFontCache()
{
	Drawing::DestroyFontCache(pDefaultFontCache);
	Drawing::Font::ReleaseDefault();
}

void
CheckInstall()
{
	puts("Checking installation...");
	if(!platform::direxists(DEF_DIRECTORY))
		installFail("Default data directory");
	if(!(stdex::fexists(DEF_FONT_PATH) || platform::direxists(DEF_FONT_DIRECTORY)))
		installFail("Default font");
	puts("OK!");
}

void
CheckSystemFontCache()
{
	if(!(pDefaultFontCache != NULL && pDefaultFontCache->GetTypesN() > 0))
		defFontFail();
}

YSL_END

