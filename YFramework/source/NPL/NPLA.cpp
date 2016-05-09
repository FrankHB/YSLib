/*
	© 2014-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA.cpp
\ingroup NPL
\brief NPLA 公共接口。
\version r745
\author FrankHB <frankhb1989@gmail.com>
\since build 663
\par 创建时间:
	2016-01-07 10:32:45 +0800
\par 修改时间:
	2016-05-09 14:20 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA
#include YFM_NPL_SContext

using namespace YSLib;

namespace NPL
{

ValueNode
MapNPLALeafNode(const TermNode& term)
{
	return AsNode(string(),
		string(Deliteralize(ParseNPLANodeString(MapToValueNode(term)))));
}

ValueNode
TransformToSyntaxNode(const ValueNode& node, const string& name)
{
	ValueNode::Container con{AsIndexNode(size_t(), node.GetName())};
	const auto nested_call([&](const ValueNode& nd){
		con.emplace(TransformToSyntaxNode(nd, MakeIndex(con)));
	});

	if(node.empty())
	{
		if(const auto p = AccessPtr<NodeSequence>(node))
			for(auto& nd : *p)
				nested_call(nd);
		else
			con.emplace(NoContainer, MakeIndex(1),
				Literalize(ParseNPLANodeString(node)));
	}
	else
		for(auto& nd : node)
			nested_call(nd);
	return {std::move(con), name};
}

string
EscapeNodeLiteral(const ValueNode& node)
{
	return EscapeLiteral(Access<string>(node));
}

string
LiteralizeEscapeNodeLiteral(const ValueNode& node)
{
	return Literalize(EscapeNodeLiteral(node));
}

string
ParseNPLANodeString(const ValueNode& node)
{
	return ystdex::value_or<string>(AccessPtr<string>(node));
}


string
DefaultGenerateIndent(size_t n)
{
	return string(n, '\t');
}

void
PrintIndent(std::ostream& os, IndentGenerator igen, size_t n)
{
	if(YB_LIKELY(n != 0))
		ystdex::write(os, igen(n));
}

void
PrintNode(std::ostream& os, const ValueNode& node, NodeToString node_to_str,
	IndentGenerator igen, size_t depth)
{
	PrintIndent(os, igen, depth);
	os << EscapeLiteral(node.GetName()) << ' ';
	if(PrintNodeString(os, node, node_to_str))
		return;
	os << '\n';
	if(node)
	{
		const auto nested_call(std::bind(PrintNodeChild, std::ref(os),
			std::placeholders::_1, node_to_str, igen, depth));

		// TODO: Null coalescing or variant value?
		if(const auto p = AccessPtr<NodeSequence>(node))
			for(const auto& nd : *p)
				nested_call(nd);
		else
			for(const auto& nd : node)
				nested_call(nd);
	}
}

void
PrintNodeChild(std::ostream& os, const ValueNode& node,
	NodeToString node_to_str, IndentGenerator igen, size_t depth)
{
	PrintIndent(os, igen, depth);
	if(IsPrefixedIndex(node.GetName()))
		PrintNodeString(os, node, node_to_str);
	else
	{
		os << '(' << '\n';
		TryExpr(PrintNode(os, node, node_to_str, igen, depth + 1))
		CatchIgnore(std::out_of_range&)
		PrintIndent(os, igen, depth);
		os << ')' << '\n';
	}
}

bool
PrintNodeString(std::ostream& os, const ValueNode& node,
	NodeToString node_to_str)
{
	TryRet(os << node_to_str(node) << '\n', true)
	CatchIgnore(ystdex::bad_any_cast&)
	return {};
}


namespace SXML
{

string
ConvertAttributeNodeString(const TermNode& term)
{
	switch(term.size())
	{
	default:
		YTraceDe(Warning, "Invalid term with more than 2 children found.");
	case 2:
		{
			auto i(term.begin());
			const auto& n(Access<string>(Deref(i)));

			return n + '=' + Access<string>(Deref(++i));
		}
	case 1:
		return Access<string>(Deref(term.begin()));
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

			const auto& cont(term.GetContainer());

			if(!cont.empty())
				try
				{
					auto i(cont.cbegin());
					const auto& str(Access<string>(Deref(i)));

					++i;
					if(str == "@")
					{
						for(; i != cont.cend(); ++i)
							res += ' ' + ConvertAttributeNodeString(Deref(i));
						return res;
					}
					if(opt == ParseOption::Attribute)
						throw LoggedEvent("Invalid non-attribute term found.");
					if(str == "*PI*")
					{
						res = "<?";
						for(; i != cont.cend(); ++i)
							res += string(Deliteralize(ConvertDocumentNode(
								Deref(i), igen, depth, ParseOption::String)))
								+ ' ';
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
						if(i != cont.end())
						{
							if(!Deref(i).empty()
								&& (i->begin())->Value == string("@"))
							{
								head += string(
									Deliteralize(ConvertDocumentNode(*i, igen,
									depth, ParseOption::Attribute)));
								if(++i == cont.cend())
									return head + " />";
							}
							head += '>';
						}
						else
							return head + " />";
						for(; i != cont.cend(); ++i)
						{
							nl = Deref(i).Value.GetType()
								!= ystdex::type_id<string>();
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
				CatchExpr(ystdex::bad_any_cast& e, YTraceDe(Warning,
					"Conversion failed: <%s> to <%s>.", e.from(), e.to()))
		}
		return res;
	}
	throw LoggedEvent("Empty term found.", Warning);
}

string
ConvertStringNode(const TermNode& term)
{
	return ystdex::call_value_or<string>(EscapeXML, AccessPtr<string>(term));
}

void
PrintSyntaxNode(std::ostream& os, const TermNode& term, IndentGenerator igen,
	size_t depth)
{
	if(!term.empty())
		ystdex::write(os,
			ConvertDocumentNode(Deref(term.begin()), igen, depth), 1);
	os << std::flush;
}


ValueNode
MakeXMLDecl(const string& name, const string& ver, const string& enc,
	const string& sd)
{
	auto decl("version=\"" + ver + '"');

	if(!enc.empty())
		decl += " encoding=\"" + enc + '"';
	if(!sd.empty())
		decl += " standalone=\"" + sd + '"';
	return AsNodeLiteral(name, {{MakeIndex(0), "*PI*"}, {MakeIndex(1), "xml"},
		{MakeIndex(2), decl + ' '}});
}

ValueNode
MakeXMLDoc(const string& name, const string& ver, const string& enc,
	const string& sd)
{
	auto doc(MakeTop(name));

	doc.Add(MakeXMLDecl(MakeIndex(1), ver, enc, sd));
	return doc;
}

} // namespace SXML;


ImplDeDtor(NPLException)


ImplDeDtor(InvalidSyntax)


ImplDeDtor(UndeclaredIdentifier)


ArityMismatch::ArityMismatch(size_t e, size_t r)
	: NPLException(ystdex::sfmt("Arity mismatch: expected %zu, received %zu.",
	e, r)),
	expected(e), received(r)
{}
ImplDeDtor(ArityMismatch)

void
ThrowArityMismatch(size_t expected, size_t received)
{
	throw ArityMismatch(expected, received);
}

} // namespace NPL;

