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
\version r353
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2014-02-06 14:33:55 +0800
\par 修改时间:
	2014-02-15 23:06 +0800
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
#include <sstream>
#include <Windows.h>

using std::cerr;
using std::cout;
using std::endl;
using std::for_each;
using std::wstring;
using namespace YSLib;
using namespace IO;
using platform_ex::MBCSToWCS;
using platform_ex::WCSToMBCS;

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
Call(const char* cmd)
{
	YAssert(cmd, "Null pointer found.");

	cout << cmd << endl;
	return system(cmd); 
}
int
Call(const string& cmd)
{
	return Call(cmd.c_str()); 
}
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


void
Search(const string& path, const wstring& opath, const wstring& flags)
{
	YAssert(path.size() > 1 && path.back() == wchar_t(YCL_PATH_DELIMITER),
		"Invalid path found.");

	PathNorm nm;
	const auto lopath(WCSToMBCS(opath));
	const auto uopath(WCSToMBCS(opath, CP_UTF8));

	cout << "Purging path: " << lopath << " ..." << endl;
	
	// XXX: Simplify implementation.
	if(![&](std::string path){
		for(char* slash(&path[0]); (slash = std::strchr(slash,
			YCL_PATH_DELIMITER)); ++slash)
		{
			*slash = char();
			if(!umkdir(path.c_str()) && errno != EEXIST)
			{
				cerr << "ERROR Failed creating directory '" << lopath << "'."
					<< endl;
				throw 3;
			}
			*slash = YCL_PATH_DELIMITER;
		}
		return umkdir(path.c_str()) == 0 || errno == EEXIST;
	}(uopath))
	{
		cerr << "ERROR Failed creating directory '" << lopath << "'." << endl;
		throw 3;
	}
	{
		HDirectory dir(uopath.c_str());

		for_each(FileIterator(&dir), FileIterator(),
			[&](const string& name){
			YAssert(!name.empty(), "Empty name found.");

			if(YB_LIKELY(name[0] != '.'))
				if(dir.GetNodeCategory() == NodeCategory::Regular)
				{
					const auto ext(GetExtensionOf(String(name, CS_Path)));

					if(ext == u"a" || ext == u"o")
					{
						const auto wname(MBCSToWCS(name, CP_UTF8));
						const auto lname(WCSToMBCS(wname));

						if(::_wremove((opath + wname).c_str()) != 0)
						{
							cerr << "ERROR: Failed deleting file '" + lname
								+ "'." << endl;
							throw 3;
						}
						cout << "Deleted file '" << lname << "'."<< endl;
					}
				}
		});
	}

	const auto wpath(MBCSToWCS(path, CP_UTF8));

	cout << "Searching path: " << WCSToMBCS(wpath) << " ..." << endl;
	{
		HDirectory dir(path.c_str());

		for_each(FileIterator(&dir), FileIterator(),
			[&](const string& name){
			YAssert(!name.empty(), "Empty name found.");

			if(YB_LIKELY(name[0] != '.'))
			{
				if(dir.GetNodeCategory() == NodeCategory::Regular)
				{
					const auto ext(GetExtensionOf(String(name, CS_Path)));

					if(ext == u"c" || ext == u"cc" || ext == u"cpp"
						|| ext == u"cxx")
					{
						const auto wname(MBCSToWCS(name, CP_UTF8));

						const int ret(Call((ext == u"c" ? L"gcc -c" : L"g++ -c")
							+ flags + L' ' + wpath + wname + L" -o " + opath
							+ wname + L".o"));

						if(ret != 0)
							throw 0x10000 + ret;
					}
				}
				else
					Search(path + name + YCL_PATH_DELIMITER, opath + MBCSToWCS(
						name, CP_UTF8) + wchar_t(YCL_PATH_DELIMITER), flags);
			}
		});
	}

	size_t anum(0), onum(0);

	{
		HDirectory dir(uopath.c_str());

		for_each(FileIterator(&dir), FileIterator(),
			[&](const string& name){
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
	}
	if(anum != 0 || onum != 0)
	{
		auto str(opath);
	
		if(str.back() == YCL_PATH_DELIMITER)
			str.pop_back();

		str = L"ar rcs \"" + str + L".a\"";
		if(anum != 0)
			str += L' ' + opath + L"*.a";
		if(onum != 0)
			str += L' ' + opath + L"*.o";

		const int ret(Call(str));

		if(ret != 0)
			throw ret + 0x20000;
	}
}

} // unnamed namespace;


int
Build(const vector<wstring>& args)
{
	YAssert(args.size() > 0, "Wrong argument number found.");

	try
	{
		auto in(args[0]);

		ystdex::rtrim(in, L"/\\");
		in += YCL_PATH_DELIMITER;

		Path ipath(String(reinterpret_cast<const ucs2_t*>(in.c_str())));

		if(ipath.empty())
		{
			cerr << "ERROR: Empty SRCPATH found." << endl;
			return 1;
		}
		if(ipath.is_relative())
			ipath = Path(FetchCurrentWorkingDirectory()) / ipath;
		ipath.Normalize();

		YAssert(ipath.is_absolute(), "Invalid path converted.");

		cout << "Absolute path recognized: " << '\n' << WCSToMBCS(
			reinterpret_cast<const wchar_t*>(to_string(ipath).c_str())) << endl;
		if(!VerifyDirectory(WCSToMBCS(in, CP_UTF8)))
		{
			cerr << "ERROR: SRCPATH is not exist." << endl;
			return 1;
		}
		if(!umkdir(build_path) && errno != EEXIST)
		{
			cerr << "ERROR: Failed creating build directory." << endl;
			return 2;
		}

		wstring flags;
		auto b(args.begin());

		for_each(++b, args.end(), [&](const wstring& opt){
			flags += L' ';
			flags += opt;
		});

		wstring opath(MBCSToWCS(build_path, CP_UTF8));

		if(!ipath.empty())
		{
			opath += reinterpret_cast<const wchar_t*>(ipath.back().c_str());
			opath += wchar_t(YCL_PATH_DELIMITER);
		}
		Search(WCSToMBCS(in, CP_UTF8).c_str(), opath, flags);
		return 0;
	}
	catch(FileOperationFailure&)
	{
		cerr << "ERROR: File operation failure." << endl;
		return 1;
	}
	catch(std::exception& e)
	{
		cerr << string("ERROR: Exception ") + typeid(e).name() + " : "
			+ e.what() + " ." << endl;  
	}
	catch(int ret)
	{
		cerr << "ERROR: Failed calling command." << endl;
		return ret;
	}
	cerr << "ERROR: Unknown failure." << endl;
	return 3;
}

void
PrintUsage(const char* prog)
{
	cout << "Usage: " << prog << " SRCPATH\n" << "SRCPATH\n"
		<< "\tThe source directory to be recursively searched.\n" << endl;
}


int
main(int argc, char* argv[])
{
	if(argc > 1)
	{
		vector<wstring> args;

		for(int i(1); i < argc; ++i)
			args.emplace_back(MBCSToWCS(argv[i]));
		return Build(args);
	}
	else if(argc == 1)
		PrintUsage(*argv);
}

