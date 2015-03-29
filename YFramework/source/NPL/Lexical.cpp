/*
	© 2012-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Lexical.cpp
\ingroup NPL
\brief NPL 词法处理。
\version r1507
\author FrankHB <frankhb1989@gmail.com>
\since build 335
\par 创建时间:
	2012-08-03 23:04:26 +0800
\par 修改时间:
	2015-03-25 18:24 +0800
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
	{
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
			default:
				cbuf += c;
		}
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

	std::for_each(qlist.cbegin(), qlist.cend(), [&](const size_t& s){
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
CheckLiteral(const string& str)
{
	if(str.size() < 2)
		return char();
	if(str.front() == '\'' && str.back() == '\'')
		return '\'';
	if(str.front() == '"' && str.back() == '"')
		return '"';
	return char();
}

string
Deliteralize(const string& str)
{
	return CheckLiteral(str) == char() ? str : ystdex::get_mid(str);
}

string
Escape(const string& str)
{
	char last{};
	string res;

	res.reserve(str.length());
	for(char c : str)
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
EscapeLiteral(const string& str)
{
	const char c(CheckLiteral(str));
	auto content(Escape(c == char() ? str : ystdex::get_mid(str)));

	if(!content.empty() && content.back() == '\\')
		content += '\\';
	return c == char() ? std::move(content) : c + content + c;
}

list<string>
Decompose(const string& src_str)
{
	list<string> dst;

	ystdex::split_l(src_str, IsDelimeter,
		[&](string::const_iterator b, string::const_iterator e){
		string str(b, e);

		YAssert(!str.empty(), "Null token found.");
		if(IsGraphicalDelimeter(*b))
		{
			dst.push_back(str.substr(0, 1));
			str.erase(0, 1);
		}
		// TODO: Optimize using %std::experimental::string_view.
		ystdex::trim(str);
		if(!str.empty())
			dst.push_back(std::move(str));
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

