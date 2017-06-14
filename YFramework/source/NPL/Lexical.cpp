/*
	© 2012-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Lexical.cpp
\ingroup NPL
\brief NPL 词法处理。
\version r1604
\author FrankHB <frankhb1989@gmail.com>
\since build 335
\par 创建时间:
	2012-08-03 23:04:26 +0800
\par 修改时间:
	2017-06-13 15:53 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Lexical
*/


#include "NPL/YModules.h"
#include YFM_NPL_Lexical
#include <ystdex/string.hpp> // for ystdex::get_mid;

namespace NPL
{

string
UnescapeContext::Done()
{
	auto res(std::move(Prefix));

	res += std::move(sequence);
	Clear();
	return res;
}


bool
HandleBackslashPrefix(char c, string& pfx)
{
	if(c == '\\')
	{
		pfx = "\\";
		return true;
	}
	return {};
}

bool
NPLUnescape(string& buf, const UnescapeContext& uctx, char ld)
{
	const auto& escs(uctx.GetSequence());

	if(uctx.IsHandling() && escs.length() == 1)
	{
		switch(escs[0])
		{
		case '\\':
			buf += '\\';
			break;
		case 'a':
			buf += '\a';
			break;
		case 'b':
			if(!buf.empty())
				buf.pop_back();
			break;
		case 'f':
			buf += '\f';
			break;
		case 'n':
			buf += '\n';
			break;
		case 'r':
			buf += '\r';
			break;
		case 't':
			buf += '\t';
			break;
		case 'v':
			buf += '\v';
			break;
		case '\'':
		case '"':
			if(escs[0] == ld)
			{
				buf += ld;
				break;
			}
			YB_ATTR_fallthrough;
		default:
			return {};
		}
		return true;
	}
	return {};
}


LexicalAnalyzer::LexicalAnalyzer()
	: unescape_context(), ld(), cbuf(), qlist()
{}

bool
LexicalAnalyzer::CheckEscape(byte b, Unescaper unescape)
{
	if(!(b < 0x80))
	{
		// NOTE: Stop unescaping. The escaped sequence should have no
		//	multibyte characters.
		if(unescape_context.IsHandling())
			cbuf += unescape_context.Done();
		cbuf += char(b);
	}
	else if(unescape_context.IsHandling())
	{
		unescape_context.Push(b);
		if(unescape(cbuf, unescape_context, ld))
			unescape_context.Clear();
		else
			cbuf += unescape_context.Done();
	}
	else
		return {};
	return true;
}

bool
LexicalAnalyzer::CheckLineConcatnater(char c, char concat, char newline)
{
	if(line_concat == concat && c == newline)
	{
		if(!unescape_context.PopIf(concat))
		{
			auto& pfx(unescape_context.Prefix);

			if(!pfx.empty() && pfx.back() == concat)
				pfx.pop_back();
			else if(!cbuf.empty() && cbuf.back() == line_concat)
				cbuf.pop_back();
		}
		return true;
	}
	else if(c == concat)
		line_concat = concat;
	else
		line_concat = {};
	return {};
}

bool
LexicalAnalyzer::FilterForParse(char c, Unescaper unescape,
	PrefixHandler prefix_handler)
{
	return !(CheckLineConcatnater(c) || CheckEscape(byte(c), unescape)
		|| prefix_handler(c, unescape_context.Prefix));
}

void
LexicalAnalyzer::ParseByte(char c, Unescaper unescape,
	PrefixHandler prefix_handler)
{
	if(FilterForParse(c, unescape, prefix_handler))
		switch(c)
		{
			case '\'':
			case '"':
				if(ld == char())
				{
					ld = c;
					qlist.push_back(cbuf.size());
					cbuf += c;
				}
				else if(ld == c)
				{
					ld = char();
					cbuf += c;
					qlist.push_back(cbuf.size());
				}
				else
					cbuf += c;
			case char():
				break;
			case ' ':
			case '\f':
			case '\n':
		//	case '\r':
			case '\t':
			case '\v':
				if(ld == char())
				{
					cbuf += ' ';
					break;
				}
				YB_ATTR_fallthrough;
			default:
				cbuf += c;
		}
}

void
LexicalAnalyzer::ParseQuoted(char c, Unescaper unescape,
	PrefixHandler prefix_handler)
{
	if(FilterForParse(c, unescape, prefix_handler))
		cbuf += c;
}

list<string>
LexicalAnalyzer::Literalize() const
{
	size_t i(0);
	list<string> result;

	std::for_each(qlist.cbegin(), qlist.cend(), [&](size_t s){
		if(s != i)
		{
			result.push_back(cbuf.substr(i, s - i));
			i = s;
		}
	});
	result.push_back(cbuf.substr(i));
	return result;
}


char
CheckLiteral(string_view sv) ynothrowv
{
	YAssertNonnull(sv.data());
	if(sv.length() > 1)
		if(const char c = ystdex::get_quote_mark_nonstrict(sv))
		{
			if(c == '\'' || c == '"')
				return c;
		}
	return {};
}

string_view
Deliteralize(string_view sv) ynothrowv
{
	return CheckLiteral(sv) != char() ? DeliteralizeUnchecked(sv) : sv;
}

string
Escape(string_view sv)
{
	YAssertNonnull(sv.data());

	char last{};
	string res;

	res.reserve(sv.length());
	for(char c : sv)
	{
		char unescaped{};

		switch(c)
		{
		case '\a':
			unescaped = 'a';
			break;
		case '\b':
			unescaped = 'b';
			break;
		case '\f':
			unescaped = 'f';
			break;
		case '\n':
			unescaped = 'n';
			break;
		case '\r':
			unescaped = 'r';
			break;
		case '\t':
			unescaped = 't';
			break;
		case '\v':
			unescaped = 'v';
			break;
		case '\'':
		case '"':
			unescaped = c;
		}
		if(last == '\\')
		{
			if(c == '\\')
			{
				yunseq(last = char(), res += '\\');
				continue;
			}
			switch(c)
			{
			case 'a':
			case 'b':
			case 'f':
			case 'n':
			case 'r':
			case 't':
			case 'v':
			case '\'':
			case '"':
				res += '\\';
			}
		}
		if(unescaped == char())
			res += c;
		else
		{
			res += '\\';
			res += unescaped;
			unescaped = char();
		}
		last = c;
	}
	return res;
}

string
EscapeLiteral(string_view sv)
{
	const char c(CheckLiteral(sv));
	// TODO: Use %get_mid for %string_view.
	auto content(Escape(c == char() ? sv : ystdex::get_mid(string(sv))));

	if(!content.empty() && content.back() == '\\')
		content += '\\';
	return c == char() ? std::move(content) : ystdex::quote(content, c);
}

string
EscapeXML(string_view sv)
{
	YAssertNonnull(sv.data());

	string res;

	res.reserve(sv.length());
	for(char c : sv)
		switch(c)
		{
		case '\0':
			YTraceDe(YSLib::Warning, "Invalid character '#x%X' found, ignored.",
				unsigned(c));
			break;
		case '&':
			res += "&amp;";
			break;
		case '<':
			res += "&lt;";
			break;
		case '>':
			res += "&gt;";
			break;
		default:
			res += c;
		}
	return res;
}

string
Literalize(string_view sv, char c)
{
	return CheckLiteral(sv) == char() && c != char()
		? ystdex::quote(string(sv), c) : string(sv);
}


list<string>
Decompose(string_view src)
{
	YAssertNonnull(src.data());

	list<string> dst;
	using iter_type = typename string_view::const_iterator;

	ystdex::split_l(src.cbegin(), src.cend(), IsDelimeter,
		// TODO: Blocked. Use C++14 generic lambda expressions.
		[&](iter_type b, iter_type e){
		YAssert(e >= b, "Invalid split result found.");

		string_view sv(b, size_t(e - b));

		YAssert(!sv.empty(), "Null token found.");
		if(IsGraphicalDelimeter(*b))
		{
			dst.push_back({sv.front()});
			sv.remove_prefix(1);
		}
		ystdex::trim(sv);
		if(!sv.empty())
			dst.push_back(string(sv));
	});
	return dst;
}

list<string>
Tokenize(const list<string>& src)
{
	list<string> dst;

	for(const auto& str : src)
		if(!str.empty())
		{
			if(str[0] != '\'' && str[0] != '"')
				dst.splice(dst.end(), Decompose(str));
			else
				dst.push_back(str);
		}
	return dst;
}

} // namespace NPL;

