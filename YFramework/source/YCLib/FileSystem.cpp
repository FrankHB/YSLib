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
\version r1362;
\author FrankHB<frankhb1989@gmail.com>
\since build 312 。
\par 创建时间:
	2012-05-30 22:41:35 +0800;
\par 修改时间:
	2012-05-31 14:00 +0800;
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


std::FILE*
ufopen(const char* filename, const char* mode)
{
	yconstraint(filename),
	yconstraint(mode);
	yconstraint(*mode != '\0');

#ifdef YCL_DS
	return std::fopen(filename, mode);
#elif defined(YCL_MINGW32)
	using namespace CHRLib;

	const auto wname(reinterpret_cast<wchar_t*>(ucsdup(filename)));
	wchar_t tmp[5];
	auto wmode(&tmp[0]);

	if(YCL_UNLIKELY(std::strlen(mode) > 4))
		wmode = reinterpret_cast<wchar_t*>(ucsdup(mode));
	else
		CHRLib::MBCSToUCS2(reinterpret_cast<ucs2_t*>(wmode), mode);

	const auto fp(::_wfopen(wname, wmode));

	if(wmode != tmp)
		std::free(wmode);
	std::free(wname);
	return fp;
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
	using CHRLib::strdup;

	std::FILE* fp(nullptr);

	const auto nfilename(strdup(filename));

	if(YCL_LIKELY(nfilename))
	{
		// TODO: small string local allocation optimization;
		const auto nmode(strdup(mode));

		if(YCL_LIKELY(nmode))
		{
			fp = std::fopen(nfilename, nmode);
			std::free(nmode);
		}
		std::free(nfilename);
	}
	return fp;
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

char*
getcwd_n(char* buf, std::size_t size)
{
	if(YCL_LIKELY(buf))
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

		if(YCL_LIKELY(buf))
#ifdef YCL_DS
		{
			const auto p(static_cast<ucs2_t*>(malloc((size + 1)
				* sizeof(ucs2_t))));

			if(YCL_LIKELY(p))
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
		static wchar_t* s_wstr;

		std::free(s_wstr);
		if((s_wstr = reinterpret_cast<wchar_t*>(ucsdup(path))))
		{
			const auto dir(::_wopendir(s_wstr));

			::_wclosedir(dir);
			return dir;
		}
	}
	return false;
#else
	return direxists(path);
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
	if(YCL_LIKELY(IsValid()))
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

