// YCommon 基础库 DS by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-12 22:14:42;
// UTime = 2010-9-16 21:09;
// Version = 0.1795;


#include "ycommon.h"
#include <cstdarg>
#include <cstring>
#include <cerrno>

namespace stdex
{
	std::size_t
	strlen_n(const char* s)
	{
		return s != NULL ? std::strlen(s) : 0;
	}

	char*
	strcpy_n(char* d, const char* s)
	{
		return d != NULL && s != NULL ? std::strcpy(d, s) : NULL;
	}

	char*
	stpcpy_n(char* d, const char* s)
	{
		return d != NULL && s != NULL ? stpcpy(d, s) : NULL;
	}

	int
	stricmp_n(const char* s1, const char* s2)
	{
		return s1 != NULL && s2 != NULL ? stricmp(s1, s2) : EOF;
	}

	char*
	strdup_n(const char* s)
	{
		return s != NULL ? strdup(s) : NULL;
	}

	char*
	strcpycat(char* d, const char* s1, const char* s2)
	{
		if(d == NULL)
			return NULL;
		if(s1 != NULL)
			std::strcpy(d, s1);
		if(s2 != NULL)
			std::strcat(d, s2);
		return d;
	}

	char*
	strcatdup(const char* s1, const char* s2, void*(*fun)(std::size_t))
	{
		char* d(static_cast<char*>(fun((strlen(s1) + strlen(s2) + 1) * sizeof(char))));

		return strcpycat(d, s1, s2);
	}


	bool
	fexists(CPATH path)
	{
		FILE* file = fopen(path, "rb");
		if(file)
		{
			fclose(file);
			return true;
		}
		return false;
	}
}


namespace platform
{
	PATHSTR HDirectory::Name;
	struct ::stat HDirectory::Stat;
	int HDirectory::LastError(0);

	HDirectory&
	HDirectory::operator++()
	{
		LastError = ::dirnext(dir, Name, &Stat);
		return *this;
	}


	bool
	direxists(CPATH path)
	{
		DIR_ITER* dirState(::diropen(path));

		::dirclose(dirState);
		return dirState;
	}

	bool
	mkdirs(CPATH cpath)
	{
		PATHSTR path;

		std::strcpy(path, cpath);
		for(char* slash = path; (slash = strchr(slash, '/')) != NULL; ++slash)
		{
			*slash = '\0';
			::mkdir(path, S_IRWXU|S_IRWXG|S_IRWXO);
			*slash = '/';
		}
		//新建目录成功或目标路径已存在时返回 true 。
		return ::mkdir(path, S_IRWXU|S_IRWXG|S_IRWXO) == 0 || errno == EEXIST;
	}

