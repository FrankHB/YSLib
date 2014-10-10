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
\version r1625
\author FrankHB <frankhb1989@gmail.com>
\since build 312
\par 创建时间:
	2012-05-30 22:41:35 +0800
\par 修改时间:
	2014-10-10 20:02 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib::FileSystem
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_FileSystem
#include YFM_YCLib_Debug
#include YFM_YCLib_NativeAPI
#include <cstring> // for std::strcpy, std::strchr;
#if YCL_DS
#	include YFM_CHRLib_CharacterProcessing
#	include <fcntl.h>

//! \since build 341
extern "C" int	_EXFUN(fileno, (FILE *));

//! \since build 475
using namespace CHRLib;
#elif YCL_Win32
#	if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
// At least one headers of <stdlib.h>, <stdio.h>, <Windows.h>, <Windef.h>
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
#	include YFM_MinGW32_YCLib_MinGW32 // for platform_ex::UTF8ToWCS;

//! \since build 540
using platform_ex::UTF8ToWCS;
#elif YCL_Android
#	include YFM_CHRLib_CharacterProcessing
#	include <fcntl.h>
#	include <dirent.h>
#	include <sys/stat.h>

//! \since build 475
using namespace CHRLib;
#endif

namespace platform
{

namespace
{

#if YCL_DS || YCL_Android
#elif YCL_Win32
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
	YAssert(!dir_name.empty() && dir_name.back() != '\\',
		"Invalid argument found.");

	const auto r(::GetFileAttributesW(dir_name.c_str()));
	yconstexpr const char* msg("Opening directory failed.");

	if(YB_UNLIKELY(r == INVALID_FILE_ATTRIBUTES))
		// TODO: Call %::GetLastError to distinguish concreate errors.
		throw FileOperationFailure(EINVAL, std::generic_category(), msg);
	if(r & FILE_ATTRIBUTE_DIRECTORY)
		dir_name += L"\\*";
	else
		throw FileOperationFailure(ENOTDIR, std::generic_category(), msg);
}


class DirectoryData : private ystdex::noncopyable
{
private:
	std::wstring dir_name;
	::WIN32_FIND_DATAW find_data;
	::HANDLE h_node = {};
	DirEnv posix_dir{dir_name, find_data};

public:
	DirectoryData(const char*);
	~DirectoryData();

	DefGetterMem(const ynothrow, ::DWORD, Attributes, posix_dir)
	//! \since build 543
	DefGetter(const ynothrow, const ::WIN32_FIND_DATAW&, FindData, find_data)
	//! \since build 543
	DefGetter(const ynothrow, const std::wstring&, DirName, dir_name)

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
	: dir_name(ystdex::rtrim(UTF8ToWCS(name), L"/\\")), find_data()
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
		YAssert(!dir_name.empty(), "Invalid directory name found.");
		YAssert(dir_name.back() != L'\\', "Invalid directory name found.");
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
		posix_dir.d_name = find_data.cFileName;
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


#define YCL_Impl_RetTryCatchAll(...) \
	try \
	{ \
		return __VA_ARGS__; \
	} \
	catch(...) \
	{}

int
uopen(const char* filename, int oflag) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_wopen(UTF8ToWCS(filename).c_str(), oflag))
	return -1;
#else
	return ::open(filename, oflag);
#endif
}
int
uopen(const char* filename, int oflag, int pmode) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_wopen(UTF8ToWCS(filename).c_str(), oflag, pmode))
	return -1;
#else
	return ::open(filename, oflag, pmode);
#endif
}
int
uopen(const char16_t* filename, int oflag) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	return ::_wopen(reinterpret_cast<const wchar_t*>(filename), oflag);
#else
	YCL_Impl_RetTryCatchAll(::open(strdup(filename).c_str(), oflag))
	return -1;
#endif
}
int
uopen(const char16_t* filename, int oflag, int pmode) ynothrow
{
	YAssertNonnull(filename);
#if YCL_Win32
	return ::_wopen(reinterpret_cast<const wchar_t*>(filename), oflag, pmode);
#else
	YCL_Impl_RetTryCatchAll(::open(strdup(filename).c_str(), oflag, pmode))
	return -1;
#endif
}

std::FILE*
ufopen(const char* filename, const char* mode) ynothrow
{
	YAssertNonnull(filename),
	YAssertNonnull(mode);
	YAssert(*mode != char(), "Invalid argument found.");
#if YCL_Win32
	YCL_Impl_RetTryCatchAll(::_wfopen(UTF8ToWCS(filename).c_str(),
		UTF8ToWCS(mode).c_str()))
	return {};
#else
	return std::fopen(filename, mode);
#endif
}
std::FILE*
ufopen(const char16_t* filename, const char16_t* mode) ynothrow
{
	YAssertNonnull(filename),
	YAssertNonnull(mode);
	YAssert(*mode != char(), "Invalid argument found.");
#if YCL_Win32
	return ::_wfopen(reinterpret_cast<const wchar_t*>(filename),
		reinterpret_cast<const wchar_t*>(mode));
#else
	YCL_Impl_RetTryCatchAll(std::fopen(strdup(filename).c_str(),
		strdup(mode).c_str()))
	return {};
#endif
}

