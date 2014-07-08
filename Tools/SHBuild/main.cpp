/*
	© 2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file main.cpp
\ingroup MaintenanceTools
\brief 递归查找源文件并编译和静态链接。
\version r633
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2014-02-06 14:33:55 +0800
\par 修改时间:
	2014-07-05 04:14 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Tools::SHBuild::Main

This is a command line tool to build project files simply.
See readme file for details.
*/


#include <ysbuild.h>
#include YFM_YSLib_Service_FileSystem
#include <iostream>
#include <iomanip> // for std::setw;
#include <sstream>
#include <Windows.h>
#include <ystdex/mixin.hpp>

using std::cerr;
using std::cout;
using std::endl;
using std::for_each;
using std::wstring;
using namespace YSLib;
using namespace IO;
using platform_ex::WCSToMBCS;
//! \since build 476
using platform_ex::MBCSToMBCS;

namespace
{

static_assert(sizeof(wchar_t) == sizeof(ucs2_t), "Character type unsupported.");
static_assert(yalignof(wchar_t) == yalignof(ucs2_t),
	"Character type unsupported.");

/*!
\brief 默认构建根目录路径。
\note 末尾的分隔符需要必须存在。
*/
yconstexpr auto build_path(u8".shbuild\\");


int
Call(const wchar_t* cmd)
{
	YAssert(cmd, "Null pointer found.");

	cout << WCSToMBCS(cmd) << endl;
	return ::_wsystem(cmd); 
}
int
Call(const wstring& cmd)
{
	return Call(cmd.c_str()); 
}
int
Call(const char* cmd)
{
	return Call(platform_ex::MBCSToWCS(cmd)); 
}
int
Call(const string& cmd)
{
	return Call(cmd.c_str()); 
}

//! \since build 477
//@{
using IntException = ystdex::wrap_mixin_t<std::exception, int>;

template<class _type>
YB_ATTR(noreturn) inline void
Raise(const _type& e)
{
	if(std::uncaught_exception())
		std::throw_with_nested(e);
	throw e;
}
YB_ATTR(noreturn) inline PDefH(void, Raise, int ret)
	ImplExpr(Raise<IntException>({std::exception(), ret}));

void
Info(const string& line)
{
	cout << MBCSToMBCS(line) << endl;
}

template<typename _fCallable>
void
Traverse(const string& path, _fCallable f)
{
	HDirectory dir(path.c_str());

	for_each(FileIterator(&dir), FileIterator(),
		std::bind(f, std::ref(dir), std::placeholders::_1));
}
//@}

//! \since build 476
void
Search(const string& path, const string& opath, const string& flags)
{
	YAssert(path.size() > 1 && path.back() == wchar_t(YCL_PATH_DELIMITER),
		"Invalid path found.");

	Info(u8"Purging path: " + opath + u8" ...");
	try
	{
		EnsureDirectory(opath);
	}
	catch(std::system_error& e)
	{
		Raise(std::runtime_error(u8"Failed creating directory '"
			+ opath + u8"'."));
	}
	Traverse(opath, [&](HDirectory& dir, const string& name){
		YAssert(!name.empty(), "Empty name found.");

		if(YB_LIKELY(name[0] != '.'))
			if(dir.GetNodeCategory() == NodeCategory::Regular)
			{
				const auto ext(GetExtensionOf(String(name, CS_Path)));

				if(ext == u"a" || ext == u"o")
				{
					if(!uremove((opath + name).c_str()))
						Raise(std::runtime_error(u8"Failed deleting file '"
							+ name + u8"'."));
					Info(u8"Deleted file '" + name + u8"'.");
				}
			}
	});
	Info(u8"Searching path: " + path + u8" ...");
	Traverse(path, [&](HDirectory& dir, const string& name){
		YAssert(!name.empty(), "Empty name found.");

		if(YB_LIKELY(name[0] != '.'))
		{
			if(dir.GetNodeCategory() == NodeCategory::Regular)
			{
				const auto ext(GetExtensionOf(String(name, CS_Path)));

				if(ext == u"c" || ext == u"cc" || ext == u"cpp"
					|| ext == u"cxx")
				{
					const int ret(Call((ext == u"c" ? u8"gcc -c" : u8"g++ -c")
						+ flags + ' ' + path + name + u8" -o " + opath + name
						+ u8".o"));

					if(ret != 0)
						Raise(0x10000 + ret);
				}
			}
			else
				Search(path + name + YCL_PATH_DELIMITER,
					opath + name + YCL_PATH_DELIMITER, flags);
		}
	});

	size_t anum(0), onum(0);

	Traverse(opath, [&](HDirectory& dir, const string& name){
		YAssert(!name.empty(), "Empty name found.");

		if(YB_LIKELY(name[0] != '.')
			&& dir.GetNodeCategory() == NodeCategory::Regular)
		{
			const auto ext(GetExtensionOf(String(name, CS_Path)));

			if(ext == u"a")
				++anum;
			else if(ext == u"o")
				++onum;
		}
	});
	if(anum != 0 || onum != 0)
	{
		auto str(opath);
	
		if(str.back() == YCL_PATH_DELIMITER)
			str.pop_back();

		str = u8"ar rcs \"" + str + u8".a\"";
		if(anum != 0)
			str += ' ' + opath + u8"*.a";
		if(onum != 0)
			str += ' ' + opath + u8"*.o";

		const int ret(Call(str));

		if(ret != 0)
			Raise(ret + 0x20000);
	}
}

//! \since build 477
void
PrintException(const std::exception& e, size_t level = 0)
{
	try
	{
		cerr << string(level, ' ') << "ERROR: " << MBCSToMBCS(e.what())
			<< '\n';
		std::rethrow_if_nested(e);
	}
	catch(std::bad_cast&)
	{
		throw;
	}
	catch(FileOperationFailure&)
	{
		cerr << "ERROR: File operation failure." << endl;
		PrintException(e, ++level);
		throw 1;
	}
	catch(std::exception& e)
	{
		PrintException(e, ++level);
	}
	catch(...)
	{
		cerr << "ERROR: PrintException." << '\n';
	}
}

} // unnamed namespace;


