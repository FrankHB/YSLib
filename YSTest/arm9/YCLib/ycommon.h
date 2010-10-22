// YCommon 基础库 DS by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-12 22:14:28 + 08:00;
// UTime = 2010-10-21 12:54 + 08:00;
// Version = 0.2167;


#ifndef INCLUDED_YCOMMON_H_
#define INCLUDED_YCOMMON_H_

// YCommon ：平台相关的公共组件无关函数与宏定义集合。

#include <platform.h>

#ifndef UNICODE
#define UNICODE
#endif

#include "ydef.h"
#include <cstdio>
#include <cstdlib>

//平台无关部分。

namespace stdex
{
	//当字符指针非空时返回 std::strlen 计算的串长，否则返回 0 。
	std::size_t
	strlen_n(const char*);

	//当目标字符串和源字符串都非空时用 std::strcpy 复制字符串并返回复制的字符串，否则返回空指针。
	char*
	strcpy_n(char*, const char*);

	//当目标字符串和源字符串都非空时用 stpcpy 复制字符串并返回复制的字符串的结尾指针，否则返回空指针。
	char*
	stpcpy_n(char*, const char*);

	//当两个字符串都非空时返回 stricmp 比较的字符串结果，否则返回 EOF 。
	int
	stricmp_n(const char*, const char*);

	//当字符指针非空时用 strdup 复制字符串并返回复制的字符串，否则返回空指针。
	char*
	strdup_n(const char*);

	//串接给定的两个字符串，结果复制至指定位置（对传入参数进行非空检查），返回目标参数。
	char*
	strcpycat(char*, const char*, const char*);

	//串接给定的两个字符串，结果复制至用指定分配函数（默认为 std::malloc）新分配的空间（对传入参数进行非空检查），返回目标参数。
	char*
	strcatdup(const char*, const char*, void*(*)(std::size_t) = std::malloc);


	//通过 C 标准函数库操作判断给定路径的文件是否存在。
	bool
	fexists(CPATH);
}

//平台相关部分。

//最大路径长度。
#define YC_MAX_PATH MAXPATHLEN;

//#define HEAP_SIZE (mallinfo().uordblks)

namespace platform
{
	using ::iprintf;

	//当 buf 非空时取当前工作目录复制至 buf 起始的长为 t 的缓冲区中，并返回 buf 。
	char*
	getcwd_n(char* buf, std::size_t);

	//判断指定目录是否存在。
	bool
	direxists(CPATH);

	using ::mkdir;
	using ::chdir;

	//按路径新建一个或多个目录。
	bool
	mkdirs(CPATH);


	//异常终止函数。
	void
	terminate();


	typedef u16 PixelType; //像素。
	typedef PixelType* BitmapPtr;
	typedef const PixelType* ConstBitmapPtr;
	typedef PixelType ScreenBufferType[SCREEN_WIDTH * SCREEN_HEIGHT]; //主显示屏缓冲区。
	#define BITALPHA BIT(15) // Alpha 位。


	//系统默认颜色空间。
	namespace ColorSpace
	{
	//	#define DefColorA(r, g, b, name) name = ARGB16(1, r, g, b),
		#define	HexAdd0x(hex) 0x##hex
		#define DefColorH_(hex, name) name = RGB8(((hex >> 16) & 0xFF), ((hex >> 8) & 0xFF), (hex & 0xFF)) | BITALPHA
		#define DefColorH(hex_, name) DefColorH_(HexAdd0x(hex_), name),
	
		//参考：http://www.w3schools.com/html/html_colornames.asp 。

		typedef enum ColorSet
		{
			DefColorH(00FFFF, Aqua)
			DefColorH(000000, Black)
			DefColorH(0000FF, Blue)
			DefColorH(FF00FF, Fuchsia)
			DefColorH(808080, Gray)
			DefColorH(008000, Green)
			DefColorH(00FF00, Lime)
			DefColorH(800000, Maroon)
			DefColorH(000080, Navy)
			DefColorH(808000, Olive)
			DefColorH(800080, Purple)
			DefColorH(FF0000, Red)
			DefColorH(C0C0C0, Silver)
			DefColorH(008080, Teal)
			DefColorH(FFFFFF, White)
			DefColorH(FFFF00, Yellow)
		} ColorSet;

		#undef DefColorH
		#undef DefColorH_
		#undef HexAdd0x
	}

	class Color
	{
	public:
		typedef ColorSpace::ColorSet ColorSet;

	private:
		PixelType _value;

	public:
		Color(PixelType = 0);
		Color(u8, u8, u8, bool = true);

		operator PixelType() const;
	};

