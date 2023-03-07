/*
	© 2012, 2014-2016, 2018, 2020-2021, 2023 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YConsole.h
\ingroup Core
\brief 通用控制台接口。
\version r295
\author FrankHB <frankhb1989@gmail.com>
\since build 585
\par 创建时间:
	2015-03-17 17:44:34 +0800
\par 修改时间:
	2023-03-07 19:11 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YConsole
*/


#ifndef YSL_INC_Core_YConsole_h_
#define YSL_INC_Core_YConsole_h_ 1

#include "YModules.h"
#include YFM_YSLib_Core_YGDIBase // for Drawing::ColorSpace, std::uint8_t,
//	Nonnull, string_view, string;
#include <ystdex/cstring.h> // for ystdex::is_nonempty;
#include <cstdlib> // for std::getenv;
#include <cstdio> // for std::FILE;
#include <ystdex/enum.hpp> // for ystdex::underlying;

namespace YSLib
{

/*!
\brief 控制台接口。
\since build 328
*/
namespace Consoles
{

//! \since build 969
//!@{
/*!
\brief 判断 \c NO_COLOR 环境变量被设置为非空值。
\see https://no-color.org 。
*/
YB_ATTR_nodiscard inline PDefH(bool, DetectsNoColor, ) ynothrow
	ImplRet(ystdex::is_nonempty(std::getenv("NO_COLOR")))

/*!
\brief 判断没有 \c NO_COLOR 环境变量被设置。
\note 在第一次调用时判断并缓存结果。
\sa DetectsNoColor
*/
YB_ATTR_nodiscard YF_API bool
AllowsColorByEnvironment() ynothrow;
//!@}


//! \since build 585
namespace ColorSpace = Drawing::ColorSpace;

/*!
\brief 控制台颜色枚举。
\since build 416
\see ISO/IEC 6429:1992 8.3.118 。
\see https://invisible-island.net/ncurses/ 。

枚举值的数值为颜色代码，同 SVr4 curses 实现。
Microsoft Windows 控制台和 FreeBSD syscons 也使用相同的颜色代码。
和 ANSI 转义序列使用的 SGR 代码的顺序存在差异，参见下列 ncurses 的实现文件：
ncurses/base/lib_color.c
ncurses/tinfo/tinfo_driver.c
ncurses/win32con/windriver.c
*/
enum Color
{
	Black = 0,
	DarkBlue,
	DarkGreen,
	DarkCyan,
	DarkRed,
	DarkMagenta,
	DarkYellow,
	Gray,
	DarkGray,
	Blue,
	Green,
	Cyan,
	Red,
	Magenta,
	Yellow,
	White
};

/*!
\brief 控制台颜色。
\note 顺序和 Consoles::Color 对应。
\since build 328
\sa ColorSpace::ColorSet
\see https://en.wikipedia.org/wiki/ANSI_escape_code#3-bit_and_4-bit 。

基准控制台颜色。
指派 Microsoft Windows XP 控制台使用的相同的饱和 8 色颜色值。
*/
yconstexpr const ColorSpace::ColorSet ConsoleColors[]{ColorSpace::Black,
	ColorSpace::Navy, ColorSpace::Green, ColorSpace::Teal, ColorSpace::Maroon,
	ColorSpace::Purple, ColorSpace::Olive, ColorSpace::Silver, ColorSpace::Gray,
	ColorSpace::Blue, ColorSpace::Lime, ColorSpace::Aqua, ColorSpace::Red,
	ColorSpace::Yellow, ColorSpace::Fuchsia, ColorSpace::White};

//! \since build 969
//!@{
/*!
\brief SGR 索引映射。

映射 Color 枚举值的初始值到 ISO/IEC 6429:1992 8.3.118 的 SGR 代码偏移值。
映射结果是代码减初始索引值 30 。
*/
yconstexpr const size_t SGRIndexMap[] = {0, 4, 2, 6, 1, 5, 3, 7};

/*!
\brief 转换颜色值为 SGR 索引值对应的个位数值字符。
\since build 969
*/
YB_ATTR_nodiscard YB_STATELESS yconstfn
	PDefH(char, ColorToSGRChar, std::uint8_t c) ynothrow
	ImplRet('0' + SGRIndexMap[c & 7])


/*!
\brief 输出终端接口。

提供终端输出功能的设备接口。
*/
DeclI(YF_API, IOutputTerminal)
	//! \return 操作成功完成。
	//!@{
	//! \brief 清除显示的内容并复位光标到起始位置。
	YB_ATTR_nodiscard DeclIEntry(bool Clear())

