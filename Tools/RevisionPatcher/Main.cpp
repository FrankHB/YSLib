/*
	© 2015-2017 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Main.cpp
\ingroup MaintenanceTools
\brief 版本补丁工具。
\version r227
\author FrankHB <frankhb1989@gmail.com>
\since build 565
\par 创建时间:
	2015-01-11 14:20:05 +0800
\par 修改时间:
	2017-05-31 02:34 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Tools::RevisionPatcher::Main

This is a command line tool to patch revision numbers in .patch files.
See readme file for details.
*/


#include <YSBuild.h>
#include YFM_YCLib_FileSystem
#include <iostream>

using namespace YSLib;

namespace
{

using Entry = vector<string>;
using EntryMap = map<size_t, Entry>;
using PatchMap = map<string, pair<size_t, EntryMap>>;
using namespace ystdex;

PatchMap
Analyze(std::istream& in)
{
	const auto rtrcrlf([](string_view sv, size_t idx){
		YAssert(sv.size() >= idx, "Invalid string found.");
		return string(rtrim(sv.substr(idx), "\r\n"));
	});
	const auto& vpfx("version r");
	PatchMap res;
	string line, name_a, name_b;
	auto at_blk(size_t(-1));
	auto at_a(size_t(-1));

	for(size_t n(0); extract_line_cr(in, line); ++n)
	{
		if(line.empty())
			continue;
		if(begins_with(line, "--- a/"))
			yunseq(at_a = n, name_a = cond_prefix(rtrcrlf(line, 3), " a/"));
		else if(n == at_a + 1 && begins_with(line, "+++ b/"))
		{
			name_b = cond_prefix(rtrcrlf(line, 3), " b/");
			yunseq(res[name_b].first = name_a.empty() ? size_t(-1) : 0,
				at_blk = size_t(-1));
		}
		else if(begins_with(line, "@@"))
		{
			line = cond_prefix(rtrcrlf(line, 2), " -");
			at_blk = !line.empty() ? std::stoul(line) : size_t(-1);
		}
		else if(!name_b.empty() && at_blk != size_t(-1))
		{
			auto& entry(res[name_b].second[at_blk]);
			auto cont(line.substr(1));

			switch(line[0])
			{
			case ' ':
				if(!entry.empty() && entry.back()[0] == ' ')
					break;
			case '-':
				if(line.length() > 2 && res[name_b].first == 0)
				{
					const auto vstr(rtrcrlf(line, 2));

					if(begins_with(vstr, vpfx))
						TryExpr(res[name_b].first
							= std::stoul(vstr.substr(string_length(vpfx))))
						CatchIgnore(std::invalid_argument&)
						CatchIgnore(std::out_of_range&)
				}
			case '+':
				entry.emplace_back(std::move(line));
				break;
			default:
				at_blk = size_t(-1);
			}
		}
	}
	return res;
}

size_t
CalcLines(EntryMap& m, size_t base, size_t limit = 2)
{
	size_t n(0), n_empty(0);
	map<string, string*> added;

	for(auto& item : m)
	{
		auto& e(item.second);

		for(size_t i(0); i != e.size(); ++i)
			if(e[i][0] == '+')
				added.emplace('-' + e[i].substr(1), &e[i]);
	}
	for(auto& item : m)
		for(auto& s : item.second)
			if(s[0] == '-')
			{
				const auto i(added.find(s));

				if(i != added.cend())
				{
					++(trim(s.substr(1)).empty() ? n_empty : n);
					Deref(i->second).clear();
					added.erase(i),
					s.clear();
				}
			}
	for(auto& item : m)
		erase_all_if(item.second, [](const string& str){
			return str.empty();
		});
	tie(n, n_empty) = std::accumulate(m.begin(), m.end(), make_pair(n, n_empty),
		[&](pair<size_t, size_t> val, decltype(*m.begin()) pr){
		auto& entry(pr.second);
		size_t res(0), res_empty(0);

		while(true)
		{
			const auto i(std::adjacent_find(entry.begin(), entry.end(),
				[](const string& x, const string& y){
					YAssert(!(x.empty() || y.empty()), "Invalid string found");
					return (x.front() == '+' && y.front() == '-')
						|| (x.front() == '-' && y.front() == '+');
				}));

			if(i == entry.cend())
				break;

			const bool empty_a(trim(i->substr(1)).empty()),
				empty_b(trim((i + 1)->substr(1)).empty());

			if(!empty_a && !empty_b)
			{
				++res;
				entry.erase(i, i + 2);
			}
			else
			{
				++res_empty;
				if(empty_b)
					entry.erase(i + 1);
				if(empty_a)
					entry.erase(i);
			}
		}
		for(const auto& str : entry)
		{
			YAssert(!str.empty(), "Invalid state found.");
			if(str[0] == '+' || str[0] == '-')
				++(trim(str.substr(1)).empty() ? res_empty : res);
		}
		return make_pair(val.first + res, val.second + res_empty);
	});
	if(base == size_t(-1))
		return n + n_empty - 1;
	if(base == 0)
		return n == 0 ? n_empty : n;
	return (n > limit ? n - limit : n_empty) + base;
}

} // unnamed namespace;


int
main()
{
	return FilterExceptions([]{
		using namespace std;

		platform::SetupBinaryStdIO();
		for(auto& m : Analyze(cin))
			cout << m.first << endl
				<< CalcLines(m.second.second, m.second.first) << endl;
	}, yfsig) ? EXIT_FAILURE : EXIT_SUCCESS;
}