	inline
	Color::Color(PixelType p)
		: _value(p)
	{}
	inline
	Color::Color(u8 r, u8 g, u8 b, bool a)
		: _value(ARGB16(int(a), r >> 3, g >> 3, b >> 3))
	{}

	inline
	Color::operator PixelType() const
	{
		return _value;
	}


	namespace KeySpace
	{
		//按键集合。
		typedef enum KeySet
		{
			Empty	= 0,
			A		= KEY_A,
			B		= KEY_B,
			Select	= KEY_SELECT,
			Start	= KEY_START,
			Right	= KEY_RIGHT,
			Left	= KEY_LEFT,
			Up		= KEY_UP,
			Down	= KEY_DOWN,
			R		= KEY_R,
			L		= KEY_L,
			X		= KEY_X,
			Y		= KEY_Y,
			Touch	= KEY_TOUCH,
			Lid		= KEY_LID
		} KeySet;

		//按键别名。
		const KeySet
			Enter = A,
			ESC = B,
			PgUp = L,
			PgDn = R;
	}


	class Key
	{
	public:
		typedef u32 InputType;

	private:
		InputType _value;

	public:
		Key(InputType = 0);

		operator InputType() const;
	};

	inline
	Key::Key(Key::InputType i)
	: _value(i)
	{}

	inline
	Key::operator Key::InputType() const
	{
		return _value;
	}


	//按键信息。
	typedef struct KeysInfo
	{
		Key up, down, held;
	} KeysInfo;


	//屏幕光标信息。
	typedef struct CursorInfo : public ::touchPosition
	{
		u16 GetX() const;
		u16 GetY() const;
	} CursorInfo;

	inline u16
	CursorInfo::GetX() const
	{
		return px;
	}
	inline u16
	CursorInfo::GetY() const
	{
		return py;
	}


	//调试模式：设置状态。
	void
	YDebugSetStatus(bool = true);

	//调试模式：取得状态。
	bool
	YDebugGetStatus();

	//调试模式：显示控制台（fc 为前景色，bc 为背景色）。
	void
	YDebugBegin(Color fc = ColorSpace::White, Color bc = ColorSpace::Blue);

	//调试模式：按键继续。
	void
	YDebug();
	//调试模式：显示控制台字符串，按键继续。
	void
	YDebug(const char*);

	//调试模式：显示控制台字（int 型数据），按键继续。
	void
	YDebugW(int);

	//调试模式 printf ：显示控制台格式化输出 ，按键继续。
	int
	yprintf(const char*, ...)
		_ATTRIBUTE ((format (printf, 1, 2)));

	//断言。
	#ifdef YC_USE_YASSERT

	#undef YAssert

	inline void
	yassert(bool exp, const char* expstr, const char* message, int line, const char* file)
	{
		if(!exp)
		{
			yprintf("Assertion failed: \n%s\nMessage: \n%s\nAt line %i in file \"%s\".\n", expstr, message, line, file);
			abort();
		}
	}

	#define YAssert(exp, message) yassert(exp, #exp, message, __LINE__, __FILE__);

	#else

	#	include <cassert>
	#	define YAssert(exp, message) assert(exp)

	#endif


	//定长路径字符串类型。
	typedef char PATHSTR[MAXPATHLEN];


	//目录迭代器。
	class HDirectory
	{
	public:
		typedef ::DIR_ITER* IteratorType;

		static PATHSTR Name; //节点名称。
		static struct ::stat Stat; //节点状态信息。
		static int LastError; //上一次操作结果，0 为无错误。

	private:
		IteratorType dir;

	public:
		explicit
		HDirectory(CPATH = NULL);

	private:
		HDirectory(IteratorType&);

	public:
		~HDirectory();

		HDirectory&
		operator++();
		HDirectory
		operator++(int);

		bool
		IsValid() const; //判断文件系统节点是否有效。
		static bool
		IsDirectory(); //从节点状态信息判断是否为目录。

		void
		Open(CPATH);

		void
		Close();

		void
		Reset();
	};

	inline
	HDirectory::HDirectory(CPATH path)
	: dir(NULL)
	{
		Open(path);
	}
	inline
	HDirectory::HDirectory(HDirectory::IteratorType& d)
	: dir(d)
	{}
	inline
	HDirectory::~HDirectory()
	{
		Close();
	}

	inline HDirectory
	HDirectory::operator++(int)
	{
		return ++HDirectory(*this);
	}

	inline bool
	HDirectory::IsValid() const
	{
		return dir != NULL;
	}

	inline bool
	HDirectory::IsDirectory()
	{
		return Stat.st_mode & S_IFDIR;
	}

