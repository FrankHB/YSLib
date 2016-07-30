/*
	© 2013-2016 FrankHB.

	This file is part of the YSLib project, and may only be used,
	modified, and distributed under the terms of the YSLib project
	license, LICENSE.TXT.  By continuing to use, modify, or distribute
	this file you indicate that you have read the license and
	understand and accept it fully.
*/

/*!	\file Registry.cpp
\ingroup YCLib
\ingroup Win32
\brief 注册表。
\version r148
\author FrankHB <frankhb1989@gmail.com>
\since build 427
\par 创建时间:
	2015-09-12 19:39:47 +0800
\par 修改时间:
	2016-07-30 19:43 +0800
\par 文本编码:
	UTF-8
\par 模块名称:
	YCLib_(Win32)::Registry
*/


#include "YCLib/YModules.h"
#include YFM_YCLib_Platform
#if YCL_Win32
#	include YFM_Win32_YCLib_Registry
#endif

namespace platform_ex
{

#if YCL_Win32

inline namespace Windows
{

void
RegistryKey::Flush()
{
	YCL_RaiseZ_Win32E(::RegFlushKey(h_key), "RegFlushKey");
}

pair<unsigned long, vector<byte>>
RegistryKey::GetRawValue(const wchar_t* name, unsigned long type) const
{
	unsigned long size;

	YCL_RaiseZ_Win32E(::RegQueryValueExW(h_key,
		platform::Nonnull(name), {}, type == REG_NONE ? &type : nullptr, {},
		&size), "RegQueryValueExW");

	vector<byte> res(size);

	YCL_RaiseZ_Win32E(::RegQueryValueExW(h_key, name,
		{}, &type, &res[0], &size), "RegQueryValueExW");
	return {type, std::move(res)};
}
size_t
RegistryKey::GetSubKeyCount() const
{
	unsigned long res;

	YCL_RaiseZ_Win32E(::RegQueryInfoKey(h_key, {}, {}, {},
		&res, {}, {}, {}, {}, {}, {}, {}), "RegQueryInfoKey");
	return size_t(res);
}
vector<wstring>
RegistryKey::GetSubKeyNames() const
{
	const auto cnt(GetSubKeyCount());
	vector<wstring> res;

	if(cnt > 0)
	{
		// NOTE: See http://msdn.microsoft.com/en-us/library/windows/desktop/ms724872(v=vs.85).aspx .
		wchar_t name[256];

		for(res.reserve(cnt); res.size() < cnt; res.emplace_back(name))
			YCL_RaiseZ_Win32E(::RegEnumKeyExW(h_key,
				static_cast<unsigned long>(res.size()), name, {}, {}, {}, {},
				{}), "RegEnumKeyExW");
	}
	return res;
}
size_t
RegistryKey::GetValueCount() const
{
	unsigned long res;

	YCL_RaiseZ_Win32E(::RegQueryInfoKey(h_key, {}, {}, {}, {},
		{}, {}, &res, {}, {}, {}, {}), "RegQueryInfoKey");
	return size_t(res);
}
vector<wstring>
RegistryKey::GetValueNames() const
{
	const auto cnt(GetValueCount());
	vector<wstring> res;

	if(cnt > 0)
	{
		// NOTE: See http://msdn.microsoft.com/en-us/library/windows/desktop/ms724872(v=vs.85).aspx .
		wchar_t name[16384];

		for(res.reserve(cnt); res.size() < cnt; res.emplace_back(name))
			YCL_RaiseZ_Win32E(::RegEnumValueW(h_key,
				static_cast<unsigned long>(res.size()), name, {}, {}, {}, {},
				{}), "RegEnumValueW");
	}
	return res;
}

wstring
FetchRegistryString(const RegistryKey& key, const wchar_t* name)
{
	try
	{
		const auto pr(key.GetRawValue(name, REG_SZ));

		if(pr.first == REG_SZ && !pr.second.empty())
			// TODO: Improve performance?
			return ystdex::rtrim(wstring(reinterpret_cast<const wchar_t*>(
				&pr.second[0]), pr.second.size() / 2), wchar_t());
	}
	catch(Win32Exception&)
	{}
	return {};
}

} // inline namespace Windows;

#endif

} // namespace YSLib;

