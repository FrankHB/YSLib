/*
	Copyright by FrankHB 2010 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Shells.cpp
\ingroup YReader
\brief Shell 框架逻辑。
\version r6272
\author FrankHB<frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-06 21:38:16 +0800
\par 修改时间:
	2013-03-20 21:36 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::Shells
*/


#include "Shells.h"
#include "ShlExplorer.h"
#include "ShlReader.h"
#include <ytest/timing.hpp>

////////
//测试用声明：全局资源定义。
//extern char gstr[128];

using namespace ystdex;

YSL_BEGIN_NAMESPACE(YReader)

namespace
{

ResourceMap GlobalResourceMap;
//! \since build 319
double gfx_init_time;

} // unnamed namespace;

DeclResource(GR_BGs)

namespace
{

using namespace YReader;

//测试函数。

template<typename _tTarget>
_tTarget&
FetchGlobalResource(ResourceIndex idx)
{
	if(!GlobalResourceMap[idx])
		GlobalResourceMap[idx] = MakeValueObjectByPtr(new _tTarget());
	return GlobalResourceMap[GR_BGs].GetObject<_tTarget>();
}

shared_ptr<Image>&
FetchGlobalImage(size_t idx)
{
	auto& spi(FetchGlobalResource<array<shared_ptr<Image>, 10>>(GR_BGs));

	YAssert(IsInInterval(idx, 10u), "Index is out of range.");
	return spi[idx];
}

} // unnamed namespace;

YSL_END_NAMESPACE(YReader)


YSL_BEGIN

void
ReleaseShells()
{
	using namespace YReader;

	for(size_t i(0); i != 10; ++i)
		FetchGlobalImage(i).reset();
	GlobalResourceMap.clear();
	ReleaseStored<ShlReader>();
	ReleaseStored<ShlExplorer>();
}

YSL_END


YSL_BEGIN_NAMESPACE(YReader)

using namespace Shells;
using namespace Drawing::ColorSpace;

shared_ptr<Image>&
FetchImage(size_t i)
{
	static Color(*const p_bg[10])(SDst, SDst){nullptr,
		[](SDst x, SDst y)->Color{
			return {~(x * y) >> 2, x | y | 128, 240 - ((x & y) >> 1)};
		},
		[](SDst x, SDst y)->Color{
			return {(x << 4) / (y | 1), (x | y << 1) % (y + 2),
				(~y | x << 1) % 27 + 3};
		},
		[](SDst x, SDst y)->Color{
			return {~(x * y) >> 2, x | y | 128, 240 - ((x & y) >> 1)};
		},
		[](SDst x, SDst y)->Color{
			return {x + y * y, (x & y) ^ (x | y), x * x + y};
		},
		[](SDst x, SDst y)->Color{
			return {(x << 4) / ((y & 1) | 1), ~x % 101 + y,
				(x + y) % (((y - 2) & 1) | 129) + (x << 2)};
		},
		[](SDst x, SDst y)->Color{
			return {(x | y) % (y + 2), (~y | x) % 27 + 3, (x << 6) / (y | 1)};
		}
	};

	if(!FetchGlobalImage(i) && p_bg[i])
	{
		auto& h(FetchGlobalImage(i));

		if(!h)
			h = make_shared<Image>(nullptr, MainScreenWidth, MainScreenHeight);
		gfx_init_time += ytest::timing::once(Timers::HighResolutionClock::now,
			ScrDraw<BitmapPtr, decltype(*p_bg)>,
			h->GetBufferPtr(), p_bg[i]).count() / 1e9;
	}
	return FetchGlobalImage(i);
}

double
FetchImageLoadTime()
{
	return gfx_init_time;
}

YSL_END_NAMESPACE(YReader)