	/*#define LOWC(x) ((x)&31)

	u8 backlight = 0;

	void FIFOBacklight(u32 value, void* userdata)
	{
		++backlight;
		backlight = value;
	}

	void toggleBacklight()
	{
		fifoSendValue32(TCOMMON_FIFO_CHANNEL_ARM7, MSG_TOGGLE_BACKLIGHT); //--in libnds 1.3.1 you HAVE to check the return value or the command won't be sent (GCC is probably to blame)
		if(!fifoSendValue32(TCOMMON_FIFO_CHANNEL_ARM7, MSG_TOGGLE_BACKLIGHT))
			iprintf("Error sending backlight message to ARM7\n");
		fifoSendValue32(FIFO_AUDIO, backlight);
		++backlight;
	}

	u8 chartohex(char c)
	{
		if(c >= '0' && c <= '9') return c - '0';
		if(c >= 'a' && c <= 'f') return 10 + c - 'a';
		if(c >= 'A' && c <= 'F') return 10 + c - 'A';
		return 0;
	}

	u32 oldMode;
	u32 oldModeSub;

	void fadeBlack(u16 frames)
	{
		oldMode = (REG_DISPCNT);
		oldModeSub = (REG_DISPCNT_SUB);
		videoSetMode(oldMode & (~DISPLAY_SPR_ACTIVE));
		videoSetModeSub(oldModeSub & (~DISPLAY_SPR_ACTIVE));

		REG_BLDCNT = BLEND_FADE_BLACK | BLEND_SRC_BG0 | BLEND_SRC_BG2 | BLEND_SRC_BG3;
		REG_BLDCNT_SUB = BLEND_FADE_BLACK | BLEND_SRC_BG2 | BLEND_SRC_BG3;

		for(u16 n = 0; n <= frames; ++n)
		{
			REG_BLDY = 0x1F * n / frames;
			REG_BLDY_SUB = 0x1F * n / frames;
			swiWaitForVBlank();
		}
	}

	void unfadeBlack2(u16 frames)
	{
		for(u16 n = 0; n <= frames; ++n)
		{
			REG_BLDY = 0x1F - 0x1F * n / frames;
			REG_BLDY_SUB = 0x1F - 0x1F * n / frames;
			swiWaitForVBlank();
		}

		REG_BLDCNT = BLEND_NONE;
		REG_BLDCNT_SUB = BLEND_NONE;
	}
	void unfadeBlack(u16 frames)
	{
		unfadeBlack2(frames);

		videoSetMode(oldMode);
		videoSetModeSub(oldModeSub);
	}

	bool loadImage(const char* file, u16* out, u8* outA, u16 w, u16 h, int format)
	{
		if(format == 0)
		{
			char path[MAXPATHLEN];
			sprintf(path, "%s.png", file);

			FILE* file = fopen(path, "rb");
			if(file)
			{
				fseek(file, 0, SEEK_END);

				int dataL(ftell(file));

				fseek(file, 0, SEEK_SET);

				char* data(new char[dataL]);

				fread(data, sizeof(char), dataL, file);

				bool result(pngLoadImage(data, dataL, out, outA, w, h));

				delete[] data;
				fclose(file);
				return result;
			}
			return false;
		}

		char dtaPath[MAXPATHLEN];
		sprintf(dtaPath, "%s.dta", file);
		char palPath[MAXPATHLEN];
		sprintf(palPath, "%s.pal", file);

		FileHandle* fhDTA(fhOpen(dtaPath));

		if(!fhDTA)
			return false;

		FileHandle* fhPAL(NULL);
		u16* pal(NULL);

		if(format != GL_RGBA)
		{
			fhPAL = fhOpen(palPath);
			if(!fhPAL)
			{
				fhClose(fhDTA);
				return false;
			}
			pal = new u16[fhPAL->length / sizeof(u16)];
			fhReadFully(pal, fhPAL);
		}

		u8* dta(new u8[fhDTA->length]);

		fhReadFully(dta, fhDTA);
		if(format == GL_RGB32_A3)
			for(u32 n = 0; n < fhDTA->length; ++n)
			{
				out[n] = pal[dta[n] & 31];
				if(outA)
				{
					outA[n] = (dta[n] & 0xE0) + 16;
					if(outA[n] >= 255 - 16)
						outA[n] = 255;
					if(outA[n] <= 16)
						outA[n] = 0;
				}
			}
		else if(format == GL_RGB8_A5)
			for(u32 n = 0; n < fhDTA->length; n++)
			{
				out[n] = pal[dta[n] & 7];
				if(outA)
				{
					outA[n] = (dta[n]&0xF8) + 4;
					if(outA[n] >= 255-4) outA[n] = 255;
					if(outA[n] <= 4) outA[n] = 0;
				}
			}
		else if(format == GL_RGB256)
			for(u32 n = 0; n < fhDTA->length; ++n)
			{
				out[n] = pal[dta[n]]|BITALPHA;
				if(outA) outA[n] = (out[n]|BITALPHA ? 255 : 0);
			}
		else if(format == GL_RGBA)
			for(u32 n = 0; n < fhDTA->length; ++n)
			{
				out[n >> 1] = (dta[n + 1] << 8) | (dta[n]);
				++n;
				if(outA)
					outA[n] = (out[n] | BITALPHA ? 255 : 0);
			}
		if(fhDTA)
			fhClose(fhDTA);
		delete[] dta;
		if(fhPAL)
			fhClose(fhPAL);
		delete[] pal;
		return true;
	}

	void darken(u16* screen, u8 factor, s16 x, s16 y, s16 w, s16 h)
	{
		int t = y * 256;
		for(int v = y; v < y + h; ++v)
		{
			t += x;
			for(int u = x; u < x + w; ++u)
			{
				screen[t] = RGB15(((screen[t] & 31) >> factor), (((screen[t] >> 5) & 31) >> factor),
					(((screen[t] >> 10) & 31) >> factor)) | BITALPHA;
				++t;
			}
			t += 256 - (x+w);
		}
	}*/
	void
	terminate()
	{
		for(;;)
			swiWaitForVBlank();
	//	std::terminate();
	}

