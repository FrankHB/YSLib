// YSLib::Adapter::YAdapter by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-2-22 20:16:21;
// UTime = 2010-7-9 10:11;
// Version = 0.1666;


#ifndef INCLUDED_YADAPTER_H_
#define INCLUDED_YADAPTER_H_

// YAdapter ：外部库关联。

//编译配置。
//#define NDEBUG
#define YC_USE_YASSERT

//包含平台文件。
#include <platform.h>

namespace YSLib
{
	namespace Drawing
	{
		typedef ::PIXEL PixelType;
	}

	using ::iprintf;

	using ::swiWaitForVBlank;

	using ::lcdMainOnTop;
	using ::lcdMainOnBottom;
	using ::lcdSwap;
	using ::videoSetMode;
	using ::videoSetModeSub;

	using ::scanKeys;
	using ::touchRead;

	// LibFAT 数据类型和函数定义。
	using ::stat;
	using ::DIR_ITER;
	using ::diropen;
	using ::dirnext;
	using ::dirreset;
	using ::dirclose;
	using ::mkdir;
	using ::chdir;
	using ::getcwd;
}


//包含 CHRLib 。

#include "../../CHRLib/chrlib.h"

namespace stdex
{
	using CHRLib::uchar_t;
}

namespace YSLib
{
	namespace Text
	{
		using namespace CHRLib;
	}
	using Text::uchar_t;
}


//包含 YCLib 。

#include "../../YCLib/ycommon.h"

namespace YSLib
{
	namespace Drawing
	{
		using platform_type::BitmapPtr;
		using platform_type::ConstBitmapPtr;
	}
	namespace Runtime
	{
		using platform_type::Keys;
		using platform_type::KeysInfo;
		using platform_type::CursorInfo;
	}
	using platform::yassert;
	namespace DS
	{
		using namespace platform;
	}
}


//包含 FreeType2 。

#include <ft2build.h>

#include FT_FREETYPE_H
#include FT_CACHE_H
#include FT_BITMAP_H
#include FT_GLYPH_H
//#include FT_OUTLINE_H


//包含 Anti-Grain Geometry 。
//#include "yagg.h"


//包含 Loki 。
#include <loki/Function.h>
#include <loki/SmartPtr.h>
#include <loki/StrongPtr.h>

namespace YSLib
{
	namespace Design
	{
		//类型操作和类型特征。
		using Loki::Typelist;
		namespace TL = Loki::TL;
		using Loki::NullType;
		using Loki::EmptyType;
		using Loki::Int2Type;
		using Loki::Type2Type;
		using Loki::Select;
		using Loki::IsCustomUnsignedInt;
		using Loki::IsCustomSignedInt;
		using Loki::IsCustomFloat;
		using Loki::TypeTraits;

		// Function 和 Function 。
		using Loki::Function;
		using Loki::Functor;

		//设计模式。
		namespace Pattern
		{
		}
	}
	//使用 Loki 智能指针。
	using Loki::SmartPtr;
	using Loki::RefCounted;
	using Loki::AllowConversion;
	using Loki::DisallowConversion;
	using Loki::RejectNull;
	using Loki::DefaultSPStorage;
	using Loki::HeapStorage;
	using Loki::RefToValue;
	using Loki::SmallObject;
}

//使用 Loki 的编译期静态检查。
#define YSL_STATIC_CHECK(expr, msg) LOKI_STATIC_CHECK(expr, msg)

//使用 Loki 的 Typelist 。
#define YSL_TL(n, ...) LOKI_TYPELIST##n(...)

//使用智能指针实现的句柄。
//#define YSL_USE_SIMPLE_HANDLE
#define YSL_HANDLEPOLICY_SIMPLE


//多播事件。
#define YSL_EVENT_MULTICAST

#endif

