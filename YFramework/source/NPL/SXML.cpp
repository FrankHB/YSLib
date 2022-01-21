/*
	© 2015-2017, 2019-2020, 2022 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file SXML.cpp
\ingroup NPL
\brief NPL SXML 实现。
\version r4729
\author FrankHB <frankhb1989@gmail.com>
\since build 936
\par 创建时间:
	2022-01-20 17:41:15 +0800
\par 修改时间:
	2022-01-21 02:03 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::SXML
*/


#include "NPL/YModules.h"
#include YFM_NPL_SXML // for YSLib::Warning, NPL::Access, string, NPL::Deref,
//	ystdex::write, AccessFirstSubterm, ystdex::unimplemented, IsTyped,
//	bad_any_cast, ystdex::quote, ystdex::call_value_or;

//! \since build 903
using YSLib::Warning;

namespace NPL
{

namespace SXML
{

string
ConvertAttributeNodeString(const TermNode& term)
{
	switch(term.size())
	{
	default:
		YTraceDe(Warning, "Invalid term with more than 2 children found.");
		YB_ATTR_fallthrough;
	case 2:
		{
			auto i(term.begin());
			const auto& n(NPL::Access<string>(NPL::Deref(i)));

			return n + '=' + NPL::Access<string>(NPL::Deref(++i));
		}
		YB_ATTR_fallthrough;
	case 1:
		return NPL::Access<string>(AccessFirstSubterm(term));
	case 0:
		break;
	}
	throw LoggedEvent("Invalid term with less than 1 children found.", Warning);
}

string
ConvertDocumentNode(const TermNode& term, IndentGenerator igen, size_t depth,
	ParseOption opt)
{
	if(term)
	{
		string res(ConvertStringNode(term));

		if(res.empty())
		{
			if(opt == ParseOption::String)
				throw LoggedEvent("Invalid non-string term found.");
			if(IsBranch(term))
				try
				{
					auto i(term.begin());
					const auto& str(NPL::Access<string>(NPL::Deref(i)));

					++i;
					if(str == "@")
					{
						for(; i != term.end(); ++i)
							res += ' '
								+ ConvertAttributeNodeString(NPL::Deref(i));
						return res;
					}
					if(opt == ParseOption::Attribute)
						throw LoggedEvent("Invalid non-attribute term found.");
					if(str == "*PI*")
					{
						res = "<?";
						for(; i != term.end(); ++i)
							res += string(Deliteralize(
								ConvertDocumentNode(NPL::Deref(i), igen, depth,
								ParseOption::String))) + ' ';
						if(!res.empty())
							res.pop_back();
						return res + "?>";
					}
					if(str == "*ENTITY*" || str == "*NAMESPACES*")
					{
						if(opt == ParseOption::Strict)
							throw ystdex::unimplemented();
					}
					else if(str == "*COMMENT*")
						;
					else if(!str.empty())
					{
						const bool is_content(str != "*TOP*");
						string head('<' + str);
						bool nl{};

						if(YB_UNLIKELY(!is_content && depth > 0))
							YTraceDe(Warning, "Invalid *TOP* found.");
						if(i != term.end())
						{
							if(!NPL::Deref(i).empty()
								&& (i->begin())->Value == string("@"))
							{
								head += string(
									Deliteralize(ConvertDocumentNode(*i, igen,
									depth, ParseOption::Attribute)));
								if(++i == term.end())
									return head + " />";
							}
							head += '>';
						}
						else
							return head + " />";
						for(; i != term.end(); ++i)
						{
							nl = !IsTyped<string>(NPL::Deref(i));
							if(nl)
								res += '\n' + igen(depth + size_t(is_content));
							else
								res += ' ';
							res += string(Deliteralize(ConvertDocumentNode(*i,
								igen, depth + size_t(is_content))));
						}
						if(res.size() > 1 && res.front() == ' ')
							res.erase(0, 1);
						if(!res.empty() && res.back() == ' ')
							res.pop_back();
						if(is_content)
						{
							if(nl)
								res += '\n' + igen(depth);
							return head + res + ystdex::quote(str, "</", '>');
						}
					}
					else
						throw LoggedEvent("Empty item found.", Warning);
				}
				CatchExpr(bad_any_cast& e, YTraceDe(Warning,
					"Conversion failed: <%s> to <%s>.", e.from(), e.to()))
		}
		return res;
	}
	throw LoggedEvent("Empty term found.", Warning);
}

string
ConvertStringNode(const TermNode& term)
{
	return ystdex::call_value_or(EscapeXML, NPL::AccessPtr<string>(term));
}

void
PrintSyntaxNode(std::ostream& os, const TermNode& term, IndentGenerator igen,
	size_t depth)
{
	if(IsBranch(term))
		ystdex::write(os,
			ConvertDocumentNode(AccessFirstSubterm(term), igen, depth), 1);
	os << std::flush;
}


TermNode
MakeXMLDecl(const string& name, const string& ver, const string& enc,
	const string& sd)
{
	auto decl("version=\"" + ver + '"');

	if(!enc.empty())
		decl += " encoding=\"" + enc + '"';
	if(!sd.empty())
		decl += " standalone=\"" + sd + '"';
	return SXML::NodeLiteralToTerm(name, {{MakeIndex(0), "*PI*"},
		{MakeIndex(1), "xml"}, {MakeIndex(2), decl + ' '}});
}

TermNode
MakeXMLDoc(const string& name, const string& ver, const string& enc,
	const string& sd)
{
	auto doc(MakeTop(name));

	doc.Add(MakeXMLDecl(MakeIndex(1), ver, enc, sd));
	return doc;
}

} // namespace SXML;

} // namespace NPL;