	void
	ResetVideo()
	{
		REG_BG0CNT = REG_BG1CNT = REG_BG2CNT = REG_BG3CNT = 0;
		REG_BG0CNT_SUB = REG_BG1CNT_SUB = REG_BG2CNT_SUB = REG_BG3CNT_SUB = 0;
		vramSetMainBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
		vramSetBankE(VRAM_E_LCD);
		vramSetBankF(VRAM_F_LCD);
		vramSetBankG(VRAM_G_LCD);
		vramSetBankH(VRAM_H_LCD);
		vramSetBankI(VRAM_I_LCD);
		memset(VRAM_A, 0, 0x90000);
	//	memset(OAM, 0, SPRITE_COUNT * sizeof(SpriteEntry));
	//	memset(OAM_SUB, 0, SPRITE_COUNT * sizeof(SpriteEntry));
		videoSetMode(MODE_5_2D);
		videoSetModeSub(MODE_5_2D);
	}


	extern const u8 default_font_bin[];

	static PrintConsole mainConsole =
	{
		//Font:
		{
			reinterpret_cast<u16*>(const_cast<u8*>(default_font_bin)), //font gfx
			0, //font palette
			0, //font color count
			4, //bpp
			0, //first ascii character in the set
			128, //number of characters in the font set
			true //convert single color
		},
		0, //font background map
		0, //font background gfx
		22, //map base
		3, //char base
		0, //bg layer in use
		-1, //bg id
		0,0, //cursorX cursorY
		0,0, //prevcursorX prevcursorY
		32, //console width
		24, //console height
		0,  //window x
		0,  //window y
		32, //window width
		24, //window height
		3, //tab size
		0, //font character offset
		0, //selected palette
		0,  //print callback
		false, //console initialized
		true, //load graphics
	};

	static PrintConsole*
	consoleMainInit()
	{
		videoSetMode(MODE_0_2D);
		vramSetBankA(VRAM_A_MAIN_BG);
		return consoleInit(NULL, mainConsole.bgLayer, BgType_Text4bpp, BgSize_T_256x256, mainConsole.mapBase, mainConsole.gfxBase, true, true);
	}

	void
	YConsoleInit(u8 dspIndex, PIXEL fc, PIXEL bc)
	{
	//	PrintConsole* p(dspIndex ? consoleMainInit() : consoleDemoInit());

		if((dspIndex ? consoleMainInit() : consoleDemoInit()) != NULL)
		{
			iprintf("\x1b[0;0H"); //使用 ANSI Escape 序列 CUrsor Position 指令设置光标位置为左上角。

			PIXEL* bg_palette = dspIndex ? BG_PALETTE : BG_PALETTE_SUB;

			bg_palette[0]	= bc | BITALPHA;
			bg_palette[255]	= fc | BITALPHA;
		}
	}


	void
	WaitForInput()
	{
		while(true)
		{
			scanKeys();
			if(keysDown())
				break;
			swiWaitForVBlank();
		}
	}

	void
	WaitForKey(u32 mask)
	{
		while(true)
		{
			scanKeys();
			if(keysDown() & mask)
				break;
			swiWaitForVBlank();
		}
	}


	void
	WriteKeysInfo(KeysInfo& key, CursorInfo& tp)
	{
		touchRead(&tp);
		key.up = keysUp();
		key.down = keysDown();
		key.held = keysHeld();
	}