bool
ufexists(const char* filename) ynothrow
{
#if YCL_Win32
	YAssertNonnull(filename);
	if(const auto file = ufopen(filename, "rb"))
	{
		std::fclose(file);
		return true;
	}
	return {};
#else
	return ystdex::fexists(filename);
#endif
}
bool
ufexists(const char16_t* filename) ynothrow
{
	YAssertNonnull(filename);
	if(const auto file = ufopen(filename, u"rb"))
	{
		std::fclose(file);
		return true;
	}
	return {};
}

char16_t*
u16getcwd_n(char16_t* buf, std::size_t size) ynothrow
{
	if(YB_UNLIKELY(!buf || size == 0))
		errno = EINVAL;
	else
	{
		using namespace std;

#if YCL_Win32
		return reinterpret_cast<char16_t*>(
			::_wgetcwd(reinterpret_cast<wchar_t*>(buf), size));
#else
		if(const auto cwd = ::getcwd(reinterpret_cast<char*>(buf), size))
			try
			{
				const auto res(ucsdup(cwd));
				const auto len(res.length());

				if(size < len + 1)
					errno = ERANGE;
				else
				{
					std::copy(res.begin(), res.end(), buf);
					return buf;
				}
			}
			catch(std::bad_alloc&)
			{
				errno = ENOMEM;
			}
#endif
	}
	return {};
}

//! \since build 476
#define YCL_Impl_FileSystem_ufunc_1(_n) \
bool \
_n(const char* path) ynothrow \
{ \
	YAssertNonnull(path); \

#if YCL_Win32
#	define YCL_Impl_FileSystem_ufunc_2(_fn, _wfn) \
	YCL_Impl_RetTryCatchAll(_wfn(UTF8ToWCS(path).c_str()) == 0) \
	return false; \
}
#else
#	define YCL_Impl_FileSystem_ufunc_2(_fn, _wfn) \
	return _fn(path) == 0; \
}
#endif

#define YCL_Impl_FileSystem_ufunc(_n, _fn, _wfn) \
	YCL_Impl_FileSystem_ufunc_1(_n) \
	YCL_Impl_FileSystem_ufunc_2(_fn, _wfn)

YCL_Impl_FileSystem_ufunc(uchdir, ::chdir, ::_wchdir)

YCL_Impl_FileSystem_ufunc_1(umkdir)
#if YCL_Win32
	YCL_Impl_FileSystem_ufunc_2(_unused_, ::_wmkdir)
#else
	return ::mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO) == 0;
}
#endif

YCL_Impl_FileSystem_ufunc(urmdir, ::rmdir, ::_wrmdir)

YCL_Impl_FileSystem_ufunc(uunlink, ::unlink, ::_wunlink)

YCL_Impl_FileSystem_ufunc(uremove, std::remove, ::_wremove)

#undef YCL_Impl_FileSystem_ufunc_1
#undef YCL_Impl_FileSystem_ufunc_2
#undef YCL_Impl_FileSystem_ufunc

bool
truncate(std::FILE* fp, std::size_t size) ynothrow
{
#if YCL_Win32
	return ::_chsize(::_fileno(fp), long(size)) == 0;
#else
	return ::ftruncate(fileno(fp), ::off_t(size)) == 0;
#endif
}


std::uint64_t
GetFileSizeOf(int fd)
{
#if YCL_Win32
	::LARGE_INTEGER sz;

	// XXX: Error handling for indirect calls.
	if(::GetFileSizeEx(::HANDLE(::_get_osfhandle(fd)), &sz) != 0
		&& YB_LIKELY(sz.QuadPart >= 0))
		return sz.QuadPart;
	// TODO: Get correct error condition.
#else
	struct ::stat st;

	if(::fstat(fd, &st) == 0)
		return st.st_size;
#endif
	throw FileOperationFailure(errno, std::generic_category(),
		"Failed getting file size.");
}
std::uint64_t
GetFileSizeOf(std::FILE* fp)
{
	YAssertNonnull(fp);
#if YCL_Win32
	return GetFileSizeOf(::_fileno(fp));
#else
	return GetFileSizeOf(fileno(fp));
#endif
}


