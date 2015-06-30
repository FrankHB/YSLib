/*
	© 2014-2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file NPLA1.cpp
\ingroup NPL
\brief NPLA1 公共接口。
\version r552
\author FrankHB <frankhb1989@gmail.com>
\since build 472
\par 创建时间:
	2014-02-02 18:02:47 +0800
\par 修改时间:
	2015-06-30 17:35 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	NPL::NPLA1
*/


#include "NPL/YModules.h"
#include YFM_NPL_NPLA1
#include YFM_NPL_SContext
#include <ystdex/exception.h> // for ystdex::unimplemented;
#include <ystdex/functional.hpp> // for ystdex::bind1;

using namespace YSLib;

namespace NPL
{

ValueNode
MapNPLALeafNode(const ValueNode& node)
{
	return {0, string(),  Deliteralize(ParseNPLANodeString(node))};
}

ValueNode
TransformToSyntaxNode(const ValueNode& node, const string& name)
{
	ValueNode::Container con{{0, MakeIndex(0), node.GetName()}};
	const auto nested_call([&](const ValueNode& nd){
		con.emplace(TransformToSyntaxNode(nd, MakeIndex(con)));
	});

	if(node.empty())
		try
		{
			auto& seq(Access<NodeSequence>(node));

			for(auto& nd : seq)
				nested_call(nd);
		}
		CatchExpr(ystdex::bad_any_cast&,
			con.emplace(0, MakeIndex(1), Literalize(ParseNPLANodeString(node))))
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
	TryRet(Access<string>(node))
	CatchIgnore(ystdex::bad_any_cast&)
	return {};
}


void
InsertNPLA1Child(ValueNode&& n, ValueNode::Container& con)
{
	con.insert(n.GetName().empty() ? ValueNode(0, '$' + MakeIndex(con),
		std::move(n.Value)) : std::move(n));
}

void
InsertNPLA1SequenceChild(ValueNode&& n, NodeSequence& con)
{
	con.emplace_back(std::move(n));
}

ValueNode
TransformNPLA1(const ValueNode& node, NodeMapper mapper,
	NodeMapper map_leaf_node, NodeToString node_to_str,
	NodeInserter insert_child)
{
	auto s(node.size());

	if(s == 0)
		return map_leaf_node(node);

	auto i(node.begin());
	auto nested_call(ystdex::bind1(TransformNPLA1, mapper, map_leaf_node,
		node_to_str, insert_child));

	if(s == 1)
		return nested_call(*i);

	const auto& name(node_to_str(*i));

	if(!name.empty())
		yunseq(++i, --s);
	if(s == 1)
	{
		auto&& n(nested_call(*i));

		if(n.GetName().empty())
			return {0, name, std::move(n.Value)};
		return {ValueNode::Container{std::move(n)}, name};
	}

	ValueNode::Container node_con;

	std::for_each(i, node.end(), [&](const ValueNode& nd){
		insert_child(mapper ? mapper(nd) : nested_call(nd), node_con);
	});
	return {std::move(node_con), name};
}

ValueNode
TransformNPLA1Sequence(const ValueNode& node, NodeMapper mapper, NodeMapper
	map_leaf_node, NodeToString node_to_str, NodeSequenceInserter insert_child)
{
	auto s(node.size());

	if(s == 0)
		return map_leaf_node(node);

	auto i(node.begin());
	auto nested_call(ystdex::bind1(TransformNPLA1Sequence, mapper,
		map_leaf_node, node_to_str, insert_child));

	if(s == 1)
		return nested_call(*i);

	const auto& name(node_to_str(*i));

	if(!name.empty())
		yunseq(++i, --s);
	if(s == 1)
	{
		auto&& n(nested_call(*i));

		if(n.GetName().empty())
			return {0, name, std::move(n.Value)};
		return {0, name, NodeSequence{std::move(n)}};
	}

	NodeSequence node_con;

	std::for_each(i, node.end(), [&](const ValueNode& nd){
		insert_child(mapper ? mapper(nd) : nested_call(nd), node_con);
	});
	return {0, name, std::move(node_con)};
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
	os << EscapeLiteral(node.GetName());
	if(node)
	{
		os << ' ';
		if(PrintNodeString(os, node, node_to_str))
			return;
		os << '\n';

		const auto nested_call(std::bind(PrintNodeChild, std::ref(os),
			std::placeholders::_1, node_to_str, igen, depth));

		try
		{
			auto& seq(Access<NodeSequence>(node));

			for(const auto& nd : seq)
				nested_call(nd);
			return;
		}
		CatchIgnore(ystdex::bad_any_cast&)
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
ConvertAttributeNodeString(const ValueNode& node)
{
	switch(node.size())
	{
	default:
		YTraceDe(Warning, "Invalid node with more than 2 children found.");
	case 2:
		{
			auto i(node.begin());
			const auto& n1(Deref(i).Value.Access<string>());
			const auto& n2(Deref(++i).Value.Access<string>());

			return n1 + '=' + n2;
		}
	case 1:
		return Deref(node.begin()).Value.Access<string>();
	case 0:
		break;
	}
	throw LoggedEvent("Invalid node with less than 1 children found.", Warning);
}

string
ConvertDocumentNode(const ValueNode& node, IndentGenerator igen, size_t depth,
	ParseOption opt)
{
	if(node)
	{
		string res(ConvertStringNode(node));

		if(res.empty())
		{
			if(opt == ParseOption::String)
				throw LoggedEvent("Invalid non-string node found.");

			const auto& cont(node.GetContainerRef());

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
						throw LoggedEvent("Invalid non-attribute node found.");
					if(str == "*PI*")
					{
						res = "<?";
						for(; i != cont.cend(); ++i)
							res += Deliteralize(ConvertDocumentNode(Deref(i),
								igen, depth, ParseOption::String)) + ' ';
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
							try
							{
								const auto& t(Access<string>(at(Deref(i), 0)));
								if(t == "@")
								{
									head += Deliteralize(ConvertDocumentNode(*i,
										igen, depth, ParseOption::Attribute));
									++i;
								}
							}
							CatchIgnore(std::out_of_range&)
							CatchIgnore(ystdex::bad_any_cast&)
							if(i == cont.cend())
								return head + " />";
							head += '>';
						}
						else
							return head + " />";
						for(; i != cont.cend(); ++i)
						{
							nl = Deref(i).Value.GetType() != typeid(string);
							if(nl)
								res += '\n' + igen(depth + size_t(is_content));
							else
								res += ' ';
							res += Deliteralize(ConvertDocumentNode(*i,
								igen, depth + size_t(is_content)));
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
	throw LoggedEvent("Empty node found.", Warning);
}

string
ConvertStringNode(const ValueNode& node)
{
	TryRet(EscapeXML(Access<string>(node)))
	CatchIgnore(ystdex::bad_any_cast&)
	return {};
}

void
PrintSyntaxNode(std::ostream& os, const ValueNode& node, IndentGenerator igen,
	size_t depth)
{
	if(!node.empty())
		ystdex::write(os,
			ConvertDocumentNode(Deref(node.begin()), igen, depth), 1);
	os << std::flush;
}


ValueNode
MakeXMLDecl(const string& name, const string& ver, const string& enc,
	const string& sd)
{
	string decl("version=\"" + ver + '"');

	if(!enc.empty())
		decl += " encoding=\"" + enc + '"';
	if(!sd.empty())
		decl += " standalone=\"" + sd + '"';
	return NodeLiteral{0, name, {{MakeIndex(0), "*PI*"}, {MakeIndex(1), "xml"},
		{MakeIndex(2), decl + ' '}}};
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

} // namespace NPL;

