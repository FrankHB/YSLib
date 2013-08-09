/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Lexical.cpp
\ingroup NPL
\brief NPL 词法处理。
\version r1289
\author FrankHB <frankhb1989@gmail.com>
\since build 335
\par 创建时间:
	2012-08-03 23:04:26 +0800
\par 修改时间:
	2013-08-05 21:21 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Lexical
*/


#include "NPL/Lexical.h"
#include <ystdex/string.hpp> // for ystdex::get_mid;

namespace NPL
{

LexicalAnalyzer::LexicalAnalyzer()
	: esc(-1), ld(), cbuf(), qlist()
{}

void
LexicalAnalyzer::HandleEscape()
{
	YAssert(esc <= MaxEscapeLength, "Escape sequence is too long.");

	if(esc == 1)
	{
		switch(*escs)
		{
		case '\\':
			cbuf += '\\';
			break;
		case 'a':
			cbuf += '\a';
			break;
		case 'b':
			if(!cbuf.empty())
				cbuf.pop_back();
			break;
		case 'f':
			cbuf += '\f';
			break;
		case 'n':
			cbuf += '\n';
			break;
		case 'r':
			cbuf += '\r';
			break;
		case 't':
			cbuf += '\t';
			break;
		case 'v':
			cbuf += '\v';
			break;
		case '\'':
		case '"':
			if(*escs == ld)
			{
				cbuf += ld;
				break;
			}
		default:
			PushEscape();
			return;
		}
		esc = -1;
	}
	else
		PushEscape();
}

void
LexicalAnalyzer::PushEscape()
{
//	cbuf += '^'; // test: 未转义。
	yunseq(cbuf += '\\', escs[esc] = char());
	yunseq(cbuf += escs, esc = -1);
}

void
LexicalAnalyzer::ParseByte(byte b)
{
	if(!(b < 0x80))
	{
		//停止转义（转义序列不接受多字节字符）。
		if(esc != size_t(-1))
			PushEscape();
		cbuf += char(b);
	}
	else if(esc != size_t(-1))
	{
		escs[esc++] = b;
		HandleEscape();
	}
	else if(b == '\\' && ld != char())
		esc = 0;
	else
	{
		switch(b)
		{
			case '\'':
			case '"':
				if(ld == char())
				{
				//	cbuf += '{'; // test;
					ld = b;
					qlist.push_back(cbuf.size());
					cbuf += char(b);
				}
				else if(ld == b)
				{
				//	cbuf += '}'; // test;
					ld = char();
					cbuf += char(b);
					qlist.push_back(cbuf.size());
				}
				else
					cbuf += char(b);
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
				cbuf += char(b);
		}
	}
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
MakeEscape(const string& str)
{
	string res;

	for(char c : str)
		switch(c)
		{
		case '\a':
			res += "\\a";
			break;
		case '\b':
			res += "\\b";
			break;
		case '\f':
			res += "\\f";
			break;
		case '\n':
			res += "\\n";
			break;
		case '\r':
			res += "\\r";
			break;
		case '\t':
			res += "\\t";
			break;
		case '\v':
			res += "\\v";
			break;
		default:
			res += c;
		}
	if(!str.empty() && str.back() == '\\')
		res += "\\";
	return std::move(res);
}


list<string>
Decompose(const string& src_str)
{
	list<string> dst;

	ystdex::split_l(src_str.cbegin(), src_str.cend(), IsDelimeter, [&](
		string::const_iterator b, string::const_iterator e){
		string str(b, e);

		YAssert(!str.empty(), "Null token found.");

		if(IsGraphicalDelimeter(*b))
		{
			dst.push_back(str.substr(0, 1));
			str.erase(0, 1);
		}
		// TODO: Optimize using %string_ref.
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

