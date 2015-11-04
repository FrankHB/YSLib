/*
	© 2013, 2015 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup MaintenanceTools Maintenance Tools
\brief 维护工具。
*/

/*!	\file Main.cpp
\ingroup MaintenanceTools
\brief 检查、备份和恢复指定目录下文件的创建时间。
\version r189
\author FrankHB <frankhb1989@gmail.com>
\since build 437
\par 创建时间:
	2013-08-14 01:36:07 +0800
\par 修改时间:
	2015-10-31 16:24 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Tools::CreationTimeManager::Main

This is a command line tool for maintain creation time of project files.
See readme file for details.
*/


#include <stdexcept>
#include <string>
#include <iostream>
#include <sstream>
#include <Windows.h>

using namespace std;

typedef ::FILETIME FileTime;

namespace
{

const auto file_attr(FILE_ATTRIBUTE_NORMAL | FILE_FLAG_BACKUP_SEMANTICS
	| FILE_FLAG_NO_BUFFERING | FILE_FLAG_OPEN_REPARSE_POINT);

//! \since build 649
//@{
std::wstring
MBCSToWCS(const char* str, unsigned cp = CP_ACP)
{
	// TODO: Use %YFramework.YCLib_('MinGW32').
	const int
		w_len(::MultiByteToWideChar(cp, 0, str, -1, nullptr, 0));

	if(w_len != 0)
	{
		std::wstring res(w_len, wchar_t());

		::MultiByteToWideChar(cp, 0, str, -1, &res[0], w_len);
		if(!res.empty())
			res.pop_back();
		return res;
	}
	return std::wstring();
}
//@}

} // unnamed namespace;

FileTime
ReadCreationTime(const char* name)
{
	const auto h_file(::CreateFileW(MBCSToWCS(name).c_str(), GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, file_attr,
		nullptr));

	if(h_file == INVALID_HANDLE_VALUE)
		throw
			invalid_argument("Wrong file name \"" + string(name) + "\" found.");

	FileTime res;

	::GetFileTime(h_file, &res, nullptr, nullptr);
	::CloseHandle(h_file);
	return res;
}

void
WriteCreationTime(const char* name, const FileTime& time)
{
	const auto h_file(::CreateFileW(MBCSToWCS(name).c_str(), GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, file_attr,
		nullptr));
	const FileTime prevent_modify = {0xFFFFFFFF, 0xFFFFFFFF};

	if(h_file == INVALID_HANDLE_VALUE)
		throw
			invalid_argument("Wrong file name \"" + string(name) + "\" found.");

	::SetFileTime(h_file, &time, &prevent_modify, &prevent_modify);
	::CloseHandle(h_file);
}

void
ReadTimeLines()
{
	string name;

	while(getline(cin, name))
	{
		cout << name << '\n';
		try
		{
			const auto t(ReadCreationTime(name.c_str()));

			cout << t.dwHighDateTime << ' ' << t.dwLowDateTime << endl;
		}
		catch(invalid_argument& e)
		{
			cout << e.what() << endl;
		}
	}
}

void
WriteTimeLines()
{
	string name, line;

	while(getline(cin, name) && getline(cin, line))
	{
		FileTime time;
		istringstream in(line);

		in >> time.dwHighDateTime >> time.dwLowDateTime;
		if(cin)
			try
			{
				WriteCreationTime(name.c_str(), time);
			}
			catch(invalid_argument& e)
			{
				cout << e.what() << endl;
			}
		else
		{
			cerr << "Parsing time \"" << line << "\" failed.";
			cin.clear();
		}
	}
}

void
PrintUsage(const char* prog)
{
	cout << "Usage: " << prog << " [-w]" << '\n' << "-w" << '\n'
		<< "\tWrite time according to lines from standard input." << '\n'
		<< "(none)" << '\n' << "\tRead file name lines from standard input."
		<< endl;
}


int
main(int argc, char* argv[])
{
	if(argc > 0)
		switch(argc)
		{
		case 1:
			ReadTimeLines();
			break;
		case 2:
			if(argv[1] == string("-w"))
			{
				WriteTimeLines();
				break;
			}
		default:
			PrintUsage(*argv);
		}
}