	inline void
	HDirectory::Open(CPATH path)
	{
		dir = path == NULL ? NULL : ::diropen(path);
	}

	inline void
	HDirectory::Close()
	{
		if(IsValid())
			LastError = ::dirclose(dir);
	}

	inline void
	HDirectory::Reset()
	{
		LastError = ::dirreset(dir);
	}


	//判断是否为绝对路径。
	bool
	IsAbsolute(CPATH);

	//取给定路径的文件系统根节点名称的长度。
	std::size_t
	GetRootNameLength(CPATH);

	//extern u8 backlight;
	/*
	void
	FIFOBacklight(u32 value, void* userdata);
	void
	toggleBacklight();

	u8
	chartohex(char c);
	*/

	//快速刷新缓存映像到主显示屏。
	inline void
	scrCopy(PixelType* scr, const PixelType* buf)
	{
		dmaCopy(buf, scr, sizeof(ScreenBufferType));
	//	swiFastCopy(buf, scr, sizeof(ScreenBufferType) >> 2);
	}

	/*void fadeBlack(u16 frames);
	void unfadeBlack(u16 frames);
	void unfadeBlack2(u16 frames);
	bool loadImage(const char* file, u16* out, u8* outA, u16 w, u16 h, int format=0);
	void darken(u16* screen, u8 factor, s16 x, s16 y, s16 w, s16 h);*/

	//复位屏幕显示模式。
	void
	ResetVideo();


	//启动控制台（fc 为前景色，bc为背景色）。
	void
	YConsoleInit(u8 dspIndex, Color fc = ColorSpace::White, Color bc = ColorSpace::Black);

	//输出控制台字（int 型数据）。
	inline void
	iputw(int n)
	{
		iprintf("%d\n", n);
	}


	//等待任意按键。
	void
	WaitForInput();

	//等待 mask 包含的按键。
	void
	WaitForKey(u32 mask);

	//等待任意按键（除触摸屏、翻盖外）。
	inline void
	WaitForKeypad()
	{
		WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y | KEY_L | KEY_R | KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN | KEY_START | KEY_SELECT);
	}

	//等待任意按键（除 L 、 R 和翻盖外）。
	inline void
	WaitForFrontKey()
	{
		WaitForKey(KEY_TOUCH | KEY_A | KEY_B | KEY_X | KEY_Y | KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN | KEY_START | KEY_SELECT);
	}

	//等待任意按键（除 L 、 R 、触摸屏和翻盖外）。
	inline void
	WaitForFrontKeypad()
	{
		WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y | KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN |KEY_START | KEY_SELECT);
	}

	//等待方向键。
	inline void
	WaitForArrowKey()
	{
		WaitForKey(KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN);
	}

	//等待按键 A 、 B 、 X 、 Y 键。
	inline void
	WaitForABXY()
	{
		WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y);
	}

	/*
	void trimString(char* string);
	void unescapeString(char* string);
	u32 versionStringToInt(char* string);
	void versionIntToString(char* out, u32 version);
	*/

	//写入当前按键信息。
	void
	WriteKeysInfo(KeysInfo&, CursorInfo&);


	//寄存器 - 毫秒转换。
	inline vu32
	timers2msRaw(vu16 l, vu16 h)
	{
		return l | (h << 16);
	}
	inline vu32
	timers2ms(vu16 l, vu16 h)
	{
		return timers2msRaw(l, h) >> 5;
	}

	//初始化计时器。
	void
	InitTimers(vu16&, vu16&);
	void
	InitTimers();

	//清除计时器。
	inline void
	ClearTimers(vu16& l = TIMER0_DATA, vu16& h = TIMER1_DATA)
	{
		l = 0;
		h = 0;
	}

	//初始化实时时钟。
	inline void
	InitRTC()
	{
		InitTimers(TIMER2_CR, TIMER3_CR);
	}

	//清除实时时钟。
	inline void
	ClearRTC()
	{
		ClearTimers(TIMER2_DATA, TIMER3_DATA);
	}

	//复位实时时钟。
	inline void
	ResetRTC()
	{
		InitRTC();
		ClearRTC();
	}

	//取实时时钟计数。
	inline vu32
	GetRTC()
	{
		return timers2ms(TIMER2_DATA, TIMER3_DATA);
	}

	//开始 tick 计时。
	void
	StartTicks();

	//取 tick 数。
	u32
	GetTicks();

	//延时 ms 毫秒（ms 仅低 25 位有效）。
	void
	Delay(u32 ms);

	/*void setupCapture(int bank);
	void waitForCapture();
	char* basename(char*);
	*/
}

#endif