	//! \brief 重置属性。
	YB_ATTR_nodiscard DeclIEntry(bool RestoreAttributes())

	//! \brief 更新背景色。
	YB_ATTR_nodiscard DeclIEntry(bool UpdateBackColor(std::uint8_t))

	//! \brief 更新前景色。
	YB_ATTR_nodiscard DeclIEntry(bool UpdateForeColor(std::uint8_t))

	//! \brief 更新下划线启用状态。
	YB_ATTR_nodiscard DeclIEntry(bool UpdateUnderline(bool))

	//! \brief 写字符串。
	YB_ATTR_nodiscard YB_NONNULL(2)
		DeclIEntry(bool WriteString(const char*, size_t))
	//!@}
EndDecl


//! \brief 使用打印实现操作的终端抽象类。
class YF_API AOutputTerminal : private noncopyable, private nonmovable,
	implements IOutputTerminal
{
private:
	//! \invariant \c stream 。
	std::FILE* stream;

public:
	YB_NONNULL(2) explicit
	AOutputTerminal(std::FILE* fp) ynothrowv
		: stream(Nonnull(fp))
	{}

	YB_ATTR_nodiscard YB_ATTR_returns_nonnull
		DefGetter(const ynothrow, std::FILE*, Stream, Nonnull(stream))

	YB_ATTR_nodiscard YB_NONNULL(2)
		PDefH(bool, PrintEscape, const char* s) const ynothrowv
		ImplRet(PrintFormat("\033[%s", s))

	template<typename... _tParams>
	YB_ATTR_nodiscard YB_NONNULL(2) bool
	PrintFormat(const char* fmt, _tParams&&... args) const ynothrowv
	{
		std::fprintf(GetStream(), Nonnull(fmt), yforward(args)...);
		return true;
	}

	YB_ATTR_nodiscard YB_NONNULL(2)
		PDefH(bool, PrintString, const char* s) const ynothrowv
		ImplRet(PrintFormat("%s", s))

	YB_ATTR_nodiscard YB_NONNULL(2) bool
	WriteString(const char*, size_t) ImplI(IOutputTerminal);
};


//! \brief 使用 \c tput 命令实现操作的终端。
class YF_API TPutTerminal : public AOutputTerminal
{
public:
	YB_NONNULL(2) explicit
	TPutTerminal(std::FILE* fp) ynothrowv
		: AOutputTerminal(fp)
	{}
	//! \brief 虚析构：类定义外默认实现。
	~TPutTerminal() override;

	/*!
	\brief 输出缓存的命令结果。
	\pre 断言：参数的数据指针非空。
	*/
	YB_ATTR_nodiscard bool
	ExecuteCachedCommand(string_view) const;

	YB_ATTR_nodiscard PDefH(bool, Clear, ) ImplI(AOutputTerminal)
		ImplRet(ExecuteCachedCommand("tput clear"))

	YB_ATTR_nodiscard PDefH(bool, RestoreAttributes, ) ImplI(AOutputTerminal)
		ImplRet(ExecuteCachedCommand("tput sgr0"))

	YB_ATTR_nodiscard PDefH(bool, UpdateBackColor, std::uint8_t c)
		ImplI(AOutputTerminal)
		ImplRet(UpdateWithColorCommand(c, "tput setab "))

	YB_ATTR_nodiscard PDefH(bool, UpdateForeColor, std::uint8_t c)
		ImplI(AOutputTerminal)
		ImplRet(UpdateWithColorCommand(c, "tput setaf "))

