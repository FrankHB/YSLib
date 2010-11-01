// YCommon 基础库 DS by Franksoft 2009 - 2010
// CodePage = UTF-8;
// CTime = 2009-11-12 22:14:28 + 08:00;
// UTime = 2010-10-29 10:18 + 08:00;
// Version = 0.2451;


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
	//********************************
	//名称:		strlen_n
	//全名:		stdex::strlen_n
	//可访问性:	public 
	//返回类型:	std::size_t
	//修饰符:	
	//形式参数:	const char *
	//功能概要:	当字符指针非空时返回 std::strlen 计算的串长，否则返回 0 。
	//备注:		
	//********************************
	std::size_t
	strlen_n(const char*);

	//********************************
	//名称:		strcpy_n
	//全名:		stdex::strcpy_n
	//可访问性:	public 
	//返回类型:	char*
	//修饰符:	
	//形式参数:	char *
	//形式参数:	const char *
	//功能概要:	当目标字符串和源字符串都非空时用 std::strcpy 复制字符串，
	//			并返回复制的字符串，否则返回空指针。
	//备注:		
	//********************************
	char*
	strcpy_n(char*, const char*);

	//********************************
	//名称:		stpcpy_n
	//全名:		stdex::stpcpy_n
	//可访问性:	public 
	//返回类型:	char*
	//修饰符:	
	//形式参数:	char *
	//形式参数:	const char *
	//功能概要:	当目标字符串和源字符串都非空时用 stpcpy 复制字符串，
	//			并返回复制的字符串的结尾指针，否则返回空指针。
	//备注:		
	//********************************
	char*
	stpcpy_n(char*, const char*);

	//********************************
	//名称:		stricmp_n
	//全名:		stdex::stricmp_n
	//可访问性:	public 
	//返回类型:	int
	//修饰符:	
	//形式参数:	const char *
	//形式参数:	const char *
	//功能概要:	当两个字符串都非空时返回 stricmp 比较的字符串结果，
	//			否则返回 EOF 。
	//备注:		
	//********************************
	int
	stricmp_n(const char*, const char*);

	//********************************
	//名称:		strdup_n
	//全名:		stdex::strdup_n
	//可访问性:	public 
	//返回类型:	char*
	//修饰符:	
	//形式参数:	const char *
	//功能概要:	当字符指针非空时用 strdup 复制字符串并返回复制的字符串，
	//			否则返回空指针。
	//备注:		
	//********************************
	char*
	strdup_n(const char*);

	//********************************
	//名称:		strcpycat
	//全名:		stdex::strcpycat
	//可访问性:	public 
	//返回类型:	char*
	//修饰符:	
	//形式参数:	char *
	//形式参数:	const char *
	//形式参数:	const char *
	//功能概要:	串接指定的两个字符串，
	//			结果复制至指定位置（对传入参数进行非空检查），
	//			返回目标参数。
	//备注:		
	//********************************
	char*
	strcpycat(char*, const char*, const char*);

	//********************************
	//名称:		strcatdup
	//全名:		stdex::strcatdup
	//可访问性:	public 
	//返回类型:	char*
	//修饰符:	
	//形式参数:	const char *
	//形式参数:	const char *
	//形式参数:	void * 
	//形式参数:	void*(*)(std::size_t)
	//形式参数:	std::size_t
	//功能概要:	串接指定的两个字符串，
	//			结果复制至用指定分配函数（默认为 std::malloc）
	//			新分配的空间（对传入参数进行非空检查），返回目标参数。
	//备注:		
	//********************************
	char*
	strcatdup(const char*, const char*, void*(*)(std::size_t) = std::malloc);

	//********************************
	//名称:		fexists
	//全名:		stdex::fexists
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	CPATH
	//功能概要:	判断指定路径的文件是否存在。
	//备注:		使用 C 标准函数库实现。
	//********************************
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

	//********************************
	//名称:		getcwd_n
	//全名:		platform::getcwd_n
	//可访问性:	public 
	//返回类型:	char*
	//修饰符:	
	//形式参数:	char * buf
	//形式参数:	std::size_t
	//功能概要:	当 buf 非空时取当前工作目录复制至 buf 起始的长为 t 的缓冲区中，
	//			并返回 buf 。
	//备注:		
	//********************************
	char*
	getcwd_n(char* buf, std::size_t);

	//********************************
	//名称:		direxists
	//全名:		platform::direxists
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	CPATH
	//功能概要:	判断指定目录是否存在。
	//备注:		
	//********************************
	bool
	direxists(CPATH);

	using ::mkdir;
	using ::chdir;

	//********************************
	//名称:		mkdirs
	//全名:		platform::mkdirs
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	CPATH
	//功能概要:	按路径新建一个或多个目录。
	//备注:		
	//********************************
	bool
	mkdirs(CPATH);


	//********************************
	//名称:		terminate
	//全名:		platform::terminate
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	异常终止函数。
	//备注:		
	//********************************
	void
	terminate();


	typedef u16 PixelType; //像素。
	typedef PixelType* BitmapPtr;
	typedef const PixelType* ConstBitmapPtr;
	typedef PixelType ScreenBufferType[SCREEN_WIDTH * SCREEN_HEIGHT]; \
		//主显示屏缓冲区。
	#define BITALPHA BIT(15) // Alpha 位。


	//系统默认颜色空间。
	namespace ColorSpace
	{
	//	#define DefColorA(r, g, b, name) name = ARGB16(1, r, g, b),
		#define	HexAdd0x(hex) 0x##hex
		#define DefColorH_(hex, name) name = \
			RGB8(((hex >> 16) & 0xFF), ((hex >> 8) & 0xFF), (hex & 0xFF)) \
			| BITALPHA
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
		typedef u8 MonoType;
		typedef bool AlphaType;

	private:
		PixelType _value;

	public:
		//********************************
		//名称:		Color
		//全名:		platform::Color::Color
		//可访问性:	public 
		//返回类型:	
		//修饰符:	
		//形式参数:	PixelType
		//功能概要:	构造：使用本机颜色对象。
		//备注:		
		//********************************
		Color(PixelType = 0);
		//********************************
		//名称:		Color
		//全名:		platform::Color::Color
		//可访问性:	public 
		//返回类型:	
		//修饰符:	
		//形式参数:	MonoType
		//形式参数:	MonoType
		//形式参数:	MonoType
		//形式参数:	AlphaType
		//功能概要:	使用 RGB 值和 alpha 位构造 Color 对象。
		//备注:		
		//********************************
		Color(MonoType, MonoType, MonoType, AlphaType = true);

		//********************************
		//名称:		operator PixelType
		//全名:		platform::Color::operator PixelType
		//可访问性:	public 
		//返回类型:	
		//修饰符:	const
		//功能概要:	转换：本机颜色对象。
		//备注:		
		//********************************
		operator PixelType() const;

		//********************************
		//名称:		GetR
		//全名:		platform::Color::GetR
		//可访问性:	public 
		//返回类型:	MonoType
		//修饰符:	const
		//功能概要:	取红色分量。
		//备注:		
		//********************************
		MonoType
		GetR() const;
		//********************************
		//名称:		GetG
		//全名:		platform::Color::GetG
		//可访问性:	public 
		//返回类型:	MonoType
		//修饰符:	const
		//功能概要:	取绿色分量。
		//备注:		
		//********************************
		MonoType
		GetG() const;
		//********************************
		//名称:		GetB
		//全名:		platform::Color::GetB
		//可访问性:	public 
		//返回类型:	MonoType
		//修饰符:	const
		//功能概要:	取蓝色分量。
		//备注:		
		//********************************
		MonoType
		GetB() const;
		//********************************
		//名称:		GetA
		//全名:		platform::Color::GetA
		//可访问性:	public 
		//返回类型:	AlphaType
		//修饰符:	const
		//功能概要:	取 alpha 分量。
		//备注:		
		//********************************
		AlphaType
		GetA() const;
	};

	inline
	Color::Color(PixelType p)
		: _value(p)
	{}
	inline
	Color::Color(MonoType r, MonoType g, MonoType b, AlphaType a)
		: _value(ARGB16(int(a), r >> 3, g >> 3, b >> 3))
	{}

	inline
	Color::operator PixelType() const
	{
		return _value;
	}

	inline Color::MonoType
	Color::GetR() const
	{
		return _value >> 7 & 248;
	}
	inline Color::MonoType
	Color::GetG() const
	{
		return _value >> 2 & 248;
	}
	inline Color::MonoType
	Color::GetB() const
	{
		return _value << 3 & 248;
	}
	inline Color::AlphaType
	Color::GetA() const
	{
		return _value & BITALPHA;
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
		typedef u32 InputType; //本机按键输入类型。

	private:
		InputType _value;

	public:
		//********************************
		//名称:		Key
		//全名:		platform::Key::Key
		//可访问性:	public 
		//返回类型:	
		//修饰符:	
		//形式参数:	InputType
		//功能概要:	构造：使用本机按键输入对象。
		//备注:		
		//********************************
		Key(InputType = 0);

		//********************************
		//名称:		operator InputType
		//全名:		platform::Key::operator InputType
		//可访问性:	public 
		//返回类型:	
		//修饰符:	const
		//功能概要:	转换：本机按键输入对象。
		//备注:		
		//********************************
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
		//********************************
		//名称:		GetX
		//全名:		platform::CursorInfo::GetX
		//可访问性:	public 
		//返回类型:	u16
		//修饰符:	const
		//功能概要:	取横坐标。
		//备注:		
		//********************************
		u16 GetX() const;
		//********************************
		//名称:		GetY
		//全名:		platform::CursorInfo::GetY
		//可访问性:	public 
		//返回类型:	u16
		//修饰符:	const
		//功能概要:	取纵坐标。
		//备注:		
		//********************************
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


	//********************************
	//名称:		YDebugSetStatus
	//全名:		platform::YDebugSetStatus
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	bool
	//功能概要:	调试模式：设置状态。
	//备注:		当且仅当状态为 true 时，
	//			以下除 YDebugGetStatus 外的调试模式函数有效。
	//********************************
	void
	YDebugSetStatus(bool = true);

	//********************************
	//名称:		YDebugGetStatus
	//全名:		platform::YDebugGetStatus
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//功能概要:	调试模式：取得状态。
	//备注:		
	//********************************
	bool
	YDebugGetStatus();

	//********************************
	//名称:		YDebugBegin
	//全名:		platform::YDebugBegin
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	Color fc
	//形式参数:	Color bc
	//功能概要:	调试模式：显示控制台（fc 为前景色，bc 为背景色）。
	//备注:		
	//********************************
	void
	YDebugBegin(Color fc = ColorSpace::White, Color bc = ColorSpace::Blue);

	//********************************
	//名称:		YDebug
	//全名:		platform::YDebug
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	调试模式：按键继续。
	//备注:		
	//********************************
	void
	YDebug();
	//********************************
	//名称:		YDebug
	//全名:		platform::YDebug
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	const char *
	//功能概要:	调试模式：显示控制台字符串，按键继续。
	//备注:		
	//********************************
	void
	YDebug(const char*);

	//********************************
	//名称:		YDebugW
	//全名:		platform::YDebugW
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	int
	//功能概要:	调试模式：显示控制台字（int 型数据），按键继续。
	//备注:		
	//********************************
	void
	YDebugW(int);

	//********************************
	//名称:		yprintf
	//全名:		platform::yprintf
	//可访问性:	public 
	//返回类型:	int
	//修饰符:	_ATTRIBUTE ((format (printf, 1, 2)))
	//形式参数:	const char *
	//形式参数:	...
	//功能概要:	调试模式 printf ：显示控制台格式化输出 ，按键继续。
	//备注:		
	//********************************
	int
	yprintf(const char*, ...)
		_ATTRIBUTE ((format (printf, 1, 2)));

	//断言。
	#ifdef YC_USE_YASSERT

	#undef YAssert

	//********************************
	//名称:		yassert
	//全名:		platform::yassert
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	bool exp
	//形式参数:	const char * expstr
	//形式参数:	const char * message
	//形式参数:	int line
	//形式参数:	const char * file
	//功能概要:	YCLib 默认断言函数。
	//备注:		当定义 YC_USE_YASSERT 宏时，宏 YAssert 操作由此函数实现。
	//********************************
	inline void
	yassert(bool exp, const char* expstr, const char* message,
		int line, const char* file)
	{
		if(!exp)
		{
			yprintf("Assertion failed: \n"
				"%s\nMessage: \n%s\nAt line %i in file \"%s\".\n",
				expstr, message, line, file);
			abort();
		}
	}

	#define YAssert(exp, message) \
		platform::yassert(exp, #exp, message, __LINE__, __FILE__)

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
		typedef ::DIR_ITER* IteratorType; //本机迭代器类型。

		static PATHSTR Name; //节点名称。
		static struct ::stat Stat; //节点状态信息。
		static int LastError; //上一次操作结果，0 为无错误。

	private:
		IteratorType dir;

	public:
		//********************************
		//名称:		HDirectory
		//全名:		platform::HDirectory::HDirectory
		//可访问性:	public 
		//返回类型:	
		//修饰符:	
		//形式参数:	CPATH
		//功能概要:	使用路径字符串构造 HDirectory 对象。
		//备注:		
		//********************************
		explicit
		HDirectory(CPATH = NULL);

	private:
		//********************************
		//名称:		HDirectory
		//全名:		platform::HDirectory::HDirectory
		//可访问性:	private 
		//返回类型:	
		//修饰符:	
		//形式参数:	IteratorType &
		//功能概要:	使用本机迭代器构造 HDirectory 对象。
		//备注:		
		//********************************
		HDirectory(IteratorType&);

	public:
		//********************************
		//名称:		~HDirectory
		//全名:		platform::HDirectory::~HDirectory
		//可访问性:	public 
		//返回类型:	
		//修饰符:	
		//功能概要:	析构函数。
		//备注:		
		//********************************
		~HDirectory();

		//********************************
		//名称:		operator++
		//全名:		platform::HDirectory::operator++
		//可访问性:	public 
		//返回类型:	HDirectory&
		//修饰符:	
		//功能概要:	迭代：向后遍历。
		//备注:		
		//********************************
		HDirectory&
		operator++();
		//********************************
		//名称:		operator++
		//全名:		platform::HDirectory::operator++
		//可访问性:	public 
		//返回类型:	platform::HDirectory
		//修饰符:	
		//形式参数:	int
		//功能概要:	迭代：向前遍历。
		//备注:		
		//********************************
		HDirectory
		operator++(int);

		//********************************
		//名称:		IsValid
		//全名:		platform::HDirectory::IsValid
		//可访问性:	public 
		//返回类型:	bool
		//修饰符:	const
		//功能概要:	判断文件系统节点有效性。
		//备注:		
		//********************************
		bool
		IsValid() const; 
		//********************************
		//名称:		IsDirectory
		//全名:		platform::HDirectory::IsDirectory
		//可访问性:	public static 
		//返回类型:	bool
		//修饰符:	
		//功能概要:	从节点状态信息判断是否为目录。
		//备注:		
		//********************************
		static bool
		IsDirectory();

		//********************************
		//名称:		Open
		//全名:		platform::HDirectory::Open
		//可访问性:	public 
		//返回类型:	void
		//修饰符:	
		//形式参数:	CPATH
		//功能概要:	打开。
		//备注:		
		//********************************
		void
		Open(CPATH);

		//********************************
		//名称:		Close
		//全名:		platform::HDirectory::Close
		//可访问性:	public 
		//返回类型:	void
		//修饰符:	
		//功能概要:	关闭。
		//备注:		
		//********************************
		void
		Close();

		//********************************
		//名称:		Reset
		//全名:		platform::HDirectory::Reset
		//可访问性:	public 
		//返回类型:	void
		//修饰符:	
		//功能概要:	复位。
		//备注:		
		//********************************
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
		return dir;
	}

	inline bool
	HDirectory::IsDirectory()
	{
		return Stat.st_mode & S_IFDIR;
	}

	inline void
	HDirectory::Open(CPATH path)
	{
		dir = path ? ::diropen(path) : NULL;
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


	//********************************
	//名称:		IsAbsolute
	//全名:		platform::IsAbsolute
	//可访问性:	public 
	//返回类型:	bool
	//修饰符:	
	//形式参数:	CPATH
	//功能概要:	判断指定路径字符串是否表示一个绝对路径。
	//备注:		
	//********************************
	bool
	IsAbsolute(CPATH);

	//********************************
	//名称:		GetRootNameLength
	//全名:		platform::GetRootNameLength
	//可访问性:	public 
	//返回类型:	std::size_t
	//修饰符:	
	//形式参数:	CPATH
	//功能概要:	取指定路径的文件系统根节点名称的长度。
	//备注:		
	//********************************
	std::size_t
	GetRootNameLength(CPATH);

	//********************************
	//名称:		ScreenSychronize
	//全名:		platform::ScreenSychronize
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	PixelType * scr
	//形式参数:	const PixelType * buf
	//功能概要:	快速刷新缓存映像到主显示屏。
	//备注:		
	//********************************
	inline void
	ScreenSychronize(PixelType* scr, const PixelType* buf)
	{
		dmaCopy(buf, scr, sizeof(ScreenBufferType));
	//	swiFastCopy(buf, scr, sizeof(ScreenBufferType) >> 2);
	}

	//********************************
	//名称:		ResetVideo
	//全名:		platform::ResetVideo
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	复位屏幕显示模式。
	//备注:		
	//********************************
	void
	ResetVideo();

	//********************************
	//名称:		YConsoleInit
	//全名:		platform::YConsoleInit
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	u8 dspIndex
	//形式参数:	Color fc
	//形式参数:	Color bc
	//功能概要:	启动控制台。
	//备注:		fc 为前景色，bc为背景色。
	//********************************
	void
	YConsoleInit(u8 dspIndex,
		Color fc = ColorSpace::White,
		Color bc = ColorSpace::Black);

	//********************************
	//名称:		iputw
	//全名:		platform::iputw
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	int n
	//功能概要:	输出控制台字（int 型数据）。
	//备注:		
	//********************************
	inline void
	iputw(int n)
	{
		iprintf("%d\n", n);
	}


	//********************************
	//名称:		WaitForInput
	//全名:		platform::WaitForInput
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	等待任意按键。
	//备注:		
	//********************************
	void
	WaitForInput();

	//********************************
	//名称:		WaitForKey
	//全名:		platform::WaitForKey
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	u32 mask
	//功能概要:	等待 mask 包含的按键。
	//备注:		
	//********************************
	void
	WaitForKey(u32 mask);

	//********************************
	//名称:		WaitForKeypad
	//全名:		platform::WaitForKeypad
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	等待任意按键（除触摸屏、翻盖外）。
	//备注:		
	//********************************
	inline void
	WaitForKeypad()
	{
		WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y | KEY_L | KEY_R
			| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
			| KEY_START | KEY_SELECT);
	}

	//********************************
	//名称:		WaitForFrontKey
	//全名:		platform::WaitForFrontKey
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	等待任意按键（除 L 、 R 和翻盖外）。
	//备注:		
	//********************************
	inline void
	WaitForFrontKey()
	{
		WaitForKey(KEY_TOUCH | KEY_A | KEY_B | KEY_X | KEY_Y
			| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
			| KEY_START | KEY_SELECT);
	}

	//********************************
	//名称:		WaitForFrontKeypad
	//全名:		platform::WaitForFrontKeypad
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	等待任意按键（除 L 、 R 、触摸屏和翻盖外）。
	//备注:		
	//********************************
	inline void
	WaitForFrontKeypad()
	{
		WaitForKey(KEY_A | KEY_B | KEY_X | KEY_Y
			| KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN
			|KEY_START | KEY_SELECT);
	}

	//********************************
	//名称:		WaitForArrowKey
	//全名:		platform::WaitForArrowKey
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	等待方向键。
	//备注:		
	//********************************
	inline void
	WaitForArrowKey()
	{
		WaitForKey(KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN);
	}

	//********************************
	//名称:		WaitForABXY
	//全名:		platform::WaitForABXY
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	等待按键 A 、 B 、 X 、 Y 键。
	//备注:		
	//********************************
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

	//********************************
	//名称:		WriteKeysInfo
	//全名:		platform::WriteKeysInfo
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	KeysInfo &
	//形式参数:	CursorInfo &
	//功能概要:	写入当前按键信息。
	//备注:		
	//********************************
	void
	WriteKeysInfo(KeysInfo&, CursorInfo&);


	//********************************
	//名称:		timers2msRaw
	//全名:		platform::timers2msRaw
	//可访问性:	public 
	//返回类型:	vu32
	//修饰符:	
	//形式参数:	vu16 l
	//形式参数:	vu16 h
	//功能概要:	组合计时器寄存器的值。
	//备注:		
	//********************************
	inline vu32
	timers2msRaw(vu16 l, vu16 h)
	{
		return l | (h << 16);
	}
	//********************************
	//名称:		timers2ms
	//全名:		platform::timers2ms
	//可访问性:	public 
	//返回类型:	vu32
	//修饰符:	
	//形式参数:	vu16 l
	//形式参数:	vu16 h
	//功能概要:	寄存器 - 毫秒转换。
	//备注:		
	//********************************
	inline vu32
	timers2ms(vu16 l, vu16 h)
	{
		return timers2msRaw(l, h) >> 5;
	}

	//********************************
	//名称:		InitTimers
	//全名:		platform::InitTimers
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	vu16 &
	//形式参数:	vu16 &
	//功能概要:	初始化计时器。
	//备注:		目标寄存器为指定参数。
	//********************************
	void
	InitTimers(vu16&, vu16&);
	//********************************
	//名称:		InitTimers
	//全名:		platform::InitTimers
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	初始化计时器。
	//备注:		会判断是否已经初始化。若已初始化则不重复进行初始化。
	//********************************
	void
	InitTimers();

	//********************************
	//名称:		ClearTimers
	//全名:		platform::ClearTimers
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	vu16 & l
	//形式参数:	vu16 & h
	//功能概要:	清除计时器。
	//备注:		目标寄存器为指定参数。
	//********************************
	inline void
	ClearTimers(vu16& l = TIMER0_DATA, vu16& h = TIMER1_DATA)
	{
		l = 0;
		h = 0;
	}

	//********************************
	//名称:		InitRTC
	//全名:		platform::InitRTC
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	初始化实时时钟。
	//备注:		
	//********************************
	inline void
	InitRTC()
	{
		InitTimers(TIMER2_CR, TIMER3_CR);
	}

	//********************************
	//名称:		ClearRTC
	//全名:		platform::ClearRTC
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	清除实时时钟。
	//备注:		
	//********************************
	inline void
	ClearRTC()
	{
		ClearTimers(TIMER2_DATA, TIMER3_DATA);
	}

	//********************************
	//名称:		ResetRTC
	//全名:		platform::ResetRTC
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	复位实时时钟。
	//备注:		
	//********************************
	inline void
	ResetRTC()
	{
		InitRTC();
		ClearRTC();
	}

	//********************************
	//名称:		GetRTC
	//全名:		platform::GetRTC
	//可访问性:	public 
	//返回类型:	vu32
	//修饰符:	
	//功能概要:	取实时时钟计数。
	//备注:		
	//********************************
	inline vu32
	GetRTC()
	{
		return timers2ms(TIMER2_DATA, TIMER3_DATA);
	}

	//********************************
	//名称:		StartTicks
	//全名:		platform::StartTicks
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//功能概要:	开始 tick 计时。
	//备注:		
	//********************************
	void
	StartTicks();

	//********************************
	//名称:		GetTicks
	//全名:		platform::GetTicks
	//可访问性:	public 
	//返回类型:	u32
	//修饰符:	
	//功能概要:	取 tick 数。
	//备注:		
	//********************************
	u32
	GetTicks();

	//********************************
	//名称:		Delay
	//全名:		platform::Delay
	//可访问性:	public 
	//返回类型:	void
	//修饰符:	
	//形式参数:	u32 ms
	//功能概要:	延时 ms 毫秒。
	//备注:		ms 仅低 25 位有效。
	//********************************
	void
	Delay(u32 ms);

	/*void setupCapture(int bank);
	void waitForCapture();
	char* basename(char*);
	*/
}

#endif

