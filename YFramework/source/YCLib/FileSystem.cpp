/*
	Copyright by FrankHB 2012 - 2013.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.cpp
\ingroup YCLib
\brief 平台相关的文件系统接口。
\version r846
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-30 22:41:35 +0800
\par 修改时间:
	2013-05-31 11:56 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileSystem
*/


#include "YCLib/FileSystem.h"
#include "YCLib/NativeAPI.h"
#include "CHRLib/chrproc.h"
#include <cstring> // for std::strcpy, std::strchr;
#if YCL_DS
#	include <fcntl.h>

//! \since build 341
extern "C" int	_EXFUN(fileno, (FILE *));
#elif YCL_MINGW32
#	include <Shlwapi.h> // for ::PathIsRelativeW;
#endif

namespace platform
{

static_assert(std::is_same<CHRLib::ucs2_t, char16_t>::value,
	"Wrong character type!");
static_assert(std::is_same<CHRLib::ucs4_t, char32_t>::value,
	"Wrong character type!");
#if YCL_MINGW32
static_assert(sizeof(wchar_t) == sizeof(CHRLib::ucs2_t),
	"Wrong character type!");
static_assert(yalignof(wchar_t) == yalignof(CHRLib::ucs2_t),
	"Inconsist alignment between character types!");
#endif

namespace
{

std::string
u16_to_u(const char16_t* u16str)
{
	yconstraint(u16str);

	static char* tstr;
	std::string str;

	str = std::string(tstr = CHRLib::strdup(u16str));
	std::free(tstr);
	return std::move(str);
}
#if YCL_DS
#elif YCL_MINGW32
std::wstring
u_to_w(const char* str)
{
	yconstraint(str);

	static CHRLib::ucs2_t* tstr;
	std::wstring wstr;

	wstr = std::wstring(reinterpret_cast<const wchar_t*>(
		tstr = CHRLib::ucsdup(str)));
	std::free(tstr);
	return std::move(wstr);
}
#else
#	error Unsupported platform found!
#endif

} // unnamed namespace;


int
uopen(const char* filename, int oflag) ynothrow
{
	yconstraint(filename);

#if YCL_DS
	return ::open(filename, oflag);
#else
	try
	{
		return ::_wopen(u_to_w(filename).c_str(), oflag);
	}
	catch(...)
	{}
	return -1;
#endif
}
int
uopen(const char* filename, int oflag, int pmode) ynothrow
{
	yconstraint(filename);

#if YCL_DS
	return ::open(filename, oflag, pmode);
#else
	try
	{
		return ::_wopen(u_to_w(filename).c_str(), oflag, pmode);
	}
	catch(...)
	{}
	return -1;
#endif
}
int
uopen(const char16_t* filename, int oflag) ynothrow
{
	yconstraint(filename);

#if YCL_DS
	try
	{
		return ::open(u16_to_u(filename).c_str(), oflag);
	}
	catch(...)
	{}
	return -1;
#else
	return ::_wopen(reinterpret_cast<const wchar_t*>(filename), oflag);
#endif
}
int
uopen(const char16_t* filename, int oflag, int pmode) ynothrow
{
	yconstraint(filename);

#if YCL_DS
	try
	{
		return ::open(u16_to_u(filename).c_str(), oflag, pmode);
	}
	catch(...)
	{}
	return -1;
#else
	return ::_wopen(reinterpret_cast<const wchar_t*>(filename), oflag, pmode);
#endif
}

std::FILE*
ufopen(const char* filename, const char* mode) ynothrow
{
	yconstraint(filename),
	yconstraint(mode);
	yconstraint(*mode != char());

#if YCL_DS
	return std::fopen(filename, mode);
#else
	try
	{
		return ::_wfopen(u_to_w(filename).c_str(), u_to_w(mode).c_str());
	}
	catch(...)
	{}
	return nullptr;
#endif
}
std::FILE*
ufopen(const char16_t* filename, const char16_t* mode) ynothrow
{
	yconstraint(filename),
	yconstraint(mode);
	yconstraint(*mode != char());

#if YCL_DS
	try
	{
		return std::fopen(u16_to_u(filename).c_str(), u16_to_u(mode).c_str());
	}
	catch(...)
	{}
	return nullptr;
#else
	return ::_wfopen(reinterpret_cast<const wchar_t*>(filename),
		reinterpret_cast<const wchar_t*>(mode));
#endif
}

bool
ufexists(const char* filename) ynothrow
{
#if YCL_DS
	return ystdex::fexists(filename);
#else
	yconstraint(filename);

	if(const auto file = ufopen(filename, "rb"))
	{
		std::fclose(file);
		return true;
	}
	return false;
#endif
}
bool
ufexists(const char16_t* filename) ynothrow
{
	yconstraint(filename);

	if(const auto file = ufopen(filename, u"rb"))
	{
		std::fclose(file);
		return true;
	}
	return false;
}

bool
direxists(const_path_t path) ynothrow
{
	const auto dir(::opendir(path));

	::closedir(dir);
	return dir;
}

bool
udirexists(const_path_t path) ynothrow
{
#if YCL_MINGW32
	using namespace CHRLib;

	if(path)
		try
		{
			const auto dir(::_wopendir(u_to_w(path).c_str()));

			::_wclosedir(dir);
			return dir;
		}
		catch(...)
		{}
	return false;
#else
	return direxists(path);
#endif
}

char*
getcwd_n(char* buf, std::size_t size) ynothrow
{
	if(YB_LIKELY(buf))
		return ::getcwd(buf, size);
	return nullptr;
}

char16_t*
u16getcwd_n(char16_t* buf, std::size_t size) ynothrow
{
	if(size == 0)
	//	last_err = EINVAL;
		;
	else
	{
		using namespace std;
		using namespace CHRLib;

		if(YB_LIKELY(buf))
#if YCL_DS
		{
			const auto p(static_cast<ucs2_t*>(malloc((size + 1)
				* sizeof(ucs2_t))));

			if(YB_LIKELY(p))
			{
				auto len(MBCSToUCS2(p,
					::getcwd(reinterpret_cast<char*>(buf), size)));

				if(size < len + 1)
				{
				//	last_err = ERANGE;
					return nullptr;
				}
				memcpy(buf, p, ++len * sizeof(ucs2_t));
				return buf;
			}
		//	else
			//	last_err = ENOMEM;
		}
#else
			return reinterpret_cast<ucs2_t*>(
				::_wgetcwd(reinterpret_cast<wchar_t*>(buf), size));
#endif
	}
	return nullptr;
}

int
uchdir(const_path_t path) ynothrow
{
#if YCL_DS
	return ::chdir(path);
#else
	try
	{
		return path ? ::_wchdir(u_to_w(path).c_str()) : -1;
	}
	catch(...)
	{}
	return -1;
#endif
}

bool
mkdirs(const_path_t cpath) ynothrow
{
	PATHSTR path;

	std::strcpy(path, cpath);
	for(char* slash(path); (slash = strchr(slash, YCL_PATH_DELIMITER)); ++slash)
	{
		*slash = char();
		::mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
		*slash = '/';
	}
	//新建目录成功或目标路径已存在时返回 true 。
	return ::mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == 0
		|| errno == EEXIST;
}

bool
truncate(std::FILE* fp, std::size_t size) ynothrow
{
#if YCL_DS
	return ::ftruncate(fileno(fp), ::off_t(size)) != 0;
#else
	return ::_chsize(_fileno(fp), long(size));
#endif
}


int HFileNode::LastError(0);

HFileNode&
HFileNode::operator++() ynothrow
{
	LastError = 0;
	if(YB_LIKELY(*this))
	{
#if YCL_DS
		if(!(p_dirent = ::readdir(dir)))
#else
		if(!(p_dirent = ::_wreaddir(dir)))
#endif
			LastError = 2;
	}
	else
		LastError = 1;
	return *this;
}

bool
HFileNode::IsDirectory() const ynothrow
{
	if(p_dirent)
	{
#if YCL_DS
		return p_dirent->d_type & DT_DIR;
#elif YCL_MINGW32
		struct ::stat st;

		::wstat(p_dirent->d_name, &st);
		return S_ISDIR(st.st_mode);
#endif
	}
	return false;
}

const char*
HFileNode::GetName() const ynothrow
{
	if(p_dirent)
	{
#if YCL_DS
		return p_dirent->d_name;
#else
		try
		{
			utf8_name = u16_to_u(reinterpret_cast<const char16_t*>(
				p_dirent->d_name));
			return utf8_name.data();
		}
		catch(...)
		{}
#endif
	}
	return ".";
}

void
HFileNode::Open(const_path_t path) ynothrow
{
	if(path)
	{
#if YCL_DS
		dir = ::opendir(path);
#else
		try
		{
			dir = ::_wopendir(u_to_w(path).c_str());
		}
		catch(...)
		{}
#endif
	}
	else
		dir = nullptr;
}

void
HFileNode::Close() ynothrow
{
#if YCL_DS
	LastError = *this ? ::closedir(dir) : 1;
#else
	LastError = *this ? ::_wclosedir(dir) : 1;
#endif
	dir = nullptr;
}

void
HFileNode::Reset() ynothrow
{
#if YCL_DS
	::rewinddir(dir);
#else
	::_wrewinddir(dir);
#endif
}


bool
IsAbsolute(const_path_t path)
{
#if YCL_DS
	if(path)
	{
		if(*path == '/')
			return true;

		const auto p(std::strstr(path, ":/"));

		return p && p != path && !std::strstr(p, ":/");
	}
	return false;
#elif YCL_MINGW32
	return !::PathIsRelativeW(u_to_w(path).c_str());
#else
#	error Unsupported platform found!
#endif
}

std::size_t
GetRootNameLength(const_path_t path)
{
	const char* p(std::strchr(path, ':'));

	return !p ? 0 : p - path + 1;
}

}

namespace platform_ex
{

#if YCL_DS
char16_t
FS_IsRoot(const char16_t* str)
{
	const std::u16string ustr(str);

	return ustr == u"/" || ustr == u"fat:/" || ustr == u"sd:/";
}
#endif

}

