/*
	© 2010-2016, 2019, 2021 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShellHelper.h
\ingroup Helper
\brief Shell 助手模块。
\version r2063
\author FrankHB <frankhb1989@gmail.com>
\since build 278
\par 创建时间:
	2010-03-14 14:07:22 +0800
\par 修改时间:
	2021-08-01 03:29 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Helper::ShellHelper
*/


#ifndef INC_Helper_ShellHelper_h_
#define INC_Helper_ShellHelper_h_ 1

#include <Helper/YModules.h>
#include <YCLib/YModules.h>
#include YFM_YCLib_Debug
#include YFM_YSLib_UI_YDesktop // for UI::Desktop;
#include YFM_YSLib_Core_YApplication // for Shell, FetchAppInstance;
#include YFM_YSLib_Service_YTimer // for Timers::HighResolutionClock;
#include <ystdex/cast.hpp> // for ystdex::polymorphic_downcast;
#include YFM_YSLib_UI_ListControl // for UI::TextList::ListType;

namespace YSLib
{

#ifndef NDEBUG
/*!
\ingroup debugging
\brief 调试计时器。
\since build 378
\todo 使用符合约定的 protected 命名。
*/
class YF_API DebugTimer
{
protected:
	//! \since build 593
	string event_info;
	Timers::HighResolutionClock::time_point base_tick;

public:
	/*!
	\pre 间接断言：参数的数据指针非空。
	\since build 658
	*/
	DebugTimer(string_view = "");
	~DebugTimer();
};
#	define YSL_DEBUG_DECL_TIMER(_name, ...) DebugTimer _name(__VA_ARGS__);
#else
#	define YSL_DEBUG_DECL_TIMER(...)
#endif


/*!
\brief Shell 连接会话。
\since build 696
*/
template<class _tShell, typename _tConnection = lref<_tShell>>
class GShellSession
{
public:
	using Shell = _tShell;
	using Connection = _tConnection;

private:
	Connection conn;

public:
	template<typename... _tParam>
	GShellSession(_tParam&&... args)
		: conn(yforward(args)...)
	{}

	virtual DefDeDtor(GShellSession)

