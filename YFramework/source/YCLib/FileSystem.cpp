/*
	© 2012-2014 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file FileSystem.cpp
\ingroup YCLib
\brief 平台相关的文件系统接口。
\version r1272
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-30 22:41:35 +0800
\par 修改时间:
	2014-02-16 01:39 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileSystem
*/


#include "YCLib/YModules.h"
#include "CHRLib/YModules.h"
#include YFM_YCLib_FileSystem
#include YFM_YCLib_NativeAPI
#include YFM_CHRLib_CharacterProcessing
#include <cstring> // for std::strcpy, std::strchr;
#if YCL_DS
#	include <fcntl.h>

//! \since build 341
extern "C" int	_EXFUN(fileno, (FILE *));
#elif YCL_Win32
#	if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
// At least one headers of <stdlib.h>, <stdio.h>, <windows.h>, <windef.h>
//	(and probably more) should have been included to make the MinGW-W64 macro
//	available if it is really being used.
//! \since build 464
//@{
extern "C" _CRTIMP int __cdecl __MINGW_NOTHROW
_fileno(::FILE*);

extern "C" _CRTIMP ::FILE* __cdecl __MINGW_NOTHROW
_wfopen(const wchar_t*, const wchar_t*);
//@}
#	endif
#	include <Shlwapi.h> // for ::PathIsRelativeW;
#endif

//! \since build 475
using namespace CHRLib;

namespace platform
{

static_assert(std::is_same<CHRLib::ucs2_t, char16_t>::value,
	"Wrong character type found.");
static_assert(std::is_same<CHRLib::ucs4_t, char32_t>::value,
	"Wrong character type found.");
#if YCL_Win32
static_assert(sizeof(wchar_t) == sizeof(CHRLib::ucs2_t),
	"Wrong character type found.");
static_assert(yalignof(wchar_t) == yalignof(CHRLib::ucs2_t),
	"Inconsistent alignment between character types found.");
#endif

namespace
{

#if YCL_DS
#elif YCL_MinGW32
std::wstring
u_to_w(const char* str)
{
	return std::wstring(reinterpret_cast<const wchar_t*>(ucsdup(str).c_str()));
}


//! \since build 474
//@{
class DirEnv
{
public:
	std::wstring d_name;
	WIN32_FIND_DATAW& find_data;

	DirEnv(std::wstring&, ::WIN32_FIND_DATAW&);

	DefGetter(const ynothrow, ::DWORD, Attributes, find_data.dwFileAttributes)
};

DirEnv::DirEnv(std::wstring& dir_name, ::WIN32_FIND_DATAW& win32_fdata)
	: d_name(), find_data(win32_fdata)
{
	yconstraint(!dir_name.empty() && dir_name.back() != '\\');

	const auto r(::GetFileAttributesW(dir_name.c_str()));

	if(r != INVALID_FILE_ATTRIBUTES && r & FILE_ATTRIBUTE_DIRECTORY)
		dir_name += L"\\*";
	else
		throw FileOperationFailure("Opening directory failed.");
}


class DirectoryData : private ystdex::noncopyable
{
private:
	std::wstring dir_name;
	::WIN32_FIND_DATAW find_data;
	::HANDLE h_node;
	DirEnv posix_dir;

public:
	DirectoryData(const char*);
	~DirectoryData();

	DefGetterMem(const ynothrow, ::DWORD, Attributes, posix_dir)

private:
	void
	Close() ynothrow;

public:
	DirEnv*
	Read();

	void
	Rewind() ynothrow;
};

DirectoryData::DirectoryData(const char* name)
	: dir_name(ystdex::rtrim(u_to_w(name), L"/\\")),
	find_data(), h_node(), posix_dir(dir_name, find_data)
{}
DirectoryData::~DirectoryData()
{
	if(h_node)
		Close();
}

void
DirectoryData::Close() ynothrow
{
	const auto res(::FindClose(h_node));

	YAssert(res, "No valid directory found.");

	yunused(res);
}

DirEnv*
DirectoryData::Read()
{
	if(!h_node)
	{
		// NOTE: See MSDN "FindFirstFile function" for details.
		yassume(!dir_name.empty());
		yassume(dir_name.back() != L'\\');

		if((h_node = ::FindFirstFileW(dir_name.c_str(), &find_data))
			== INVALID_HANDLE_VALUE)
			h_node = {};
	}
	else if(!::FindNextFileW(h_node, &find_data))
	{
		Close();
		h_node = {};
	}
	if(h_node && h_node != INVALID_HANDLE_VALUE)
	{
		yassume(find_data.cFileName);

		posix_dir.d_name = find_data.cFileName;
	}
	return !h_node ? nullptr : &posix_dir;
}

void
DirectoryData::Rewind() ynothrow
{
	if(h_node)
	{
		Close();
		h_node = {};
	}
}
//@}
#else
#	error "Unsupported platform found."
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
		return ::open(strdup(filename).c_str(), oflag);
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
		return ::open(strdup(filename).c_str(), oflag, pmode);
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
		return std::fopen(strdup(filename).c_str(), strdup(mode).c_str());
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

//! \since build 476
#define YCL_FileSystem_ufunc_impl1(_n) \
bool \
_n(const char* path) ynothrow \
{ \
	yconstraint(path); \
\

#if YCL_DS
//! \since build 476
#define YCL_FileSystem_ufunc_impl2(_fn, _wfn) \
	return _fn(path) == 0; \
}
#else
//! \since build 476
#define YCL_FileSystem_ufunc_impl2(_fn, _wfn) \
	try \
	{ \
		return _wfn(u_to_w(path).c_str()) == 0; \
	} \
	catch(...) \
	{} \
	return false; \
}
#endif

//! \since build 476
#define YCL_FileSystem_ufunc_impl(_n, _fn, _wfn) \
	YCL_FileSystem_ufunc_impl1(_n) \
	YCL_FileSystem_ufunc_impl2(_fn, _wfn)

YCL_FileSystem_ufunc_impl(uchdir, ::chdir, ::_wchdir)

YCL_FileSystem_ufunc_impl1(umkdir)
#if YCL_DS
	return ::mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == 0;
}
#else
	YCL_FileSystem_ufunc_impl2(_unused_, ::_wmkdir)
