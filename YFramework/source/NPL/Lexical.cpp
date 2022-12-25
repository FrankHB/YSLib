﻿/*
	© 2012-2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Lexical.cpp
\ingroup NPL
\brief NPL 词法处理。
\version r2192
\author FrankHB <frankhb1989@gmail.com>
\since build 335
\par 创建时间:
	2012-08-03 23:04:26 +0800
\par 修改时间:
	2022-12-28 19:49 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::Lexical
*/


#include "NPL/YModules.h"
#include YFM_NPL_Lexical // for YSLib::octet;
#include <ystdex/string.hpp> // for ystdex::get_mid;

namespace NPL
{

bool
HandleBackslashPrefix(string_view buf, UnescapeContext& uctx)
{
	YAssert(!buf.empty(), "Invalid buffer found.");
	if(!uctx.IsHandling() && buf.back() == '\\')
	{
		yunseq(uctx.Start = buf.length() - 1, uctx.Length = 1);
		return true;
	}
	return {};
}

bool
NPLUnescape(string& buf, char c, UnescapeContext& uctx, char ld)
{
	if(uctx.Length == 1)
	{
		uctx.VerifyBufferLength(buf.length());

		const auto i(uctx.Start);

		YAssert(i == buf.length() - 1, "Invalid buffer found.");
		switch(c)
		{
		case '\\':
			buf[i] = '\\';
			break;
		case 'a':
			buf[i] = '\a';
			break;
		case 'b':
			buf[i] = '\b';
			break;
		case 'f':
			buf[i] = '\f';
			break;
		case 'n':
			buf[i] = '\n';
			break;
		case 'r':
			buf[i] = '\r';
			break;
		case 't':
			buf[i] = '\t';
			break;
		case 'v':
			buf[i] = '\v';
			break;
		case '\n':
			buf.pop_back();
			break;
		case '\'':
		case '"':
			if(c == ld)
			{
				buf[i] = ld;
				break;
			}
			YB_ATTR_fallthrough;
		default:
			uctx.Clear();
			buf += c;
			return {};
		}
		uctx.Clear();
		return true;
	}
	buf += c;
	return {};
}


bool
LexicalAnalyzer::UpdateBack(char& b, char c)
{
	switch(c)
	{
		case '\'':
		case '"':
			if(ld == char())
			{
				ld = c;
				return true;
			}
			else if(ld == c)
			{
				ld = char();
				return true;
			}
			break;
		// XXX: Case ' ' is equivalent in the default branch.
	//	case ' ':
		case '\f':
		case '\n':
		// XXX: Case '\r' is ignored.
	//	case '\r':
		case '\t':
		case '\v':
			if(ld == char())
			{
				b = ' ';
				break;
			}
			YB_ATTR_fallthrough;
		default:
			break;
	}
	return {};
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
	auto content(Escape(c == char() ? sv : ystdex::get_mid(sv)));

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

// XXX: The code generated by x86_64-pc-linux G++ 8.3 at '-O3' is wrong. This
//	seems not filed at https://gcc.gnu.org/bugzilla.
// XXX: There is no need to specifiy GCC version because the lowest version
//	supported in YFramework is more recent. See
//	https://gcc.gnu.org/onlinedocs/gcc-4.4.0/gcc/Function-Attributes.html#Function-Attributes.
#if (YB_IMPL_GNUCPP < 90100 && !YB_IMPL_CLANGPP) && YCL_Linux && \
	defined(NDEBUG) && __OPTIMIZE__ && !__OPTIMIZE_SIZE__
// XXX: As there is no way to easily get the optimization level, leave all
//	optimized builds as '-O2'. All '-O2' options plus all documented additional
//	options for '-O3' or options used in 'gcc/opts.c' in trunk code as of
//	writing (2019-05-16, also cf.
//	https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81191) does not make the bug
//	reappear, but as now just use '-O2' instead.
YB_ATTR(optimize("O2"))
#endif
string
Literalize(string_view sv, char c)
{
	return CheckLiteral(sv) == char() && c != char()
		? ystdex::quote(string(sv), c) : string(sv);
}


LexemeList
Decompose(string_view src, LexemeList::allocator_type a)
{
	YAssertNonnull(src.data());

	LexemeList dst(a);
	using iter_type = typename string_view::const_iterator;

	ystdex::split_l(src.cbegin(), src.cend(), IsDelimiter,
		// TODO: Blocked. Use C++14 generic lambda expressions.
		[&](iter_type b, iter_type e){
		YAssert(e >= b, "Invalid split result found.");

		string_view sv(&*b, size_t(e - b));

		YAssert(!sv.empty(), "Null token found.");
		if(IsGraphicalDelimiter(*b))
		{
			dst.push_back({sv.front()});
			sv.remove_prefix(1);
		}
		ystdex::trim(sv);
		if(!sv.empty())
			dst.push_back({sv.data(), sv.size()});
	});
	return dst;
}


//! \since build 891
namespace
{

template<typename _fAdd, typename _fAppend, class _tParseResult>
void
UpdateByteRaw(_fAdd add, _fAppend append, _tParseResult& res, bool got_delim,
	const LexicalAnalyzer& lexer, string& cbuf, bool& update_current)
{
	const auto len(cbuf.length());

	YAssert(!(res.empty() && update_current), "Invalid state found.");
	if(len > 0 && !lexer.GetUnescapeContext().IsHandling())
	{
		if(len == 1)
		{
			const auto update_c([&](char c){
				if(update_current)
					append(res, c);
				else
					add(res, string({c}, res.get_allocator()));
			});
			const char c(cbuf.back());
			const bool unquoted(lexer.GetDelimiter() == char());

			if(got_delim)
			{
				update_c(c);
				update_current = !unquoted;
			}
			else if(unquoted && IsDelimiter(c))
			{
				if(IsGraphicalDelimiter(c))
					add(res, string({c}, res.get_allocator()));
				update_current = {};
			}
			else
			{
				update_c(c);
				update_current = true;
			}
		}
		else if(update_current)
			append(res, std::move(cbuf));
		else
			add(res, std::move(cbuf));
		cbuf.clear();
	}
}

template<class _tParseResult>
struct SequenceAdd final
{
	template<typename _tParam>
	void
	operator()(_tParseResult& res, _tParam&& arg) const
	{
		res.push_back(yforward(arg));
	}
};

template<class _tParseResult>
struct SequenceAppend final
{
	template<typename _tParam>
	void
	operator()(_tParseResult& res, _tParam&& arg) const
	{
		res.back() += yforward(arg);
	}
};

template<class _tParseResult>
struct SourcedSequenceAdd final
{
	const SourcedByteParser& Parser;

	template<typename _tParam>
	void
	operator()(_tParseResult& res, _tParam&& arg) const
	{
		res.emplace_back(Parser.GetSourceLocation(), yforward(arg));
	}
};

template<class _tParseResult>
struct SourcedSequenceAppend final
{
	template<typename _tParam>
	void
	operator()(_tParseResult& res, _tParam&& arg) const
	{
		res.back().second += yforward(arg);
	}
};

} // unnamed namespace;


void
ByteParser::Update(bool got_delim)
{
	// TODO: Blocked. Use C++14 generic lambda expressions.
	UpdateByteRaw(SequenceAdd<ParseResult>(), SequenceAppend<ParseResult>(),
		lexemes, got_delim, GetLexerRef(), GetBufferRef(), update_current);
}


void
SourcedByteParser::Update(bool got_delim)
{
	// TODO: Blocked. Use C++14 generic lambda expressions.
	UpdateByteRaw(SourcedSequenceAdd<ParseResult>{*this},
		SourcedSequenceAppend<ParseResult>(), lexemes, got_delim, GetLexerRef(),
		GetBufferRef(), update_current);
}


LexemeList
DelimitedByteParser::GetResult() const
{
	const auto& cbuf(GetBuffer());
	const auto a(cbuf.get_allocator());
	LexemeList res(a);
	size_t i(0);

	std::for_each(qlist.cbegin(), qlist.cend(), [&](size_t s){
		if(s != i)
		{
			DecomposeString(res, string_view(&cbuf[i], s - i));
			i = s;
		}
	});
	if(cbuf.length() != i)
		DecomposeString(res, string_view(&cbuf[i], cbuf.length() - i));
	return res;
}

void
DelimitedByteParser::DecomposeString(LexemeList& lst, bool not_quoted,
	string_view sv)
{
	if(not_quoted)
		lst.splice(lst.end(), Decompose(sv, lst.get_allocator()));
	else
		lst.push_back(LexemeList::value_type(sv.begin(), sv.end()));
}

} // namespace NPL;

