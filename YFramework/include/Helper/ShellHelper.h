/*
	Copyright (C) by Franksoft 2010 - 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ShellHelper.h
\ingroup Helper
\brief Shell 助手模块。
\version r2462;
\author FrankHB<frankhb1989@gmail.com>
\since build 278 。
\par 创建时间:
	2010-03-14 14:07:22 +0800;
\par 修改时间:
	2012-07-03 17:16 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	Helper::ShellHelper;
*/


#ifndef YSL_INC_HELPER_SHELLHELPER_H_
#define YSL_INC_HELPER_SHELLHELPER_H_

#include "../ysbuild.h"
#include <ystdex/cast.hpp>
#include <YSLib/UI/textlist.h> // for Components::TextList::ListType;

YSL_BEGIN

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 转换为字符串。
\since build 308 。
*/
//@{
PDefTmplH1(_type)
string
to_string(const GBinaryGroup<_type>& val)
{
	using YSL_ to_string;

	return "(" + to_string(val.X) + ", " + to_string(val.Y) + ')';
}
string
to_string(const Size&);
string
to_string(const Rect&);
//@}

YSL_END_NAMESPACE(Drawing)

YSL_BEGIN_NAMESPACE(Text)

/*!
\brief 编码信息项目。
\since build 290 。
*/
typedef std::pair<Encoding, const ucs2_t*> EncodingInfoItem;

/*!
\brief 编码信息。
\since build 290 。
*/
yconstexpr EncodingInfoItem Encodings[] = {{CharSet::UTF_8, u"UTF-8"},
	{CharSet::GBK, u"GBK"},
	{CharSet::UTF_16BE, u"UTF-16 Big Endian"},
	{CharSet::UTF_16LE, u"UTF-16 Little Endian"},
	{CharSet::UTF_32BE, u"UTF-32 Big Endian"},
	{CharSet::UTF_32LE, u"UTF-16 Little Endian"}};

YSL_END_NAMESPACE(Text)


/*!
\ingroup HelperFunctions
*/
//@{

//句柄语法糖。

/*!
\brief 句柄转换：对象引用。
\since 早于 build 132 。
*/
template<class _type, class _tHandle>
inline _type&
HandleToReference(_tHandle h) ythrow(std::bad_cast)
{
	_type* _tmp(dynamic_cast<_type*>(raw(h)));

	if(YB_LIKELY(!_tmp))
		throw std::bad_cast();
	return *_tmp;
}

/*!
\brief 取指定 Shell 句柄对应的 Shell 引用 。
\since build 205 。
*/
template<class _tShell>
inline _tShell&
FetchShell()
{
	auto hShl(FetchShellHandle());

	YAssert(bool(hShl), "Null handle found.");

	return ystdex::polymorphic_downcast<_tShell&>(*hShl);
}


//全局函数。

/*!
\brief 取全局 Shell 句柄。
\since build 195 。
*/
template<class _tShl>
inline shared_ptr<Shell>
FetchStored()
{
	return GLocalStaticCache<_tShl, shared_ptr<Shell>>::GetPointer();
}

/*!
\brief 释放全局 Shell 。
\since build 195 。
*/
template<class _tShl>
inline void
ReleaseStored()
{
	GLocalStaticCache<_tShl, shared_ptr<Shell>>::Release();
}


/*!
\brief 判断句柄指定的 Shell 是否为当前线程空间中运行的 Shell 。
\since 早于 build 132 。
*/
inline bool
IsNowShell(const shared_ptr<Shell>& hShl)
{
	return FetchAppInstance().GetShellHandle() == hShl;
}

/*!
\brief 向句柄指定的 Shell 对象转移线程控制权。
\since 早于 build 132 。
*/
inline errno_t
NowShellTo(const shared_ptr<Shell>& hShl)
{
	return -!Activate(hShl);
}

/*!
\brief 向新建 Shell 对象转移控制权。
\since 早于 build 132 。
*/
template<class _tShl>
inline errno_t
NowShellToNew()
{
	return NowShellTo(new _tShl());
}

/*!
\brief 向全局 Shell 管理器的对象转移控制权。
\since 早于 build 132 。
*/
template<class _tShl>
inline errno_t
NowShellToStored()
{
	return NowShellTo(FetchStored<_tShl>());
}

/*!
\brief 通过主消息队列向指定 Shell 对象转移控制权。
\since 早于 build 132 。
*/
inline void
SetShellTo(const shared_ptr<Shell>& hShl, Messaging::Priority p = 0x80)
{
	// NOTE: It would make the message loop in dead lock when called more
	//	than once specifying on same destination shell.
	PostMessage<SM_SET>(p, hShl);
}

/*!
\brief 通过主消息队列向新建 Shell 对象转移控制权。
\since 早于 build 132 。
*/
template<class _tShl>
inline void
SetShellToNew()
{
	SetShellTo(make_shared<_tShl>());
}

/*!
\brief 通过主消息队列向全局 Shell 管理器内的对象转移控制权。
\since 早于 build 132 。
*/
template<class _tShl>
inline void
SetShellToStored()
{
	SetShellTo(FetchStored<_tShl>());
}

