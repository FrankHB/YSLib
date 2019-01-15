/*
	© 2012-2013, 2015-2016, 2019 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file ReaderSetting.cpp
\ingroup YReader
\brief 阅读器设置。
\version r603
\author FrankHB <frankhb1989@gmail.com>
\since build 328
\par 创建时间:
	2012-07-24 22:14:21 +0800
\par 修改时间:
	2019-01-14 18:34 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YReader::ReaderSetting
*/


#include "ReaderSetting.h" // ystdex::min;

namespace YReader
{

using namespace Text;
using std::chrono::milliseconds;

//! \since build 334
namespace
{

//! \since build 399
//@{
template<typename _type>
YB_ATTR_nodiscard YB_PURE _type
FetchSetting(const ValueNode::Container&, const string&);

template<>
YB_ATTR_nodiscard YB_PURE inline string
FetchSetting<string>(const ValueNode::Container& con, const string& name)
{
	return Access<string>(AccessNode(con, name));
}

template<>
YB_ATTR_nodiscard YB_PURE int
FetchSetting<int>(const ValueNode::Container& con, const string& name)
{
	return std::stoi(FetchSetting<string>(con, name));
}

template<>
YB_ATTR_nodiscard YB_PURE Color
FetchSetting<Color>(const ValueNode::Container& con, const string& name)
{
	const auto s(FetchSetting<string>(con, name).c_str());
	unsigned r, g, b;

	if(std::sscanf(s, "%u%u%u", &r, &g, &b) != 3)
		throw std::invalid_argument("Color components are not enough.");
#if false
	if(r < 0x100 && g < 0x100 && b < 0x100)
		return Color(r, g, b);
	throw std::invalid_argument("Invalid color components found.");
#endif
	return Color(ystdex::min<MonoType>(r, 0xFF), ystdex::min<MonoType>(g, 0xFF),
		ystdex::min<MonoType>(b, 0xFF));
}
//@}

//! \since build 345
YB_ATTR_nodiscard YB_PURE ValueNode
ColorToNode(const string& name, const Color& value)
{
	return YSLib::AsNode(name, to_string(value.GetR()) + ' '
		+ to_string(value.GetG()) + ' ' + to_string(value.GetB()));
}

//! \since build 399
YB_ATTR_nodiscard YB_PURE Font
FetchFontSetting(const ValueNode::Container& con, const string& family,
	const string& size)
{
	if(const auto p = FetchDefaultFontCache().GetFontFamilyPtr(
		FetchSetting<string>(con, family)))
		return Font(*p, FetchSetting<int>(con, size));
	return {};
}

} // unnamed namespace;

ReaderSetting::ReaderSetting()
	: UpColor(240, 216, 192), DownColor(192, 216, 240), FontColor(),
	Font(FetchDefaultTypeface().GetFontFamily(), 14), SmoothScroll(true),
	ScrollDuration(1000), SmoothScrollDuration(80)
{}
ReaderSetting::ReaderSetting(const ValueNode::Container& con)
	: UpColor(FetchSetting<Color>(con, "UpColor")), DownColor(
	FetchSetting<Color>(con, "DownColor")), FontColor(FetchSetting<Color>(con,
	"FontColor")), Font(FetchFontSetting(con, "FontFamily", "FontSize")),
	SmoothScroll(FetchSetting<int>(con, "SmoothScroll") != 0),
	ScrollDuration(FetchSetting<int>(con, "ScrollDuration")),
	SmoothScrollDuration(FetchSetting<int>(con, "SmoothScrollDuration"))
{}

ReaderSetting::operator ValueNode::Container() const
{
	return ValueNode::Container{ColorToNode("UpColor", UpColor),
		ColorToNode("DownColor", DownColor),
		ColorToNode("FontColor", FontColor),
		AsNode("FontFamily", Font.GetFontFamily().GetFamilyName()),
		StringifyToNode("FontSize", Font.GetSize()),
		StringifyToNode("SmoothScroll", int(SmoothScroll)),
		StringifyToNode("ScrollDuration", ScrollDuration.count()),
		StringifyToNode("SmoothScrollDuration", SmoothScrollDuration.count())};
}

} // namespace YReader;

