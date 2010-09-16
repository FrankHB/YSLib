// YSLib::Helper -> Global by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-22 15:28:52;
// UTime = 2010-9-16 23:28;
// Version = 0.2365;


#include "yglobal.h"
#include "../ysbuild.h"
#include <exception>
//#include <clocale>

using namespace platform;

YSL_BEGIN

using namespace Runtime;

//全局常量。
const SDST SCRW(SCREEN_WIDTH), SCRH(SCREEN_HEIGHT);

//全局变量。
YScreen *pScreenUp, *pScreenDown;
YDesktop *pDesktopUp, *pDesktopDown;
YLog DefaultLog;

//全局变量映射。
YScreen*& pDefaultScreen(pScreenUp);
YDesktop*& pDefDesktop(pDesktopUp);
YApplication& theApp(YApplication::GetApp());
YMessageQueue& DefaultMQ(theApp.DefaultMQ);
YMessageQueue& DefaultMQ_Backup(theApp.DefaultMQ_Backup);
YFontCache*& pDefaultFontCache(theApp.FontCache);
YApplication* const pApp(&theApp);
const HSHL hShellMain(new YShellMain); //取主 Shell 句柄。


namespace
{
	bool
	operator!=(const KeysInfo& a, const KeysInfo& b)
	{
		return a.up != b.up || a.down != b.down || a.held != b.held;
	}

	//图形用户界面输入等待函数。
	void
	WaitForGUIInput()
	{
		static KeysInfo Key;
		static CursorInfo TouchPos_Old, TouchPos;
		static Message /*InputMessage_Old, */InputMessage;

		if(Key.held & Key::Touch)
			TouchPos_Old = TouchPos;
		scanKeys();
		WriteKeysInfo(Key, TouchPos);

		const SPoint pt(ToSPoint(Key.held & Key::Touch ? TouchPos : TouchPos_Old));

		if(DefaultMQ.empty() || Key != *reinterpret_cast<KeysInfo*>(InputMessage.GetWParam()) || pt != InputMessage.GetCursorLocation())
			InsertMessage((InputMessage = Message(NULL, SM_INPUT, 0x40, reinterpret_cast<WPARAM>(&Key), 0, pt)));
	/*
		InputMessage = Message(NULL, SM_INPUT, 0x40, reinterpret_cast<WPARAM>(&Key), 0, ToSPoint(tp));

		if(InputMessage != InputMessage_Old)
		{
			InsertMessage(InputMessage);
			InputMessage_Old = InputMessage;
		}
	*/
	}
}


void
Def::Idle()
{
	if(hShellMain->insRefresh)
		InsertMessage(NULL, SM_SCRREFRESH, 0x80, hShellMain->scrType, 0);
	WaitForGUIInput();
}

bool
Def::InitVideo()
{
	ResetVideo();
	//设置显示模式。
	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);
	Def::InitScreenAll();
	return true;
}
void
Def::InitScreenAll()
{
	//设置主显示引擎渲染上屏。
	lcdMainOnTop();
//	lcdMainOnBottom();
//	lcdSwap();
	InitScrUp();
	InitScrDown();
}
void
Def::InitScrUp()
{
	//初始化背景，并得到屏幕背景ID 。
	pScreenUp->bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

	//获得屏幕背景所用的显存地址。
	pScreenUp->SetPtr(static_cast<BitmapPtr>(bgGetGfxPtr(pScreenUp->bg)));
}
void
Def::InitScrDown()
{
	//初始化背景，并得到屏幕背景ID 。
	pScreenDown->bg = bgInitSub(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);

	//获得屏幕背景所用的显存地址。
	pScreenDown->SetPtr(static_cast<BitmapPtr>(bgGetGfxPtr(pScreenDown->bg)));
}

bool
Def::InitConsole(YScreen& scr, Drawing::PixelType fc, Drawing::PixelType bc)
{
	if(&scr == pScreenUp)
		YConsoleInit(true, fc, bc);
	else if(&scr == pScreenDown)
		YConsoleInit(false, fc, bc);
	else
		return false;
	return true;
}

void
Def::Destroy(YObject&, const MEventArgs&)
{
	//释放默认字体资源。
	DestroySystemFontCache();

	//释放显示设备。
	delete pDesktopUp;
	delete pScreenUp;
	delete pDesktopDown;
	delete pScreenDown;
}

LRES
Def::ShlProc(HSHL hShl, const Message& msg)
{
	return hShl->ShlProc(msg);
}


void
Terminate(int exitCode)
{
	exit(exitCode);
}


namespace
{
	//初始化函数。
	void
	YInit()
	{
		//设置默认终止函数。
		std::set_terminate(terminate);

		//启用设备。
		powerOn(POWER_ALL);

		//启用 LibNDS 默认异常处理。
		defaultExceptionHandler();

		//初始化主控制台。
		InitYSConsole();

	/*	if(std::setlocale(LC_ALL, "zh_CN.GBK") == NULL)
		{
			EpicFail();
			platform::yprintf("setlocale() with %s failed.\n", "zh_CN.GBK");
			terminate();
		}*/

		//初始化文件系统。
		//初始化 EFSLib 和 LibFAT 。
		//当 .nds 文件大于32MB时， EFS 行为异常。
	#ifdef USE_EFS
		if(!EFS_Init(EFS_AND_FAT | EFS_DEFAULT_DEVICE, NULL))
		{
			//如果初始化 EFS 失败则初始化 FAT 。
	#endif
			if(!fatInitDefault())
			{
				LibfatFail();
				terminate();
			}
			IO::ChDir(DEF_DIRECTORY);
	#ifdef USE_EFS
		}
	#endif

		//检查程序是否被正确安装。
		CheckInstall();

		//初始化系统字体资源。
		InitializeSystemFontCache();

		//初始化显示设备。
		if((pScreenUp = new YScreen(SCRW, SCRH)))
			pDesktopUp = new YDesktop(*pScreenUp);
		else //初始化上屏失败。
			throw Exceptions::LoggedEvent("Initialization of up screen failed.");
		if((pScreenDown = new YScreen(SCRW, SCRH)))
			pDesktopDown = new YDesktop(*pScreenDown);
		else //初始化下屏失败。
			throw Exceptions::LoggedEvent("Initialization of down screen failed.");

		//注册全局应用程序对象。
		theApp.ResetShellHandle();
		//theApp.SetOutputPtr(pDesktopUp);
		//hShellMain->SetShlProc(ShlProc);
	}
}

YSL_END

int
YMain(int argc, char* argv[]);

int
main(int argc, char* argv[])
{
	try
	{
		//全局初始化。
		YSL_ YInit();

		return YMain(argc, argv);
	}
	catch(std::bad_alloc&)
	{
		YSL_ theApp.Log.FatalError("Unhandled std::bad_alloc @@ int main(int, char*[]);");
	}
	catch(std::bad_cast&)
	{
		YSL_ theApp.Log.FatalError("Unhandled std::bad_cast @@ int main(int, char*[]);");
	}
	catch(YSL_ Exceptions::LoggedEvent& e)
	{
		YSL_ theApp.Log.FatalError(e.what());
	}
	catch(...)
	{
		YSL_ theApp.Log.FatalError("Unhandled exception @@ int main(int, char*[]);");
	}
}

