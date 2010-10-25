// YSLib::Adaptor::YAdaptor by Franksoft 2010
// CodePage = UTF-8;
// CTime = 2010-02-22 20:16:21 + 08:00;
// UTime = 2010-10-23 15:38 + 08:00;
// Version = 0.1878;


#ifndef INCLUDED_YADAPTOR_H_
#define INCLUDED_YADAPTOR_H_

// YAdaptor ：外部库关联。

//编译配置。
#include "config.h"

//包含平台文件。
#include <platform.h>

//引入 YSLib 命名空间宏。
#include "base.h"


//包含 CHRLib 。

#include "../../CHRLib/chrlib.h"

namespace stdex
{
	using CHRLib::uchar_t;
	using CHRLib::uint_t;
}

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Text)

	using namespace CHRLib;

YSL_END_NAMESPACE(Text)

	using Text::fchar_t;
	using Text::uchar_t;
	using Text::uint_t;

YSL_END

//包含 YCLib 。

#include "../../YCLib/ycommon.h"

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

	using platform::PixelType;
	using platform::BitmapPtr;
	using platform::ConstBitmapPtr;
	using platform::ScreenBufferType;
	using platform::Color;
	namespace ColorSpace = platform::ColorSpace;

YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Runtime)

	namespace KeySpace = platform::KeySpace;
	using platform::KeysInfo;
	using platform::CursorInfo;

YSL_END_NAMESPACE(Runtime)

	using platform::terminate;
	using platform::HDirectory;

	namespace DS
	{
		using namespace platform;

		using ::swiWaitForVBlank;

		using ::lcdMainOnTop;
		using ::lcdMainOnBottom;
		using ::lcdSwap;
		using ::videoSetMode;
		using ::videoSetModeSub;

		using ::scanKeys;
		using ::touchRead;
	}

YSL_END


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

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Design)

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
YSL_BEGIN_NAMESPACE(Pattern)
YSL_END_NAMESPACE(Pattern)

YSL_END_NAMESPACE(Design)

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

YSL_END

//使用 Loki 的编译期静态检查。
#define YSL_STATIC_CHECK(expr, msg) LOKI_STATIC_CHECK(expr, msg)

//使用 Loki 的 Typelist 。
#define YSL_TL(n, ...) LOKI_TYPELIST##n(...)


#endif