/*!
\brief 封装向全局 Shell 管理器内的对象转移控制权。
\since 早于 build 132 。
*/
template<class _tShl>
inline void
CallStoredAtOnce()
{
	NowShellToStored<_tShl>();
}

/*!
\brief 封装通过消息队列向全局 Shell 管理器内的对象转移控制权。
\since 早于 build 132 。
*/
template<class _tShl>
inline void
CallStored()
{
	SetShellToStored<_tShl>();
}


//资源相关定义和函数。

YSL_BEGIN_NAMESPACE(Drawing)

/*!
\brief 简单屏幕绘图函数指针类型。
\since 早于 build 132 。
*/
typedef void (*PPDRAW)(BitmapPtr, SDst, SDst);

/*!
\brief 全屏幕描点。
\note 颜色由坐标决定。
\since 早于 build 132 。
*/
inline void
ScrDraw(BitmapPtr buf, PPDRAW f)
{
	for(SDst y(0); y < MainScreenHeight; ++y)
		for(SDst x(0); x < MainScreenWidth; ++x)
			f(buf, x, y);
}

/*!
\brief 新建屏幕图像。
\since build 213 。
*/
inline shared_ptr<Image>
CreateSharedScreenImage(ConstBitmapPtr p)
{
	return make_shared<Image>(p, MainScreenWidth, MainScreenHeight);
}

/*!
\brief 使用 new 分配空间并复制无压缩位图。
\since build 213 。
*/
PDefTmplH1(_tPixel)
_tPixel*
CreateRawBitmap(const _tPixel* s, size_t n)
{
	if(YB_LIKELY(s && n))
	{
		size_t size(sizeof(_tPixel) * n);
		_tPixel* d(new _tPixel[size]);

		std::copy_n(s, size, d);
		return d;
	}
	return nullptr;
}

YSL_END_NAMESPACE(Drawing)

/*!
\brief 从全局消息队列中移除所有后台消息。
\since build 320 。
*/
void
RemoveGlobalTasks();


/*!
\brief 默认时间格式字符串。
\since build 264 。
*/
yconstexpr const char* DefaultTimeFormat("%04u-%02u-%02u %02u:%02u:%02u");

/*!
\brief 格式化时间字符串。
\since build 264 。
*/
//@{
const char*
TranslateTime(const std::tm&, const char* = DefaultTimeFormat);
const char*
TranslateTime(const std::time_t&, const char* = DefaultTimeFormat)
	ythrow(GeneralEvent);
//@}


/*!
\brief 取字型家族名称。
\since build 283 。
*/
shared_ptr<Components::TextList::ListType>
FetchFontFamilyNames();


/*!
\brief 帧速率计数器。
\since build 223 。
*/
class FPSCounter
{
private:
	/*!
	\brief 内部计数。
	\note 单位为纳秒。
	\since build 291 。
	*/
	//@{
	u64 last_tick;
	u64 now_tick;
	//@}
	/*!
	\brief 刷新计数。
	\since build 295 。
	*/
	u32 refresh_count;

public:
	/*!
	\brief 计时间隔下界。
	\since build 295 。
	*/
	u64 MinimalInterval;

	/*!
	\brief 构造：使用指定计时间隔下界。
	\since build 295 。
	*/
	FPSCounter(u64 = 0);

	/*!
	\brief 取内部计数。
	\since build 291 。
	*/
	//@{
	DefGetter(const ynothrow, u64, LastTick, last_tick)
	DefGetter(const ynothrow, u64, NowTick, now_tick)
	//@}

	/*!
	\brief 刷新：更新计数器内部计数。
	\return 内部计数差值大于计时间隔下界时的每秒毫计数次数；否则为 0 。
	*/
	u32
	Refresh();
};

//@}

YSL_BEGIN_NAMESPACE(Components)

/*!
\brief 切换部件显示状态并无效化。
\since build 229 。
*/
void
SwitchVisible(IWidget&);


/*!
\brief 序列设置仿函数。
\since build 278 。
*/
struct ContainerSetter
{
public:
	/*!
	\brief 容器。
	\since build 302 。
	*/
	std::reference_wrapper<IWidget> Container;

	ContainerSetter(IWidget& con)
		: Container(con)
	{}

	void
	operator()(IWidget& wgt)
	{
		SetContainerPtrOf(wgt, &Container.get());
	}
};


/*!
\brief 子部件绘制仿函数。
\since build 278 。
*/
struct ChildPainter
{
public:
	/*!
	\brief 绘制上下文。
	\since build 302 。
	*/
	std::reference_wrapper<const PaintContext> Context;

	ChildPainter(const PaintContext& pc)
		: Context(pc)
	{}

	void
	operator()(IWidget& wgt)
	{
		PaintChild(wgt, Context);
	}
};


/*!
\brief 设置部件渲染器为 BufferedRenderer 及部件的 Text 成员。
\since build 301 。
*/
template<class _tWidget>
inline void
SetBufferRendererAndText(_tWidget& wgt, const String& s)
{
	wgt.SetRenderer(make_unique<BufferedRenderer>()),
	wgt.Text = s;
}

YSL_END_NAMESPACE(Components)


YSL_END

#endif

