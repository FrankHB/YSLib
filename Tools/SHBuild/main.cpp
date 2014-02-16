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
\version r434
\author FrankHB <frankhb1989@gmail.com>
\since build 473
\par 创建时间:
	2014-02-06 14:33:55 +0800
\par 修改时间:
	2014-02-16 17:56 +0800
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
	return Call(MBCSToWCS(cmd)); 
}
int
Call(const string& cmd)
{
	return Call(cmd.c_str()); 
}

//! \since build 476
void
Search(const string& path, const string& opath, const string& flags)
{
	YAssert(path.size() > 1 && path.back() == wchar_t(YCL_PATH_DELIMITER),
		"Invalid path found.");

	const auto lopath(MBCSToMBCS(opath));

	cout << "Purging path: " << lopath << " ..." << endl;
	try
	{
		EnsureDirectory(opath);
	}
	catch(std::system_error& e)
	{
		cerr << "ERROR: Failed creating directory '" << lopath << "':" 
			<< e.what() << '.' << endl;
		throw 3;
	}
	{
		HDirectory dir(opath.c_str());

		for_each(FileIterator(&dir), FileIterator(),
			[&](const string& name){
			YAssert(!name.empty(), "Empty name found.");

			if(YB_LIKELY(name[0] != '.'))
				if(dir.GetNodeCategory() == NodeCategory::Regular)
				{
					const auto ext(GetExtensionOf(String(name, CS_Path)));

					if(ext == u"a" || ext == u"o")
					{
						const auto lname(MBCSToMBCS(name));

						if(!uremove((opath + name).c_str()))
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
	cout << "Searching path: " << MBCSToMBCS(path) << " ..." << endl;
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
						const int ret(Call((ext == u"c" ? u8"gcc -c"
							: u8"g++ -c") + flags + ' ' + path + name
							+ u8" -o " + opath + name + u8".o"));

						if(ret != 0)
							throw 0x10000 + ret;
					}
				}
				else
					Search(path + name + YCL_PATH_DELIMITER,
						opath + name + YCL_PATH_DELIMITER, flags);
			}
		});
	}

	size_t anum(0), onum(0);

	{
		HDirectory dir(opath.c_str());

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

		str = u8"ar rcs \"" + str + u8".a\"";
		if(anum != 0)
			str += ' ' + opath + u8"*.a";
		if(onum != 0)
			str += ' ' + opath + u8"*.o";

		const int ret(Call(str));

		if(ret != 0)
			throw ret + 0x20000;
	}
}

} // unnamed namespace;


//! \since build 476
int
Build(const vector<string>& args)
{
	YAssert(args.size() > 0, "Wrong argument number found.");

	try
	{
		auto in(args[0]);

		ystdex::rtrim(in, "/\\");
		in += YCL_PATH_DELIMITER;

		Path ipath(in);

		if(ipath.empty())
		{
			cerr << "ERROR: Empty SRCPATH found." << endl;
			return 1;
		}
		if(IsRelative(ipath))
			ipath = Path(FetchCurrentWorkingDirectory()) / ipath;
		ipath.Normalize();

		YAssert(IsAbsolute(ipath), "Invalid path converted.");

		cout << "Absolute path recognized: " << '\n' << WCSToMBCS(
			reinterpret_cast<const wchar_t*>(to_string(ipath).c_str())) << endl;
		if(!VerifyDirectory(in))
		{
			cerr << "ERROR: SRCPATH is not exist." << endl;
			return 1;
		}
		try
		{
			EnsureDirectory(build_path);
		}
		catch(std::system_error&)
		{
			cerr << "ERROR: Failed creating build directory."  << 
				'\'' << build_path << '\'' << endl;
			throw 2;
		}

		string flags;
		auto b(args.begin());

		for_each(++b, args.end(), [&](const string& opt){
			flags += ' ';
			flags += opt;
		});

		string opath(build_path);

		if(!ipath.empty())
		{
			opath += ipath.back().GetMBCS();
			opath += YCL_PATH_DELIMITER;
		}
		Search(in, opath, flags);
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
		vector<string> args;

		for(int i(1); i < argc; ++i)
			args.emplace_back(MBCSToMBCS(argv[i], CP_ACP, CP_UTF8));
		return Build(args);
	}
	else if(argc == 1)
		PrintUsage(*argv);
}

