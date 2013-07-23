/*
	Copyright by FrankHB 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\defgroup Tools Tools
\brief 工具。
*/
/*!	\defgroup BuildTools Build-time Tools
\brief 构建时工具。
*/

/*!	\file main.cpp
\ingroup BuildTools
\brief 检查是否存在预定义宏。
\version r109
\author FrankHB <frankhb1989@gmail.com>
\since build 431
\par 创建时间:
	2013-07-20 10:57:06 +0800
\par 修改时间:
	2013-07-20 15:07 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	Tools::PredefinedMacroDetector::Main

This is a generator for detecting predefined macros.
See readme file for details.
*/


#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <iostream>

#ifndef YCONF_CppExt
#	define YCONF_CppExt ".cc"
#endif
#ifndef YCONF_ExeExt
#	define YCONF_ExeExt ".exe"
#endif
#ifndef YCONF_ConfExt
#	define YCONF_ConfExt ".txt"
#endif
#ifndef YCONF_LogExt
#	define YCONF_LogExt ".log"
#endif
#ifndef YCONF_GenPrefix
#	define YCONF_GenPrefix "__gen_"
#endif
#ifndef YCONF_Name
#	define YCONF_Name "predefined-macro-detector"
#endif

#define YSL_GenFile YCONF_GenPrefix YCONF_Name YCONF_CppExt
#define YSL_ExeFile YCONF_GenPrefix YCONF_Name YCONF_ExeExt
#define YSL_ConfFile YCONF_Name YCONF_ConfExt
#define YSL_LogFile YCONF_Name YCONF_LogExt

using namespace std;

void
WriteDectector(ofstream& fout, const string& name, const string& line)
{
	fout << "#ifdef " << name << '\n' << line << "\n#endif\n";
}
void
WriteDectector(ofstream& fout, const string& name)
{
	WriteDectector(fout, name, string("\t") + R"(fout << ")" + name
		+ R"(" << '\n';)");
}

void
WriteContent(ofstream& fout)
{
	// For default configuration, see http://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html.
	vector<string> names;
	ifstream fin(YSL_ConfFile);

	copy(istream_iterator<string>(fin), istream_iterator<string>(),
		back_inserter(names));
	for(const auto& name : names)
		WriteDectector(fout, name);
}


int
main()
{
	ofstream fout(YSL_GenFile);

	fout << "#include <fstream>\n";
	fout << "\n\nint\nmain()\n{\n";
	fout << "\tstd::ofstream fout(\"" << YSL_LogFile << "\");\n";
	WriteContent(fout);
	fout << "}\n\n";
	cout << YSL_GenFile " generated OK!" << endl;
}