	YB_ATTR_nodiscard PDefH(bool, UpdateUnderline, bool enabled)
		ImplI(AOutputTerminal)
		ImplRet(ExecuteCachedCommand(enabled ? "tput smul" : "tupt rmul"))

private:
	YB_ATTR_nodiscard YB_NONNULL(3)
		PDefH(bool, UpdateWithColorCommand, std::uint8_t c, const char* cmd)
		ImplRet(ExecuteCachedCommand((yimpl(std::)string(cmd)
			+ Consoles::ColorToSGRChar(c)).data()) && (c < ystdex::underlying(
			DarkGray) || ExecuteCachedCommand("tput bold")))
};


//! \brief 使用 ISO/IEC 6429 转义序列实现操作的终端。
class YF_API ISO6429Terminal : public AOutputTerminal
{
public:
	YB_NONNULL(2) explicit
	ISO6429Terminal(std::FILE* fp) ynothrowv
		: AOutputTerminal(fp)
	{}
	//! \brief 虚析构：类定义外默认实现。
	~ISO6429Terminal() override;

	YB_ATTR_nodiscard PDefH(bool, Clear, ) ynothrow ImplI(AOutputTerminal)
		ImplRet(PrintEscape("H\033[J"))

	YB_ATTR_nodiscard PDefH(bool, RestoreAttributes, ) ynothrow
		ImplI(AOutputTerminal)
		ImplRet(PrintEscape("0;10m"))

	YB_ATTR_nodiscard PDefH(bool, UpdateBackColor, std::uint8_t c) ynothrow
		ImplI(AOutputTerminal)
		ImplRet(PrintFormat("\033[%s%cm", c < ystdex::underlying(DarkGray)
			? "4" : "10", Consoles::ColorToSGRChar(c)))

	YB_ATTR_nodiscard PDefH(bool, UpdateForeColor, std::uint8_t c) ynothrow
		ImplI(AOutputTerminal)
		ImplRet(PrintFormat("\033[%c%cm", c < ystdex::underlying(DarkGray)
			? '3' : '9', Consoles::ColorToSGRChar(c)))

	YB_ATTR_nodiscard PDefH(bool, UpdateUnderline, bool enabled)
		ImplI(AOutputTerminal)
		ImplRet(PrintEscape(&"24m"[enabled ? 1 : 0]))
};


//! \brief 派生基类并在所有操作中跳过属性设置的输出终端类模板。
template<class _tOutTerm>
class GDumbOutputTerminal final : public _tOutTerm
{
	using _tOutTerm::_tOutTerm;

	YB_ATTR_nodiscard YB_STATELESS PDefH(bool, RestoreAttributes, ) ynothrow
		override
		ImplRet(true)

	YB_ATTR_nodiscard YB_STATELESS PDefH(bool, UpdateBackColor, std::uint8_t)
		ynothrow override
		ImplRet(true)

	YB_ATTR_nodiscard YB_STATELESS PDefH(bool, UpdateForeColor, std::uint8_t)
		ynothrow override
		ImplRet(true)

	YB_ATTR_nodiscard YB_STATELESS PDefH(bool, UpdateUnderline, bool) ynothrow
		override
		ImplRet(true)
};


/*!
\brief 根据等级设置终端的前景色。
\since build 969
*/
template<class _tOutTerm>
void
UpdateForeColorByLevel(_tOutTerm& te, RecordLevel lv)
{
	static yconstexpr const RecordLevel
		lvs[]{Err, Warning, Notice, Informative, Debug};
	static yconstexpr const Color
		colors[]{Red, Yellow, Cyan, Magenta, DarkGreen};
	const auto
		i(std::lower_bound(ystdex::begin(lvs), ystdex::end(lvs), lv));

	if(i == ystdex::end(lvs))
		te.RestoreAttributes();
	else
		te.UpdateForeColor(colors[i - lvs]);
}
//!@}

} // namespace Consoles;

} // namespace YSLib;

#endif