	/*void
	trimString(char* string) {
		int a = 0;
		int b = strlen(string);

		while(isspace(string[a]) && a < b) a++;
		while(isspace(string[b-1]) && b > a) b--;

		if(b-a <= 0) {
			//string length is zero
			string[0] = '\0';
			return;
		}
		memmove(string, string+a, b-a);
		string[b-a] = '\0';
	}
	void
	unescapeString(char* str) {
		char* s = str;
		while(*s != '\0')
		{
			if(*s == '\\')
			{
				switch (*++s) {
					case '\\': *str = '\\'; break;
					case '\'': *str = '\''; break;
					case '\"': *str = '\"'; break;
					case 'n':  *str = '\n'; break;
					case 'r':  *str = '\r'; break;
					case 't':  *str = '\t'; break;
					case 'f':  *str = '\f'; break;
					default: //Error
						break;
				}
			} else {
				*str = *s;
			}
			str++;
			s++;
		}
		*str = '\0';
	}

	//Accepts version strings in a(.b(.c)?)? format
	u32
	versionStringToInt(char* string) {
		u32 version = 0;

		char* firstDot = strchr(string, '.');
		char* secondDot = NULL;
		if(firstDot) {
			*firstDot = '\0';
			secondDot = strchr(firstDot+1, '.');
			if(secondDot) {
				*secondDot = '\0';
			}
		}

		//Example result: 1.12.37 = 11237
		version = atoi(string)*10000;
		if(firstDot) {
			version += atoi(firstDot+1)*100;
			*firstDot = '.';
		}
		if(secondDot) {
			version += atoi(secondDot+1);
			*secondDot = '.';
		}
		return version;
	}
	void versionIntToString(char* out, u32 version) {
		sprintf(out, "%d.%d.%d", (int)(version/10000), (int)((version/100)%100), (int)(version%100));
	}*/


	static bool bUninitializedTimers(true);

	void
	InitTimers(vu16& l, vu16& h)
	{
		l = TIMER_ENABLE | TIMER_DIV_1024;
		h = TIMER_ENABLE | TIMER_CASCADE;
	}
	void
	InitTimers()
	{
		if(bUninitializedTimers)
		{
			TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1024;
			TIMER1_CR = TIMER_ENABLE | TIMER_CASCADE;
			bUninitializedTimers = false;
		}
	}

	void
	StartTicks()
	{
		InitTimers();
		ClearTimers();
	}

	u32
	GetTicks()
	{
		InitTimers();
		return timers2ms(TIMER0_DATA, TIMER1_DATA);
	}

	void
	Delay(u32 ms)
	{
		bUninitializedTimers = true;
		StartTicks();
	/*
		ms &= 0x01FFFFFF;
		while(static_cast<u32>(timers2ms(TIMER0_DATA, TIMER1_DATA)) < ms)
			;
	*/
		ms <<= 5;
		while(static_cast<u32>(timers2msRaw(TIMER0_DATA, TIMER1_DATA)) < ms)
			;
	}


	/*void
	setupCapture(int bank) {
		REG_DISPCAPCNT = DCAP_ENABLE | DCAP_MODE(0) | DCAP_DST(0) | DCAP_SRC(0) | DCAP_SIZE(3) |
			DCAP_OFFSET(0) | DCAP_BANK(bank) | DCAP_B(15) | DCAP_A(0);
	}
	void
	waitForCapture() {
		while(REG_DISPCAPCNT & DCAP_ENABLE) {
			swiWaitForVBlank();
		}
	}

	char*
	basename(char* path)
	{
		int len = strlen(path);
		int sindex;
		for(int i = 0; i < len; ++i) {
			if(path[i] == '/')
				sindex = i;
		}
		return path+sindex+1;
	}*/


	static bool
	bDebugStatus = false;

	void
	YDebugSetStatus(bool s)
	{
		bDebugStatus = s;
	}

	bool
	YDebugGetStatus()
	{
		return bDebugStatus;
	}

	void
	YDebugBegin(PIXEL fc, PIXEL bc)
	{
		if(bDebugStatus)
			YConsoleInit(false, fc, bc);
	}

	void
	YDebug()
	{
		if(bDebugStatus)
		{
			YDebugBegin();
			WaitForInput();
		}
	}
	void
	YDebug(const char* s)
	{
		if(bDebugStatus)
		{
			YDebugBegin();
			std::puts(s);
			WaitForInput();
		}
	}

	void
	YDebugW(int n)
	{
		if(bDebugStatus)
		{
			YDebugBegin();
			iputw(n);
			WaitForInput();
		}
	}

	int
	yprintf(const char* str, ...)
	{
		int t = -1;

		if(bDebugStatus)
		{
			YDebugBegin();

			va_list list;

			va_start(list, str);

			t = viprintf(str, list);

			va_end(list);
			WaitForInput();
		}
		return t;
	}
}

