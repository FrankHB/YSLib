/*
	Copyright (C) by Franksoft 2012.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.cpp
\ingroup YCLib
\brief 平台相关的文件系统接口。
\version r1539;
\author FrankHB<frankhb1989@gmail.com>
\since build 312 。
\par 创建时间:
	2012-05-30 22:41:35 +0800;
\par 修改时间:
	2012-06-02 13:38 +0800;
\par 文本编码:
	UTF-8;
\par 模块名称:
	YCLib::FileSystem;
*/


#include "YCLib/FileSystem.h"
#include "YCLib/NativeAPI.h"
#include <CHRLib/chrproc.h>
#ifdef YCL_MINGW32
#include <Shlwapi.h> // for ::PathIsRelative;

//! \since build 312 。
//@{
extern "C"
{

struct _WDIR;
struct _wdirent;

struct _WDIR* __cdecl __MINGW_NOTHROW
_wopendir(const wchar_t*);
struct _wdirent* __cdecl __MINGW_NOTHROW
_wreaddir(_WDIR*);
int __cdecl __MINGW_NOTHROW
_wclosedir(_WDIR*);

} // extern "C";
//@}

#endif

namespace platform
{

static_assert(std::is_same<CHRLib::ucs2_t, char16_t>::value,
	"Wrong character type!");
static_assert(std::is_same<CHRLib::ucs4_t, char32_t>::value,
	"Wrong character type!");
#ifdef YCL_MINGW32
// TODO: assert %alignof equality;
static_assert(sizeof(wchar_t) == sizeof(CHRLib::ucs2_t),
	"Wrong character type!");
#endif

namespace
{

#ifdef YCL_DS
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
#elif defined(YCL_MINGW32)
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


std::FILE*
ufopen(const char* filename, const char* mode)
{
	yconstraint(filename),
	yconstraint(mode);
	yconstraint(*mode != '\0');

#ifdef YCL_DS
	return std::fopen(filename, mode);
#elif defined(YCL_MINGW32)
	return ::_wfopen(u_to_w(filename).c_str(), u_to_w(mode).c_str());
#else
#	error Unsupported platform found!
#endif
}
std::FILE*
ufopen(const char16_t* filename, const char16_t* mode)
{
	yconstraint(filename),
	yconstraint(mode);
	yconstraint(*mode != '\0');

#ifdef YCL_DS
	return std::fopen(u16_to_u(filename).c_str(), u16_to_u(mode).c_str());
#elif defined(YCL_MINGW32)
	return ::_wfopen(reinterpret_cast<const wchar_t*>(filename),
		reinterpret_cast<const wchar_t*>(mode));
#else
#	error Unsupported platform found!
#endif
}

bool
ufexists(const char* filename)
{
#ifdef YCL_DS
	return ystdex::fexists(filename);
#elif defined(YCL_MINGW32)
	yconstraint(filename);

	if(const auto file = ufopen(filename, "rb"))
	{
		std::fclose(file);
		return true;
	}
	return false;
#else
#	error Unsupported platform found!
#endif
}
bool
ufexists(const char16_t* filename)
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
direxists(const_path_t path)
{
	const auto dir(::opendir(path));

	::closedir(dir);
	return dir;
}

bool
udirexists(const_path_t path)
{
#ifdef YCL_MINGW32
	using namespace CHRLib;

	if(path)
	{
		const auto dir(::_wopendir(u_to_w(path).c_str()));

		::_wclosedir(dir);
		return dir;
	}
	return false;
#else
	return direxists(path);
#endif
}

char*
getcwd_n(char* buf, std::size_t size)
{
	if(YB_LIKELY(buf))
		return ::getcwd(buf, size);
	return nullptr;
}

char16_t*
u16getcwd_n(char16_t* buf, std::size_t size)
{
	if(size == 0)
	//	last_err = EINVAL;
		;
	else
	{
		using namespace std;
		using namespace CHRLib;

		if(YB_LIKELY(buf))
#ifdef YCL_DS
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
#elif defined(YCL_MINGW32)
			return reinterpret_cast<ucs2_t*>(
				::_wgetcwd(reinterpret_cast<wchar_t*>(buf), size));
#else
#	error Unsupported platform found!
#endif
	}
	return nullptr;
}

int
uchdir(const_path_t path)
{
#ifdef YCL_DS
	return ::chdir(path);
#elif defined(YCL_MINGW32)
	return path ? ::_wchdir(u_to_w(path).c_str()) : -1;
#else
#	error Unsupported platform found!
#endif
}

bool
mkdirs(const_path_t cpath)
{
	PATHSTR path;

	std::strcpy(path, cpath);
	for(char* slash(path); (slash = strchr(slash, YCL_PATH_DELIMITER));
		++slash)
	{
		*slash = '\0';
		::mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
		*slash = '/';
	}
	//新建目录成功或目标路径已存在时返回 true 。
	return ::mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == 0
		|| errno == EEXIST;
}


int HFileNode::LastError(0);

HFileNode&
HFileNode::operator++()
{
	LastError = 0;
	if(YB_LIKELY(IsValid()))
	{
		if(!(p_dirent = ::readdir(dir)))
			LastError = 2;
	}
	else
		LastError = 1;
	return *this;
}

bool
HFileNode::IsDirectory() const
{
#if defined(YCL_DS) || defined(YCL_MINGW32)
	return p_dirent && platform_ex::IsDirectory(*p_dirent);
#else
#	error Unsupported platform found!
#endif
}

const char*
HFileNode::GetName() const
{
	return p_dirent ? p_dirent->d_name : ".";
}

void
HFileNode::Open(const_path_t path)
{
	dir = path ? ::opendir(path) : nullptr;
}

void
HFileNode::Close()
{
	LastError = IsValid() ? ::closedir(dir) : 1;
}

void
HFileNode::Reset()
{
	::rewinddir(dir);
}


bool
IsAbsolute(const_path_t path)
{
#ifdef YCL_DS
	return std::strchr(path, '/') == path
		|| std::strstr(path, "fat:/") == path
		|| std::strstr(path, "sd:/");
#elif defined(YCL_MINGW32)
	// TODO: use PathIsRelativeW;
	return !::PathIsRelativeA(path);
	return false;
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

}

