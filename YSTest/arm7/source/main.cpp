// YShell DS 测试程序 -> ARM7 主程序 by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-12-18 12:27:40;
// UTime = 2010-7-4 5:17;
// Version = 0.1017;


#include <nds.h>
//#include <dswifi7.h>
#include <maxmod7.h>

void VcountHandler()
{
	inputGetAndSend();
}

void VblankHandler(void)
{
	//Wifi_Update();
}


int main()
{
	irqInit();
	fifoInit();

	//从固件中读取用户设置。
	readUserSettings();

	// 开始使用 RTC 跟踪 IRQ 。
	initClockIRQ();

	SetYtrigger(80);

	//installWifiFIFO();
	installSoundFIFO();

	mmInstall(FIFO_MAXMOD);

	installSystemFIFO();

	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);

	irqEnable( IRQ_VBLANK | IRQ_VCOUNT | IRQ_NETWORK);

	//保持 ARM7 空闲状态。
	while(true)
		swiWaitForVBlank();
}

