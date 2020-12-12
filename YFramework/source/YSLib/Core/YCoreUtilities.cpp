/*
	© 2014-2015, 2017-2018, 2020 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file YCoreUtilities.cpp
\ingroup Core
\brief 核心实用模块。
\version r167
\author FrankHB <frankhb1989@gmail.com>
\since build 539
\par 创建时间:
	2014-10-01 08:52:17 +0800
\par 修改时间:
	2020-12-12 10:09 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YSLib::Core::YCoreUtilities
*/


#include "YSLib/Core/YModules.h"
#include YFM_YSLib_Core_YCoreUtilities // for std::exception, ExtractException,
//	FatalError, upopen, upclose, ExtractAndTrace;
#include <random> // for std::random_device, std::mt19937,
//	std::uniform_int_distribution;
#include <cstdlib> // for std::getenv;
#include <ystdex/cstdio.h> // for ystdex::read_all_with_buffer;

namespace YSLib
{

void
PerformKeyAction(function<void()> f, const char* sig,
	const char* t, string_view sv)
{
	string res;

	try
	{
		f();
		return;
	}
	CatchExpr(std::exception& e, ExtractException(
		[&](const string& str, size_t level){
		res += string(level, ' ') + "ERROR: " + str + '\n';
	}, e))
	CatchExpr(..., res += string("Unknown exception @ ") + sig + ".\n")
	throw FatalError(t, string(sv) + res);
}


string
RandomizeTemplatedString(string str, char mask, string_view tmpl)
{
	YAssert(tmpl.data() && !tmpl.empty(),
		"The template string argument shall specify valid nonempty string.");

	const auto m(tmpl.size() - 1);
	const auto randomize([&]{
		// TODO: Use common instance of the random objects?
		static std::random_device rd;
		static std::mt19937 mt(rd());

		return tmpl[std::uniform_int_distribution<size_t>(0, m)(mt)];
	});

	for(auto& c : str)
		if(c == mask)
			c = randomize();
	return str;
}


void
ArgumentsVector::Reset(int argc, char* argv[])
{
	Arguments.clear();
	Arguments.reserve(CheckNonnegative<size_t>(argc, "argument number"));
	for(size_t i(0); i < size_t(argc); ++i)
		Arguments.push_back(Nonnull(argv[i]));
}

locked_ptr<ArgumentsVector, recursive_mutex>
LockCommandArguments()
{
	static ArgumentsVector vec;
	static recursive_mutex mtx;

	return {&vec, mtx};
}


pair<string, int>
FetchCommandOutput(const char* cmd, size_t buf_size)
{
	if(buf_size != 0)
	{
		string str;
		int exit_code(0);
		ystdex::temporary_buffer<char> buf(buf_size);

		if(const auto fp = ystdex::unique_raw(upopen(cmd, "r"),
			[&](std::FILE* p){
			exit_code = upclose(p);
		}))
			ystdex::read_all_with_buffer(fp.get(), buf.get().get(), buf.size(),
				[&](const char* s, size_t n){
				str.append(s, n);
			});
		else
			ystdex::throw_error(errno, yfsig);
		return {std::move(str), exit_code};
	}
	throw std::invalid_argument("Zero buffer size found.");
}


locked_ptr<CommandCache>
LockCommandCache()
{
	static CommandCache cache;
	static mutex mtx;

	return {&cache, mtx};
}

const string&
FetchCachedCommandResult(string_view cmd, size_t buf_size)
{
	YAssertNonnull(cmd.data());

	auto p_locked(LockCommandCache());
	auto& cache(Deref(p_locked));

	try
	{
		// TODO: Blocked. Use %string_view as argument using C++14 heterogeneous
		//	%find template.
		const auto i_entry(cache.find(string(cmd)));

		return (i_entry != cache.cend() ? i_entry : (cache.emplace(cmd,
			!cmd.empty() ? FetchCommandOutput(cmd.data(), buf_size).first
			: string())).first)->second;
	}
	CatchExpr(std::system_error& e,
		YTraceDe(Err, "Failed execution of command."), ExtractAndTrace(e, Err))
	return cache[string()];
}

} // namespace YSLib;