	DefGetter(const ynothrow, _tShell&, Shell, conn)
};


namespace Text
{

/*!
\brief 编码信息项目。
\since build 641
*/
using EncodingInfoItem = pair<Encoding, const char16_t*>;

/*!
\brief 编码信息。
\since build 307
*/
#if __cplusplus > 201103L
// XXX: The constructor of %std::pair used here has 'constexpr' since ISO C++14.
yconstexpr
#endif
	const EncodingInfoItem Encodings[]{{CharSet::UTF_8, u"UTF-8"},
	{CharSet::GBK, u"GBK"}, {CharSet::UTF_16BE, u"UTF-16 Big Endian"},
	{CharSet::UTF_16LE, u"UTF-16 Little Endian"},
	{CharSet::UTF_32BE, u"UTF-32 Big Endian"},
	{CharSet::UTF_32LE, u"UTF-32 Little Endian"}};

} // namespace Text;


//! \ingroup helper_functions
//@{
/*!
\brief 取指定 Shell 句柄对应的 Shell 引用 。
\since build 205
*/
template<class _tShell>
inline _tShell&
FetchShell()
{
	return ystdex::polymorphic_downcast<_tShell&>(Deref(FetchShellHandle()));
}


/*!
\brief 判断句柄指定的 Shell 是否为当前线程空间中运行的 Shell 。
\since 早于 build 132
*/
inline PDefH(bool, IsNowShell, const shared_ptr<Shell>& hShl)
	ImplRet(FetchAppInstance().GetShellHandle() == hShl)

/*!
\brief 向句柄指定的 Shell 对象转移线程控制权。
\since build 594
*/
inline PDefH(void, NowShellTo, const shared_ptr<Shell>& hShl)
	ImplExpr(Activate(hShl))

/*!
\brief 通过主消息队列向指定 Shell 对象转移控制权。
\since 早于 build 132
*/
inline PDefH(void, SetShellTo, const shared_ptr<Shell>& hShl,
	Messaging::Priority prior = Messaging::NormalPriority)
	// NOTE: It would make the message loop in dead lock when called more
	//	than once specifying on same destination shell.
	ImplRet(PostMessage<SM_Set>(prior, hShl))


/*!
\brief 复位桌面。
\since build 396

销毁每个桌面并在原存储位置创建新的对象。
*/
inline PDefH(void, ResetDesktop, UI::Desktop& dsk, Devices::Screen& scr)
	ImplExpr(dsk.~Desktop(), new(&dsk) UI::Desktop(scr))


namespace Drawing
{

//! \since build 360
inline PDefH(Color, GenerateRandomColor, )
//使用 std::time(0) 初始化随机数种子在 DeSmuME 上无效。
//	std::srand(std::time(0));
	ImplRet(Color(std::rand(), std::rand(), std::rand(), 1))

} // namespace Drawing;

/*!
\brief 从全局消息队列中移除所有后台消息。
\since build 320
*/
YF_API void
RemoveGlobalTasks();


/*!
\brief 默认时间格式字符串。
\since build 307
*/
yconstexpr const char* const DefaultTimeFormat("%04u-%02u-%02u %02u:%02u:%02u");

/*!
\brief 格式化时间字符串。
\warning 非线程安全。
\since build 307
*/
//@{
YF_API YB_ATTR_returns_nonnull const char*
TranslateTime(const std::tm&, const char* = DefaultTimeFormat);
//! \since build 563
YF_API YB_ATTR_returns_nonnull const char*
TranslateTime(const std::time_t&, const char* = DefaultTimeFormat);
//@}


/*!
\brief 取字型家族名称。
\post 结果是指向非空列表的非空指针。
\since build 307
*/
YF_API shared_ptr<UI::TextList::ListType>
FetchFontFamilyNames();


/*!
\brief 帧速率计数器。
\since build 307
*/
class YF_API FPSCounter
{
private:
	/*!
	\brief 内部计数。
	\since build 405
	*/
	std::chrono::nanoseconds last_tick, now_tick;
	/*!
	\brief 刷新计数。
	\since build 405
	*/
	size_t refresh_count;

public:
	/*!
	\brief 计时间隔下界。
	\since build 405
	*/
	std::chrono::nanoseconds MinimalInterval;

	/*!
	\brief 构造：使用指定计时间隔下界。
	\since build 405
	*/
	FPSCounter(std::chrono::nanoseconds = {});

	/*!
	\brief 取内部计数。
	\since build 405
	*/
	//@{
	DefGetter(const ynothrow, std::chrono::nanoseconds, LastTick, last_tick)
	DefGetter(const ynothrow, std::chrono::nanoseconds, NowTick, now_tick)
	//@}

	/*!
	\brief 刷新：更新计数器内部计数。
	\return 内部计数差值大于计时间隔下界时的每秒毫计数次数；否则为 0 。
	\since build 405
	*/
	size_t
	Refresh();
};

//@}

namespace UI
{

/*!
\brief 切换部件显示状态并无效化。
\since build 229
*/
YF_API void
SwitchVisible(IWidget&);

/*!
\brief 切换部件显示状态并请求提升至前端。
\since build 467
*/
YF_API void
SwitchVisibleToFront(IWidget&);


/*!
\brief 设置部件渲染器为 BufferedRenderer 及部件的 Text 成员。
\since build 301
*/
template<class _tWidget>
inline void
SetBufferRendererAndText(_tWidget& wgt, const String& str)
{
	wgt.SetRenderer(make_unique<BufferedRenderer>()),
	wgt.Text = str;
}


/*!
\brief 取视觉样式名称。
\post 结果是指向非空列表的非空指针。
\note 使用参数替换默认名称的空串。
\since build 469
*/
YF_API shared_ptr<TextList::ListType>
FetchVisualStyleNames(String = u"<Default>");

} // namespace UI;

} // namespace YSLib;

#endif

