/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ReaderSetting.cpp
\ingroup YReader
\brief 阅读器设置。
\version r557
\author FrankHB <frankhb1989@gmail.com>
\since build 328
\par 创建时间:
	2012-07-24 22:14:21 +0800
\par 修改时间:
	2013-03-20 20:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ReaderSetting
*/


#include "ReaderSetting.h"

YSL_BEGIN_NAMESPACE(YReader)

using namespace Text;
using std::chrono::milliseconds;

//! \since build 334
namespace
{

//! \since build 339
//@{
template<typename _type>
_type
FetchSetting(const ValueNode&, const string&);

template<>
inline string
FetchSetting<string>(const ValueNode& node, const string& name)
{
	return AccessChild<string>(node, name);
}

template<>
int
FetchSetting<int>(const ValueNode& node, const string& name)
{
	return std::stoi(FetchSetting<string>(node, name));
}

template<>
Color
FetchSetting<Color>(const ValueNode& node, const string& name)
{
	const auto s(FetchSetting<string>(node, name).c_str());
	unsigned r, g, b;

	if(std::sscanf(s, "%u%u%u", &r, &g, &b) != 3)
		throw std::invalid_argument("Color components are not enough.");
#if 0
	if(r < 0x100 && g < 0x100 && b < 0x100)
		return Color(r, g, b);
	throw std::invalid_argument("Invalid color components found.");
#endif
	return Color(min<Color::MonoType>(r, 0xFF), min<Color::MonoType>(g, 0xFF),
		min<Color::MonoType>(b, 0xFF));
}
//@}

//! \since build 345
//@{
ValueNode
ColorToNode(const string& name, const Color& value)
{
	using ystdex::to_string;

	return YSLib::MakeNode(name, to_string(value.GetR()) + ' '
		+ to_string(value.GetG()) + ' ' + to_string(value.GetB()));
}

Font
FetchFontSetting(const ValueNode& node, const string& family,
	const string& size)
{
	if(const auto p = FetchDefaultFontCache().GetFontFamilyPtr(
		FetchSetting<string>(node, family)))
		return Font(*p, FetchSetting<int>(node, size));
	return Font();
}
//@}

} // unnamed namespace;

ReaderSetting::ReaderSetting()
	: UpColor(240, 216, 192), DownColor(192, 216, 240), FontColor(),
	Font(FetchDefaultTypeface().GetFontFamily(), 14), SmoothScroll(true),
	ScrollDuration(1000), SmoothScrollDuration(80)
{}
ReaderSetting::ReaderSetting(const ValueNode& node)
	: UpColor(FetchSetting<Color>(node, "UpColor")), DownColor(
	FetchSetting<Color>(node, "DownColor")), FontColor(FetchSetting<Color>(node,
	"FontColor")), Font(FetchFontSetting(node, "FontFamily", "FontSize")),
	SmoothScroll(FetchSetting<int>(node, "SmoothScroll") != 0),
	ScrollDuration(FetchSetting<int>(node, "ScrollDuration")),
	SmoothScrollDuration(FetchSetting<int>(node, "SmoothScrollDuration"))
{}

ReaderSetting::operator ValueNode() const
{
	return PackNodes("ReaderSetting", ColorToNode("UpColor", UpColor),
		ColorToNode("DownColor", DownColor),
		ColorToNode("FontColor", FontColor),
		MakeNode("FontFamily", Font.GetFontFamily().GetFamilyName()),
		StringifyToNode("FontSize", Font.GetSize()),
		StringifyToNode("SmoothScroll", int(SmoothScroll)),
		StringifyToNode("ScrollDuration", ScrollDuration.count()),
		StringifyToNode("SmoothScrollDuration", SmoothScrollDuration.count()));
}

YSL_END_NAMESPACE(YReader)