DirectorySession::DirectorySession(const char* path)
	: dir(
#if YCL_Win32
		new DirectoryData
#else
		::opendir
#endif
		(path && *path != '\0' ? path : ".")
	)
{
	if(!dir)
		throw FileOperationFailure(errno, std::generic_category(),
			"Opening directory failed.");
}
DirectorySession::~DirectorySession()
{
#if !YCL_Win32
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
#if YCL_Win32
	static_cast<DirectoryData*>(dir)->Rewind();
#else
	::rewinddir(dir);
#endif
}


HDirectory&
HDirectory::operator++()
{
	YAssert(bool(GetNativeHandle()) == bool(p_dirent),
		"Invariant violation found.");
#if YCL_Win32
	p_dirent = static_cast<DirectoryData*>(GetNativeHandle())->Read();
#else
	p_dirent = ::readdir(GetNativeHandle());
#endif
	return *this;
}

NodeCategory
HDirectory::GetNodeCategory() const ynothrow
{
	YAssert(bool(GetNativeHandle()) == bool(p_dirent),
		"Invariant violation found.");
	if(p_dirent)
	{
		NodeCategory res(NodeCategory::Empty);
#if YCL_Win32
		const auto& dir_data(*static_cast<DirectoryData*>(GetNativeHandle()));
		const auto& find_data(dir_data.GetFindData());

		if(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			res |= NodeCategory::Directory;
		if(find_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
		{
			switch(find_data.dwReserved0)
			{
			case IO_REPARSE_TAG_SYMLINK:
				res |= NodeCategory::SymbolicLink;
				break;
			case IO_REPARSE_TAG_MOUNT_POINT:
				res |= NodeCategory::MountPoint;
			default:
				// TODO: Implement for other tags?
				;
			}
		}

		struct ::_stat st;
		auto name(dir_data.GetDirName());

		YAssert(!name.empty() && name.back() == L'*', "Invalid state found.");
		name.pop_back();
		YAssert(!name.empty() && name.back() == L'\\', "Invalid state found.");
		if(::_wstat((name + static_cast<DirEnv*>(p_dirent)->d_name).c_str(),
			&st) == 0)
		{
			const auto m(st.st_mode & _S_IFMT);

			if(YB_UNLIKELY(m & _S_IFDIR))
				res |= NodeCategory::Directory;
			if(m & _S_IFREG)
				res |= NodeCategory::Regular;
			if(YB_UNLIKELY(m & _S_IFCHR))
				res |= NodeCategory::Character;
			if(YB_UNLIKELY(m & _S_IFIFO))
				res |= NodeCategory::FIFO;
		}
#else
		// FIXME: Implement for platforms without 'DT_*'.
		const auto d_type(p_dirent->d_type);

		if(YB_UNLIKELY(d_type) == DT_UNKNOWN)
			return NodeCategory::Unknown;
		if(d_type & DT_REG)
			res |= NodeCategory::Regular;
		if(d_type & DT_DIR)
			res |= NodeCategory::Directory;
		if(d_type & DT_LNK)
			res |= NodeCategory::SymbolicLink;
		if(d_type & DT_CHR)
			res |= NodeCategory::Character;
		else if(d_type & DT_BLK)
			res |= NodeCategory::Block;
		if(d_type & DT_FIFO)
			res |= NodeCategory::FIFO;
		else if(d_type & DT_SOCK)
			res |= NodeCategory::Socket;
		if(d_type & DT_WHT)
			res |= NodeCategory::Missing;
#endif
		return res != NodeCategory::Empty ? res : NodeCategory::Invalid;
	}
	return NodeCategory::Empty;
}

const char*
HDirectory::GetName() const ynothrow
{
	YAssert(bool(GetNativeHandle()) == bool(p_dirent),
		"Invariant violation found.");
	if(p_dirent)
	{
#if !YCL_Win32
		return p_dirent->d_name;
#else
		YCL_Impl_RetTryCatchAll(utf8_name = platform_ex::WCSToUTF8(
			(static_cast<DirEnv*>(p_dirent))->d_name), &utf8_name[0])
#endif
	}
	return ".";
}

#undef YCL_Impl_RetTryCatchAll


bool
IsAbsolute(const char* path)
{
#if YCL_Win32
	return !::PathIsRelativeW(UTF8ToWCS(path).c_str());
#else
	if(path)
	{
		if(*path == '/')
			return true;

		const auto p(std::strstr(path, ":/"));

		return p && p != path && !std::strstr(p, ":/");
	}
	return {};
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

#if !YCL_Win32
char16_t
FS_IsRoot(const char16_t* str)
{
	const std::u16string ustr(str);

	return ustr == u"/"
#if YCL_DS
	|| ustr == u"fat:/" || ustr == u"sd:/"
#endif
	;
}
#endif

}

