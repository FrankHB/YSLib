/*
	© 2010-2016, 2019, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Shells.cpp
\ingroup YReader
\brief Shell 框架逻辑。
\version r6477
\author FrankHB <frankhb1989@gmail.com>
\since 早于 build 132
\par 创建时间:
	2010-03-06 21:38:16 +0800
\par 修改时间:
	2021-10-22 18:10 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::Shells
*/


#include "Shells.h"
#include "ShlExplorer.h"
#include "ShlReader.h"
#include <ytest/timing.hpp>
#if YReader_Impl_TestNPL
#	if YF_Hosted
#		define YReader_Impl_TestNPL_UseBacktrace true
#		include <ystdex/scope_guard.hpp> // for ystdex::make_guard;
#		define YReader_Impl_TestNPL_UseSourceInfo true
#	endif
#	include YFM_NPL_Dependency // for NPL, LoadStandardContext, InvokeIn,
//	ContextNode;
#endif

namespace YReader
{

//! \since build 601
using Timers::HighResolutionClock;

namespace
{

ResourceMap GlobalResourceMap;
//! \since build 601
HighResolutionClock::duration gfx_init_time;

} // unnamed namespace;

DeclResource(GR_BGs)

namespace
{

using namespace YReader;

template<typename _tTarget>
_tTarget&
FetchGlobalResource(ResourceIndex idx)
{
	if(!GlobalResourceMap[idx])
		GlobalResourceMap[idx] = ValueObject(new _tTarget(), PointerTag());
	return GlobalResourceMap[GR_BGs].GetObject<_tTarget>();
}

shared_ptr<Image>&
FetchGlobalImage(size_t idx)
{
	auto& spi(FetchGlobalResource<array<shared_ptr<Image>, 10>>(GR_BGs));

	YAssert(IsInInterval(idx, size_t(10)), "Index is out of range.");
	return spi[idx];
}

} // unnamed namespace;

using namespace Shells;
using namespace Drawing::ColorSpace;

void
Cleanup()
{
	for(size_t i(0); i != Image_N; ++i)
		FetchGlobalImage(i).reset();
	GlobalResourceMap.clear();
}

shared_ptr<Image>&
FetchImage(size_t i)
{
	static Color(*const p_bg[Image_N])(SDst, SDst){
		[](SDst x, SDst y) ynothrow -> Color{
			return {~(x * y) >> 2, x | y | 128, 240 - ((x & y) >> 1)};
		},
		[](SDst x, SDst y) ynothrow -> Color{
			return {(x << 4) / (y | 1), (x | y << 1) % (y + 2),
				(~y | x << 1) % 27 + 3};
		},
		[](SDst x, SDst y) ynothrow -> Color{
			return {~(x * y) >> 2, x | y | 128, 240 - ((x & y) >> 1)};
		}
	};

	auto& h(FetchGlobalImage(i));

	if(!h && p_bg[i])
	{
		h = make_shared<Image>(nullptr, MainScreenWidth, MainScreenHeight);
		gfx_init_time += ytest::timing::once(HighResolutionClock::now,
			[](BitmapPtr buf, decltype(*p_bg) f){
				for(SDst y(0); y < MainScreenHeight; ++y)
					for(SDst x(0); x < MainScreenWidth; yunseq(++x, ++buf))
						*buf = f(x, y);
			}, h->GetBufferPtr(), p_bg[i]);
	}
	return h;
}

double
FetchImageLoadTime()
{
	return gfx_init_time.count() / 1e9;
}


WidgetLoader&
FetchWidgetLoader()
{
	static WidgetLoader wgt_ldr;
	static const struct Init
	{
		Init()
		{
#define YSL_RegWidget Widget, Control, Panel, Window, Label, \
	Button, CheckButton, RadioButton, DropDownList, FileBox, \
	TextBox, TabBar, TabControl, TreeList, TreeView
#define YSL_RegWidget_Str {"Widget", "Control", "Panel", "Window", "Label", \
	"Button", "CheckButton", "RadioButton", "DropDownList", "FileBox", \
	"TextBox", "TabBar", "TabControl", "TreeList", "TreeView"}

			wgt_ldr.Default.Register<YSL_RegWidget>(YSL_RegWidget_Str),
			wgt_ldr.Bounds.Register<YSL_RegWidget>(YSL_RegWidget_Str),
			wgt_ldr.Insert.Register<Panel, TabBar, TabControl>({"Panel",
				"TabBar", "TabControl"}),
			wgt_ldr.InsertZOrdered.Register<Panel>({"Panel"});
#undef YSL_RegWidget_Str
#undef YSL_RegWidget
		}
	} obj;

	yunused(obj);
	return wgt_ldr;
}

void
AddButtonToTabBar(TabControl& tc, ValueNode& node, const string& name,
	const String& text, SDst w)
{
	auto& tb(tc.GetTabBarRef());
	auto p_tab(make_shared<Button>(Rect({}, w, tc.BarHeight)));

	p_tab->Text = text,
	tb += *p_tab,
	tc.Attach(*p_tab);
	node += AsNode(name, std::move(p_tab));
}


#if YReader_Impl_TestNPL
void
TestNPL(NPL::pmr::memory_resource& rsrc, std::istream&& is, std::ostream& os)
{
	using namespace NPL;
	using namespace A1;
	using namespace Forms;
	REPLContext context{rsrc};
	TermNode term{context.Allocator};
#	if YReader_Impl_TestNPL_UseBacktrace
	ContextNode::ReducerSequence backtrace{context.Allocator};
#	endif

#	if YReader_Impl_TestNPL_UseSourceInfo
	context.UseSourceLocation = true;
#	endif
	LoadStandardContext(context);
	context.OutputStreamPtr = NPL::make_observer(&os);

	auto& rctx(context.Root);

	InvokeIn(rctx, [&]{
		context.ShareCurrentSource("*TEST*");
		try
		{
			context.Root.Rewrite(NPL::ToReducer(context.Allocator,
				any_ops::trivial_swap, [&](ContextNode& ctx){
				ctx.SaveExceptionHandler();
				// TODO: Blocked. Use C++14 lambda initializers to simplify the
				//	implementation.
				ctx.HandleException = ystdex::bind1([&](std::exception_ptr p,
					const ContextNode::ReducerSequence::const_iterator& i){
					ctx.TailAction = nullptr;
#	if YReader_Impl_TestNPL_UseBacktrace
					ctx.Shift(backtrace, i);
#	endif
					YAssertNonnull(p);
					TryExpr(std::rethrow_exception(std::move(p)))
					catch(std::exception& e)
					{
#	if YReader_Impl_TestNPL_UseBacktrace
						const auto gd(ystdex::make_guard([&]() ynothrowv{
							backtrace.clear();
						}));
#	endif
						auto& trace(ctx.Trace);

						TraceException(e, trace);
						trace.TraceFormat(Notice, "Location: %s.",
							context.CurrentSource
							? context.CurrentSource->c_str() : "<unknown>");
#	if YReader_Impl_TestNPL_UseBacktrace
						TraceBacktrace(backtrace, trace);
#	endif
						// NOTE: As %Tools.SHBuild.Main.
						throw NPLException("Error detected in the execution"
							" (see the backtrace for details).");
					}
				}, ctx.GetCurrent().cbegin());
				term = context.ReadFrom(is);
				return A1::ReduceOnce(term, ctx);
			}));
		}
		// NOTE: As %A1::TryLoadSource.
		CatchExpr(..., std::throw_with_nested(
			NPLException("Failed loading the test unit.")));
	});
}
#endif

} // namespace YReader;