//! \since build 477
void
Build(const vector<string>& args)
{
	YAssert(!args.empty(), "Argument not found.");

	auto in(ystdex::rtrim(string(args[0]), "/\\") + YCL_PATH_DELIMITER);
	Path ipath(in);

	if(ipath.empty())
	{
		cerr << "ERROR: Empty SRCPATH found." << endl;
		Raise(1);
	}
	if(IsRelative(ipath))
		ipath = Path(FetchCurrentWorkingDirectory()) / ipath;
	ipath.Normalize();

	YAssert(IsAbsolute(ipath), "Invalid path converted.");

	Info(u8"Absolute path recognized: " + to_string(ipath).GetMBCS());
	if(!VerifyDirectory(in))
	{
		cerr << "ERROR: SRCPATH is not exist." << endl;
		Raise(1);
	}
	try
	{
		EnsureDirectory(build_path);
	}
	catch(std::system_error&)
	{
		cerr << "ERROR: Failed creating build directory." << '\'' << build_path
			<< '\'' << endl;
		Raise(2);
	}

	string flags;

	for_each(next(args.begin()), args.end(), [&](const string& opt){
		flags += ' ' + opt;
	});

	string opath(build_path);

	if(!ipath.empty())
		opath += ipath.back().GetMBCS() + YCL_PATH_DELIMITER;
	Search(in, opath, flags);
}

void
PrintUsage(const char* prog)
{
	Info(u8"Usage: " + string(prog) + u8" SRCPATH\n" + "SRCPATH\n"
		+ "\tThe source directory to be recursively searched.\n");
}


int
main(int argc, char* argv[])
{
	try
	{
		if(argc > 1)
		{
			vector<string> args;

			for(int i(1); i < argc; ++i)
				args.emplace_back(MBCSToMBCS(argv[i], CP_ACP, CP_UTF8));
			try
			{
				Build(args);
			}
			catch(IntException& e)
			{
				cerr << "IntException: " << std::setw(8) << std::showbase
					<< std::uppercase << std::hex << int(e) - 0x10000 << '.'
					<< endl;
				throw 3;
			}
			catch(std::exception& e)
			{
				PrintException(e);
				throw 3;
			}
		}
		else if(argc == 1)
			PrintUsage(*argv);
	}
	catch(std::bad_alloc&)
	{
		cerr << "ERROR: Allocation failed." << endl;
		return 3;
	}
	catch(int ret)
	{
		if(ret == 3)
			cerr << "ERROR: Failed calling command." << endl;
		return ret;
	}
	catch(...)
	{
		cerr << "ERROR: Unknown failure." << endl;
		return 3;
	}
}