#endif

YCL_FileSystem_ufunc_impl(urmdir, ::rmdir, ::_wrmdir)

YCL_FileSystem_ufunc_impl(uunlink, ::unlink, ::_wunlink)

YCL_FileSystem_ufunc_impl(uremove, std::remove, ::_wremove)

#undef YCL_FileSystem_ufunc_impl1
#undef YCL_FileSystem_ufunc_impl2
#undef YCL_FileSystem_ufunc_impl

bool
truncate(std::FILE* fp, std::size_t size) ynothrow
{
#if YCL_DS
	return ::ftruncate(fileno(fp), ::off_t(size)) == 0;
#else
	return ::_chsize(::_fileno(fp), long(size)) == 0;
#endif
}


std::uint64_t
GetFileSizeOf(int fd)
{
#if YCL_DS
	struct ::stat st;

	if(::fstat(fd, &st) == 0)
		return st.st_size;
#else
	::LARGE_INTEGER sz;

	// XXX: Error handling for indirect calls.
	if(::GetFileSizeEx(::HANDLE(::_get_osfhandle(fd)), &sz) != 0
		&& YB_LIKELY(sz.QuadPart >= 0))
		return sz.QuadPart;
#endif
	throw std::runtime_error("Failed getting file size.");
}
std::uint64_t
GetFileSizeOf(std::FILE* fp)
{
	yconstraint(fp);

#if YCL_DS
	return GetFileSizeOf(fileno(fp));
#else
	return GetFileSizeOf(::_fileno(fp));
#endif
}


DirectorySession::DirectorySession(const char* path)
	: dir(
#if YCL_DS
		::opendir
#else
		new DirectoryData
#endif
		(path && *path != '\0' ? path : ".")
	)
{
	if(!dir)
		throw FileOperationFailure("Opening directory failed.");
}
DirectorySession::~DirectorySession()
{
#if YCL_DS
	const auto res(::closedir(dir));

	YAssert(res == 0, "No valid directory found.");

	yunused(res);
#else
	delete static_cast<DirectoryData*>(dir);
#endif
}

void
DirectorySession::Rewind() ynothrow
{
	YAssert(dir, "Invalid native handle found.");

#if YCL_DS
	::rewinddir(dir);
#else
	static_cast<DirectoryData*>(dir)->Rewind();
#endif
}


HDirectory&
HDirectory::operator++()
{
#if YCL_DS
	p_dirent = ::readdir(GetNativeHandle());
#else
	p_dirent = static_cast<DirectoryData*>(GetNativeHandle())->Read();
#endif
	return *this;
}

NodeCategory
HDirectory::GetNodeCategory() const ynothrow
{
	if(p_dirent)
	{
#if YCL_DS
		return p_dirent->d_type & DT_DIR ? NodeCategory::Directory
			: NodeCategory::Regular;
#elif YCL_MinGW32
		return static_cast<DirectoryData*>(GetNativeHandle())->GetAttributes()
			& FILE_ATTRIBUTE_DIRECTORY ? NodeCategory::Directory
			: NodeCategory::Regular;
#endif
	}
	return NodeCategory::Empty;
}

const char*
HDirectory::GetName() const ynothrow
{
	if(p_dirent)
	{
#if YCL_DS
		return p_dirent->d_name;
#else
		try
		{
			utf8_name = strdup(reinterpret_cast<const char16_t*>(
				(static_cast<DirEnv*>(p_dirent))->d_name.c_str()));
			return &utf8_name[0];
		}
		catch(...)
		{}
#endif
	}
	return ".";
}


bool
IsAbsolute(const char* path)
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
#elif YCL_MinGW32
	return !::PathIsRelativeW(u_to_w(path).c_str());
#else
#	error "Unsupported platform found."
#endif
}

std::size_t
GetRootNameLength(const char* path)
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

