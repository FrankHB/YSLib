// YSLib::Adaptor::Config by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-10-09 09:25:26 + 08:00;
// UTime = 2010-10-23 16:35;
// Version = 0.1486;


#ifndef INCLUDED_CONFIG_H_
#define INCLUDED_CONFIG_H_

// Config ：库编译配置。

//#define NDEBUG //非调试模式。

#ifndef NDEBUG
#	define YC_USE_YASSERT //使用 YCLib 断言。
#endif

// YSLib 选项。

#define YSL_USE_EXCEPTION_SPECIFICATION //使用 YSLib 异常规范。

//#define YSL_MULTITHREAD //多线程。

#define YSL_USE_COPY_ON_WRITE //写时复制。

#ifdef YSL_MULTITHREAD
#	undef YSL_USE_COPY_ON_WRITE
#endif

//#define YSL_USE_YASLI_VECTOR //使用 yasli::vector 。
//#define YSL_USE_FLEX_STRING //使用 flex_string 。
#define YSL_OPT_SMALL_STRING_LENGTH 16 //使用小字符串优化。

/*
YSLib No_Cursor
定义无指针设备。
消息成员列表中不会包含 Point pt 。
*/
//#define YSLIB_NO_CURSOR

// YSLib Debug 选项。
/*
YSLib MSG Debug
bit0 ： InsertMessage ；
bit1 ： PeekMessage 。
注意： PeekMessage 包含 InsertMessage 。
*/
#define YSLIB_DEBUG_MSG 0

//多播事件。
#define YSL_EVENT_MULTICAST

#endif

